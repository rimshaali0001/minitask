// system_info.h  (Rimsha's part)
// Reads system wide CPU and memory numbers from /proc
#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

struct MemInfo {
    long totalKB = 0;
    long availableKB = 0;
    long usedKB = 0;
    double usedPercent = 0.0;
};

// Reads /proc/meminfo
MemInfo readMemory();

// Reads /proc/stat
// CPU usage needs TWO readings to compare so call getUsagePercent()
// again and again (for example every 1 second) on the SAME object
class CpuMonitor {
public:
    // returns CPU usage in percent (0 to 100)
    double getUsagePercent();

private:
    bool readStat(unsigned long long &total, unsigned long long &idle);
    unsigned long long prevTotal = 0;
    unsigned long long prevIdle = 0;
    bool hasPrev = false;
};

#endif
