#ifndef DEBUGWRITER_HPP
#define DEBUGWRITER_HPP
// allows to write debug info to a file, used for debugging purposes only, not for end users
#include <fstream>
#include <stdio.h>


class DebugWriter {
    private:
        std::ofstream debugFile;
        std::string filePath;
        bool isOpen;

    public:
        DebugWriter() : isOpen(false) {}

        ~DebugWriter() {
            if (debugFile.is_open()) {
                debugFile.close();
            }
        }
        DebugWriter(const DebugWriter&) = delete;
        DebugWriter& operator=(const DebugWriter&) = delete;

        DebugWriter(DebugWriter&& other) noexcept 
            : debugFile(std::move(other.debugFile)), 
              filePath(std::move(other.filePath)),
              isOpen(other.isOpen) {
            other.isOpen = false;
        }

        DebugWriter& operator=(DebugWriter&& other) noexcept {
            if (this != &other) {
                if (debugFile.is_open()) {
                    debugFile.close();
                }
                debugFile = std::move(other.debugFile);
                filePath = std::move(other.filePath);
                isOpen = other.isOpen;
                other.isOpen = false;
            }
            return *this;
        }

        bool setFilePath(const std::string& path) {
            if (debugFile.is_open()) {
                debugFile.close();
            }
            filePath = path;
            debugFile.open(filePath, std::ios::app);
            if (debugFile.is_open()) {
                isOpen = true;
                return true;
            } else {
                isOpen = false;
                fprintf(stderr, "Unable to open %s for writing\n", path.c_str());
                return false;
            }
        }

        void write(const std::string& info) {
            if (!isOpen) {
                fprintf(stderr, "File not open. Call setFilePath() first.\n");
                return;
            }
            debugFile << info << std::endl;
            debugFile.flush();  
        }

        bool isFileOpen() const {
            return isOpen;
        }
};


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