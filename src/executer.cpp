#include "executer.hpp"
#include "builtins.hpp"
#include "unistd.h"
#include "sys/wait.h"
#include <iostream>
#include <vector>

std::vector<const char *> stringVectorToCString(const std::vector<std::string> &strVec) {
    std::vector<const char *> cStrVec;
    for (const std::string &str : strVec)
        cStrVec.push_back(str.c_str());
    cStrVec.push_back(nullptr);
    return cStrVec;
}

void Executer::execute(const std::vector<std::string> &tokens) {
    if (Builtins::handle(tokens)) return;
    // equivalent of java var keyword for nicer readability.
    auto argv = stringVectorToCString(tokens);

    pid_t pid = fork();
    if (pid < 0) // fork failed
        std::cerr << tokens[0] << ": failed to execute command" << std::endl;
    else if (pid == 0) { // child proces
        int status = execvp(argv[0], const_cast<char *const *>(argv.data()));
        if (status != 0) {
            std::string msg = "failed to execute command";
            if (errno == ENOENT)
                msg = "command not found";
            std::cerr << tokens[0] << ": " << msg << std::endl;
        }
    }
    else // parent process (pid > 0)
        waitpid(pid, nullptr, 0);
}
