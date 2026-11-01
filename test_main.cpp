// test_main.cpp  quick terminal test for Rimsha's part
#include "system_info.h"
#include "signal_control.h"
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>

// reads the state letter (R S D T Z) of a process from /proc/[pid]/stat
static char stateOf(pid_t pid) {
    std::ifstream f("/proc/" + std::to_string(pid) + "/stat");
    std::string data;
    std::getline(f, data);
    size_t pos = data.rfind(')');          // name is inside brackets
    return (pos == std::string::npos) ? '?' : data[pos + 2];
}

int main() {
    std::cout << "--- System numbers ---\n";
    CpuMonitor cpu;
    cpu.getUsagePercent();                 // first call just stores a reading
    for (int i = 0; i < 3; i++) {
        sleep(1);
        MemInfo m = readMemory();
        std::cout << "CPU: " << cpu.getUsagePercent() << " %   "
                  << "Memory: " << m.usedKB / 1024 << " MB of "
                  << m.totalKB / 1024 << " MB (" << m.usedPercent << " %)\n";
    }

    std::cout << "\n--- Signal test on a dummy process ---\n";
    pid_t child = fork();
    if (child == 0) { sleep(100); _exit(0); }   // dummy process that just sleeps

    usleep(200000);
    std::cout << "Dummy PID " << child << " state: " << stateOf(child) << "\n";

    std::cout << "Pause:     " << resultToString(pauseProcess(child)) << "\n";
    usleep(200000);
    std::cout << "State now: " << stateOf(child) << "  (T means stopped)\n";

    std::cout << "Resume:    " << resultToString(resumeProcess(child)) << "\n";
    usleep(200000);
    std::cout << "State now: " << stateOf(child) << "  (S means sleeping)\n";

    std::cout << "Terminate: " << resultToString(terminateProcess(child)) << "\n";
    waitpid(child, nullptr, 0);
    std::cout << "Terminate again: " << resultToString(terminateProcess(child)) << "\n";
    return 0;
}
