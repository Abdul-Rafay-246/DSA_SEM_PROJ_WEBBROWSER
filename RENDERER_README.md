# HTML Renderer - DSA Demonstration

A comprehensive HTML renderer implementation that heavily demonstrates Data Structures and Algorithms (DSA) concepts. HTML rendering serves as the application domain, but the primary focus is on showcasing DSA structures and operations.

## Features

### Data Structures Implemented

1. **Tree (General Tree)**
   - DOM tree structure for HTML elements
   - Pre-order, in-order, post-order, and level-order traversals
   - Tree height and size calculations
   - Tree construction and manipulation

2. **Linked List**
   - Sibling node representation
   - Advanced operations: reverse, merge, split
   - Cycle detection (Floyd's algorithm)
   - Finding middle node (fast pointer technique)
   - Nth node from end (two-pointer approach)

3. **Hash Table**
   - Style property storage
   - Collision handling with chaining
   - Dynamic rehashing when load factor exceeds 0.75
   - Load factor and collision statistics

4. **AVL Tree**
   - Rendering order management
   - All rotation types (LL, RR, LR, RL)
   - Automatic balancing
   - Range queries
   - Successor/predecessor operations

5. **Graph**
   - Element relationship representation
   - BFS (Breadth-First Search)
   - DFS (Depth-First Search)
   - Topological sort
   - Cycle detection
   - Shortest path calculation

6. **BST (Binary Search Tree)**
   - Spatial organization by y-position
   - Range queries for visible elements
   - In-order traversal for sorted output
   - Tree validation

## Dependencies

- **SDL2** - For window and rendering
- **SDL2_ttf** - For text rendering (optional, falls back gracefully)
- **C++11** or later compiler

### Installing Dependencies

**macOS:**
```bash
brew install sdl2 sdl2_ttf
```

**Linux (Ubuntu/Debian):**
```bash
sudo apt-get install libsdl2-dev libsdl2-ttf-dev
```

**Linux (Fedora):**
```bash
sudo dnf install SDL2-devel SDL2_ttf-devel
```

## Compilation

### Using Makefile:
```bash
make
```

### Manual Compilation:
```bash
g++ -std=c++11 -Wall html_renderer.cpp -o html_renderer -lSDL2 -lSDL2_ttf
```

On macOS with Homebrew:
```bash
g++ -std=c++11 -Wall -I/opt/homebrew/include -L/opt/homebrew/lib html_renderer.cpp -o html_renderer -lSDL2 -lSDL2_ttf
```

## Usage

### Basic Usage:
```bash
./html_renderer parsed_output.txt
```

### With Custom File:
```bash
./html_renderer <path_to_parsed_html_file>
```

## Input Format

The renderer expects a parsed HTML file in the following format:
```
TAG:html
  TAG:head
    TAG:title
      TEXT:Page Title
  TAG:body
    TAG:h1
      TEXT:Heading
    TAG:p
      TEXT:Paragraph text
```

- `TAG:` prefix indicates an HTML tag
- `TEXT:` prefix indicates text content
- Indentation (2 spaces) indicates hierarchy

## Output

### Console Output:
- DSA Statistics (Hash table load factors, AVL rotations, BST height, etc.)
- Tree traversal demonstrations (pre-order, in-order, post-order, level-order)
- Graph algorithm results
- Linked list operation counts

### Visual Output:
- SDL2 window displaying rendered HTML
- Simple text rendering with proper layout
- Element positioning based on DSA calculations

## DSA Demonstrations

### 1. Tree Traversals
All four traversal types are demonstrated:
- **Pre-order**: Used for parsing and initial processing
- **In-order**: Used for specific layout calculations
- **Post-order**: Used for style application (bottom-up)
- **Level-order (BFS)**: Used for rendering queue building

### 2. Hash Table Operations
- Insertion with collision handling
- Dynamic rehashing demonstration
- Load factor monitoring
- Collision statistics

### 3. AVL Tree Operations
- Insertion with automatic balancing
- Rotation demonstrations (all 4 types)
- Balance factor tracking
- Range queries for rendering order

### 4. Graph Algorithms
- **BFS**: Used for level-order layout calculation
- **DFS**: Used for depth-based calculations
- **Topological Sort**: For dependency resolution
- **Cycle Detection**: Validates DOM structure

### 5. BST Operations
- Spatial indexing by y-position
- Range queries for viewport culling
- In-order traversal for sorted rendering
- Tree validation

### 6. Linked List Operations
- Reverse operation
- Merge operation
- Split operation
- Cycle detection
- Middle node finding
- Nth from end

## Code Structure

```
html_renderer.cpp
├── String Utilities
├── Enhanced Linked List (with advanced operations)
├── Stack
├── Queue
├── Enhanced Hash Table (with rehashing)
├── Enhanced AVL Tree (with rotations)
├── BST (spatial indexing)
├── Enhanced Graph (with BFS, DFS, topological sort)
├── RenderNode Structure (DOM tree node)
├── HTMLRenderer Class
│   ├── Parsing (Tree building with Stack)
│   ├── Style System (HashTable operations)
│   ├── Layout Calculation (Graph algorithms)
│   ├── Spatial Indexing (BST operations)
│   ├── Render Order (AVL Tree operations)
│   ├── Render Queue (Linked List operations)
│   └── DSA Statistics & Demonstrations
└── SDL2 Rendering (Simple implementation)
```

## Example Output

```
HTML Renderer - DSA Demonstration
===================================
Loading file: parsed_output.txt

File loaded successfully!

========== DSA STATISTICS ==========

--- Hash Table Statistics ---
Style Registry:
  Size: 12
  Load Factor: 0.24
  Collisions: 2

--- AVL Tree Statistics ---
Render Order Tree:
  Height: 4
  Balance Factor: 0
  Rotations: 3

--- BST Statistics ---
Spatial Index:
  Height: 5
  Valid BST: Yes

--- Graph Statistics ---
Element Graph:
  Vertices: 15
  Has Cycle: No

--- DOM Tree Statistics ---
  Height: 6
  Size: 15

--- Linked List Statistics ---
  Operations Count: 45

=====================================

========== TREE TRAVERSAL DEMONSTRATIONS ==========
...
```

## Notes

- The renderer focuses heavily on DSA demonstrations
- HTML rendering is simplified to showcase DSA operations
- All DSA operations are clearly commented in the code
- Statistics are printed to console for analysis
- The visual rendering window demonstrates the practical application

## Troubleshooting

### SDL2 Not Found:
- Ensure SDL2 is installed (see Dependencies section)
- Check include/library paths in compilation command
- On macOS, use Homebrew paths: `-I/opt/homebrew/include -L/opt/homebrew/lib`

### Font Loading Issues:
- The renderer will continue without fonts (basic rendering)
- Font paths are hardcoded for macOS/Linux - modify if needed

### Compilation Errors:
- Ensure C++11 or later is used (`-std=c++11`)
- Check all dependencies are installed
- Verify SDL2 library paths

## Author

DSA Semester Project - HTML Renderer Implementation

