// signal_control.h
// Mariam's Part: Process Control using Linux Signals

#ifndef SIGNAL_CONTROL_H
#define SIGNAL_CONTROL_H

#include <sys/types.h>
#include <string>

enum class SignalResult {
    Ok,
    NoPermission,
    NoSuchProcess,
    Error
};

// Process control functions
SignalResult terminateProcess(pid_t pid);
SignalResult killProcess(pid_t pid);
SignalResult pauseProcess(pid_t pid);
SignalResult resumeProcess(pid_t pid);

// Convert result into readable message
std::string resultToString(SignalResult r);

#endif