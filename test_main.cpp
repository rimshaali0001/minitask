// test_main.cpp
// Mariam's Part: Testing Process Control and System Monitoring

#include "system_info.h"
#include "signal_control.h"

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#include <unistd.h>
#include <sys/wait.h>
#include <cerrno>


// Read process state from /proc/[pid]/stat
static char stateOf(pid_t pid) {

    std::ifstream file(
        "/proc/" + std::to_string(pid) + "/stat"
    );

    std::string data;

    if (!std::getline(file, data)) {
        return '?';
    }

    // Process name is inside brackets
    size_t pos = data.rfind(')');

    if (pos == std::string::npos || pos + 2 >= data.size()) {
        return '?';
    }

    return data[pos + 2];
}


int main() {

    // --------------------------------
    // TEST 1: SYSTEM MEMORY AND CPU
    // --------------------------------

    std::cout << "\n===== SYSTEM MONITORING =====\n";

    CpuMonitor cpu;

    // First reading establishes baseline
    cpu.getUsagePercent();

    for (int i = 0; i < 3; i++) {

        sleep(1);

        MemInfo memory = readMemory();

        std::cout << std::fixed << std::setprecision(2);

        std::cout << "CPU Usage: "
                  << cpu.getUsagePercent()
                  << " %\n";

        if (memory.totalKB > 0) {

            std::cout << "Total RAM: "
                      << memory.totalKB / 1024
                      << " MB\n";

            std::cout << "Used RAM: "
                      << memory.usedKB / 1024
                      << " MB\n";

            std::cout << "Available RAM: "
                      << memory.availableKB / 1024
                      << " MB\n";

            std::cout << "Memory Usage: "
                      << memory.usedPercent
                      << " %\n";
        }

        else {
            std::cout << "Unable to read memory information.\n";
        }

        std::cout << "-----------------------------\n";
    }


    // --------------------------------
    // TEST 2: PROCESS CONTROL
    // --------------------------------

    std::cout << "\n===== PROCESS CONTROL TEST =====\n";

    pid_t child = fork();

    // Check fork failure
    if (child < 0) {

        std::cerr << "Error: Unable to create child process.\n";

        return 1;
    }

    // Child process
    if (child == 0) {

        while (true) {
            sleep(1);
        }

        _exit(0);
    }

    // Parent process
    std::cout << "Dummy child PID: " << child << "\n";

    usleep(200000);

    std::cout << "Initial state: "
              << stateOf(child) << "\n";


    // Pause process
    std::cout << "\nPause: "
              << resultToString(pauseProcess(child))
              << "\n";

    usleep(200000);

    std::cout << "State after pause: "
              << stateOf(child)
              << " (T means stopped)\n";


    // Resume process
    std::cout << "\nResume: "
              << resultToString(resumeProcess(child))
              << "\n";

    usleep(200000);

    std::cout << "State after resume: "
              << stateOf(child)
              << " (S means sleeping)\n";


    // Terminate process
    std::cout << "\nTerminate: "
              << resultToString(terminateProcess(child))
              << "\n";


    // Wait for child to finish
    int status;

    while (waitpid(child, &status, 0) == -1) {

        if (errno == EINTR) {
            continue;
        }

        std::cerr << "Error while waiting for child process.\n";

        return 1;
    }


    // Try sending signal to terminated process
    std::cout << "\nTerminate again: "
              << resultToString(terminateProcess(child))
              << "\n";


    std::cout << "\n===== ALL TESTS COMPLETED =====\n";

    return 0;
}