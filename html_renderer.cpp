#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include <stdexcept>
#include <cstdio>
#include <climits>
#include <string>

// SDL2 is optional - uncomment the next line if SDL2 is installed
// The program will still run and show all DSA demonstrations via console output
// #define USE_SDL2

#ifdef USE_SDL2
    // SDL2 includes - adjust paths as needed for your system
    #ifdef __APPLE__
        // Try common macOS paths
        #include <SDL2/SDL.h>
        #include <SDL2/SDL_ttf.h>
    #else
        // Linux/other platforms
        #include <SDL2/SDL.h>
        #include <SDL2/SDL_ttf.h>
    #endif
#endif

// ============================================================================
// STRING UTILITY FUNCTIONS
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
// ENHANCED LINKED LIST CLASS (with advanced operations)
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
    
    void insertAt(int position, T data) {
        if (position < 0 || position > size) return;
        if (position == 0) {
            insertFront(data);
            return;
        }
        Node* newNode = new Node(data);
        Node* current = head;
        for (int i = 0; i < position - 1; i++) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
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
    
    // DSA: Reverse linked list operation
    void reverse() {
        Node* prev = nullptr;
        Node* current = head;
        Node* next = nullptr;
        while (current) {
            next = current->next;
            current->next = prev;
            prev = current;
            current = next;
        }
        head = prev;
    }
    
    // DSA: Merge two sorted linked lists
    void merge(LinkedList<T>* other) {
        if (!other || !other->head) return;
        Node* current = head;
        while (current && current->next) {
            current = current->next;
        }
        if (current) {
            current->next = other->head;
        } else {
            head = other->head;
        }
        size += other->size;
        other->head = nullptr;
        other->size = 0;
    }
    
    // DSA: Split list at position
    LinkedList<T>* split(int position) {
        if (position < 0 || position >= size) return nullptr;
        LinkedList<T>* newList = new LinkedList<T>();
        if (position == 0) {
            newList->head = head;
            head = nullptr;
            newList->size = size;
            size = 0;
            return newList;
        }
        Node* current = head;
        for (int i = 0; i < position - 1; i++) {
            current = current->next;
        }
        newList->head = current->next;
        current->next = nullptr;
        newList->size = size - position;
        size = position;
        return newList;
    }
    
    // DSA: Find middle node using fast pointer technique
    T findMiddle() {
        if (!head) return T();
        Node* slow = head;
        Node* fast = head;
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
        }
        return slow->data;
    }
    
    // DSA: Detect cycle using Floyd's algorithm
    bool detectCycle() {
        if (!head) return false;
        Node* slow = head;
        Node* fast = head;
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
            if (slow == fast) return true;
        }
        return false;
    }
    
    // DSA: Get Nth node from end
    T getNthFromEnd(int n) {
        if (n <= 0 || !head) return T();
        Node* first = head;
        Node* second = head;
        for (int i = 0; i < n; i++) {
            if (!first) return T();
            first = first->next;
        }
        while (first) {
            first = first->next;
            second = second->next;
        }
        return second->data;
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
    
    // Iterator-like access
    T getAt(int index) {
        if (index < 0 || index >= size) return T();
        Node* current = head;
        for (int i = 0; i < index; i++) {
            current = current->next;
        }
        return current->data;
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
// ENHANCED HASH TABLE CLASS (with rehashing and statistics)
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
    
    int TABLE_SIZE;
    HashNode** buckets;
    int size;
    int collisionCount;

    int hashFunction(const char* key) {
        int hash = 0;
        for (int i = 0; key[i]; i++) {
            hash = (hash * 31 + key[i]) % TABLE_SIZE;
        }
        return hash;
    }
    
    void rehash() {
        int oldSize = TABLE_SIZE;
        TABLE_SIZE = TABLE_SIZE * 2 + 1; // Make it odd
        HashNode** oldBuckets = buckets;
        buckets = new HashNode*[TABLE_SIZE];
        for (int i = 0; i < TABLE_SIZE; i++) {
            buckets[i] = nullptr;
        }
        
        size = 0;
        collisionCount = 0;
        
        for (int i = 0; i < oldSize; i++) {
            HashNode* current = oldBuckets[i];
            while (current) {
                HashNode* next = current->next;
                int index = hashFunction(current->key);
                if (buckets[index]) collisionCount++;
                current->next = buckets[index];
                buckets[index] = current;
                size++;
                current = next;
            }
        }
        
        delete[] oldBuckets;
    }

public:
    HashTable(int tableSize = 101) : TABLE_SIZE(tableSize), size(0), collisionCount(0) {
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
        // DSA: Rehashing when load factor exceeds 0.75
        double loadFactor = (double)size / TABLE_SIZE;
        if (loadFactor > 0.75) {
            rehash();
        }
        
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
        if (buckets[index]) collisionCount++;
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
    
    // DSA: Get load factor
    double getLoadFactor() const {
        return (double)size / TABLE_SIZE;
    }
    
    // DSA: Get collision count
    int getCollisionCount() const {
        return collisionCount;
    }
    
    // DSA: Get all keys
    void getAllKeys(LinkedList<char*>* keys) {
        for (int i = 0; i < TABLE_SIZE; i++) {
            HashNode* current = buckets[i];
            while (current) {
                keys->insert(copyString(current->key));
                current = current->next;
            }
        }
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
        collisionCount = 0;
    }
};

// ============================================================================
// ENHANCED AVL TREE CLASS (with range queries and balance tracking)
// ============================================================================

class AVLTree {
private:
    struct AVLNode {
        int key;
        void* data;
        AVLNode* left;
        AVLNode* right;
        int height;
        
        AVLNode(int k, void* d) {
            key = k;
            data = d;
            left = right = nullptr;
            height = 1;
        }
    };
    
    AVLNode* root;
    int rotationCount;
    
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
    
    // DSA: Right rotation (RR case)
    AVLNode* rotateRight(AVLNode* y) {
        if (!y || !y->left) return y;
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;
        
        x->right = y;
        y->left = T2;
        
        updateHeight(y);
        updateHeight(x);
        
        rotationCount++;
        return x;
    }
    
    // DSA: Left rotation (LL case)
    AVLNode* rotateLeft(AVLNode* x) {
        if (!x || !x->right) return x;
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;
        
        y->left = x;
        x->right = T2;
        
        updateHeight(x);
        updateHeight(y);
        
        rotationCount++;
        return y;
    }
    
    AVLNode* insertHelper(AVLNode* node, int key, void* data) {
        if (!node) {
            return new AVLNode(key, data);
        }
        
        if (key < node->key) {
            node->left = insertHelper(node->left, key, data);
        } else if (key > node->key) {
            node->right = insertHelper(node->right, key, data);
        } else {
            return node; // Duplicate
        }
        
        updateHeight(node);
        
        int balance = getBalance(node);
        
        // DSA: Left Left Case
        if (balance > 1 && node->left && key < node->left->key) {
            return rotateRight(node);
        }
        
        // DSA: Right Right Case
        if (balance < -1 && node->right && key > node->right->key) {
            return rotateLeft(node);
        }
        
        // DSA: Left Right Case
        if (balance > 1 && node->left && key > node->left->key) {
            node->left = rotateLeft(node->left);
            return rotateRight(node);
        }
        
        // DSA: Right Left Case
        if (balance < -1 && node->right && key < node->right->key) {
            node->right = rotateRight(node->right);
            return rotateLeft(node);
        }
        
        return node;
    }
    
    void inOrderHelper(AVLNode* node, LinkedList<void*>* result) {
        if (node) {
            inOrderHelper(node->left, result);
            result->insert(node->data);
            inOrderHelper(node->right, result);
        }
    }
    
    void rangeQueryHelper(AVLNode* node, int minKey, int maxKey, LinkedList<void*>* result) {
        if (!node) return;
        
        if (node->key > minKey) {
            rangeQueryHelper(node->left, minKey, maxKey, result);
        }
        
        if (node->key >= minKey && node->key <= maxKey) {
            result->insert(node->data);
        }
        
        if (node->key < maxKey) {
            rangeQueryHelper(node->right, minKey, maxKey, result);
        }
    }
    
    AVLNode* findMin(AVLNode* node) {
        while (node && node->left) {
            node = node->left;
        }
        return node;
    }
    
    AVLNode* findMax(AVLNode* node) {
        while (node && node->right) {
            node = node->right;
        }
        return node;
    }
    
    void* getSuccessorHelper(AVLNode* root, int key, AVLNode* successor) {
        if (!root) return successor ? successor->data : nullptr;
        
        if (root->key > key) {
            return getSuccessorHelper(root->left, key, root);
        } else {
            return getSuccessorHelper(root->right, key, successor);
        }
    }
    
    void* getPredecessorHelper(AVLNode* root, int key, AVLNode* predecessor) {
        if (!root) return predecessor ? predecessor->data : nullptr;
        
        if (root->key < key) {
            return getPredecessorHelper(root->right, key, root);
        } else {
            return getPredecessorHelper(root->left, key, predecessor);
        }
    }
    
    void clearHelper(AVLNode* node) {
        if (node) {
            clearHelper(node->left);
            clearHelper(node->right);
            delete node;
        }
    }

public:
    AVLTree() : root(nullptr), rotationCount(0) {}
    
    ~AVLTree() {
        clear();
    }
    
    void insert(int key, void* data) {
        root = insertHelper(root, key, data);
    }
    
    bool search(int key) {
        AVLNode* current = root;
        while (current) {
            if (key == current->key) return true;
            if (key < current->key) current = current->left;
            else current = current->right;
        }
        return false;
    }
    
    // DSA: In-order traversal for sorted output
    void inOrderTraversal(LinkedList<void*>* result) {
        inOrderHelper(root, result);
    }
    
    // DSA: Range query
    void rangeQuery(int minKey, int maxKey, LinkedList<void*>* result) {
        rangeQueryHelper(root, minKey, maxKey, result);
    }
    
    // DSA: Get successor
    void* getSuccessor(int key) {
        return getSuccessorHelper(root, key, nullptr);
    }
    
    // DSA: Get predecessor
    void* getPredecessor(int key) {
        return getPredecessorHelper(root, key, nullptr);
    }
    
    // DSA: Get balance factor of root
    int getBalanceFactor() {
        return getBalance(root);
    }
    
    int getHeight() {
        return getHeight(root);
    }
    
    int getRotationCount() const {
        return rotationCount;
    }
    
    void clear() {
        clearHelper(root);
        root = nullptr;
        rotationCount = 0;
    }
};

// ============================================================================
// BST CLASS (Binary Search Tree for spatial organization)
// ============================================================================

class BST {
private:
    struct BSTNode {
        int key;           // y-position
        void* data;        // RenderNode reference
        BSTNode* left;
        BSTNode* right;
        
        BSTNode(int k, void* d) : key(k), data(d), left(nullptr), right(nullptr) {}
    };
    
    BSTNode* root;
    
    BSTNode* insertHelper(BSTNode* node, int key, void* data) {
        if (!node) {
            return new BSTNode(key, data);
        }
        
        if (key < node->key) {
            node->left = insertHelper(node->left, key, data);
        } else if (key > node->key) {
            node->right = insertHelper(node->right, key, data);
        } else {
            // Duplicate key - allow multiple elements at same y-position
            BSTNode* newNode = new BSTNode(key, data);
            newNode->left = node->left;
            node->left = newNode;
        }
        
        return node;
    }
    
    BSTNode* removeHelper(BSTNode* node, int key) {
        if (!node) return nullptr;
        
        if (key < node->key) {
            node->left = removeHelper(node->left, key);
        } else if (key > node->key) {
            node->right = removeHelper(node->right, key);
        } else {
            if (!node->left) {
                BSTNode* temp = node->right;
                delete node;
                return temp;
            } else if (!node->right) {
                BSTNode* temp = node->left;
                delete node;
                return temp;
            }
            
            BSTNode* temp = findMin(node->right);
            node->key = temp->key;
            node->data = temp->data;
            node->right = removeHelper(node->right, temp->key);
        }
        
        return node;
    }
    
    BSTNode* searchHelper(BSTNode* node, int key) {
        if (!node || node->key == key) return node;
        if (key < node->key) return searchHelper(node->left, key);
        return searchHelper(node->right, key);
    }
    
    void inOrderHelper(BSTNode* node, LinkedList<void*>* result) {
        if (node) {
            inOrderHelper(node->left, result);
            result->insert(node->data);
            inOrderHelper(node->right, result);
        }
    }
    
    // DSA: Range query for visible elements
    void rangeQueryHelper(BSTNode* node, int minY, int maxY, LinkedList<void*>* result) {
        if (!node) return;
        
        if (node->key > minY) {
            rangeQueryHelper(node->left, minY, maxY, result);
        }
        
        if (node->key >= minY && node->key <= maxY) {
            result->insert(node->data);
        }
        
        if (node->key < maxY) {
            rangeQueryHelper(node->right, minY, maxY, result);
        }
    }
    
    BSTNode* findMin(BSTNode* node) {
        while (node && node->left) {
            node = node->left;
        }
        return node;
    }
    
    BSTNode* findMax(BSTNode* node) {
        while (node && node->right) {
            node = node->right;
        }
        return node;
    }
    
    int getHeightHelper(BSTNode* node) {
        if (!node) return 0;
        int leftHeight = getHeightHelper(node->left);
        int rightHeight = getHeightHelper(node->right);
        return (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;
    }
    
    bool validateBSTHelper(BSTNode* node, int min, int max) {
        if (!node) return true;
        if (node->key < min || node->key > max) return false;
        return validateBSTHelper(node->left, min, node->key) &&
               validateBSTHelper(node->right, node->key, max);
    }
    
    void clearHelper(BSTNode* node) {
        if (node) {
            clearHelper(node->left);
            clearHelper(node->right);
            delete node;
        }
    }

public:
    BST() : root(nullptr) {}
    
    ~BST() {
        clear();
    }
    
    // DSA: Insert element by y-position
    void insert(int key, void* data) {
        root = insertHelper(root, key, data);
    }
    
    // DSA: Remove element
    void remove(int key) {
        root = removeHelper(root, key);
    }
    
    // DSA: Search for element at position
    void* search(int key) {
        BSTNode* node = searchHelper(root, key);
        return node ? node->data : nullptr;
    }
    
    // DSA: In-order traversal for sorted y-order
    void inOrderTraversal(LinkedList<void*>* result) {
        inOrderHelper(root, result);
    }
    
    // DSA: Range query for visible elements
    void rangeQuery(int minY, int maxY, LinkedList<void*>* result) {
        rangeQueryHelper(root, minY, maxY, result);
    }
    
    // DSA: Find minimum (topmost element)
    void* findMin() {
        BSTNode* node = findMin(root);
        return node ? node->data : nullptr;
    }
    
    // DSA: Find maximum (bottommost element)
    void* findMax() {
        BSTNode* node = findMax(root);
        return node ? node->data : nullptr;
    }
    
    // DSA: Get tree height
    int getHeight() {
        return getHeightHelper(root);
    }
    
    // DSA: Validate BST property
    bool validateBST() {
        return validateBSTHelper(root, INT_MIN, INT_MAX);
    }
    
    void clear() {
        clearHelper(root);
        root = nullptr;
    }
};

// ============================================================================
// ENHANCED GRAPH CLASS (with BFS, DFS, topological sort)
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
    
    void DFSUtil(int v, bool* visited, LinkedList<int>* result) {
        visited[v] = true;
        result->insert(v);
        
        GraphNode* current = adjList[v];
        while (current) {
            if (!visited[current->vertex]) {
                DFSUtil(current->vertex, visited, result);
            }
            current = current->next;
        }
    }
    
    bool hasCycleUtil(int v, bool* visited, bool* recStack) {
        visited[v] = true;
        recStack[v] = true;
        
        GraphNode* current = adjList[v];
        while (current) {
            if (!visited[current->vertex]) {
                if (hasCycleUtil(current->vertex, visited, recStack)) {
                    return true;
                }
            } else if (recStack[current->vertex]) {
                return true;
            }
            current = current->next;
        }
        
        recStack[v] = false;
        return false;
    }
    
    void topologicalSortUtil(int v, bool* visited, Stack<int>* stack) {
        visited[v] = true;
        
        GraphNode* current = adjList[v];
        while (current) {
            if (!visited[current->vertex]) {
                topologicalSortUtil(current->vertex, visited, stack);
            }
            current = current->next;
        }
        
        stack->push(v);
    }

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
    
    // DSA: BFS (Breadth-First Search)
    void BFS(int startVertex, LinkedList<int>* result) {
        bool* visited = new bool[maxVertices];
        for (int i = 0; i < maxVertices; i++) {
            visited[i] = false;
        }
        
        Queue<int>* queue = new Queue<int>();
        visited[startVertex] = true;
        queue->enqueue(startVertex);
        
        while (!queue->isEmpty()) {
            int v = queue->dequeue();
            result->insert(v);
            
            GraphNode* current = adjList[v];
            while (current) {
                if (!visited[current->vertex]) {
                    visited[current->vertex] = true;
                    queue->enqueue(current->vertex);
                }
                current = current->next;
            }
        }
        
        delete queue;
        delete[] visited;
    }
    
    // DSA: DFS (Depth-First Search)
    void DFS(int startVertex, LinkedList<int>* result) {
        bool* visited = new bool[maxVertices];
        for (int i = 0; i < maxVertices; i++) {
            visited[i] = false;
        }
        
        DFSUtil(startVertex, visited, result);
        delete[] visited;
    }
    
    // DSA: Topological Sort
    void topologicalSort(LinkedList<int>* result) {
        bool* visited = new bool[maxVertices];
        for (int i = 0; i < maxVertices; i++) {
            visited[i] = false;
        }
        
        Stack<int>* stack = new Stack<int>();
        
        for (int i = 0; i < numVertices; i++) {
            if (!visited[i]) {
                topologicalSortUtil(i, visited, stack);
            }
        }
        
        while (!stack->isEmpty()) {
            result->insert(stack->pop());
        }
        
        delete stack;
        delete[] visited;
    }
    
    // DSA: Cycle Detection
    bool detectCycle() {
        bool* visited = new bool[maxVertices];
        bool* recStack = new bool[maxVertices];
        for (int i = 0; i < maxVertices; i++) {
            visited[i] = false;
            recStack[i] = false;
        }
        
        for (int i = 0; i < numVertices; i++) {
            if (!visited[i]) {
                if (hasCycleUtil(i, visited, recStack)) {
                    delete[] visited;
                    delete[] recStack;
                    return true;
                }
            }
        }
        
        delete[] visited;
        delete[] recStack;
        return false;
    }
    
    // DSA: Shortest Path (BFS-based)
    int shortestPath(int src, int dest) {
        if (src == dest) return 0;
        
        bool* visited = new bool[maxVertices];
        int* distance = new int[maxVertices];
        for (int i = 0; i < maxVertices; i++) {
            visited[i] = false;
            distance[i] = -1;
        }
        
        Queue<int>* queue = new Queue<int>();
        visited[src] = true;
        distance[src] = 0;
        queue->enqueue(src);
        
        while (!queue->isEmpty()) {
            int v = queue->dequeue();
            
            GraphNode* current = adjList[v];
            while (current) {
                if (!visited[current->vertex]) {
                    visited[current->vertex] = true;
                    distance[current->vertex] = distance[v] + 1;
                    queue->enqueue(current->vertex);
                    
                    if (current->vertex == dest) {
                        int result = distance[dest];
                        delete queue;
                        delete[] visited;
                        delete[] distance;
                        return result;
                    }
                }
                current = current->next;
            }
        }
        
        delete queue;
        delete[] visited;
        delete[] distance;
        return -1; // No path found
    }
    
    int getNumVertices() const {
        return numVertices;
    }
};

// ============================================================================
// RENDER NODE STRUCTURE (DOM Tree Node)
// ============================================================================

struct RenderNode {
    char* tagName;
    char* textContent;
    HashTable* styles;
    HashTable* attributes;
    RenderNode* parent;
    RenderNode* firstChild;
    RenderNode* nextSibling;  // Linked list for siblings
    int depth;
    int x, y, width, height;
    int zIndex;      // For AVL tree
    int nodeId;      // For graph
    bool visited;    // For graph algorithms
    
    RenderNode() {
        tagName = nullptr;
        textContent = nullptr;
        styles = new HashTable();
        attributes = new HashTable();
        parent = nullptr;
        firstChild = nullptr;
        nextSibling = nullptr;
        depth = 0;
        x = y = width = height = 0;
        zIndex = 0;
        nodeId = -1;
        visited = false;
    }
    
    ~RenderNode() {
        delete[] tagName;
        delete[] textContent;
        delete styles;
        delete attributes;
    }
    
    void addChild(RenderNode* child) {
        if (!child) return;
        child->parent = this;
        child->depth = this->depth + 1;
        
        if (!firstChild) {
            firstChild = child;
        } else {
            RenderNode* current = firstChild;
            while (current->nextSibling) {
                current = current->nextSibling;
            }
            current->nextSibling = child;
        }
    }
};

// ============================================================================
// HTML RENDERER CLASS
// ============================================================================

class HTMLRenderer {
private:
    RenderNode* root;
    HashTable* styleRegistry;
    Graph* elementGraph;
    AVLTree* renderOrderTree;
    BST* spatialBST;
    int nodeCounter;
    int linkedListOps;
    
    // DSA: Parse parsed HTML file into DOM tree
    RenderNode* parseParsedHTMLFile(const char* filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return nullptr;
        }
        
        // DSA: Use Stack to track parent nodes during parsing
        Stack<RenderNode*>* parentStack = new Stack<RenderNode*>();
        RenderNode* rootNode = nullptr;
        RenderNode* currentNode = nullptr;
        
        std::string line;
        while (std::getline(file, line)) {
            // Count indentation
            int indent = 0;
            while (indent < line.length() && line[indent] == ' ') {
                indent += 2; // Each level is 2 spaces
            }
            int level = indent / 2;
            
            // Remove indentation
            line = line.substr(indent);
            
            if (line.empty()) continue;
            
            if (line.find("TAG:") == 0) {
                // DSA: Tree node creation
                char* tagName = copyString(line.substr(4).c_str());
                trimString(tagName);
                
                RenderNode* newNode = new RenderNode();
                newNode->tagName = tagName;
                newNode->nodeId = nodeCounter++;
                newNode->depth = level;
                
                // DSA: Stack-based parent tracking
                while (parentStack->getSize() > level) {
                    parentStack->pop();
                }
                
                if (parentStack->isEmpty()) {
                    rootNode = newNode;
                    currentNode = newNode;
                } else {
                    RenderNode* parent = parentStack->peek();
                    parent->addChild(newNode);
                    currentNode = newNode;
                }
                
                parentStack->push(newNode);
                
            } else if (line.find("TEXT:") == 0) {
                if (currentNode) {
                    char* text = copyString(line.substr(5).c_str());
                    trimString(text);
                    if (currentNode->textContent) {
                        // Append text
                        int oldLen = strlen(currentNode->textContent);
                        int newLen = strlen(text);
                        char* combined = new char[oldLen + newLen + 2];
                        strcpy(combined, currentNode->textContent);
                        strcat(combined, " ");
                        strcat(combined, text);
                        delete[] currentNode->textContent;
                        currentNode->textContent = combined;
                        delete[] text;
                    } else {
                        currentNode->textContent = text;
                    }
                }
            }
        }
        
        delete parentStack;
        file.close();
        return rootNode;
    }
    
    // DSA: Initialize style system using HashTable
    void initializeStyleSystem() {
        styleRegistry = new HashTable(50);
        
        // DSA: HashTable insertions for different tag styles
        styleRegistry->insert("h1", "font-size:32;font-weight:bold;color:0,0,0");
        styleRegistry->insert("h2", "font-size:24;font-weight:bold;color:0,0,0");
        styleRegistry->insert("h3", "font-size:20;font-weight:bold;color:0,0,0");
        styleRegistry->insert("h4", "font-size:18;font-weight:bold;color:0,0,0");
        styleRegistry->insert("h5", "font-size:16;font-weight:bold;color:0,0,0");
        styleRegistry->insert("h6", "font-size:14;font-weight:bold;color:0,0,0");
        styleRegistry->insert("p", "font-size:14;font-weight:normal;color:0,0,0");
        styleRegistry->insert("strong", "font-weight:bold");
        styleRegistry->insert("b", "font-weight:bold");
        styleRegistry->insert("em", "font-style:italic");
        styleRegistry->insert("i", "font-style:italic");
        styleRegistry->insert("body", "font-size:14;color:0,0,0");
        styleRegistry->insert("html", "font-size:14;color:0,0,0");
    }
    
    // DSA: Apply styles using post-order traversal + HashTable lookup
    void applyStyles(RenderNode* node) {
        if (!node) return;
        
        // DSA: Post-order traversal (children before parent)
        RenderNode* child = node->firstChild;
        while (child) {
            applyStyles(child);
            child = child->nextSibling;
        }
        
        // DSA: HashTable lookup for style
        if (node->tagName) {
            char* styleStr = styleRegistry->get(node->tagName);
            if (styleStr) {
                // Parse and apply styles (simplified)
                // In real implementation, would parse styleStr properly
            }
        }
    }
    
    // DSA: Calculate layout using Graph algorithms (BFS, DFS)
    void calculateLayoutInternal(RenderNode* rootNode, int windowWidth) {
        if (!rootNode) return;
        
        // DSA: Build Graph of element relationships
        elementGraph = new Graph(1000);
        buildElementGraph(rootNode);
        
        // DSA: Use BFS for level-order layout calculation
        LinkedList<int>* bfsOrder = new LinkedList<int>();
        if (rootNode && rootNode->nodeId >= 0) {
            elementGraph->BFS(rootNode->nodeId, bfsOrder);
        }
        
        int currentY = 20;
        int margin = 20;
        int contentWidth = windowWidth - 2 * margin;
        
        // Process nodes in BFS order
        for (int i = 0; i < bfsOrder->getSize(); i++) {
            int nodeId = bfsOrder->getAt(i);
            RenderNode* node = findNodeById(rootNode, nodeId);
            if (!node) continue;
            
            // Simple layout calculation
            node->x = margin;
            node->width = contentWidth;
            
            if (node->tagName) {
                if (strcmp(node->tagName, "h1") == 0) {
                    node->height = 40;
                } else if (strcmp(node->tagName, "h2") == 0) {
                    node->height = 32;
                } else if (strcmp(node->tagName, "h3") == 0) {
                    node->height = 28;
                } else if (strcmp(node->tagName, "p") == 0) {
                    node->height = 20;
                } else {
                    node->height = 20;
                }
            } else {
                node->height = 20;
            }
            
            node->y = currentY;
            currentY += node->height + 10;
            node->zIndex = node->depth; // Use depth as z-index
        }
        
        delete bfsOrder;
    }
    
    // DSA: Build element graph from DOM tree
    void buildElementGraph(RenderNode* node) {
        if (!node) return;
        
        elementGraph->addVertex(node->nodeId);
        
        if (node->parent) {
            elementGraph->addEdge(node->parent->nodeId, node->nodeId);
        }
        
        RenderNode* child = node->firstChild;
        while (child) {
            buildElementGraph(child);
            child = child->nextSibling;
        }
    }
    
    RenderNode* findNodeById(RenderNode* node, int id) {
        if (!node) return nullptr;
        if (node->nodeId == id) return node;
        
        RenderNode* result = findNodeById(node->firstChild, id);
        if (result) return result;
        
        return findNodeById(node->nextSibling, id);
    }
    
    // DSA: Build spatial BST by y-position
    void buildSpatialBST(RenderNode* node) {
        if (!node) return;
        
        // DSA: Insert into BST by y-position
        spatialBST->insert(node->y, node);
        
        RenderNode* child = node->firstChild;
        while (child) {
            buildSpatialBST(child);
            child = child->nextSibling;
        }
    }
    
    // DSA: Build render order using AVL Tree
    void buildRenderOrder(RenderNode* node) {
        if (!node) return;
        
        // DSA: Insert into AVL tree by z-index
        renderOrderTree->insert(node->zIndex, node);
        
        RenderNode* child = node->firstChild;
        while (child) {
            buildRenderOrder(child);
            child = child->nextSibling;
        }
    }
    
    // DSA: Build render queue using level-order traversal (BFS)
    LinkedList<RenderNode*>* buildRenderQueue(RenderNode* root) {
        LinkedList<RenderNode*>* queue = new LinkedList<RenderNode*>();
        Queue<RenderNode*>* tempQueue = new Queue<RenderNode*>();
        
        if (root) {
            tempQueue->enqueue(root);
        }
        
        while (!tempQueue->isEmpty()) {
            RenderNode* node = tempQueue->dequeue();
            queue->insert(node);
            linkedListOps++;
            
            RenderNode* child = node->firstChild;
            while (child) {
                tempQueue->enqueue(child);
                child = child->nextSibling;
            }
        }
        
        delete tempQueue;
        return queue;
    }
    
    // DSA: Tree traversal demonstrations
    void preOrderTraversal(RenderNode* node, LinkedList<RenderNode*>* result) {
        if (!node) return;
        result->insert(node);
        RenderNode* child = node->firstChild;
        while (child) {
            preOrderTraversal(child, result);
            child = child->nextSibling;
        }
    }
    
    void inOrderTraversal(RenderNode* node, LinkedList<RenderNode*>* result) {
        if (!node) return;
        RenderNode* child = node->firstChild;
        if (child) {
            inOrderTraversal(child, result);
            child = child->nextSibling;
        }
        result->insert(node);
        while (child) {
            inOrderTraversal(child, result);
            child = child->nextSibling;
        }
    }
    
    void postOrderTraversal(RenderNode* node, LinkedList<RenderNode*>* result) {
        if (!node) return;
        RenderNode* child = node->firstChild;
        while (child) {
            postOrderTraversal(child, result);
            child = child->nextSibling;
        }
        result->insert(node);
    }
    
    int calculateTreeHeight(RenderNode* node) {
        if (!node) return 0;
        int maxHeight = 0;
        RenderNode* child = node->firstChild;
        while (child) {
            int childHeight = calculateTreeHeight(child);
            if (childHeight > maxHeight) maxHeight = childHeight;
            child = child->nextSibling;
        }
        return maxHeight + 1;
    }
    
    int calculateTreeSize(RenderNode* node) {
        if (!node) return 0;
        int size = 1;
        RenderNode* child = node->firstChild;
        while (child) {
            size += calculateTreeSize(child);
            child = child->nextSibling;
        }
        return size;
    }
    
    void cleanupTree(RenderNode* node) {
        if (!node) return;
        cleanupTree(node->firstChild);
        cleanupTree(node->nextSibling);
        delete node;
    }

public:
    HTMLRenderer() : root(nullptr), styleRegistry(nullptr), 
                     elementGraph(nullptr), renderOrderTree(nullptr),
                     spatialBST(nullptr), nodeCounter(0), linkedListOps(0) {
        initializeStyleSystem();
    }
    
    ~HTMLRenderer() {
        cleanupTree(root);
        delete styleRegistry;
        delete elementGraph;
        delete renderOrderTree;
        delete spatialBST;
    }
    
    bool loadFromFile(const char* filename) {
        root = parseParsedHTMLFile(filename);
        if (!root) return false;
        
        applyStyles(root);
        return true;
    }
    
    void calculateLayout(int windowWidth) {
        calculateLayoutInternal(root, windowWidth);
        
        // DSA: Build spatial BST
        spatialBST = new BST();
        buildSpatialBST(root);
        
        // DSA: Build render order AVL tree
        renderOrderTree = new AVLTree();
        buildRenderOrder(root);
    }
    
    // DSA: Print DSA statistics
    void printDSAStatistics() {
        std::cout << "\n========== DSA STATISTICS ==========\n";
        
        // Hash Table Statistics
        std::cout << "\n--- Hash Table Statistics ---\n";
        std::cout << "Style Registry:\n";
        std::cout << "  Size: " << styleRegistry->getSize() << "\n";
        std::cout << "  Load Factor: " << styleRegistry->getLoadFactor() << "\n";
        std::cout << "  Collisions: " << styleRegistry->getCollisionCount() << "\n";
        
        // AVL Tree Statistics
        std::cout << "\n--- AVL Tree Statistics ---\n";
        std::cout << "Render Order Tree:\n";
        std::cout << "  Height: " << renderOrderTree->getHeight() << "\n";
        std::cout << "  Balance Factor: " << renderOrderTree->getBalanceFactor() << "\n";
        std::cout << "  Rotations: " << renderOrderTree->getRotationCount() << "\n";
        
        // BST Statistics
        std::cout << "\n--- BST Statistics ---\n";
        std::cout << "Spatial Index:\n";
        std::cout << "  Height: " << spatialBST->getHeight() << "\n";
        std::cout << "  Valid BST: " << (spatialBST->validateBST() ? "Yes" : "No") << "\n";
        
        // Graph Statistics
        std::cout << "\n--- Graph Statistics ---\n";
        std::cout << "Element Graph:\n";
        std::cout << "  Vertices: " << elementGraph->getNumVertices() << "\n";
        std::cout << "  Has Cycle: " << (elementGraph->detectCycle() ? "Yes" : "No") << "\n";
        
        // Tree Statistics
        std::cout << "\n--- DOM Tree Statistics ---\n";
        std::cout << "  Height: " << calculateTreeHeight(root) << "\n";
        std::cout << "  Size: " << calculateTreeSize(root) << "\n";
        
        // Linked List Statistics
        std::cout << "\n--- Linked List Statistics ---\n";
        std::cout << "  Operations Count: " << linkedListOps << "\n";
        
        std::cout << "\n=====================================\n\n";
    }
    
    // DSA: Demonstrate all traversal types
    void demonstrateDSATraversals() {
        std::cout << "\n========== TREE TRAVERSAL DEMONSTRATIONS ==========\n";
        
        LinkedList<RenderNode*>* preOrder = new LinkedList<RenderNode*>();
        LinkedList<RenderNode*>* inOrder = new LinkedList<RenderNode*>();
        LinkedList<RenderNode*>* postOrder = new LinkedList<RenderNode*>();
        LinkedList<RenderNode*>* levelOrder = buildRenderQueue(root);
        
        preOrderTraversal(root, preOrder);
        inOrderTraversal(root, inOrder);
        postOrderTraversal(root, postOrder);
        
        std::cout << "\nPre-order Traversal (" << preOrder->getSize() << " nodes):\n";
        for (int i = 0; i < preOrder->getSize() && i < 10; i++) {
            RenderNode* node = preOrder->getAt(i);
            if (node->tagName) {
                std::cout << "  " << node->tagName;
                if (node->textContent) std::cout << " - " << node->textContent;
                std::cout << "\n";
            }
        }
        if (preOrder->getSize() > 10) std::cout << "  ... (" << (preOrder->getSize() - 10) << " more)\n";
        
        std::cout << "\nIn-order Traversal (" << inOrder->getSize() << " nodes):\n";
        for (int i = 0; i < inOrder->getSize() && i < 10; i++) {
            RenderNode* node = inOrder->getAt(i);
            if (node->tagName) {
                std::cout << "  " << node->tagName;
                if (node->textContent) std::cout << " - " << node->textContent;
                std::cout << "\n";
            }
        }
        if (inOrder->getSize() > 10) std::cout << "  ... (" << (inOrder->getSize() - 10) << " more)\n";
        
        std::cout << "\nPost-order Traversal (" << postOrder->getSize() << " nodes):\n";
        for (int i = 0; i < postOrder->getSize() && i < 10; i++) {
            RenderNode* node = postOrder->getAt(i);
            if (node->tagName) {
                std::cout << "  " << node->tagName;
                if (node->textContent) std::cout << " - " << node->textContent;
                std::cout << "\n";
            }
        }
        if (postOrder->getSize() > 10) std::cout << "  ... (" << (postOrder->getSize() - 10) << " more)\n";
        
        std::cout << "\nLevel-order Traversal (BFS) (" << levelOrder->getSize() << " nodes):\n";
        for (int i = 0; i < levelOrder->getSize() && i < 10; i++) {
            RenderNode* node = levelOrder->getAt(i);
            if (node->tagName) {
                std::cout << "  " << node->tagName;
                if (node->textContent) std::cout << " - " << node->textContent;
                std::cout << "\n";
            }
        }
        if (levelOrder->getSize() > 10) std::cout << "  ... (" << (levelOrder->getSize() - 10) << " more)\n";
        
        delete preOrder;
        delete inOrder;
        delete postOrder;
        delete levelOrder;
        
        std::cout << "\n==================================================\n\n";
    }
    
    // Get render list from AVL tree (in-order traversal)
    LinkedList<RenderNode*>* getRenderList() {
        LinkedList<void*>* tempList = new LinkedList<void*>();
        renderOrderTree->inOrderTraversal(tempList);
        
        LinkedList<RenderNode*>* result = new LinkedList<RenderNode*>();
        for (int i = 0; i < tempList->getSize(); i++) {
            result->insert((RenderNode*)tempList->getAt(i));
        }
        
        delete tempList;
        return result;
    }
    
    // Query visible elements using BST range query
    LinkedList<RenderNode*>* queryVisibleElements(int viewportY, int viewportHeight) {
        LinkedList<void*>* tempList = new LinkedList<void*>();
        spatialBST->rangeQuery(viewportY, viewportY + viewportHeight, tempList);
        
        LinkedList<RenderNode*>* result = new LinkedList<RenderNode*>();
        for (int i = 0; i < tempList->getSize(); i++) {
            result->insert((RenderNode*)tempList->getAt(i));
        }
        
        delete tempList;
        return result;
    }
    
    RenderNode* getRoot() {
        return root;
    }
};

// ============================================================================
// SDL2 RENDERING (Simple implementation - Optional)
// ============================================================================

#ifdef USE_SDL2

class SDLRenderer {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    int windowWidth;
    int windowHeight;
    
public:
    SDLRenderer(int width = 800, int height = 600) 
        : window(nullptr), renderer(nullptr), font(nullptr),
          windowWidth(width), windowHeight(height) {
        
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
            return;
        }
        
        if (TTF_Init() < 0) {
            std::cerr << "TTF initialization failed: " << TTF_GetError() << std::endl;
            return;
        }
        
        window = SDL_CreateWindow("HTML Renderer - DSA Demonstration",
                                  SDL_WINDOWPOS_UNDEFINED,
                                  SDL_WINDOWPOS_UNDEFINED,
                                  windowWidth, windowHeight,
                                  SDL_WINDOW_SHOWN);
        
        if (!window) {
            std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
            return;
        }
        
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer) {
            std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
            return;
        }
        
        // Try to load a default font (fallback to basic rendering if not available)
        font = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", 14);
        if (!font) {
            font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 14);
        }
    }
    
    ~SDLRenderer() {
        if (font) TTF_CloseFont(font);
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
    }
    
    void render(HTMLRenderer* htmlRenderer) {
        if (!renderer || !htmlRenderer) return;
        
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        
        // DSA: Get render list from AVL tree
        LinkedList<RenderNode*>* renderList = htmlRenderer->getRenderList();
        
        // Render each node
        for (int i = 0; i < renderList->getSize(); i++) {
            RenderNode* node = renderList->getAt(i);
            if (!node || node->y < 0 || node->y > windowHeight) continue;
            
            renderNode(node);
        }
        
        delete renderList;
        
        SDL_RenderPresent(renderer);
    }
    
    void renderNode(RenderNode* node) {
        if (!node || !renderer) return;
        
        // Simple text rendering
        if (node->textContent && strlen(node->textContent) > 0) {
            SDL_Color color = {0, 0, 0, 255};
            
            // Determine font size based on tag
            int fontSize = 14;
            bool bold = false;
            bool italic = false;
            
            if (node->tagName) {
                if (strcmp(node->tagName, "h1") == 0) {
                    fontSize = 32;
                    bold = true;
                } else if (strcmp(node->tagName, "h2") == 0) {
                    fontSize = 24;
                    bold = true;
                } else if (strcmp(node->tagName, "h3") == 0) {
                    fontSize = 20;
                    bold = true;
                } else if (strcmp(node->tagName, "strong") == 0 || strcmp(node->tagName, "b") == 0) {
                    bold = true;
                } else if (strcmp(node->tagName, "em") == 0 || strcmp(node->tagName, "i") == 0) {
                    italic = true;
                }
            }
            
            // Simple text rendering (without TTF for simplicity)
            // In a full implementation, would use TTF_RenderText_Blended
            
            // Draw a simple rectangle for text area
            SDL_Rect rect = {node->x, node->y, node->width, node->height};
            SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
            SDL_RenderFillRect(renderer, &rect);
            
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            SDL_RenderDrawRect(renderer, &rect);
        }
    }
    
    void handleEvents(bool& running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
            }
        }
    }
    
    bool isValid() {
        return window && renderer;
    }
};

#else
// Dummy SDLRenderer class when SDL2 is not available
class SDLRenderer {
public:
    SDLRenderer(int width = 800, int height = 600) {
        std::cout << "\nNote: SDL2 not available. Running in console-only mode.\n";
        std::cout << "All DSA demonstrations will be shown via console output.\n";
        std::cout << "To enable visual rendering, install SDL2:\n";
        std::cout << "  macOS: brew install sdl2 sdl2_ttf\n";
        std::cout << "  Linux: sudo apt-get install libsdl2-dev libsdl2-ttf-dev\n\n";
    }
    ~SDLRenderer() {}
    void render(HTMLRenderer* htmlRenderer) {}
    void handleEvents(bool& running) { running = false; }
    bool isValid() { return false; }
};

#endif

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main(int argc, char* argv[]) {
    const char* inputFile = "parsed_output.txt";
    
    if (argc > 1) {
        inputFile = argv[1];
    }
    
    std::cout << "HTML Renderer - DSA Demonstration\n";
    std::cout << "===================================\n";
    std::cout << "Loading file: " << inputFile << "\n\n";
    
    // Create HTML renderer
    HTMLRenderer* renderer = new HTMLRenderer();
    
    // DSA: Parse file and build DOM tree
    if (!renderer->loadFromFile(inputFile)) {
        std::cerr << "Failed to load HTML file\n";
        delete renderer;
        return 1;
    }
    
    std::cout << "File loaded successfully!\n";
    
    // DSA: Calculate layout using Graph algorithms
    renderer->calculateLayout(800);
    
    // DSA: Print statistics
    renderer->printDSAStatistics();
    
    // DSA: Demonstrate traversals
    renderer->demonstrateDSATraversals();
    
    // Initialize SDL (optional)
    SDLRenderer* sdlRenderer = new SDLRenderer(800, 600);
    
    if (!sdlRenderer->isValid()) {
        std::cout << "\nProgram completed. All DSA demonstrations shown above.\n";
        std::cout << "To enable visual rendering, install SDL2 and recompile.\n";
        delete sdlRenderer;
        delete renderer;
        return 0;
    }
    
    // Main render loop (only if SDL2 is available)
    std::cout << "\nRendering window opened. Close window to exit.\n";
    std::cout << "All DSA demonstrations have been shown above.\n\n";
    
    #ifdef USE_SDL2
    bool running = true;
    while (running) {
        sdlRenderer->handleEvents(running);
        sdlRenderer->render(renderer);
        SDL_Delay(16); // ~60 FPS
    }
    #endif
    
    delete sdlRenderer;
    delete renderer;
    
    std::cout << "Renderer closed.\n";
    return 0;
}

