#include "process_reader.h"
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

std::string stateToText(char s) {
    switch (s) {
        case 'R': return "Running";
        case 'S': return "Sleeping";
        case 'D': return "Disk sleep";
        case 'T': return "Stopped";
        case 't': return "Tracing stop";
        case 'Z': return "Zombie";
        case 'I': return "Idle";
        default:  return "Unknown";
    }
}

// total CPU time of the whole machine (first line of /proc/stat)
unsigned long long ProcessReader::readTotalCpuTicks() {
    std::ifstream file("/proc/stat");
    std::string label;
    file >> label;                       // "cpu"
    unsigned long long v, total = 0;
    for (int i = 0; i < 8 && (file >> v); i++) total += v;
    return total;
}

// reads /proc/[pid]/stat and /proc/[pid]/status for one process
bool ProcessReader::readOne(int pid, ProcessInfo &p, unsigned long long &procTicks) {
    std::ifstream statFile("/proc/" + std::to_string(pid) + "/stat");
    std::string line;
    if (!std::getline(statFile, line)) return false;   // process already gone

    // name is inside brackets and can contain spaces so find the brackets first
    size_t open = line.find('(');
    size_t close = line.rfind(')');
    if (open == std::string::npos || close == std::string::npos) return false;

    p.pid = pid;
    p.name = line.substr(open + 1, close - open - 1);

    // everything after the closing bracket: state then other numbers
    std::istringstream iss(line.substr(close + 2));
    iss >> p.state;
    p.stateText = stateToText(p.state);

    long skip;
    for (int i = 0; i < 10; i++) iss >> skip;   // skip fields 4 to 13
    unsigned long long utime = 0, stime = 0;
    iss >> utime >> stime;                      // fields 14 and 15
    procTicks = utime + stime;

    // memory (VmRSS) from the status file. Kernel threads have none so 0
    p.memoryKB = 0;
    std::ifstream statusFile("/proc/" + std::to_string(pid) + "/status");
    while (std::getline(statusFile, line)) {
        if (line.compare(0, 6, "VmRSS:") == 0) {
            std::istringstream m(line.substr(6));
            m >> p.memoryKB;
            break;
        }
    }
    return true;
}

std::vector<ProcessInfo> ProcessReader::readAll() {
    std::vector<ProcessInfo> result;
    std::map<int, unsigned long long> newTicks;

    unsigned long long total = readTotalCpuTicks();
    unsigned long long dTotal = (prevTotal > 0 && total > prevTotal) ? total - prevTotal : 0;

    DIR *dir = opendir("/proc");
    if (!dir) return result;

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        // only folders that are all digits are processes
        if (name.empty() || !std::all_of(name.begin(), name.end(), ::isdigit)) continue;
        int pid = std::stoi(name);

        ProcessInfo p;
        unsigned long long ticks = 0;
        if (!readOne(pid, p, ticks)) continue;

        newTicks[pid] = ticks;
        auto old = prevProcTicks.find(pid);
        if (old != prevProcTicks.end() && dTotal > 0 && ticks >= old->second)
            p.cpuPercent = 100.0 * (ticks - old->second) / dTotal;

        result.push_back(p);
    }
    closedir(dir);

    prevTotal = total;
    prevProcTicks = newTicks;      // also removes dead processes
    return result;
}

void sortProcesses(std::vector<ProcessInfo> &list, SortBy by, bool ascending) {
    // returns true if x should come before y when going small to big
    auto isLess = [by](const ProcessInfo &x, const ProcessInfo &y) {
        switch (by) {
            case SortBy::Pid:    return x.pid < y.pid;
            case SortBy::Name:   return x.name < y.name;
            case SortBy::State:  return x.stateText < y.stateText;
            case SortBy::Cpu:    return x.cpuPercent < y.cpuPercent;
            case SortBy::Memory: return x.memoryKB < y.memoryKB;
        }
        return false;
    };
    std::stable_sort(list.begin(), list.end(),
        [&](const ProcessInfo &a, const ProcessInfo &b) {
            return ascending ? isLess(a, b) : isLess(b, a);
        });
}

// finds processes whose name or PID contains the text (ignores capital letters)
std::vector<ProcessInfo> searchProcesses(const std::vector<ProcessInfo> &list,
                                         const std::string &text) {
    auto lower = [](std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    };
    std::string t = lower(text);
    std::vector<ProcessInfo> out;
    for (const auto &p : list)
        if (lower(p.name).find(t) != std::string::npos ||
            std::to_string(p.pid).find(t) != std::string::npos)
            out.push_back(p);
    return out;
}
