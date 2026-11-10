// system_info.h
// Mariam's Part: System CPU and Memory Monitoring

#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

// Structure to store memory information
struct MemInfo {

    long totalKB = 0;
    long availableKB = 0;
    long usedKB = 0;

    double usedPercent = 0.0;
};

// Read system memory information
MemInfo readMemory();

// Monitor CPU usage
class CpuMonitor {

public:

    // Returns CPU usage percentage (0–100)
    // Call repeatedly on the SAME object
    double getUsagePercent();

private:

    bool readStat(
        unsigned long long &total,
        unsigned long long &idle
    );

    unsigned long long prevTotal = 0;
    unsigned long long prevIdle = 0;

    bool hasPrev = false;
};

#endif