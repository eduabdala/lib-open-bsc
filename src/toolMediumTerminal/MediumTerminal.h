#ifndef MEDIUM_TERMINAL_H
#define MEDIUM_TERMINAL_H

/**
 * @file MediumTerminal.h
 * @brief CLI tool handler for communicating with OpenBSC DLL
 * @version 0.2
 * @date 2025-07-30
 * @author Eduardo abdala
 */

class MediumTerminal {
public:
    /**
     * @brief Run the CLI tool
     * @param argc Argument count
     * @param argv Argument values
     * @return Exit code
     */
    int run(int argc, char* argv[]);

private:
    void printUsage(const char* progName) const;
};

#endif  // MEDIUM_TERMINAL_H
