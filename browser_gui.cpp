#include <windows.h>
#include <commctrl.h>
#include <windowsx.h>
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
static int g_tabSerial = 0;

static HWND g_tabCtrl = nullptr;
static HWND g_urlEdit = nullptr;
static HWND g_goBtn = nullptr;
static HWND g_addTabBtn = nullptr;
static HWND g_closeTabBtn = nullptr;
static HWND g_contentWnd = nullptr;

struct RenderLine {
    std::string text;
    int size = 16;
    bool bold = false;
    int spaceBefore = 6;
    int spaceAfter = 6;
};

static std::vector<RenderLine> g_renderLines;
static int g_contentHeight = 0;
static int g_scrollY = 0;

static bool startsWith(const std::string& s, const char* prefix) {
    return s.rfind(prefix, 0) == 0;
}

static bool fileExists(const std::string& path) {
    DWORD attrs = GetFileAttributesA(path.c_str());
    return attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY);
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

static void clearContent();

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
    g_contentHeight = 0;
    g_scrollY = 0;
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
    if (g_tabs[index].url.empty() || !fileExists(g_tabs[index].pageFile)) {
        clearContent();
    } else {
        loadRenderLines(g_tabs[index].pageFile);
    }
}

static void addTab(const std::string& url) {
    TabData tab{};
    tab.url = url;

    int idx = static_cast<int>(g_tabs.size());
    int serial = ++g_tabSerial;
    tab.htmlFile = g_exeDir + "\\output_tab" + std::to_string(serial) + ".html";
    tab.pageFile = g_exeDir + "\\page_tab" + std::to_string(serial) + ".txt";
    DeleteFileA(tab.htmlFile.c_str());
    DeleteFileA(tab.pageFile.c_str());
    g_tabs.push_back(tab);

    TCITEMA item{};
    item.mask = TCIF_TEXT;
    std::string label = "Tab " + std::to_string(idx + 1);
    item.pszText = const_cast<char*>(label.c_str());
    TabCtrl_InsertItem(g_tabCtrl, idx, &item);

    TabCtrl_SetCurSel(g_tabCtrl, idx);
    if (url.empty()) {
        g_currentTab = idx;
        SetWindowTextA(g_urlEdit, "");
        clearContent();
    } else {
        selectTab(idx);
    }
}

static void clearContent() {
    g_renderLines.clear();
    g_contentHeight = 0;
    g_scrollY = 0;
    SetScrollPos(g_contentWnd, SB_VERT, 0, TRUE);
    InvalidateRect(g_contentWnd, nullptr, TRUE);
}

static void closeCurrentTab() {
    if (g_currentTab < 0 || g_currentTab >= static_cast<int>(g_tabs.size())) return;

    DeleteFileA(g_tabs[g_currentTab].htmlFile.c_str());
    DeleteFileA(g_tabs[g_currentTab].pageFile.c_str());

    TabCtrl_DeleteItem(g_tabCtrl, g_currentTab);
    g_tabs.erase(g_tabs.begin() + g_currentTab);

    if (g_tabs.empty()) {
        g_currentTab = -1;
        SetWindowTextA(g_urlEdit, "");
        clearContent();
        return;
    }

    int newIndex = g_currentTab;
    if (newIndex >= static_cast<int>(g_tabs.size())) {
        newIndex = static_cast<int>(g_tabs.size()) - 1;
    }
    TabCtrl_SetCurSel(g_tabCtrl, newIndex);
    selectTab(newIndex);
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
    int btnW = 70;
    int addW = 80;
    int closeW = 90;

    MoveWindow(g_tabCtrl, pad, pad, rc.right - pad * 2, 28, TRUE);
    MoveWindow(g_urlEdit, pad, 40, rc.right - pad * 4 - btnW - addW - closeW, 24, TRUE);
    MoveWindow(g_goBtn, rc.right - pad * 3 - addW - closeW - btnW, 40, btnW, 24, TRUE);
    MoveWindow(g_addTabBtn, rc.right - pad * 2 - closeW - addW, 40, addW, 24, TRUE);
    MoveWindow(g_closeTabBtn, rc.right - pad - closeW, 40, closeW, 24, TRUE);
    MoveWindow(g_contentWnd, pad, topBarH, rc.right - pad * 2, rc.bottom - topBarH - pad, TRUE);
}

static void drawContent(HDC hdc, const RECT& rc) {
    int x = rc.left + 10;
    int layoutY = 10;
    int maxW = rc.right - rc.left - 20;

    g_contentHeight = 0;
    for (const auto& line : g_renderLines) {
        layoutY += line.spaceBefore;

        LOGFONT lf{};
        lf.lfCharSet = DEFAULT_CHARSET;
        lf.lfHeight = -line.size;
        lf.lfWeight = line.bold ? FW_BOLD : FW_NORMAL;
        HFONT font = CreateFontIndirect(&lf);
        HFONT oldFont = (HFONT)SelectObject(hdc, font);

        RECT calcRc{ x, layoutY, x + maxW, rc.bottom };
        DrawTextA(hdc, line.text.c_str(), -1, &calcRc, DT_LEFT | DT_TOP | DT_WORDBREAK | DT_CALCRECT | DT_NOPREFIX);
        int drawY = rc.top + layoutY - g_scrollY;
        RECT textRc{ x, drawY, x + maxW, rc.bottom };
        DrawTextA(hdc, line.text.c_str(), -1, &textRc, DT_LEFT | DT_TOP | DT_WORDBREAK | DT_NOPREFIX);

        layoutY = (calcRc.bottom - rc.top) + line.spaceAfter;
        g_contentHeight = layoutY + 10;

        SelectObject(hdc, oldFont);
        DeleteObject(font);
    }
}

static void updateScrollBar(HWND hwnd) {
    RECT rc{};
    GetClientRect(hwnd, &rc);
    int viewH = rc.bottom - rc.top;
    if (viewH < 1) viewH = 1;
    if (g_contentHeight < 0) g_contentHeight = 0;

    SCROLLINFO si{};
    si.cbSize = sizeof(si);
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin = 0;
    si.nMax = (g_contentHeight > 0) ? g_contentHeight : 0;
    si.nPage = viewH;
    int maxPos = si.nMax - (int)si.nPage + 1;
    if (maxPos < 0) maxPos = 0;
    if (g_scrollY > maxPos) g_scrollY = maxPos;
    if (g_scrollY < 0) g_scrollY = 0;
    si.nPos = g_scrollY;
    SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
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
        updateScrollBar(hwnd);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_VSCROLL: {
        SCROLLINFO si{};
        si.cbSize = sizeof(si);
        si.fMask = SIF_ALL;
        GetScrollInfo(hwnd, SB_VERT, &si);
        int pos = si.nPos;
        int page = si.nPage;

        switch (LOWORD(wParam)) {
        case SB_LINEUP: pos -= 20; break;
        case SB_LINEDOWN: pos += 20; break;
        case SB_PAGEUP: pos -= page; break;
        case SB_PAGEDOWN: pos += page; break;
        case SB_THUMBTRACK: pos = si.nTrackPos; break;
        default: break;
        }

        if (pos < 0) pos = 0;
        int maxPos = si.nMax - (int)si.nPage + 1;
        if (maxPos < 0) maxPos = 0;
        if (pos > maxPos) pos = maxPos;

        if (pos != g_scrollY) {
            g_scrollY = pos;
            SetScrollPos(hwnd, SB_VERT, g_scrollY, TRUE);
            InvalidateRect(hwnd, nullptr, TRUE);
        }
        return 0;
    }
    case WM_MOUSEWHEEL: {
        int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        int step = (delta > 0) ? -60 : 60;
        g_scrollY += step;

        SCROLLINFO si{};
        si.cbSize = sizeof(si);
        si.fMask = SIF_ALL;
        GetScrollInfo(hwnd, SB_VERT, &si);
        int maxPos = si.nMax - (int)si.nPage + 1;
        if (maxPos < 0) maxPos = 0;
        if (g_scrollY < 0) g_scrollY = 0;
        if (g_scrollY > maxPos) g_scrollY = maxPos;

        SetScrollPos(hwnd, SB_VERT, g_scrollY, TRUE);
        InvalidateRect(hwnd, nullptr, TRUE);
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

        g_closeTabBtn = CreateWindowExA(0, "BUTTON", "Close Tab",
            WS_CHILD | WS_VISIBLE,
            0, 0, 0, 0, hwnd, (HMENU)4, nullptr, nullptr);

        g_contentWnd = CreateWindowExA(WS_EX_CLIENTEDGE, "STATIC", "",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_CLIPSIBLINGS,
            0, 0, 0, 0, hwnd, nullptr, nullptr, nullptr);
        SetWindowLongPtrA(g_contentWnd, GWLP_WNDPROC, (LONG_PTR)ContentProc);

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
            addTab("");
            return 0;
        }
        if (LOWORD(wParam) == 4) {
            closeCurrentTab();
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
    case WM_MOUSEWHEEL: {
        POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        HWND target = WindowFromPoint(pt);
        if (target == g_contentWnd || IsChild(g_contentWnd, target)) {
            ScreenToClient(g_contentWnd, &pt);
            LPARAM childPos = MAKELPARAM(pt.x, pt.y);
            SendMessageA(g_contentWnd, WM_MOUSEWHEEL, wParam, childPos);
        }
        return 0;
    }
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
