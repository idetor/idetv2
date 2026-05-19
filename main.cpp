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
    DebugWriter debugWriter;
    if (debugWriter.setFilePath("/dev/pts/8")) {
        debugWriter.write("Debug message");
    } else {
        fprintf(stderr, "Failed to set debug file path\n");
    }
    hideCursor();
    
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
        debugWriter.write("Key pressed: " + intToString(key));
        // Ctrl + S to save
        if (key == 'S' || key == 's') {
            editor.handleKeyInput(key);
            continue;
        }
        
        // Ctrl + Q to quit - Check if we receive a control sequence
        if (key == 17) { // Ctrl key code
            int32_t nextKey = getKeyboardPress();
            if (nextKey == 81 || nextKey == 113) { // 'Q' or 'q'
                break;
            }
            if (nextKey == 83 || nextKey == 115) { // 'S' or 's'
                editor.saveFile();
            }
            continue;
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
