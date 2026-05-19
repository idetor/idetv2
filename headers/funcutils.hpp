#ifndef FUNCUTILS_HPP
#define FUNCUTILS_HPP
#include <string>
#include <vector>

std::string intToString(int num) {
    return std::to_string(num);
}
std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    for (char ch : str) {
        if (ch == delimiter) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token += ch;
        }
    }
    if (!token.empty()) {
        tokens.push_back(token);
    }
    return tokens;
}
std::string vecToString(const std::vector<std::string>& vec) {
    std::string result;
    for (const auto& str : vec) {
        result += str + " ";
    }
    if (!result.empty()) {
        result.pop_back(); 
    }
    return result;
}

int getCharLenformBytes(const std::string& str) {
    int charCount = 0;
    for (size_t i = 0; i < str.size();) {
        unsigned char c = str[i];
        if (c < 128) {
            i += 1; 
        } else if ((c >> 5) == 0b110) {
            i += 2; 
        } else if ((c >> 4) == 0b1110) {
            i += 3; 
        } else if ((c >> 3) == 0b11110) {
            i += 4; 
        } else {
            i += 1; 
        }
        charCount++;
    }
    return charCount;
}
#endif