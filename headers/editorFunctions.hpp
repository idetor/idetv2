#ifndef EDITOR_FUNCTIONS_HPP
#define EDITOR_FUNCTIONS_HPP
#include "displayEngine.hpp"
#include "funcutils.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>

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
class File {
    FileInfos context;
    std::vector<std::string> content;
    
};

class Editor {
    std::vector<File> openFiles;
    File* currentFile;
    cursorElement cursor;
    editorSettings settings;
    public:
        windowInfo winInfo = windowInfo::getInstance();
        void
    drawUI() {
        //heading Line
        clearScreen();
        setBackgroundColor(settings.headBackgroundColor);
        setTextColor(settings.headTextColor);
        printf("Idet v2");
        

        //test so print the terminal width and height
        setBackgroundColor(settings.backgroundColor);
        setTextColor(settings.textColor);
        for (int i = 0; i < (winInfo.getHeight() - 1 ); i++) {
            for (int j = 0; j < winInfo.getWidth(); j++) {
                printf(".");
            }
            printf("\n");
        }
    }
};
#endif