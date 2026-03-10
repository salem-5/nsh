#include "executer.hpp"
#include "builtins.hpp"
#include "unistd.h"
#include "sys/wait.h"
#include <iostream>
#include <vector>

std::vector<const char *> Executer::stringVectorToCString(const std::vector<std::string> &strVec) {
    std::vector<const char *> cStrVec;
    for (const std::string &str : strVec)
        cStrVec.push_back(str.c_str()); // lifetime of new vector must be same as old vector to avoid nullptr reference as c_str() returns a pointer to original strings internal c string.
    cStrVec.push_back(nullptr);
    return cStrVec;
}

std::string Executer::runCommand(std::vector<const char *> argv) { //
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        std::cerr << argv[0] << ": failed to create pipe" << std::endl;
        return "";
    }

    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << argv[0] << ": failed to fork" << std::endl;
        return "";
    }

    if (pid == 0) { // child procccess
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO); // this redirect the stderr and stdout streams
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[1]);

        int status = execvp(argv[0], const_cast<char *const *>(argv.data()));
        if (status != 0) {
            std::string msg = "failed to execute command";
            if (errno == ENOENT)
                msg = "command not found ";
            std::cerr << argv[0] << ": " << msg << std::endl;
        }
        _exit(1);
    }
    close(pipefd[1]);
    char buffer[128];
    std::string output;
    ssize_t count;

    while ((count = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
        output.append(buffer, count);
    }

    close(pipefd[0]);

    int status;
    waitpid(pid, &status, 0); // wait for to finish executing
    return output; //return the output stream of stderr and stout as a string

}

void Executer::execute(const std::vector<std::string> &tokens) {
    if (Builtins::handle(tokens)) return;

    // auto is equivalent of java var keyword for nicer readability.
    auto argv = stringVectorToCString(tokens);

    std::string output = runCommand(argv);
    std::cout << output << std::endl;
}
