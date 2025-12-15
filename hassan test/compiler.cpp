#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include <fstream>

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
    int x, y, w, h;
};

std::vector<Node> g_nodes;

// ----------------------
// Parsing
// ----------------------

void loadPageFromFile(const std::string& filename)
{
    g_nodes.clear();
    std::ifstream in(filename);
    if (!in) return;

    std::string line;
    while (std::getline(in, line))
    {
        if (line.empty()) continue;

        Node n{};
        if (line.rfind("H1:", 0) == 0)
        {
            n.type = NodeType::H1;
            n.text = line.substr(3);
        }
        else if (line.rfind("H2:", 0) == 0)
        {
            n.type = NodeType::H2;
            n.text = line.substr(3);
        }
        else
        {
            n.type = NodeType::Paragraph;
            n.text = line;
        }

        if (!n.text.empty() && n.text[0] == ' ')
            n.text.erase(0, 1);

        g_nodes.push_back(n);
    }
}

// ----------------------
// Layout
// ----------------------

void layoutNodes(int width)
{
    int y = 20;
    for (auto& n : g_nodes)
    {
        n.x = 20;
        n.y = y;
        n.w = width - 40;

        if (n.type == NodeType::H1) n.h = 40;
        else if (n.type == NodeType::H2) n.h = 30;
        else n.h = 24;

        y += n.h + 10;
    }
}

// ----------------------
// Rendering helpers
// ----------------------

TTF_Font* fontH1;
TTF_Font* fontH2;
TTF_Font* fontP;

void drawText(SDL_Renderer* r, TTF_Font* font,
              const std::string& text, SDL_Rect rect)
{
    SDL_Color black{0,0,0,255};
    SDL_Surface* surf = TTF_RenderUTF8_Blended_Wrapped(
        font, text.c_str(), black, rect.w);

    SDL_Texture* tex = SDL_CreateTextureFromSurface(r, surf);
    SDL_Rect dst{rect.x, rect.y, surf->w, surf->h};

    SDL_RenderCopy(r, tex, nullptr, &dst);
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

// ----------------------
// Render page
// ----------------------

void renderPage(SDL_Renderer* r)
{
    for (auto& n : g_nodes)
    {
        SDL_Rect rect{n.x, n.y, n.w, n.h};

        if (n.type == NodeType::H1)
            SDL_SetRenderDrawColor(r, 220,220,255,255);
        else if (n.type == NodeType::H2)
            SDL_SetRenderDrawColor(r, 230,230,255,255);
        else
            SDL_SetRenderDrawColor(r, 255,255,255,255);

        SDL_RenderFillRect(r, &rect);

        if (n.type == NodeType::H1)
            drawText(r, fontH1, n.text, rect);
        else if (n.type == NodeType::H2)
            drawText(r, fontH2, n.text, rect);
        else
            drawText(r, fontP, n.text, rect);
    }
}

// ----------------------
// Main
// ----------------------

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* win = SDL_CreateWindow(
        "Neutral Renderer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        SDL_WINDOW_RESIZABLE
    );

    SDL_Renderer* renderer = SDL_CreateRenderer(
        win, -1, SDL_RENDERER_ACCELERATED);

    fontH1 = TTF_OpenFont("Arial.ttf", 24);
    fontH2 = TTF_OpenFont("Arial.ttf", 18);
    fontP  = TTF_OpenFont("Arial.ttf", 16);

    loadPageFromFile("parsed.txt");
    layoutNodes(800);

    bool running = true;
    while (running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_WINDOWEVENT &&
                e.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                layoutNodes(e.window.data1);
            }
        }

        SDL_SetRenderDrawColor(renderer, 255,255,255,255);
        SDL_RenderClear(renderer);

        renderPage(renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_Quit();
    return 0;
}
