#ifndef DEBUGWRITER_HPP
#define DEBUGWRITER_HPP
// allows to write debug info to a file, used for debugging purposes only, not for end users
#include <fstream>
#include <stdio.h>

void writeDebugInfo(const std::string& info , std::string filePath = "debug.log") {
    std::ofstream debugFile(filePath, std::ios::app);
    if (debugFile.is_open()) {
        debugFile << info << std::endl;
        debugFile.close();
    } else {
        fprintf(stderr, "Unable to open %s for writing\n", filePath.c_str());
    }
}





#endif