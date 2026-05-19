#include "headers/displayEngine.hpp"
#include "headers/funcutils.hpp"
#include "headers/editorFunctions.hpp"
windowInfo winInfo = windowInfo::getInstance();





int main(){
    Color c;
    c.r = 255;
    c.g = 0;
    c.b = 0;

    printColored(c, "This is red text!");
    printCentered(intToString(winInfo.getWidth()).c_str());
    printCentered(intToString(winInfo.getHeight()).c_str());
    Editor editor;
    editor.drawUI();
}