// process_reader.h  (Part 1 Backend)
// Reads the list of running processes from /proc
#ifndef PROCESS_READER_H
#define PROCESS_READER_H

#include <string>
#include <vector>
#include <map>

struct ProcessInfo {
    int pid = 0;
    std::string name;
    char state = '?';          // R S D T Z ...
    std::string stateText;     // Running Sleeping Stopped Zombie ...
    double cpuPercent = 0.0;   // percent of the whole machine
    long memoryKB = 0;         // RAM used by this process
};

enum class SortBy { Pid, Name, State, Cpu, Memory };

class ProcessReader {
public:
    // Returns every running process. Call it again and again (every 1 second)
    // on the SAME object. CPU needs two readings so the first call gives 0
    std::vector<ProcessInfo> readAll();

private:
    unsigned long long readTotalCpuTicks();
    bool readOne(int pid, ProcessInfo &p, unsigned long long &procTicks);

    unsigned long long prevTotal = 0;
    std::map<int, unsigned long long> prevProcTicks;   // pid -> old ticks
};

// helpers for the table
void sortProcesses(std::vector<ProcessInfo> &list, SortBy by, bool ascending);
std::vector<ProcessInfo> searchProcesses(const std::vector<ProcessInfo> &list,
                                         const std::string &text);

// "R" -> "Running" and so on
std::string stateToText(char state);

#endif
