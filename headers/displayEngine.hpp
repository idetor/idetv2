#ifndef DISPLAY_ENGINE_HPP
#define DISPLAY_ENGINE_HPP

#include "standardColors.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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
#endif