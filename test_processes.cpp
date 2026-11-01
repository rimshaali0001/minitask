// test_processes.cpp  quick terminal test for the process reader
#include "process_reader.h"
#include <iostream>
#include <iomanip>
#include <unistd.h>

static void printTable(const std::vector<ProcessInfo> &list, int maxRows) {
    std::cout << std::left << std::setw(8) << "PID" << std::setw(22) << "NAME"
              << std::setw(14) << "STATE" << std::setw(8) << "CPU%"
              << "MEM(MB)\n";
    int n = 0;
    for (const auto &p : list) {
        if (n++ >= maxRows) break;
        std::cout << std::left << std::setw(8) << p.pid
                  << std::setw(22) << p.name.substr(0, 20)
                  << std::setw(14) << p.stateText
                  << std::setw(8) << std::fixed << std::setprecision(1) << p.cpuPercent
                  << p.memoryKB / 1024.0 << "\n";
    }
}

int main() {
    ProcessReader reader;
    reader.readAll();            // first reading just stores old numbers
    sleep(1);
    auto list = reader.readAll();

    std::cout << "Total processes: " << list.size() << "\n\n";

    std::cout << "--- Top 8 by memory ---\n";
    sortProcesses(list, SortBy::Memory, false);
    printTable(list, 8);

    std::cout << "\n--- Top 5 by CPU ---\n";
    sortProcesses(list, SortBy::Cpu, false);
    printTable(list, 5);

    std::cout << "\n--- Search: \"sleep\" ---\n";
    printTable(searchProcesses(list, "sleep"), 5);
    return 0;
}
