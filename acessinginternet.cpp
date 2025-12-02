#include <curl/curl.h>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

// Callback to write downloaded data to file
static size_t write_cb(void* ptr, size_t size, size_t nmemb, void* userdata) {
    ofstream* ofs = static_cast<ofstream*>(userdata);
    size_t total = size * nmemb;

    if (!ofs || !ofs->good()) {
        return 0; // signal error to curl
    }

    ofs->write(static_cast<const char*>(ptr), total);
    if (!ofs->good()) {
        return 0; // write failed
    }

    return total; // number of bytes successfully handled
}

int main() {
    string url;
    cout << "Enter URL (http:// or https://): ";
    getline(cin, url);

    if (url.empty()) {
        cerr << "Error: Empty URL\n";
        return 1;
    }

    // Global init
    CURLcode global_res = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (global_res != CURLE_OK) {
        cerr << "Error: curl_global_init failed: "
             << curl_easy_strerror(global_res) << "\n";
        return 1;
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        cerr << "Error: curl_easy_init failed\n";
        curl_global_cleanup();
        return 1;
    }

    ofstream ofs("output.html", ios::binary);
    if (!ofs) {
        cerr << "Error: Failed to open output.html for writing\n";
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return 1;
    }

    // Configure curl options
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);      // follow redirects
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb); // callback for data
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ofs);         // file stream to callback
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "CPP-Browser/1.0");
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);            // 30 second timeout

    // For your project it is fine to disable verification
    // Do NOT do this in real production software
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    // Perform the download
    CURLcode res = curl_easy_perform(curl);

    ofs.close();

    if (res != CURLE_OK) {
        cerr << "Error: Download failed - "
             << curl_easy_strerror(res) << "\n";
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return 1;
    }

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    if (http_code >= 400) {
        cerr << "Error: HTTP status code " << http_code << "\n";
        return 1;
    }

    cout << "Success: HTML saved to output.html (HTTP " << http_code << ")\n";
    return 0;
}
