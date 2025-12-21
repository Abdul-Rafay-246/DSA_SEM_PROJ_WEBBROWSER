#include <windows.h>
#include <commctrl.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

struct TabData {
    std::string url;
    std::string htmlFile;
    std::string pageFile;
};

static std::vector<TabData> g_tabs;
static int g_currentTab = -1;
static std::string g_exeDir;

static HWND g_tabCtrl = nullptr;
static HWND g_urlEdit = nullptr;
static HWND g_goBtn = nullptr;
static HWND g_addTabBtn = nullptr;
static HWND g_contentWnd = nullptr;

struct RenderLine {
    std::string text;
    int size = 16;
    bool bold = false;
    int spaceBefore = 6;
    int spaceAfter = 6;
};

static std::vector<RenderLine> g_renderLines;

static bool startsWith(const std::string& s, const char* prefix) {
    return s.rfind(prefix, 0) == 0;
}

static std::string quote(const std::string& s) {
    return std::string("\"") + s + "\"";
}

static std::string readFileToString(const std::string& path) {
    std::ifstream in(path);
    if (!in.is_open()) return "";
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

static bool runCommand(const std::string& cmdLine) {
    STARTUPINFOA si{};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi{};

    std::string mutableCmd = cmdLine;
    BOOL ok = CreateProcessA(
        nullptr,
        &mutableCmd[0],
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        nullptr,
        &si,
        &pi
    );

    if (!ok) {
        return false;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD exitCode = 1;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return exitCode == 0;
}

static void ensureExeDir() {
    if (!g_exeDir.empty()) return;
    char exePath[MAX_PATH]{};
    GetModuleFileNameA(nullptr, exePath, MAX_PATH);
    g_exeDir = exePath;
    size_t slash = g_exeDir.find_last_of("\\/");
    if (slash != std::string::npos) {
        g_exeDir = g_exeDir.substr(0, slash);
    }
}

static void loadRenderLines(const std::string& path) {
    g_renderLines.clear();
    std::ifstream in(path);
    if (!in.is_open()) return;

    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;

        RenderLine out{};
        out.text = line;

        if (startsWith(line, "TITLE:")) {
            continue;
        } else if (startsWith(line, "H1:")) {
            out.size = 28;
            out.bold = true;
            out.spaceBefore = 10;
            out.spaceAfter = 12;
            out.text = line.substr(3);
        } else if (startsWith(line, "H2:")) {
            out.size = 22;
            out.bold = true;
            out.spaceBefore = 8;
            out.spaceAfter = 10;
            out.text = line.substr(3);
        } else if (startsWith(line, "H3:")) {
            out.size = 18;
            out.bold = true;
            out.spaceBefore = 6;
            out.spaceAfter = 8;
            out.text = line.substr(3);
        } else if (startsWith(line, "P:")) {
            out.size = 16;
            out.bold = false;
            out.spaceBefore = 4;
            out.spaceAfter = 8;
            out.text = line.substr(2);
        } else {
            out.size = 16;
            out.bold = false;
            out.spaceBefore = 4;
            out.spaceAfter = 6;
        }

        if (!out.text.empty() && out.text[0] == ' ') {
            out.text.erase(out.text.begin());
        }

        g_renderLines.push_back(out);
    }

    InvalidateRect(g_contentWnd, nullptr, TRUE);
}

static void selectTab(int index) {
    if (index < 0 || index >= static_cast<int>(g_tabs.size())) return;
    g_currentTab = index;
    SetWindowTextA(g_urlEdit, g_tabs[index].url.c_str());
    loadRenderLines(g_tabs[index].pageFile);
}

static void addTab(const std::string& url) {
    TabData tab{};
    tab.url = url;

    int idx = static_cast<int>(g_tabs.size());
    tab.htmlFile = g_exeDir + "\\output_tab" + std::to_string(idx + 1) + ".html";
    tab.pageFile = g_exeDir + "\\page_tab" + std::to_string(idx + 1) + ".txt";
    g_tabs.push_back(tab);

    TCITEMA item{};
    item.mask = TCIF_TEXT;
    std::string label = "Tab " + std::to_string(idx + 1);
    item.pszText = const_cast<char*>(label.c_str());
    TabCtrl_InsertItem(g_tabCtrl, idx, &item);

    TabCtrl_SetCurSel(g_tabCtrl, idx);
    selectTab(idx);
}

static void handleGo() {
    if (g_currentTab < 0 || g_currentTab >= static_cast<int>(g_tabs.size())) return;

    char urlBuf[2048]{};
    GetWindowTextA(g_urlEdit, urlBuf, static_cast<int>(sizeof(urlBuf)));
    g_tabs[g_currentTab].url = urlBuf;

    const std::string fetchScript = g_exeDir + "\\internet access & html in txt file\\html_fetching_URL.py";
    const std::string parserExe = g_exeDir + "\\html_parser.exe";

    std::string fetchCmd = "python " + quote(fetchScript) + " " + quote(g_tabs[g_currentTab].url) + " " + quote(g_tabs[g_currentTab].htmlFile);
    std::string parseCmd = quote(parserExe) + " " + quote(g_tabs[g_currentTab].htmlFile) + " " + quote(g_tabs[g_currentTab].pageFile);

    if (!runCommand(fetchCmd)) {
        MessageBoxA(nullptr, "Fetch failed. Check URL or python.", "Error", MB_OK | MB_ICONERROR);
        return;
    }
    if (!runCommand(parseCmd)) {
        MessageBoxA(nullptr, "Parse failed. Check html_parser.exe.", "Error", MB_OK | MB_ICONERROR);
        return;
    }

    loadRenderLines(g_tabs[g_currentTab].pageFile);
}

static void layoutControls(HWND hwnd) {
    RECT rc{};
    GetClientRect(hwnd, &rc);

    int topBarH = 70;
    int pad = 8;
    int btnW = 80;
    int addW = 90;

    MoveWindow(g_tabCtrl, pad, pad, rc.right - pad * 2, 28, TRUE);
    MoveWindow(g_urlEdit, pad, 40, rc.right - pad * 3 - btnW - addW, 24, TRUE);
    MoveWindow(g_goBtn, rc.right - pad * 2 - addW - btnW, 40, btnW, 24, TRUE);
    MoveWindow(g_addTabBtn, rc.right - pad - addW, 40, addW, 24, TRUE);
    MoveWindow(g_contentWnd, pad, topBarH, rc.right - pad * 2, rc.bottom - topBarH - pad, TRUE);
}

static void drawContent(HDC hdc, const RECT& rc) {
    int x = rc.left + 10;
    int y = rc.top + 10;
    int maxW = rc.right - rc.left - 20;

    for (const auto& line : g_renderLines) {
        y += line.spaceBefore;

        LOGFONT lf{};
        lf.lfCharSet = DEFAULT_CHARSET;
        lf.lfHeight = -line.size;
        lf.lfWeight = line.bold ? FW_BOLD : FW_NORMAL;
        HFONT font = CreateFontIndirect(&lf);
        HFONT oldFont = (HFONT)SelectObject(hdc, font);

        RECT textRc{ x, y, x + maxW, rc.bottom };
        DrawTextA(hdc, line.text.c_str(), -1, &textRc, DT_LEFT | DT_TOP | DT_WORDBREAK | DT_NOPREFIX);

        RECT calcRc{ x, y, x + maxW, rc.bottom };
        DrawTextA(hdc, line.text.c_str(), -1, &calcRc, DT_LEFT | DT_TOP | DT_WORDBREAK | DT_CALCRECT | DT_NOPREFIX);
        y = calcRc.bottom + line.spaceAfter;

        SelectObject(hdc, oldFont);
        DeleteObject(font);
    }
}

static LRESULT CALLBACK ContentProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        RECT rc{};
        GetClientRect(hwnd, &rc);
        drawContent(hdc, rc);
        EndPaint(hwnd, &ps);
        return 0;
    }
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        INITCOMMONCONTROLSEX icc{};
        icc.dwSize = sizeof(icc);
        icc.dwICC = ICC_TAB_CLASSES;
        InitCommonControlsEx(&icc);

        g_tabCtrl = CreateWindowExA(0, WC_TABCONTROLA, "", WS_CHILD | WS_VISIBLE,
            0, 0, 0, 0, hwnd, nullptr, nullptr, nullptr);

        g_urlEdit = CreateWindowExA(WS_EX_CLIENTEDGE, "EDIT", "",
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
            0, 0, 0, 0, hwnd, (HMENU)1, nullptr, nullptr);

        g_goBtn = CreateWindowExA(0, "BUTTON", "Go",
            WS_CHILD | WS_VISIBLE,
            0, 0, 0, 0, hwnd, (HMENU)2, nullptr, nullptr);

        g_addTabBtn = CreateWindowExA(0, "BUTTON", "+ Tab",
            WS_CHILD | WS_VISIBLE,
            0, 0, 0, 0, hwnd, (HMENU)3, nullptr, nullptr);

        g_contentWnd = CreateWindowExA(WS_EX_CLIENTEDGE, "STATIC", "",
            WS_CHILD | WS_VISIBLE,
            0, 0, 0, 0, hwnd, nullptr, nullptr, nullptr);
        SetWindowLongPtrA(g_contentWnd, GWLP_WNDPROC, (LONG_PTR)ContentProc);

        addTab("https://www.w3schools.com/html/html_basic.asp");
        layoutControls(hwnd);
        return 0;
    }
    case WM_SIZE:
        layoutControls(hwnd);
        return 0;
    case WM_COMMAND:
        if (LOWORD(wParam) == 2) {
            handleGo();
            return 0;
        }
        if (LOWORD(wParam) == 3) {
            addTab("https://www.w3schools.com/html/html_basic.asp");
            return 0;
        }
        return 0;
    case WM_NOTIFY:
        if (((LPNMHDR)lParam)->hwndFrom == g_tabCtrl && ((LPNMHDR)lParam)->code == TCN_SELCHANGE) {
            int idx = TabCtrl_GetCurSel(g_tabCtrl);
            selectTab(idx);
            return 0;
        }
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    ensureExeDir();

    WNDCLASSA contentClass{};
    contentClass.lpfnWndProc = ContentProc;
    contentClass.hInstance = hInstance;
    contentClass.lpszClassName = "ContentView";
    contentClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassA(&contentClass);

    const char CLASS_NAME[] = "SimpleBrowserWindow";
    WNDCLASSA wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassA(&wc);

    HWND hwnd = CreateWindowExA(
        0,
        CLASS_NAME,
        "Simple Browser",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 900, 700,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!hwnd) return 0;
    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
