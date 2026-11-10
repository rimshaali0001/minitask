// Mariam's Part: Reading RAM and CPU information from Linux

#include "system_info.h"

#include <fstream>
#include <sstream>
#include <string>

// Read RAM information from /proc/meminfo
MemInfo readMemory() {

    MemInfo info;

    std::ifstream file("/proc/meminfo");

    // Check if file opened successfully
    if (!file.is_open()) {
        return info;
    }

    std::string line;

    while (std::getline(file, line)) {

        std::istringstream iss(line);

        std::string key;
        long value = 0;

        if (!(iss >> key >> value)) {
            continue;
        }

        if (key == "MemTotal:") {
            info.totalKB = value;
        }

        else if (key == "MemAvailable:") {
            info.availableKB = value;
        }
    }

    // Calculate used memory
    if (info.totalKB > 0) {

        info.usedKB = info.totalKB - info.availableKB;

        info.usedPercent =
            100.0 * info.usedKB / info.totalKB;
    }

    return info;
}


// Read CPU statistics from /proc/stat
bool CpuMonitor::readStat(
    unsigned long long &total,
    unsigned long long &idle
) {

    std::ifstream file("/proc/stat");

    if (!file.is_open()) {
        return false;
    }

    std::string label;

    unsigned long long user = 0;
    unsigned long long nice = 0;
    unsigned long long system = 0;
    unsigned long long idleT = 0;
    unsigned long long iowait = 0;
    unsigned long long irq = 0;
    unsigned long long softirq = 0;
    unsigned long long steal = 0;

    // Read first CPU line
    if (!(file >> label)) {
        return false;
    }

    if (label != "cpu") {
        return false;
    }

    if (!(file >> user >> nice >> system >> idleT
               >> iowait >> irq >> softirq >> steal)) {
        return false;
    }

    // Calculate idle and total CPU time
    idle = idleT + iowait;

    total = user + nice + system + idleT
          + iowait + irq + softirq + steal;

    return true;
}


// Calculate CPU usage percentage
double CpuMonitor::getUsagePercent() {

    unsigned long long total = 0;
    unsigned long long idle = 0;

    if (!readStat(total, idle)) {
        return 0.0;
    }

    double usage = 0.0;

    // Compare current reading with previous reading
    if (hasPrev && total >= prevTotal && idle >= prevIdle) {

        unsigned long long dTotal = total - prevTotal;
        unsigned long long dIdle = idle - prevIdle;

        if (dTotal > 0 && dIdle <= dTotal) {

            usage = 100.0 *
                    (dTotal - dIdle) / dTotal;
        }
    }

    // Save current reading for next comparison
    prevTotal = total;
    prevIdle = idle;

    hasPrev = true;

    return usage;
}
