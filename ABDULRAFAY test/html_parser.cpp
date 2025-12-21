#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <stdexcept>
#include <cstdio>
#include <string>

// ============================================================================
// STRING UTILITY FUNCTIONS (Manual implementation)
// ============================================================================

char* copyString(const char* str) {
    if (!str) return nullptr;
    int len = strlen(str);
    char* result = new char[len + 1];
    strcpy(result, str);
    return result;
}

char* substring(const char* str, int start, int len) {
    if (!str || start < 0 || len < 0) return nullptr;
    int strLen = strlen(str);
    if (start >= strLen) return nullptr;
    if (start + len > strLen) len = strLen - start;
    if (len <= 0) return nullptr;
    char* result = new char[len + 1];
    if (!result) return nullptr;
    strncpy(result, str + start, len);
    result[len] = '\0';
    return result;
}

void trimString(char* str) {
    if (!str) return;
    int start = 0;
    while (str[start] && isspace(str[start])) start++;
    int end = strlen(str) - 1;
    while (end >= start && isspace(str[end])) end--;
    int len = end - start + 1;
    memmove(str, str + start, len);
    str[len] = '\0';
}

// ============================================================================
// LINKED LIST CLASS
// ============================================================================

template<typename T>
class LinkedList {
private:
    struct Node {
        T data;
        Node* next;
        Node(T d) : data(d), next(nullptr) {}
    };
    Node* head;
    int size;

public:
    LinkedList() : head(nullptr), size(0) {}
    
    ~LinkedList() {
        clear();
    }
    
    void insert(T data) {
        Node* newNode = new Node(data);
        if (!head) {
            head = newNode;
        } else {
            Node* current = head;
            while (current->next) {
                current = current->next;
            }
            current->next = newNode;
        }
        size++;
    }
    
    void insertFront(T data) {
        Node* newNode = new Node(data);
        newNode->next = head;
        head = newNode;
        size++;
    }
    
    bool remove(T data) {
        if (!head) return false;
        if (head->data == data) {
            Node* temp = head;
            head = head->next;
            delete temp;
            size--;
            return true;
        }
        Node* current = head;
        while (current->next) {
            if (current->next->data == data) {
                Node* temp = current->next;
                current->next = temp->next;
                delete temp;
                size--;
                return true;
            }
            current = current->next;
        }
        return false;
    }
    
    bool search(T data) {
        Node* current = head;
        while (current) {
            if (current->data == data) return true;
            current = current->next;
        }
        return false;
    }
    
    bool isEmpty() const {
        return head == nullptr;
    }
    
    int getSize() const {
        return size;
    }
    
    T getFront() {
        if (head) return head->data;
        return T();
    }
    
    void clear() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
        size = 0;
    }
    
    Node* getHead() const {
        return head;
    }
};

// ============================================================================
// STACK CLASS
// ============================================================================

template<typename T>
class Stack {
private:
    struct Node {
        T data;
        Node* next;
        Node(T d) : data(d), next(nullptr) {}
    };
    Node* top;
    int size;

public:
    Stack() : top(nullptr), size(0) {}
    
    ~Stack() {
        clear();
    }
    
    void push(T data) {
        Node* newNode = new Node(data);
        newNode->next = top;
        top = newNode;
        size++;
    }
    
    T pop() {
        if (!top) return T();
        Node* temp = top;
        T data = top->data;
        top = top->next;
        delete temp;
        size--;
        return data;
    }
    
    T peek() const {
        if (!top) return T();
        return top->data;
    }
    
    bool isEmpty() const {
        return top == nullptr;
    }
    
    int getSize() const {
        return size;
    }
    
    void clear() {
        while (top) {
            Node* temp = top;
            top = top->next;
            delete temp;
        }
        size = 0;
    }
};

// ============================================================================
// QUEUE CLASS
// ============================================================================

template<typename T>
class Queue {
private:
    struct Node {
        T data;
        Node* next;
        Node(T d) : data(d), next(nullptr) {}
    };
    Node* front;
    Node* rear;
    int size;

public:
    Queue() : front(nullptr), rear(nullptr), size(0) {}
    
    ~Queue() {
        clear();
    }
    
    void enqueue(T data) {
        Node* newNode = new Node(data);
        if (!rear) {
            front = rear = newNode;
        } else {
            rear->next = newNode;
            rear = newNode;
        }
        size++;
    }
    
    T dequeue() {
        if (!front) return T();
        Node* temp = front;
        T data = front->data;
        front = front->next;
        if (!front) rear = nullptr;
        delete temp;
        size--;
        return data;
    }
    
    T peek() const {
        if (!front) return T();
        return front->data;
    }
    
    bool isEmpty() const {
        return front == nullptr;
    }
    
    int getSize() const {
        return size;
    }
    
    void clear() {
        while (front) {
            Node* temp = front;
            front = front->next;
            delete temp;
        }
        rear = nullptr;
        size = 0;
    }
};

// ============================================================================
// HASH TABLE CLASS
// ============================================================================

class HashTable {
private:
    struct HashNode {
        char* key;
        char* value;
        HashNode* next;
        HashNode(const char* k, const char* v) {
            key = copyString(k);
            value = copyString(v);
            next = nullptr;
        }
        ~HashNode() {
            delete[] key;
            delete[] value;
        }
    };
    
    static const int TABLE_SIZE = 101;
    HashNode** buckets;
    int size;

    int hashFunction(const char* key) {
        int hash = 0;
        for (int i = 0; key[i]; i++) {
            hash = (hash * 31 + key[i]) % TABLE_SIZE;
        }
        return hash;
    }

public:
    HashTable() : size(0) {
        buckets = new HashNode*[TABLE_SIZE];
        for (int i = 0; i < TABLE_SIZE; i++) {
            buckets[i] = nullptr;
        }
    }
    
    ~HashTable() {
        clear();
        delete[] buckets;
    }
    
    void insert(const char* key, const char* value) {
        int index = hashFunction(key);
        HashNode* current = buckets[index];
        
        // Check if key already exists
        while (current) {
            if (strcmp(current->key, key) == 0) {
                delete[] current->value;
                current->value = copyString(value);
                return;
            }
            current = current->next;
        }
        
        // Insert new node at front of chain
        HashNode* newNode = new HashNode(key, value);
        newNode->next = buckets[index];
        buckets[index] = newNode;
        size++;
    }
    
    char* get(const char* key) {
        int index = hashFunction(key);
        HashNode* current = buckets[index];
        while (current) {
            if (strcmp(current->key, key) == 0) {
                return current->value;
            }
            current = current->next;
        }
        return nullptr;
    }
    
    bool contains(const char* key) {
        return get(key) != nullptr;
    }
    
    void remove(const char* key) {
        int index = hashFunction(key);
        HashNode* current = buckets[index];
        HashNode* prev = nullptr;
        
        while (current) {
            if (strcmp(current->key, key) == 0) {
                if (prev) {
                    prev->next = current->next;
                } else {
                    buckets[index] = current->next;
                }
                delete current;
                size--;
                return;
            }
            prev = current;
            current = current->next;
        }
    }
    
    int getSize() const {
        return size;
    }
    
    void clear() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            while (buckets[i]) {
                HashNode* temp = buckets[i];
                buckets[i] = buckets[i]->next;
                delete temp;
            }
        }
        size = 0;
    }
};

// ============================================================================
// AVL TREE CLASS (for tag indexing)
// ============================================================================

class AVLTree {
private:
    struct AVLNode {
        char* data;
        AVLNode* left;
        AVLNode* right;
        int height;
        
        AVLNode(const char* d) {
            data = copyString(d);
            left = right = nullptr;
            height = 1;
        }
        
        ~AVLNode() {
            delete[] data;
        }
    };
    
    AVLNode* root;
    
    int getHeight(AVLNode* node) {
        return node ? node->height : 0;
    }
    
    int getBalance(AVLNode* node) {
        return node ? getHeight(node->left) - getHeight(node->right) : 0;
    }
    
    void updateHeight(AVLNode* node) {
        if (node) {
            int leftHeight = getHeight(node->left);
            int rightHeight = getHeight(node->right);
            node->height = (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
        }
    }
    
    AVLNode* rotateRight(AVLNode* y) {
        if (!y || !y->left) return y;
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;
        
        x->right = y;
        y->left = T2;
        
        updateHeight(y);
        updateHeight(x);
        
        return x;
    }
    
    AVLNode* rotateLeft(AVLNode* x) {
        if (!x || !x->right) return x;
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;
        
        y->left = x;
        x->right = T2;
        
        updateHeight(x);
        updateHeight(y);
        
        return y;
    }
    
    AVLNode* insertHelper(AVLNode* node, const char* data) {
        if (!node) {
            return new AVLNode(data);
        }
        
        int cmp = strcmp(data, node->data);
        if (cmp < 0) {
            node->left = insertHelper(node->left, data);
        } else if (cmp > 0) {
            node->right = insertHelper(node->right, data);
        } else {
            return node; // Duplicate
        }
        
        updateHeight(node);
        
        int balance = getBalance(node);
        
        // Left Left Case
        if (balance > 1 && node->left && strcmp(data, node->left->data) < 0) {
            return rotateRight(node);
        }
        
        // Right Right Case
        if (balance < -1 && node->right && strcmp(data, node->right->data) > 0) {
            return rotateLeft(node);
        }
        
        // Left Right Case
        if (balance > 1 && node->left && strcmp(data, node->left->data) > 0) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        
        // Right Left Case
        if (balance < -1 && node->right && strcmp(data, node->right->data) < 0) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        
        return node;
    }
    
    bool searchHelper(AVLNode* node, const char* data) {
        if (!node) return false;
        
        int cmp = strcmp(data, node->data);
        if (cmp == 0) return true;
        if (cmp < 0) return searchHelper(node->left, data);
        return searchHelper(node->right, data);
    }
    
    void clearHelper(AVLNode* node) {
        if (node) {
            clearHelper(node->left);
            clearHelper(node->right);
            delete node;
        }
    }

public:
    AVLTree() : root(nullptr) {}
    
    ~AVLTree() {
        clear();
    }
    
    void insert(const char* data) {
        root = insertHelper(root, data);
    }
    
    bool search(const char* data) {
        return searchHelper(root, data);
    }
    
    void clear() {
        clearHelper(root);
        root = nullptr;
    }
};

// ============================================================================
// GRAPH CLASS (adjacency list representation)
// ============================================================================

class Graph {
private:
    struct GraphNode {
        int vertex;
        GraphNode* next;
        GraphNode(int v) : vertex(v), next(nullptr) {}
    };
    
    GraphNode** adjList;
    int numVertices;
    int maxVertices;

public:
    Graph(int maxV = 1000) : numVertices(0), maxVertices(maxV) {
        adjList = new GraphNode*[maxVertices];
        for (int i = 0; i < maxVertices; i++) {
            adjList[i] = nullptr;
        }
    }
    
    ~Graph() {
        for (int i = 0; i < maxVertices; i++) {
            GraphNode* current = adjList[i];
            while (current) {
                GraphNode* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] adjList;
    }
    
    void addVertex(int v) {
        if (v >= 0 && v < maxVertices && v >= numVertices) {
            numVertices = v + 1;
        }
    }
    
    void addEdge(int src, int dest) {
        if (src >= 0 && src < maxVertices && dest >= 0 && dest < maxVertices) {
            addVertex(src);
            addVertex(dest);
            GraphNode* newNode = new GraphNode(dest);
            newNode->next = adjList[src];
            adjList[src] = newNode;
        }
    }
    
    void printGraph() {
        for (int i = 0; i < numVertices; i++) {
            std::cout << "Vertex " << i << ": ";
            GraphNode* current = adjList[i];
            while (current) {
                std::cout << current->vertex << " ";
                current = current->next;
            }
            std::cout << std::endl;
        }
    }
    
    int getNumVertices() const {
        return numVertices;
    }
};

// ============================================================================
// HTML NODE STRUCTURE (General Tree for DOM)
// ============================================================================

enum TokenType {
    OPEN_TAG,
    CLOSE_TAG,
    TEXT,
    SELF_CLOSE_TAG
};

struct Token {
    TokenType type;
    char* content;
    HashTable* attributes;
    
    Token() : type(TEXT), content(nullptr), attributes(nullptr) {}
    
    ~Token() {
        delete[] content;
        delete attributes;
    }
};

struct HTMLNode {
    char* tagName;
    char* textContent;
    HashTable* attributes;
    HTMLNode* parent;
    HTMLNode* firstChild;
    HTMLNode* nextSibling;
    int depth;
    int nodeId; // For graph representation
    
    HTMLNode() {
        tagName = nullptr;
        textContent = nullptr;
        attributes = new HashTable();
        parent = nullptr;
        firstChild = nullptr;
        nextSibling = nullptr;
        depth = 0;
        nodeId = -1;
    }
    
    ~HTMLNode() {
        delete[] tagName;
        delete[] textContent;
        delete attributes;
        // Children and siblings are managed by tree cleanup
    }
    
    void addChild(HTMLNode* child) {
        if (!child) return;
        child->parent = this;
        child->depth = this->depth + 1;
        
        if (!firstChild) {
            firstChild = child;
        } else {
            HTMLNode* current = firstChild;
            while (current->nextSibling) {
                current = current->nextSibling;
            }
            current->nextSibling = child;
        }
    }
    
    void addSibling(HTMLNode* sibling) {
        if (!sibling) return;
        HTMLNode* current = this;
        while (current->nextSibling) {
            current = current->nextSibling;
        }
        current->nextSibling = sibling;
        sibling->parent = this->parent;
        sibling->depth = this->depth;
    }
};

// ============================================================================
// HTML PARSER CLASS
// ============================================================================

class HTMLParser {
private:
    HTMLNode* root;
    AVLTree* tagRegistry;
    Graph* elementGraph;
    int nodeCounter;
    std::string pageTitle;
    
    // HTML4 valid tags (no duplicates)
    const char* html4Tags[49] = {
        "html", "head", "body", "title", "meta", "div", "span",
        "h1", "h2", "h3", "h4", "h5", "h6",
        "p", "br", "hr", "pre", "code",
        "strong", "em", "b", "i", "u", "small", "big",
        "ul", "ol", "li", "dl", "dt", "dd",
        "a", "img",
        "table", "tr", "td", "th", "thead", "tbody",
        "form", "input", "textarea", "button", "select", "option",
        "script", "style", "link", "base"
    };
    
    void initializeTagRegistry() {
        tagRegistry = new AVLTree();
        for (int i = 0; i < 49; i++) {
            tagRegistry->insert(html4Tags[i]);
        }
    }
    
    char* toLowerCase(const char* str) {
        if (!str) return nullptr;
        int len = strlen(str);
        char* result = new char[len + 1];
        for (int i = 0; i < len; i++) {
            result[i] = tolower(str[i]);
        }
        result[len] = '\0';
        return result;
    }
    
    Token* createToken(TokenType type, const char* content) {
        Token* token = new Token();
        token->type = type;
        token->content = copyString(content);
        token->attributes = new HashTable();
        return token;
    }
    
    void parseAttributes(const char* tagStr, HashTable* attrs) {
        if (!tagStr || !attrs) return;
        
        int len = strlen(tagStr);
        int i = 0;
        
        while (i < len) {
            // Skip whitespace
            while (i < len && isspace(tagStr[i])) i++;
            if (i >= len) break;
            
            // Find attribute name
            int nameStart = i;
            while (i < len && !isspace(tagStr[i]) && tagStr[i] != '=') i++;
            if (i == nameStart) break;
            
            char* attrName = substring(tagStr, nameStart, i - nameStart);
            char* lowerName = toLowerCase(attrName);
            
            // Skip whitespace and =
            while (i < len && (isspace(tagStr[i]) || tagStr[i] == '=')) i++;
            
            if (i < len && tagStr[i] == '"') {
                i++; // Skip opening quote
                int valueStart = i;
                while (i < len && tagStr[i] != '"') i++;
                if (i < len) {
                    char* attrValue = substring(tagStr, valueStart, i - valueStart);
                    attrs->insert(lowerName, attrValue);
                    delete[] attrValue;
                    i++; // Skip closing quote
                }
            }
            
            delete[] attrName;
            delete[] lowerName;
        }
    }
    
    Queue<Token*>* tokenize(const char* html) {
        Queue<Token*>* tokens = new Queue<Token*>();
        if (!html) return tokens;
        
        int len = strlen(html);
        int i = 0;
        
        while (i < len) {
            // Skip whitespace
            while (i < len && isspace(html[i])) i++;
            if (i >= len) break;
            
            if (html[i] == '<') {
                // Found a tag
                int tagStart = i;
                i++;
                
                if (i >= len) break;
                
                // Check for closing tag
                bool isClosing = false;
                if (html[i] == '/') {
                    isClosing = true;
                    i++;
                }
                
                // Find end of tag first
                int tagEnd = tagStart + 1;
                while (tagEnd < len && html[tagEnd] != '>') tagEnd++;
                
                // Check for self-closing (before >)
                bool isSelfClose = false;
                if (tagEnd < len && tagEnd > tagStart + 1 && html[tagEnd - 1] == '/') {
                    isSelfClose = true;
                }
                
                // Extract tag name
                int nameStart = i;
                while (i < len && i < tagEnd && html[i] != '>' && !isspace(html[i]) && html[i] != '/') {
                    i++;
                }
                int nameLen = i - nameStart;
                
                if (nameLen > 0 && nameStart >= 0 && nameStart + nameLen <= len) {
                    char* tagName = substring(html, nameStart, nameLen);
                    if (tagName && strlen(tagName) > 0) {
                        char* lowerTag = toLowerCase(tagName);
                        if (lowerTag) {
                            Token* token;
                            if (isSelfClose && !isClosing) {
                                token = createToken(SELF_CLOSE_TAG, lowerTag);
                            } else if (isClosing) {
                                token = createToken(CLOSE_TAG, lowerTag);
                            } else {
                                token = createToken(OPEN_TAG, lowerTag);
                                // Extract full tag for attributes (only for opening tags)
                                if (tagEnd > nameStart && tagEnd <= len) {
                                    int fullTagLen = tagEnd - nameStart;
                                    char* fullTag = substring(html, nameStart, fullTagLen);
                                    if (fullTag) {
                                        parseAttributes(fullTag, token->attributes);
                                        delete[] fullTag;
                                    }
                                }
                            }
                            
                            if (token) {
                                tokens->enqueue(token);
                            }
                            
                            delete[] lowerTag;
                        }
                        delete[] tagName;
                    }
                }
                
                // Skip to end of tag
                i = tagEnd;
                if (i < len) i++;
            } else {
                // Text content
                int textStart = i;
                while (i < len && html[i] != '<') i++;
                int textLen = i - textStart;
                
                if (textLen > 0) {
                    char* text = substring(html, textStart, textLen);
                    trimString(text);
                    if (strlen(text) > 0) {
                        Token* token = createToken(TEXT, text);
                        tokens->enqueue(token);
                    }
                    delete[] text;
                }
            }
        }
        
        return tokens;
    }
    
    HTMLNode* createNode(const char* tagName) {
        HTMLNode* node = new HTMLNode();
        node->tagName = copyString(tagName);
        node->nodeId = nodeCounter++;
        return node;
    }
    
    void buildDOMTree(Queue<Token*>* tokens) {
        root = nullptr;
        Stack<HTMLNode*>* nodeStack = new Stack<HTMLNode*>();
        HTMLNode* currentNode = nullptr;
        
        elementGraph = new Graph(1000);
        
        while (!tokens->isEmpty()) {
            Token* token = tokens->dequeue();
            
            try {
                if (token->type == OPEN_TAG) {
                    // Validate tag
                    bool isValid = tagRegistry->search(token->content);
                    if (!isValid) {
                        // Unknown tag (HTML5 or invalid) - handle gracefully
                        throw std::runtime_error("Unknown tag encountered");
                    }
                    
                    HTMLNode* newNode = createNode(token->content);
                    
                    // Attributes are stored in token but HashTable doesn't have iterator
                    // For now, attributes are parsed but not copied to node
                    // This can be enhanced later if needed
                    
                    if (!root) {
                        root = newNode;
                        currentNode = newNode;
                    } else {
                        if (currentNode) {
                            currentNode->addChild(newNode);
                        }
                        currentNode = newNode;
                    }
                    
                    nodeStack->push(newNode);
                    elementGraph->addVertex(newNode->nodeId);
                    
                    if (newNode->parent) {
                        elementGraph->addEdge(newNode->parent->nodeId, newNode->nodeId);
                    }
                    
                } else if (token->type == CLOSE_TAG) {
                    // Validate tag
                    bool isValid = tagRegistry->search(token->content);
                    if (!isValid) {
                        throw std::runtime_error("Unknown closing tag");
                    }
                    
                    // Pop stack until matching tag (don't delete nodes, they're in the tree)
                    Stack<HTMLNode*>* tempStack = new Stack<HTMLNode*>();
                    bool found = false;
                    
                    while (!nodeStack->isEmpty()) {
                        HTMLNode* top = nodeStack->pop();
                        if (top && strcmp(top->tagName, token->content) == 0) {
                            found = true;
                            // Don't delete - node is part of tree structure
                            break;
                        }
                        tempStack->push(top);
                    }
                    
                    // Push back unmatched nodes
                    while (!tempStack->isEmpty()) {
                        nodeStack->push(tempStack->pop());
                    }
                    delete tempStack;
                    
                    if (found) {
                        if (!nodeStack->isEmpty()) {
                            currentNode = nodeStack->peek();
                        } else {
                            currentNode = nullptr;
                        }
                    }
                    
                } else if (token->type == SELF_CLOSE_TAG) {
                    bool isValid = tagRegistry->search(token->content);
                    if (!isValid) {
                        throw std::runtime_error("Unknown self-closing tag");
                    }
                    
                    HTMLNode* newNode = createNode(token->content);
                    if (currentNode) {
                        currentNode->addChild(newNode);
                    } else if (!root) {
                        root = newNode;
                    }
                    
                    elementGraph->addVertex(newNode->nodeId);
                    if (newNode->parent) {
                        elementGraph->addEdge(newNode->parent->nodeId, newNode->nodeId);
                    }
                    
                } else if (token->type == TEXT) {
                    if (currentNode) {
                        if (!currentNode->textContent) {
                            currentNode->textContent = copyString(token->content);
                        } else {
                            // Append text
                            int oldLen = strlen(currentNode->textContent);
                            int newLen = strlen(token->content);
                            char* combined = new char[oldLen + newLen + 2];
                            strcpy(combined, currentNode->textContent);
                            strcat(combined, " ");
                            strcat(combined, token->content);
                            delete[] currentNode->textContent;
                            currentNode->textContent = combined;
                        }
                    }
                }
            } catch (const std::exception& e) {
                // Unknown tag - skip it but continue parsing
                // Tag is already logged, continue
            }
            
            delete token;
        }
        
        delete nodeStack;
    }

    bool tagEquals(HTMLNode* node, const char* name) {
        return node && node->tagName && name && strcmp(node->tagName, name) == 0;
    }

    bool isInlineBold(HTMLNode* node) {
        return tagEquals(node, "strong") || tagEquals(node, "b");
    }

    bool isInlineItalic(HTMLNode* node) {
        return tagEquals(node, "em") || tagEquals(node, "i");
    }

    void appendTrimmed(std::string& out, const char* text) {
        if (!text) return;
        std::string t(text);
        size_t start = t.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return;
        size_t end = t.find_last_not_of(" \t\r\n");
        if (!out.empty()) out += " ";
        out += t.substr(start, end - start + 1);
    }

    void appendWrapped(std::string& out, const char* marker, const std::string& inner) {
        if (inner.empty()) return;
        if (!out.empty()) out += " ";
        out += marker;
        out += inner;
        out += marker;
    }

    void buildInlineText(HTMLNode* node, std::string& out) {
        if (!node) return;
        if (node->textContent && strlen(node->textContent) > 0) {
            appendTrimmed(out, node->textContent);
        }

        HTMLNode* child = node->firstChild;
        while (child) {
            if (isInlineBold(child)) {
                std::string inner;
                buildInlineText(child, inner);
                appendWrapped(out, "**", inner);
            } else if (isInlineItalic(child)) {
                std::string inner;
                buildInlineText(child, inner);
                appendWrapped(out, "*", inner);
            } else if (tagEquals(child, "br")) {
                if (!out.empty()) out += " ";
            } else {
                buildInlineText(child, out);
            }
            child = child->nextSibling;
        }
    }

    void extractTitle(HTMLNode* node) {
        if (!node || !pageTitle.empty()) return;
        if (tagEquals(node, "title")) {
            std::string text;
            buildInlineText(node, text);
            pageTitle = text;
            return;
        }
        HTMLNode* child = node->firstChild;
        while (child) {
            extractTitle(child);
            if (!pageTitle.empty()) return;
            child = child->nextSibling;
        }
    }
    
    void writeNodeToFile(std::ofstream& file, HTMLNode* node, int indent) {
        if (!node) return;
        
        for (int i = 0; i < indent; i++) {
            file << "  ";
        }
        
        if (node->tagName) {
            file << "TAG:" << node->tagName << std::endl;
        }
        
        if (node->textContent && strlen(node->textContent) > 0) {
            for (int i = 0; i < indent + 1; i++) {
                file << "  ";
            }
            file << "TEXT:" << node->textContent << std::endl;
        }
        
        HTMLNode* child = node->firstChild;
        while (child) {
            writeNodeToFile(file, child, indent + 1);
            child = child->nextSibling;
        }
    }

    void writeRenderNodes(std::ofstream& file, HTMLNode* node) {
        if (!node) return;

        if (tagEquals(node, "h1") || tagEquals(node, "h2") || tagEquals(node, "h3") || tagEquals(node, "p")) {
            std::string text;
            buildInlineText(node, text);
            if (!text.empty()) {
                if (tagEquals(node, "h1")) file << "H1: ";
                else if (tagEquals(node, "h2")) file << "H2: ";
                else if (tagEquals(node, "h3")) file << "H3: ";
                else file << "P: ";
                file << text << std::endl;
            }
        }

        HTMLNode* child = node->firstChild;
        while (child) {
            writeRenderNodes(file, child);
            child = child->nextSibling;
        }
    }
    
    void cleanupTree(HTMLNode* node) {
        if (!node) return;
        cleanupTree(node->firstChild);
        cleanupTree(node->nextSibling);
        delete node;
    }

public:
    HTMLParser() : root(nullptr), tagRegistry(nullptr), elementGraph(nullptr), nodeCounter(0) {
        initializeTagRegistry();
    }
    
    ~HTMLParser() {
        cleanupTree(root);
        delete tagRegistry;
        delete elementGraph;
    }
    
    void parse(const char* html) {
        Queue<Token*>* tokens = tokenize(html);
        buildDOMTree(tokens);
        
        // Cleanup tokens queue
        while (!tokens->isEmpty()) {
            Token* token = tokens->dequeue();
            delete token;
        }
        delete tokens;
    }
    
    void writeDebugToFile(const char* filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << " for writing" << std::endl;
            return;
        }
        
        if (root) {
            writeNodeToFile(file, root, 0);
        }
        
        file.close();
    }

    void writeRenderToFile(const char* filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << " for writing" << std::endl;
            return;
        }

        pageTitle.clear();
        extractTitle(root);
        if (!pageTitle.empty()) {
            file << "TITLE: " << pageTitle << std::endl;
        }

        if (root) {
            writeRenderNodes(file, root);
        }

        file.close();
    }
    
    HTMLNode* getRoot() {
        return root;
    }
};

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main(int argc, char* argv[]) {
    const char* inputFile = "output.html";
    const char* outputFile = "page.txt";
    const char* debugFile = "parsed_output.txt";
    bool writeDebug = false;
    
    if (argc > 1) {
        inputFile = argv[1];
    }
    if (argc > 2) {
        outputFile = argv[2];
    }
    if (argc > 3 && strcmp(argv[3], "--debug") == 0) {
        writeDebug = true;
        if (argc > 4) {
            debugFile = argv[4];
        }
    }
    
    std::cout << "Input HTML: " << inputFile << std::endl;
    std::cout << "Render output: " << outputFile << std::endl;
    if (writeDebug) {
        std::cout << "Debug output: " << debugFile << std::endl;
    }

    // Read HTML file using C-style I/O
    FILE* file = fopen(inputFile, "r");
    if (!file) {
        std::cerr << "Error: Cannot open file " << inputFile << std::endl;
        return 1;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Allocate buffer
    char* htmlContent = new char[fileSize + 1];
    size_t bytesRead = fread(htmlContent, 1, fileSize, file);
    htmlContent[bytesRead] = '\0';
    fclose(file);
    
    // Parse HTML
    HTMLParser parser;
    parser.parse(htmlContent);
    
    // Write output
    parser.writeRenderToFile(outputFile);
    if (writeDebug) {
        parser.writeDebugToFile(debugFile);
    }
    
    delete[] htmlContent;
    
    std::cout << "HTML parsing completed. Output written to " << outputFile << std::endl;
    
    return 0;
}

