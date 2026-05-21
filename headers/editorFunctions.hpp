#ifndef EDITOR_FUNCTIONS_HPP
#define EDITOR_FUNCTIONS_HPP
#include "displayEngine.hpp"
#include "funcutils.hpp"
#include "debugwriter.hpp"
#include "keypress.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include <filesystem>
#include <chrono>
#include <fstream>
namespace fs = std::filesystem;

struct SelectionElement {
    int startX;
    int startY;
    int endX;
    int endY;
    bool isActive;
};

struct cursorElement {
    int x;
    int y;
    Color color;
    Color BackgroundColor;
    std::string mode;
    SelectionElement selection;
    std::vector<std::string> utilVec1;
    std::vector<std::string> utilVec2;
    std::vector<int> utilVec3;

};

struct editorSettings {
    bool showLineNumbers;
    bool autoIndent;
    bool syntaxHighlighting;
    int tabSize;
    std::string theme;
    Color backgroundColor;
    Color textColor;
    Color headBackgroundColor;
    Color headTextColor;
};

struct FileInfos {
    std::string name;
    std::string path;
    std::string extension;
    bool isOpen;
    bool hasChanges;
    int lastModified;
};

struct File {
    FileInfos context;
    std::vector<std::string> content;
    
};

class Editor {
    std::vector<File> openFiles;
    File currentFile;
    cursorElement cursor;
    editorSettings settings;
    int scrollOffsetY = 0;  // Vertical scroll offset
    int scrollOffsetX = 0;  // Horizontal scroll offset
    int editorHeight = 0;
    int editorWidth = 0;
    std::string utf8Buffer;


    void updateScrollOffsets() {
        
        editorHeight = winInfo.getHeight() - 2;
        editorWidth = winInfo.getWidth();
        
        
        if (cursor.y < scrollOffsetY) {
            scrollOffsetY = cursor.y;
        }
        if (cursor.y >= scrollOffsetY + editorHeight) {
            scrollOffsetY = cursor.y - editorHeight + 1;
        }
        
        
        int lineNumberWidth = settings.showLineNumbers ? 5 : 0;
        int availableWidth = editorWidth - lineNumberWidth - 1;
        int cursorScreenX = cursor.x - scrollOffsetX;
        
        if (cursorScreenX < 2) {
            scrollOffsetX = cursor.x - 2;
        } else if (cursorScreenX >= availableWidth - 2) {
            scrollOffsetX = cursor.x - availableWidth + 3;
        }
        
        
        if (scrollOffsetY < 0) scrollOffsetY = 0;
        if (scrollOffsetX < 0) scrollOffsetX = 0;
        if (scrollOffsetY > (int)currentFile.content.size() - 1) {
            scrollOffsetY = (int)currentFile.content.size() - 1;
        }
    }
    void moveCursorTo(int newX, int newY){
        if (newY < 0) newY = 0;
        if (newY >= (int)currentFile.content.size()) newY = (int)currentFile.content.size() - 1;
        cursor.x = newX;
        cursor.y = newY;
        updateScrollOffsets();
    }

    void initializeSettings() {
        settings.showLineNumbers = true;
        settings.autoIndent = true;
        settings.syntaxHighlighting = false;
        settings.tabSize = 4;
        settings.theme = "default";
        settings.backgroundColor = {30, 30, 30};
        settings.textColor = {200, 200, 200};
        settings.headBackgroundColor = {50, 50, 100};
        settings.headTextColor = {255, 255, 255};
    }
    
    void loadFileContent(const std::string& filePath) {
        currentFile.content.clear();
        FILE* file = fopen(filePath.c_str(), "r");
        if (file) {
            char buffer[1024];
            bool hasLines = false;
            while (fgets(buffer, sizeof(buffer), file)) {
                // Remove trailing newline
                std::string line(buffer);
                if (!line.empty() && line.back() == '\n') {
                    line.pop_back();
                }
                if (!line.empty() && line.back() == '\r') {
                    line.pop_back();
                }
                currentFile.content.push_back(line);
                hasLines = true;
            }
            fclose(file);
            
            
            if (!hasLines) {
                currentFile.content.push_back("");
            }
        } else {
            
            currentFile.content.push_back("");
        }
        
        cursor.x = 0;
        cursor.y = 0;
        cursor.mode = "normal";
        scrollOffsetY = 0;
        scrollOffsetX = 0;
    }
    
    public:
        void initSelection(){
            cursor.selection.startX = 0;
            cursor.selection.startY = 0;
            cursor.selection.endX = 0;
            cursor.selection.endY = 0;
            cursor.selection.isActive = false;
        }
        void startSelect(){
            SelectionElement selection;
            selection.startX = cursor.x;
            selection.startY = cursor.y;
            selection.endX = cursor.x;
            selection.endY = cursor.y;
            selection.isActive = true;
            cursor.selection = selection;
        }
        void updateSelect(){
            cursor.selection.endX = cursor.x;
            cursor.selection.endY = cursor.y;
        }
        std::string getSelectionInfoStr(){
            return "StartX: " + std::to_string(cursor.selection.startX) + 
                ", StartY: " + std::to_string(cursor.selection.startY) + 
                ", EndX: " + std::to_string(cursor.selection.endX) + 
                ", EndY: " + std::to_string(cursor.selection.endY) + 
                ", isActive: " + boolToString(cursor.selection.isActive);

        }
        windowInfo winInfo = windowInfo::getInstance();
        
        Editor() {
            initializeSettings();
        }
        bool checkQuit(){
        // q is 113
            if (currentFile.context.hasChanges){
                std::string warnStr = inBox("You have unsaved changes! Press q again to quit.");
                // clear screen
                clearScreen();
                printf("%s", warnStr.c_str());
                int k = getKeyboardPress();
                if ( k == 113 ){
                    return true;
                }
                else { 
                    return false;
                }
            }
            return true;
        }
        void openFile(const std::string& filePath){
            // Inserts File info
            FileInfos fileInfo;
            fileInfo.path = filePath;
            
            size_t lastSlash = filePath.find_last_of("/\\");
            if (lastSlash != std::string::npos) {
                fileInfo.name = filePath.substr(lastSlash + 1);
            } else {
                fileInfo.name = filePath; 
            }
            
            size_t lastDot = fileInfo.name.find_last_of(".");
            if (lastDot != std::string::npos) {
                fileInfo.extension = fileInfo.name.substr(lastDot + 1);
            } else {
                fileInfo.extension = ""; 
            }
            
            fileInfo.isOpen = true;
            fileInfo.hasChanges = false;
            
            if (!fs::exists(filePath)) {
                fileInfo.lastModified = std::chrono::system_clock::to_time_t(
                    std::chrono::system_clock::now()
                );
            } else {
                auto lastWriteTime = fs::last_write_time(filePath);
                auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
                    lastWriteTime - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
                );
                fileInfo.lastModified = std::chrono::system_clock::to_time_t(sctp);
            }
            
            File openFile;
            openFile.context = fileInfo;
            openFiles.push_back(openFile);  
            currentFile = openFile;
            
            loadFileContent(filePath);
        }
        
        void saveFile() {
            std::ofstream file(currentFile.context.path);
            if (!file.is_open()) {
                writeDebugInfo("Error: Could not open file for writing: " + currentFile.context.path);
                return;
            }

            for (size_t i = 0; i < currentFile.content.size(); i++) {
                file << currentFile.content[i];
                if (i < currentFile.content.size() - 1) {
                    file << "\n";
                }
            }

            if (!file.good()) {
                writeDebugInfo("Error: Write operation failed for: " + currentFile.context.path);
                return;
            }

            file.close();
            currentFile.context.hasChanges = false;
            writeDebugInfo("File saved: " + currentFile.context.path);
        }
        
        void handleKeyInput(int32_t key) {
            if (key == 27) { // Escape
                cursor.mode = cursor.mode == "normal" ? "insert" : "normal";
                utf8Buffer.clear();  // Clear buffer on mode switch
                return;
            }
            
            if (key == 1013){ // shift + right
                if (cursor.selection.isActive){
                    updateSelect();
                }
                else{
                    startSelect();
                }
                moveCursorTo(cursor.x + 1, cursor.y);
                updateSelect();
                return;
            }
            if (key == 1012){ //shift + left
                if (cursor.x == 0){
                    return;
                }
                if (cursor.selection.isActive){updateSelect();}
                else{startSelect();}
                moveCursorTo(cursor.x -1, cursor.y);
                updateSelect();
                return;
            }
            if (key == 1010){ // shift + up
                if (cursor.y == 0){
                    return;
                }

                if (cursor.selection.isActive){updateSelect();}
                else{startSelect();}

                if (cursor.x > currentFile.content[cursor.y -1 ].length()){
                    writeToDebugChannel("cursorposX is to high");
                    moveCursorTo(currentFile.content[cursor.y -1 ].length(), cursor.y -1);
                }
                else{

                    moveCursorTo(cursor.x, cursor.y -1);
                }
                updateSelect();
                return;
            }


            // Arrow keys for cursor movement
            if (key == 1000) { // Up
                utf8Buffer.clear();
                if (cursor.y > 0) cursor.y--;
                const std::string& line = currentFile.content[cursor.y];
                if (cursor.x > (int)line.length()) {
                    cursor.x = (int)line.length();
                }
                updateScrollOffsets();
                return;
            }
            if (key == 1001) { // Down
                utf8Buffer.clear();
                if (cursor.y < (int)currentFile.content.size() - 1) cursor.y++;
                const std::string& line = currentFile.content[cursor.y];
                if (cursor.x > (int)line.length()) {
                    cursor.x = (int)line.length();
                }
                updateScrollOffsets();
                return;
            }
            if (key == 1002) { // Left
                utf8Buffer.clear();
                if (cursor.x > 0) {
                    const std::string& line = currentFile.content[cursor.y];
                    int byteIdx = cursor.x - 1;
                    // Scan backwards to find the start of the character
                    while (byteIdx > 0 && (unsigned char)line[byteIdx] >= 0x80 
                        && (unsigned char)line[byteIdx] < 0xC0) {
                        byteIdx--;
                    }
                    cursor.x = byteIdx;
                } else if (cursor.y > 0) {
                    cursor.y--;
                    cursor.x = currentFile.content[cursor.y].length();
                }
                updateScrollOffsets();
                return;
            }
            if (key == 1003) { // Right
                utf8Buffer.clear();
                const std::string& line = currentFile.content[cursor.y];
                if (cursor.x < (int)line.length()) {
                    int charLen = getCharLenfromBytes((unsigned char)line[cursor.x]);
                    cursor.x += charLen;
                } else if (cursor.y < (int)currentFile.content.size() - 1) {
                    cursor.y++;
                    cursor.x = 0;
                }
                updateScrollOffsets();
                return;
            }

            
            // Page Up and Page Down
            if (key == 1008) { // Page Up
                utf8Buffer.clear();
                cursor.y = cursor.y - editorHeight > 0 ? cursor.y - editorHeight : 0;
                if (cursor.x > (int)currentFile.content[cursor.y].length()) {
                    cursor.x = currentFile.content[cursor.y].length();
                }
                updateScrollOffsets();
                return;
            }
            if (key == 1009) { // Page Down
                utf8Buffer.clear();
                cursor.y = cursor.y + editorHeight < (int)currentFile.content.size() - 1 
                    ? cursor.y + editorHeight 
                    : (int)currentFile.content.size() - 1;
                if (cursor.x > (int)currentFile.content[cursor.y].length()) {
                    cursor.x = currentFile.content[cursor.y].length();
                }
                updateScrollOffsets();
                return;
            }
            
            // Home and End keys
            if (key == 1004) { // Home
                utf8Buffer.clear();
                cursor.x = 0;
                updateScrollOffsets();
                return;
            }
            if (key == 1005) { // End
                utf8Buffer.clear();
                cursor.x = currentFile.content[cursor.y].length();
                updateScrollOffsets();
                return;
            }
            
            // Text editing
            if (key == 10 || key == 13) { // Enter
                utf8Buffer.clear();
                std::string currentLine = currentFile.content[cursor.y];
                std::string beforeCursor = currentLine.substr(0, cursor.x);
                std::string afterCursor = currentLine.substr(cursor.x);
                
                currentFile.content[cursor.y] = beforeCursor;
                currentFile.content.insert(currentFile.content.begin() + cursor.y + 1, afterCursor);
                
                cursor.y++;
                cursor.x = 0;
                currentFile.context.hasChanges = true;
                updateScrollOffsets();
                return;
            }
            
            if (key == 127) { // Backspace
                utf8Buffer.clear();
                if (cursor.x > 0) {
                    int charStart = cursor.x - 1;
                    while (charStart > 0 && (unsigned char)currentFile.content[cursor.y][charStart] >= 0x80 
                        && (unsigned char)currentFile.content[cursor.y][charStart] < 0xC0) {
                        charStart--;
                    }
                    int charLen = getCharLenfromBytes((unsigned char)currentFile.content[cursor.y][charStart]);
                    currentFile.content[cursor.y].erase(charStart, charLen);
                    cursor.x = charStart;
                } else if (cursor.y > 0) {
                    int prevLineLen = currentFile.content[cursor.y - 1].length();
                    currentFile.content[cursor.y - 1] += currentFile.content[cursor.y];
                    currentFile.content.erase(currentFile.content.begin() + cursor.y);
                    cursor.y--;
                    cursor.x = prevLineLen;
                }
                currentFile.context.hasChanges = true;
                updateScrollOffsets();
                return;
            }
            
            if (key == 1006) { // Delete
                utf8Buffer.clear();
                if (cursor.x < (int)currentFile.content[cursor.y].length()) {
                    int charLen = getCharLenfromBytes((unsigned char)currentFile.content[cursor.y][cursor.x]);
                    currentFile.content[cursor.y].erase(cursor.x, charLen);
                } else if (cursor.y < (int)currentFile.content.size() - 1) {
                    currentFile.content[cursor.y] += currentFile.content[cursor.y + 1];
                    currentFile.content.erase(currentFile.content.begin() + cursor.y + 1);
                }
                currentFile.context.hasChanges = true;
                updateScrollOffsets();
                return;
            }
            
            if (key == 24) {
                // implement cut later
                return;
            }
            if ( key == 19 ) {
                saveFile();
                currentFile.context.hasChanges = false;
                return;
            }

            if (key == 10) {
                // Return
                if (cursor.y < currentFile.content.size() - 1) {
                    std::string restOfLine = currentFile.content[cursor.y].substr(cursor.x);
                    currentFile.content[cursor.y].erase(cursor.x);
                    currentFile.content.insert(currentFile.content.begin() + cursor.y + 1, restOfLine);
                    cursor.y++;
                    cursor.x = 0;
                    currentFile.context.hasChanges = true;
                    updateScrollOffsets();
                }
                return;
            }

            if (key >= 0 && key <= 255) {
                unsigned char byte = (unsigned char)key;
                utf8Buffer += (char)byte;
                cursor.selection.isActive = false;
                
                if (isCompleteUTF8(utf8Buffer)) {
                    currentFile.content[cursor.y].insert(cursor.x, utf8Buffer);
                    cursor.x += utf8Buffer.length();
                    currentFile.context.hasChanges = true;
                    updateScrollOffsets();
                    utf8Buffer.clear();
                    return;
                }
                
                return;
            }
        }

        
        bool isCompleteUTF8(const std::string& buffer) {
            if (buffer.empty()) return false;
            
            unsigned char firstByte = (unsigned char)buffer[0];
            int expectedLen = getCharLenfromBytes(firstByte);
            
            return (int)buffer.length() == expectedLen;
        }
        void drawUI() {

            fflush(stdout);
            
            editorHeight = winInfo.getHeight() - 2;
            editorWidth = winInfo.getWidth();
            int lineNumberWidth = settings.showLineNumbers ? 5 : 0;
            int availableWidth = editorWidth - lineNumberWidth;
            
            updateScrollOffsets();
            
            int cursorScreenRow = -1;
            int cursorScreenCol = -1;
            
            std::string frameBuffer;
            frameBuffer.reserve(editorWidth * editorHeight * 4);
            frameBuffer += "\033[?25l";  // Hide cursor to prevent flicker
            frameBuffer += "\033[H";  // Home (move to top-left)
            
            frameBuffer += setBackgroundColorStr(settings.headBackgroundColor);
            frameBuffer += setTextColorStr(settings.headTextColor);
            
            std::string title = "Idet v2 - " + currentFile.context.name;
            if (currentFile.context.hasChanges) {
                title += "*";
            }
            
            title.resize(winInfo.getWidth(), ' ');
            frameBuffer += title + "\n";
            
            frameBuffer += setBackgroundColorStr(settings.backgroundColor);
            frameBuffer += setTextColorStr(settings.textColor);
            
            for (int i = 0; i < (editorHeight ); i++) {
                int lineIdx = scrollOffsetY + i;
                int screenRow = i + 2;  
                
                // Line number
                if (settings.showLineNumbers) {
                    frameBuffer += setBackgroundColorStr(settings.headBackgroundColor);
                    frameBuffer += setTextColorStr(settings.headTextColor);
                    std::string lineNum = std::to_string(lineIdx + 1);
                    frameBuffer += std::string(4 - lineNum.length(), ' ');  
                    frameBuffer += lineNum;
                    frameBuffer += " ";
                    frameBuffer += setBackgroundColorStr(settings.backgroundColor);
                    frameBuffer += setTextColorStr(settings.textColor);
                }
                
                int screenCol = lineNumberWidth + 1;  
                int screenX = 0;
                int byteIdx = 0;
                
                if (lineIdx < (int)currentFile.content.size()) {
                    const std::string& line = currentFile.content[lineIdx];
                    
                    // Calc the byte index for the start of scrollOffsetX (character position)
                    int targetByteIdx = 0;
                    int charCount = 0;
                    while (charCount < scrollOffsetX && targetByteIdx < (int)line.length()) {
                        targetByteIdx += getCharLenfromBytes((unsigned char)line[targetByteIdx]);
                        charCount++;
                    }
                    byteIdx = targetByteIdx;
                    
                    while (screenX < availableWidth && byteIdx < (int)line.length()) {
                        int charLen = getCharLenfromBytes((unsigned char)line[byteIdx]);
                        bool isCursorPos = (lineIdx == cursor.y && byteIdx == cursor.x);
                        // Selection highlight logic
                        bool isSelected = false;
                        if (cursor.selection.isActive) {
                            int selStartY = cursor.selection.startY;
                            int selEndY = cursor.selection.endY;
                            int selStartX = cursor.selection.startX;
                            int selEndX = cursor.selection.endX;
                            // Normalize selection
                            if (selStartY > selEndY || (selStartY == selEndY && selStartX > selEndX)) {
                                std::swap(selStartY, selEndY);
                                std::swap(selStartX, selEndX);
                            }
                            if (lineIdx > selStartY && lineIdx < selEndY) {
                                isSelected = true;
                            } else if (lineIdx == selStartY && lineIdx == selEndY) {
                                if (byteIdx >= selStartX && byteIdx < selEndX) isSelected = true;
                            } else if (lineIdx == selStartY) {
                                if (byteIdx >= selStartX) isSelected = true;
                            } else if (lineIdx == selEndY) {
                                if (byteIdx < selEndX) isSelected = true;
                            }
                        }
                        if (isCursorPos) {
                            cursorScreenRow = screenRow;
                            cursorScreenCol = screenCol + screenX;
                            frameBuffer += "\033[48;2;100;100;100m";
                        } else if (isSelected) {
                            
                            frameBuffer += "\033[48;2;60;90;200m";
                        }
                        // Extract and add to frameBuffer
                        for (int j = 0; j < charLen && byteIdx + j < (int)line.length(); j++) {
                            frameBuffer += line[byteIdx + j];
                        }
                        if (isCursorPos || isSelected) {
                            // Reset to normal background after selected/cursor char
                            frameBuffer += "\033[48;2;" + std::to_string(settings.backgroundColor.r) + ";" +
                                        std::to_string(settings.backgroundColor.g) + ";" +
                                        std::to_string(settings.backgroundColor.b) + "m";
                        }
                        byteIdx += charLen;
                        screenX++;
                    }
                    
                    // Fill remaining space with blank characters
                    while (screenX < availableWidth) {
                        // For empty space, cursor position is based on character offset from start of line
                        int charIdxFromLineStart = scrollOffsetX + screenX;
                        bool isCursorPos = (lineIdx == cursor.y && charIdxFromLineStart == convertByteIdxToCharIdx(line, cursor.x));
                        
                        if (isCursorPos) {
                            cursorScreenRow = screenRow;
                            cursorScreenCol = screenCol + screenX;
                            frameBuffer += "\033[48;2;100;100;100m ";
                            frameBuffer += "\033[48;2;" + std::to_string(settings.backgroundColor.r) + ";" +
                                        std::to_string(settings.backgroundColor.g) + ";" +
                                        std::to_string(settings.backgroundColor.b) + "m";
                        } else {
                            frameBuffer += " ";
                        }
                        screenX++;
                    }
                } else {
                    // Empty lines
                    
                    for (int screenX = 0; screenX < availableWidth; screenX++) {
                        int charIdxFromLineStart = scrollOffsetX + screenX;
                        bool isCursorPos = (lineIdx == cursor.y && charIdxFromLineStart == 0 && cursor.x == 0);
                        
                        if (isCursorPos) {
                            cursorScreenRow = screenRow;
                            cursorScreenCol = screenCol + screenX;
                            frameBuffer += "\033[48;2;200;200;200m ";
                            //frameBuffer += "\033[48;2;" + std::to_string(settings.backgroundColor.r) + ";" +
                            //            std::to_string(settings.backgroundColor.g) + ";" +
                            //            std::to_string(settings.backgroundColor.b) + "m";
                        } else {
                            frameBuffer += "\033[48;2;0;0;0m ";
                            //frameBuffer += " ";
                        }
                    }
                }
                
                frameBuffer += "\n";
            }
            
            // === STATUS BAR ===
            frameBuffer += setBackgroundColorStr(settings.headBackgroundColor);
            frameBuffer += setTextColorStr(settings.headTextColor);
            
            
            char statusPosCmd[32];
            snprintf(statusPosCmd, sizeof(statusPosCmd), "\033[%d;1H", winInfo.getHeight());
            frameBuffer += statusPosCmd;
            
            char statusBar[512];
            int totalLines = currentFile.content.size();
            int cursorCharCol = convertByteIdxToCharIdx(currentFile.content[cursor.y], cursor.x) + 1;

            snprintf(statusBar, sizeof(statusBar), "Line %d/%d, Col %d | ScrollY:%d ScrollX:%d", 
                    cursor.y + 1, totalLines, cursorCharCol, scrollOffsetY, scrollOffsetX);

            // style the selection if active
            if (cursor.selection.isActive) {
                // insert the bg color code at the start of the selection, and reset code at the end
                


            }

            std::string statusStr(statusBar);
            statusStr.resize(editorWidth, ' ');
            frameBuffer += statusStr;
            if (cursorScreenRow >= 0 && cursorScreenCol >= 0) {
                char posBuf[32];
                snprintf(posBuf, sizeof(posBuf), "\033[%d;%dH", cursorScreenRow, cursorScreenCol);
                frameBuffer += posBuf;
            }
            // === ATOMIC OUTPUT ===
            frameBuffer += resetColorsStr();
            frameBuffer += "\033[?25h";  // Show cursor
            fflush(stdout);
            printf("%s", frameBuffer.c_str());
        }




};
#endif