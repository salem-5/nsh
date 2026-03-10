#pragma once
#include <string>
#include <vector>

class Executer
{
public:
    static void execute(const std::vector<std::string> &tokens);
    static std::string runCommand(std::vector<const char *> argv);
    static std::vector<const char *> stringVectorToCString(const std::vector<std::string> &strVec);
};
