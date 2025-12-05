#include <iostream>
#include <cstdlib>
using namespace std;

int main() {
    cout << "Fetching HTML...\n";
    int result = system("python url_fetching.py");
    if (result != 0) {
        cerr << "Error: Python script failed\n";
        return 1;
    }
    cout << "Done.\n";
    return 0;
}