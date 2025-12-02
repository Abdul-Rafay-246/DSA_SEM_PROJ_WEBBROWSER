// html_parser.cpp
// Compile: g++ -std=c++17 html_parser.cpp -lcurl -O2 -o html_parser
// Usage: ./html_parser "https://www.w3schools.com/html/html_basic.asp" "selector"
// Example: ./html_parser "https://www.w3schools.com/html/html_basic.asp" "div > p"

#include <curl/curl.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <cctype>
#include <algorithm>

using AttrMap = std::map<std::string, std::string>;

struct Node {
    bool is_text = false;
    std::string tag;           // for elements
    AttrMap attrs;
    std::string text;          // for text nodes
    std::vector<Node*> children;
    Node* parent = nullptr;

    ~Node() {
        for (auto c : children) delete c;
    }
};

// --- libcurl fetch ---
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string fetch_url(const std::string& url) {
    CURL* curl = curl_easy_init();
    std::string buffer;
    if (!curl) return {};
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "html-parser/1.0");
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "curl error: " << curl_easy_strerror(res) << "\n";
    }
    curl_easy_cleanup(curl);
    return buffer;
}

// --- helpers ---
static inline void trim(std::string &s) {
    size_t a = 0;
    while (a < s.size() && isspace((unsigned char)s[a])) ++a;
    size_t b = s.size();
    while (b > a && isspace((unsigned char)s[b-1])) --b;
    s = s.substr(a, b-a);
}

static inline std::string to_lower(std::string s) {
    for (auto &c : s) c = std::tolower((unsigned char)c);
    return s;
}

// --- tokenizer & simple HTML parser ---
enum TokenType { TOK_OPEN, TOK_CLOSE, TOK_SELF_CLOSE, TOK_TEXT };

struct Token {
    TokenType type;
    std::string content;
};

std::vector<Token> tokenize(const std::string& html) {
    std::vector<Token> tokens;
    size_t i = 0, n = html.size();
    while (i < n) {
        if (html[i] == '<') {
            if (i + 4 < n && html.substr(i, 4) == "<!--") {
                size_t endc = html.find("-->", i+4);
                if (endc == std::string::npos) break;
                i = endc + 3;
                continue;
            }
            size_t j = html.find('>', i+1);
            if (j == std::string::npos) j = n-1;
            std::string inside = html.substr(i+1, j - (i+1));
            std::string tmp = inside;
            trim(tmp);
            if (!tmp.empty() && tmp[0] == '/') {
                tmp = tmp.substr(1);
                trim(tmp);
                tokens.push_back({TOK_CLOSE, tmp});
            } else {
                bool self = false;
                if (!tmp.empty() && tmp.back() == '/') {
                    self = true;
                    tmp.pop_back();
                    trim(tmp);
                }
                tokens.push_back({ self ? TOK_SELF_CLOSE : TOK_OPEN, tmp });
            }
            i = j + 1;
        } else {
            size_t j = html.find('<', i);
            if (j == std::string::npos) j = n;
            std::string text = html.substr(i, j - i);
            std::string t2 = text;
            trim(t2);
            if (!t2.empty()) tokens.push_back({TOK_TEXT, text});
            i = j;
        }
    }
    return tokens;
}

static inline bool isnamechar(char c) {
    return std::isalnum((unsigned char)c) || c=='-' || c==':' || c=='_';
}

void parse_attributes(const std::string& s, std::string& tagname, AttrMap& attrs) {
    size_t i = 0, n = s.size();
    while (i < n && isspace((unsigned char)s[i])) ++i;
    size_t start = i;
    while (i < n && isnamechar(s[i])) ++i;
    tagname = s.substr(start, i - start);

    while (i < n) {
        while (i < n && isspace((unsigned char)s[i])) ++i;
        if (i >= n) break;
        size_t astart = i;
        while (i < n && (isnamechar(s[i]) || s[i]=='/')) ++i;
        std::string an = s.substr(astart, i - astart);
        trim(an);
        if (an.empty()) { ++i; continue; }
        while (i < n && isspace((unsigned char)s[i])) ++i;
        if (i < n && s[i] == '=') {
            ++i;
            while (i < n && isspace((unsigned char)s[i])) ++i;
            char quote = 0;
            if (s[i] == '"' || s[i] == '\'') { quote = s[i++]; }
            size_t vstart = i;
            if (quote) {
                while (i < n && s[i] != quote) ++i;
                attrs[an] = s.substr(vstart, i-vstart);
                if (i < n && s[i] == quote) ++i;
            } else {
                while (i < n && !isspace((unsigned char)s[i])) ++i;
                attrs[an] = s.substr(vstart, i-vstart);
            }
        } else {
            attrs[an] = "";
        }
    }
}

Node* build_dom(const std::vector<Token>& toks) {
    Node* root = new Node();
    root->tag = "document";
    root->is_text = false;
    std::vector<Node*> stack;
    stack.push_back(root);

    for (auto &tk : toks) {
        if (tk.type == TOK_TEXT) {
            Node* t = new Node();
            t->is_text = true;
            t->text = tk.content;
            t->parent = stack.back();
            stack.back()->children.push_back(t);
        } else if (tk.type == TOK_OPEN || tk.type == TOK_SELF_CLOSE) {
            std::string tagn;
            AttrMap attrs;
            parse_attributes(tk.content, tagn, attrs);
            Node* n = new Node();
            n->is_text = false;
            n->tag = to_lower(tagn);
            n->attrs = attrs;
            n->parent = stack.back();
            stack.back()->children.push_back(n);
            if (tk.type == TOK_OPEN) stack.push_back(n);
        } else if (tk.type == TOK_CLOSE) {
            std::string closing = to_lower(tk.content);
            for (int k = (int)stack.size()-1; k > 0; --k) {
                if (stack[k]->tag == closing) {
                    stack.resize(k);
                    break;
                }
            }
        }
    }
    return root;
}

// --- Selector Matching Logic ---
struct SimpleSelector {
    std::string tag;
    std::string id;
    std::string cls;
    std::string attr;
    std::string attr_val;
};

bool match_simple(Node* n, const SimpleSelector& s) {
    if (!n || n->is_text) return false;
    if (!s.tag.empty() && s.tag != n->tag) return false;
    if (!s.id.empty()) {
        auto it = n->attrs.find("id");
        if (it==n->attrs.end() || it->second != s.id) return false;
    }
    if (!s.cls.empty()) {
        auto it = n->attrs.find("class");
        if (it==n->attrs.end()) return false;
        std::istringstream ss(it->second);
        std::string token;
        bool found=false;
        while (ss >> token) if (token == s.cls) found=true;
        if (!found) return false;
    }
    if (!s.attr.empty()) {
        auto it = n->attrs.find(s.attr);
        if (it==n->attrs.end()) return false;
        if (!s.attr_val.empty() && it->second != s.attr_val) return false;
    }
    return true;
}

SimpleSelector parse_simple_selector(const std::string& sel) {
    SimpleSelector s;
    size_t i = 0, n = sel.size();
    while (i < n && isnamechar(sel[i])) ++i;
    if (i>0) s.tag = to_lower(sel.substr(0,i));
    while (i < n) {
        if (sel[i] == '#') {
            ++i; size_t st=i;
            while (i<n && isnamechar(sel[i])) ++i;
            s.id = sel.substr(st, i-st);
        } else if (sel[i] == '.') {
            ++i; size_t st=i;
            while (i<n && isnamechar(sel[i])) ++i;
            s.cls = sel.substr(st, i-st);
        } else if (sel[i] == '[') {
            ++i; size_t st=i;
            while (i<n && sel[i] != ']' && sel[i] != '=') ++i;
            s.attr = sel.substr(st, i-st);
            if (i<n && sel[i]=='=') {
                ++i;
                char q=0;
                if (sel[i]=='"' || sel[i]=='\'') q=sel[i++];
                size_t vst=i;
                while (i<n && ((q && sel[i]!=q) || (!q && sel[i]!=']'))) ++i;
                s.attr_val = sel.substr(vst, i-vst);
                if (q && i<n && sel[i]==q) ++i;
            }
            while (i<n && sel[i]!=']') ++i;
            if (i<n) ++i;
        } else {
            ++i;
        }
    }
    return s;
}

struct SelToken { SimpleSelector s; char comb; };

std::vector<Node*> select(Node* root, const std::string& selector);

// --- main ---
int main(int argc, char** argv) {
    if (argc < 3) {
        std::cerr << "Usage:\n"
                  << argv[0] << " \"https://www.w3schools.com/html/html_basic.asp\" \"selector\"\n"
                  << "Example:\n"
                  << argv[0] << " \"https://www.w3schools.com/html/html_basic.asp\" \"h1\"\n";
        return 1;
    }

    std::string url = argv[1];       // Replaced URL will come from here
    std::string selector = argv[2];

    std::string html = fetch_url(url);
    if (html.empty()) {
        std::cerr << "Failed to fetch HTML.\n";
        return 2;
    }

    auto tokens = tokenize(html);
    Node* root = build_dom(tokens);

    auto matches = select(root, selector);
    if (matches.empty()) {
        std::cout << "No matches.\n";
    } else {
        std::cout << matches.size() << " matches:\n\n";
        for (auto n : matches) {
            std::cout << "<" << n->tag << ">\n";
        }
    }

    delete root;
    return 0;
}