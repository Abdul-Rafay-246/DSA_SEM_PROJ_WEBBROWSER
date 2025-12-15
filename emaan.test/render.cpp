#include <windows.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

// ----------------------
// Data structures
// ----------------------

enum class NodeType
{
    H1,
    H2,
    Paragraph
};

struct Node
{
    NodeType type;
    std::string text;

    // Layout
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

std::vector<Node> g_nodes;

// ----------------------
// Parsing: txt -> nodes
// ----------------------

void loadPageFromFile(const std::string& filename)
{
    g_nodes.clear();

    std::ifstream in(filename);
    if (!in.is_open())
        return;

    std::string line;
    while (std::getline(in, line))
    {
        // trim spaces
        while (!line.empty() && (line.back() == '\r' || line.back() == '\n' || line.back() == ' '))
            line.pop_back();

        if (line.empty())
            continue;

        Node node;

        if (line.rfind("H1:", 0) == 0)
        {
            node.type = NodeType::H1;
            node.text = line.substr(3);
        }
        else if (line.rfind("H2:", 0) == 0)
        {
            node.type = NodeType::H2;
            node.text = line.substr(3);
        }
        else if (line.rfind("P:", 0) == 0)
        {
            node.type = NodeType::Paragraph;
            node.text = line.substr(2);
        }
        else
        {
            // fallback: treat as paragraph if no prefix
            node.type = NodeType::Paragraph;
            node.text = line;
        }

        // remove leading space after prefix
        if (!node.text.empty() && node.text[0] == ' ')
            node.text.erase(node.text.begin());

        g_nodes.push_back(node);
    }
}

// ----------------------
// Layout: assign x,y,w,h
// ----------------------

void layoutNodes(int windowWidth)
{
    int marginLeft = 20;
    int marginRight = 20;
    int currentY = 20;
    int spacing = 10;

    int contentWidth = windowWidth - marginLeft - marginRight;
    if (contentWidth < 50)
        contentWidth = 50;

    for (auto& node : g_nodes)
    {
        node.x = marginLeft;
        node.y = currentY;
        node.width = contentWidth;

        // Fixed heights per type (simple)
        switch (node.type)
        {
        case NodeType::H1:
            node.height = 40;
            break;
        case NodeType::H2:
            node.height = 30;
            break;
        case NodeType::Paragraph:
        default:
            node.height = 24;
            break;
        }

        currentY += node.height + spacing;
    }
}

// ----------------------
// Rendering
// ----------------------

void drawNode(HDC hdc, const Node& node)
{
    // Background rectangle (optional)
    HBRUSH brush;
    switch (node.type)
    {
    case NodeType::H1:
        brush = CreateSolidBrush(RGB(220, 220, 255));
        break;
    case NodeType::H2:
        brush = CreateSolidBrush(RGB(230, 230, 255));
        break;
    case NodeType::Paragraph:
    default:
        brush = CreateSolidBrush(RGB(255, 255, 255));
        break;
    }

    RECT rect{ node.x, node.y, node.x + node.width, node.y + node.height };
    FillRect(hdc, &rect, brush);
    DeleteObject(brush);

    // Text settings based on type
    HFONT hFont = nullptr;
    LOGFONT lf{};
    lf.lfCharSet = DEFAULT_CHARSET;
    lf.lfHeight = (node.type == NodeType::H1) ? -24 :
                  (node.type == NodeType::H2) ? -18 : -16;
    lf.lfWeight = (node.type == NodeType::H1) ? FW_BOLD :
                  (node.type == NodeType::H2) ? FW_BOLD : FW_NORMAL;

    hFont = CreateFontIndirect(&lf);
    HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));

    RECT textRect = rect;
    textRect.left += 5;
    textRect.top += 4;

    DrawTextA(hdc, node.text.c_str(), -1, &textRect,
              DT_LEFT | DT_TOP | DT_WORDBREAK);

    SelectObject(hdc, oldFont);
    DeleteObject(hFont);

    // Optional border
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
    HPEN oldPen = (HPEN)SelectObject(hdc, pen);
    SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
    Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

void renderPage(HDC hdc)
{
    for (const auto& node : g_nodes)
    {
        drawNode(hdc, node);
    }
}

// ----------------------
// Win32 boilerplate
// ----------------------

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_SIZE:
    {
        int width = LOWORD(lParam);
        layoutNodes(width);
        InvalidateRect(hwnd, nullptr, TRUE);
        return 0;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        renderPage(hdc);
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    // 1. Load and parse the txt file
    loadPageFromFile("page.txt");

    // 2. Register window class
    const char CLASS_NAME[] = "SimpleHtmlRendererWindow";

    WNDCLASSA wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClassA(&wc);

    // 3. Create window
    HWND hwnd = CreateWindowExA(
        0,
        CLASS_NAME,
        "Simple HTML Renderer",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!hwnd)
        return 0;

    ShowWindow(hwnd, nCmdShow);

    // Initial layout based on initial size
    RECT rc;
    GetClientRect(hwnd, &rc);
    layoutNodes(rc.right - rc.left);

    // 4. Message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}