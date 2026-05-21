#include "headers/displayEngine.hpp"
#include "headers/funcutils.hpp"
#include "headers/editorFunctions.hpp"
#include "headers/keypress.hpp"
#include "headers/debugwriter.hpp"

#if defined(__linux__)
    #define PLATFORM_LINUX
extern void resetTerminal();
#endif

windowInfo winInfo = windowInfo::getInstance();

int main(int argc, char* argv[]){
    hideCursor();
    writeToDebugChannel("");
    Editor editor;
    // use arg1 as file path
    if (argc > 1) {
        editor.openFile(argv[1]);
    } else {
        editor.openFile("untitled.txt");
    }
    
    while (true) {
        editor.drawUI();
        int32_t key = getKeyboardPress();
        writeToDebugChannel("Key presed: " + intToString(key));
        
        if ( key == 17 ) {
            if (editor.checkQuit()){
                // later add popup if not saved
                break;
            }
            else{
                return 0;}
            }
        editor.handleKeyInput(key);
    }
    
    showCursor();
    clearScreen();
    
    #ifdef PLATFORM_LINUX
    resetTerminal();
    #endif
    
    return 0;
}
