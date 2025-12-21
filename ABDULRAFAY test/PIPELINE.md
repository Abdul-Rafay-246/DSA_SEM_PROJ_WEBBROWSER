# ABDULRAFAY Test Pipeline

This folder uses the Win32 renderer in `render.cpp` and a simple
line-based render format written by `html_parser.cpp`.

## Render format (page.txt)
- `TITLE: <text>`
- `H1: <text>`
- `H2: <text>`
- `H3: <text>`
- `P: <text>`

Inline markers in text:
- `**bold**` from `<strong>` / `<b>`
- `*italic*` from `<em>` / `<i>`

## Build (example with MinGW g++)
From `ABDULRAFAY test`:

```bash
g++ html_parser.cpp -o html_parser.exe
g++ render.cpp -o render.exe -lgdi32
g++ run_pipeline.cpp -o run_pipeline.exe
```

## Run
Parse a local HTML file:

```bash
html_parser.exe test1.html page.txt --debug parsed_output.txt
render.exe
```

Fetch -> parse -> render (default URL):

```bash
run_pipeline.exe
```

Fetch -> parse -> render (custom URL):

```bash
run_pipeline.exe "https://example.com"
```
