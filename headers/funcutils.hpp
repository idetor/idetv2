#ifndef FUNCUTILS_HPP
#define FUNCUTILS_HPP
#include <string>
#include <vector>
#ifdef _WIN32
#include <cstdint>
#endif


int getCharLenfromBytes(unsigned char firstByte) {
    // Single-byte ASCII character (0xxxxxxx)
    if ((firstByte & 0x80) == 0) {
        return 1;
    }
    
    // Two-byte character (110xxxxx)
    if ((firstByte & 0xE0) == 0xC0) {
        return 2;
    }
    
    // Three-byte character (1110xxxx)
    if ((firstByte & 0xF0) == 0xE0) {
        return 3;
    }
    
    // Four-byte character (11110xxx)
    if ((firstByte & 0xF8) == 0xF0) {
        return 4;
    }
    
    // Invalid UTF-8 leading byte, treat as single byte
    return 1;
}

int charIndexToByteIndex(const std::string& line, int charIndex) {
    int byteIdx = 0;
    int charCount = 0;
    
    while (charCount < charIndex && byteIdx < (int)line.length()) {
        byteIdx += getCharLenfromBytes((unsigned char)line[byteIdx]);
        charCount++;
    }
    
    return byteIdx;
}

int byteIndexToCharIndex(const std::string& line, int byteIndex) {
    int charCount = 0;
    int byteIdx = 0;
    
    while (byteIdx < byteIndex && byteIdx < (int)line.length()) {
        byteIdx += getCharLenfromBytes((unsigned char)line[byteIdx]);
        charCount++;
    }
    
    return charCount;
}

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

int convertByteIdxToCharIdx(const std::string& line, int byteIdx) {
    int charIdx = 0;
    int currentByteIdx = 0;
    while (currentByteIdx < byteIdx && currentByteIdx < (int)line.length()) {
        currentByteIdx += getCharLenfromBytes((unsigned char)line[currentByteIdx]);
        charIdx++;
    }
    return charIdx;
}



std::string inBox(std::string stringToContain, int marginTop = 2, int marginBottom = 2, 
                  int marginLeft = 2, int marginRight = 2, int margin = 2, char toSurround = '@', int overprint = 10) {
    std::string emptyString = "";
    for (int i = 0; i < overprint; ++i) {
        emptyString += " ";
    }
    int top = marginTop;
    int bottom = marginBottom;
    int left = marginLeft;
    int right = marginRight;
    size_t stringLength = stringToContain.length();
    size_t boxWidth = stringLength + left + right + 2; 
    std::string result;
    result += std::string(boxWidth, toSurround) + emptyString +"\n";    
    for (int i = 0; i < top; ++i) {
        result += toSurround;
        result += std::string(boxWidth - 2, ' ');
        result += toSurround + emptyString + "\n";
    }    
    int totalInnerWidth = boxWidth - 2; 
    int textPadding = left;
    int rightPadding = right;


    result += toSurround;
    result += std::string(textPadding, ' ');
    result += stringToContain + std::string(rightPadding, ' ');
    result += toSurround + emptyString + "\n";
    
    
    for (int i = 0; i < bottom; ++i) {
        result += toSurround;
        result += std::string(boxWidth - 2, ' ');
        result += toSurround + emptyString + "\n";
    }
    
    result += std::string(boxWidth, toSurround) + emptyString + "\n";
    
    return result;
}

std::string stringBefore(std::string str, char delimiter) {
    size_t pos = str.find(delimiter);
    if (pos != std::string::npos) {
        return str.substr(0, pos);
    }
    return str; 
}
int stringCharLength(const std::string& str) {
    int length = 0;
    int byteIdx = 0;
    while (byteIdx < (int)str.length()) {
        byteIdx += getCharLenfromBytes((unsigned char)str[byteIdx]);
        length++;
    }
    return length;
}

int getByteLenForLeftChar(const std::string& line, int byteIdx) {
    if (byteIdx <= 0) return 0;
    int charLen = getCharLenfromBytes((unsigned char)line[byteIdx - 1]);
    if (byteIdx - charLen < 0) return 0; 
    return charLen;
}
std::string encodeUTF8(int32_t codePoint) {
    std::string result;
    if (codePoint <= 0x7F) {
        result += (char)codePoint;
    } else if (codePoint <= 0x7FF) {
        result += (char)(0xC0 | (codePoint >> 6));
        result += (char)(0x80 | (codePoint & 0x3F));
    } else if (codePoint <= 0xFFFF) {
        result += (char)(0xE0 | (codePoint >> 12));
        result += (char)(0x80 | ((codePoint >> 6) & 0x3F));
        result += (char)(0x80 | (codePoint & 0x3F));
    } else if (codePoint <= 0x10FFFF) {
        result += (char)(0xF0 | (codePoint >> 18));
        result += (char)(0x80 | ((codePoint >> 12) & 0x3F));
        result += (char)(0x80 | ((codePoint >> 6) & 0x3F));
        result += (char)(0x80 | (codePoint & 0x3F));
    }
    return result;
}
std::string insertIntoStr(const std::string& original, size_t index, const std::string& toInsert) {
    if (index > original.size()) index = original.size();
    return original.substr(0, index) + toInsert + original.substr(index);
}

std::string boolToString(bool value){
    if (value){
        return "true";
    }
    return "false";
}

#endif