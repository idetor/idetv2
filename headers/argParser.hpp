#ifndef ARGPARSER_HPP
#define ARGPARSER_HPP

#include <string>
#include <vector>
#include <map>

class ArgParser {
    public:

        static std::vector<std::string> parseArgs(int argc, char* argv[]) {
            std::vector<std::string> args;
            for (int i = 1; i < argc; ++i) {
                args.push_back(std::string(argv[i]));
            }
            return args;
        }
};

class ArgParserWithFlags {
    public:

        static std::vector<std::string> parseArgs(int argc, char* argv[], 
                                                   std::vector<std::string>& flags) {
            std::vector<std::string> args;
            for (int i = 1; i < argc; ++i) {
                std::string arg(argv[i]);
                if (!arg.empty() && arg[0] == '-') {
                    flags.push_back(arg);
                } else {
                    args.push_back(arg);
                }
            }
            return args;
        }
};

class ArgParserAdvanced {
    public:
        struct ParseResult {
            std::vector<std::string> arguments;
            std::map<std::string, std::string> flags;
            std::vector<std::string> rawFlags;
        };


        static ParseResult parseArgs(int argc, char* argv[]) {
            ParseResult result;
            
            for (int i = 1; i < argc; ++i) {
                std::string arg(argv[i]);
                
                if (!arg.empty() && arg[0] == '-') {
                    
                    result.rawFlags.push_back(arg);
                    
                    
                    size_t eqPos = arg.find('=');
                    if (eqPos != std::string::npos) {
                        std::string key = arg.substr(0, eqPos);
                        std::string value = arg.substr(eqPos + 1);
                        result.flags[key] = value;
                    } else {
                        result.flags[arg] = "";
                    }
                } else {
                    
                    result.arguments.push_back(arg);
                }
            }
            
            return result;
        }
};

#endif
