#ifndef EDITOR_FUNCTIONS_HPP
#define EDITOR_FUNCTIONS_HPP
#include "displayEngine.hpp"
#include "funcutils.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>
#include <filesystem>
#include <chrono>
namespace fs = std::filesystem;
struct SelectionElement {
    int startX;
    int startY;
    int endX;
    int endY;
};

struct cursorElement {
    int x;
    int y;
    std::string mode;
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
    public:
        windowInfo winInfo = windowInfo::getInstance();
                
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
        }

        void drawUI() {
            //heading Line
            clearScreen();
            setBackgroundColor(settings.headBackgroundColor);
            setTextColor(settings.headTextColor);
            printf("Idet v2 - %s \n", currentFile.context.name.c_str());
            
            

            //test so print the terminal width and height
            setBackgroundColor(settings.backgroundColor);
            setTextColor(settings.textColor);
            for (int i = 0; i < (winInfo.getHeight() - 2 ); i++) {
                for (int j = 0; j < winInfo.getWidth(); j++) {
                    printf(".");
                }
                printf("\n");
            }
        }
};
#endif