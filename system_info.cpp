#include "system_info.h"
#include <fstream>
#include <sstream>
#include <string>

MemInfo readMemory() {
    MemInfo info;
    std::ifstream file("/proc/meminfo");
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string key;
        long value;
        iss >> key >> value;   // example line: "MemTotal:  16384000 kB"
        if (key == "MemTotal:") info.totalKB = value;
        else if (key == "MemAvailable:") info.availableKB = value;
    }
    info.usedKB = info.totalKB - info.availableKB;
    if (info.totalKB > 0)
        info.usedPercent = 100.0 * info.usedKB / info.totalKB;
    return info;
}

bool CpuMonitor::readStat(unsigned long long &total, unsigned long long &idle) {
    std::ifstream file("/proc/stat");
    std::string label;
    unsigned long long user = 0, nice = 0, system = 0, idleT = 0;
    unsigned long long iowait = 0, irq = 0, softirq = 0, steal = 0;
    if (!(file >> label)) return false;          // first word is "cpu"
    file >> user >> nice >> system >> idleT >> iowait >> irq >> softirq >> steal;
    idle = idleT + iowait;
    total = user + nice + system + idleT + iowait + irq + softirq + steal;
    return true;
}

double CpuMonitor::getUsagePercent() {
    unsigned long long total = 0, idle = 0;
    if (!readStat(total, idle)) return 0.0;

    double usage = 0.0;
    if (hasPrev) {
        unsigned long long dTotal = total - prevTotal;
        unsigned long long dIdle = idle - prevIdle;
        if (dTotal > 0)
            usage = 100.0 * (dTotal - dIdle) / dTotal;
    }
    prevTotal = total;
    prevIdle = idle;
    hasPrev = true;
    return usage;   // first call always gives 0 because nothing to compare with
}
