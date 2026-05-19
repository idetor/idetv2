#ifndef DISPLAY_ENGINE_HPP
#define DISPLAY_ENGINE_HPP

#include "standardColors.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

// Forward declarations
int getTerminalWidth();
int getTerminalHeight();

class windowInfo{
    private:
        int width;
        int height;
        windowInfo(int w, int h) : width(w), height(h) {}
    public:
        static windowInfo getInstance() {
            static windowInfo instance(getTerminalWidth(), getTerminalHeight());
            return instance;
        }
        int getWidth() { return width; }
        int getHeight() { return height; }
    
};

void printColored(Color c, const char* text = "Hello World"){
    printf("\033[38;2;%d;%d;%dm%s\033[0m\n", c.r, c.g, c.b, text);
}
void clearScreen(){
    printf("\033[2J\033[H");
    fflush(stdout);
}
std::string clearScreenStr() {
    return "\033[2J\033[H";
}
void hideCursor() {
    printf("\033[?25l");
    fflush(stdout);
}

void showCursor() {
    printf("\033[?25h");
    fflush(stdout);
}

void setCursorPosition(int row, int col) {
    printf("\033[%d;%dH", row + 1, col + 1);
    fflush(stdout);
}

std::string getBackgroundColorCode(Color c) {
    char colorCode[32];
    snprintf(colorCode, sizeof(colorCode), "\033[48;2;%d;%d;%dm", c.r, c.g, c.b);
    return colorCode;  
}

std::string getTextColorCode(Color c) {
    char colorCode[32];
    snprintf(colorCode, sizeof(colorCode), "\033[38;2;%d;%d;%dm", c.r, c.g, c.b);
    return colorCode;  
}
std::string setBackgroundColorStr(Color c) {
    char colorCode[32];
    snprintf(colorCode, sizeof(colorCode), "\033[48;2;%d;%d;%dm", c.r, c.g, c.b);
    return std::string(colorCode);
}
std::string setTextColorStr(Color c) {
    char colorCode[32];
    snprintf(colorCode, sizeof(colorCode), "\033[38;2;%d;%d;%dm", c.r, c.g, c.b);
    return std::string(colorCode);
}
std::string resetColorsStr() {
    return "\033[0m";
}

#ifdef _WIN32
    #include <windows.h>
    
    int getTerminalHeight() {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
    int getTerminalWidth() {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
    
    int getTerminalHeight() {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        return w.ws_row;
    }
    int getTerminalWidth() {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        return w.ws_col;
    }
#endif
void printCentered(const char* text){
    int width = getTerminalWidth();
    int padding = (width - strlen(text)) / 2;
    printf("%*s%s\n", padding, "", text);
}
void setBackgroundColor(Color c){
    printf("\033[48;2;%d;%d;%dm", c.r, c.g, c.b);
}
void setTextColor(Color c){
    printf("\033[38;2;%d;%d;%dm", c.r, c.g, c.b);
}
void resetColors(){
    printf("\033[0m");
}




class ScreenBuffer{
    // reduces printf calls by buffering output and printing it all at once
    private:
        char* buffer;
        size_t bufferSize;
        size_t contentLength;
    public:
        ScreenBuffer(size_t size) : bufferSize(size), contentLength(0) {
            buffer = (char*)malloc(size);
            buffer[0] = '\0';
        }
        ~ScreenBuffer() {
            free(buffer);
        }
        void append(const char* text) {
            size_t textLen = strlen(text);
            if (contentLength + textLen < bufferSize) {
                strcat(buffer, text);
                contentLength += textLen;
            }
        }
        void print() {
            printf("%s", buffer);
            fflush(stdout);
            contentLength = 0;
            buffer[0] = '\0';
        }
        void clear() {
            contentLength = 0;
            buffer[0] = '\0';
        }
        void set(size_t index, char c) {
            if (index < bufferSize) {
                buffer[index] = c;
                if (index >= contentLength) {
                    contentLength = index + 1;
                    buffer[contentLength] = '\0';
                }
            }
        }
        void setText(size_t index, const char* text) {
            size_t textLen = strlen(text);
            if (index + textLen < bufferSize) {
                memcpy(buffer + index, text, textLen);
                if (index + textLen > contentLength) {
                    contentLength = index + textLen;
                    buffer[contentLength] = '\0';
                }
            }
        }
        void setTxtColor(size_t index, Color c) {
            char colorCode[32];
            snprintf(colorCode, sizeof(colorCode), "\033[38;2;%d;%d;%dm", c.r, c.g, c.b);
            setText(index, colorCode);
        }
        void insertColorTag(size_t index, Color c) {
            char colorCode[32];
            snprintf(colorCode, sizeof(colorCode), "\033[38;2;%d;%d;%dm", c.r, c.g, c.b);
            setText(index, colorCode);
        }
        void changeColorTo(Color c){
            // sets the following text color, until next color change or reset
            char colorCode[32];
            snprintf(colorCode, sizeof(colorCode), "\033[38;2;%d;%d;%dm", c.r, c.g, c.b);
            append(colorCode);
        }
        void changeBackgroundColorTo(Color c){
            // sets the following background color, until next color change or reset
            char colorCode[32];
            snprintf(colorCode, sizeof(colorCode), "\033[48;2;%d;%d;%dm", c.r, c.g, c.b);
            append(colorCode);
        }

};





#endif