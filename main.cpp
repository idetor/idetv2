#include "headers/displayEngine.hpp"
#include "headers/funcutils.hpp"
#include "headers/editorFunctions.hpp"
#include "headers/keypress.hpp"
#include "headers/debugwriter.hpp"
windowInfo winInfo = windowInfo::getInstance();



int main(int argc, char* argv[]){
    Color c;
    c.r = 255;
    c.g = 0;
    c.b = 0;

    printColored(c, "This is red text!");
    printCentered(intToString(winInfo.getWidth()).c_str());
    printCentered(intToString(winInfo.getHeight()).c_str());
    Editor editor;
    // use arg1 as file path
    if (argc > 1) {
        editor.openFile(argv[1]);
    } else {
        editor.openFile("untitled.txt");
    }
    while (true) {
        editor.drawUI();
        int key = getKeyboardPress();
        // Strg + q to quit
        writeDebugInfo("Key pressed: " + intToString(key));
        if (key == 17) { // Ctrl key
            int nextKey = getKeyboardPress();
            if (nextKey == 81 || nextKey == 113) { // 'q' or 'Q'
                break;
                          }
        }
    }
}
