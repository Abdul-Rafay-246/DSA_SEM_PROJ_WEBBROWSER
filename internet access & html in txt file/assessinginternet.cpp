#include <iostream>
#include <cstdlib>
#include <string>

int main(int argc, char* argv[]) {
    std::string url = "https://www.w3schools.com/html/html_basic.asp";
    std::string outputFile = "..\\fetched.html";

    if (argc > 1) {
        url = argv[1];
    }
    if (argc > 2) {
        outputFile = argv[2];
    }

    std::cout << "Fetching HTML...\n";
    std::string cmd = "python html_fetching_URL.py \"" + url + "\" \"" + outputFile + "\"";
    int result = system(cmd.c_str());
    if (result != 0) {
        std::cerr << "Error: Python script failed\n";
        return 1;
    }
    std::cout << "Done. Output: " << outputFile << "\n";
    return 0;
}
