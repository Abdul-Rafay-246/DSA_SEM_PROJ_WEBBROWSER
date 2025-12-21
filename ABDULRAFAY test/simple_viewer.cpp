#include <windows.h>
#include <string>
#include <vector>
#include <fstream>

struct Line {
    std::string text;
    int size;
    bool bold;
};

std::vector<Line> g_lines;

static bool startsWith(const std::string& s, const char* prefix) {
    return s.rfind(prefix, 0) == 0;
}

void loadLines(const std::string& filename) {
    g_lines.clear();
    std::ifstream in(filename);
    if (!in.is_open()) return;

    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) continue;

        Line out{};
        out.size = 16;
        out.bold = false;
        out.text = line;

        if (startsWith(line, "TITLE:")) {
            continue;
        } else if (startsWith(line, "H1:")) {
            out.size = 24;
            out.bold = true;
            out.text = line.substr(3);
        } else if (startsWith(line, "H2:")) {
            out.size = 20;
            out.bold = true;
            out.text = line.substr(3);
        } else if (startsWith(line, "H3:")) {
            out.size = 18;
            out.bold = true;
            out.text = line.substr(3);
        } else if (startsWith(line, "P:")) {
            out.size = 16;
            out.bold = false;
            out.text = line.substr(2);
        }

        if (!out.text.empty() && out.text[0] == ' ') {
            out.text.erase(out.text.begin());
        }

        g_lines.push_back(out);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        int y = 20;
        for (const auto& line : g_lines) {
            LOGFONT lf{};
            lf.lfCharSet = DEFAULT_CHARSET;
            lf.lfHeight = -line.size;
            lf.lfWeight = line.bold ? FW_BOLD : FW_NORMAL;

            HFONT font = CreateFontIndirect(&lf);
            HFONT oldFont = (HFONT)SelectObject(hdc, font);

            RECT rect{ 20, y, ps.rcPaint.right - 20, y + line.size + 12 };
            DrawTextA(hdc, line.text.c_str(), -1, &rect, DT_LEFT | DT_TOP | DT_WORDBREAK);

            SelectObject(hdc, oldFont);
            DeleteObject(font);

            y += line.size + 12;
        }

        EndPaint(hwnd, &ps);
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
    loadLines("page.txt");

    const char CLASS_NAME[] = "SimpleViewerWindow";

    WNDCLASSA wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassA(&wc);

    HWND hwnd = CreateWindowExA(
        0,
        CLASS_NAME,
        "Parsed Page Viewer",
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
