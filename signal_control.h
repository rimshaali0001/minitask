// signal_control.h  (Rimsha's part)
// Kill / pause / resume a process using Linux signals
#ifndef SIGNAL_CONTROL_H
#define SIGNAL_CONTROL_H

#include <sys/types.h>
#include <string>

enum class SignalResult {
    Ok,
    NoPermission,     // not your process (need root)
    NoSuchProcess,    // PID does not exist
    Error
};

SignalResult terminateProcess(pid_t pid);  // SIGTERM  polite kill
SignalResult killProcess(pid_t pid);       // SIGKILL  force kill
SignalResult pauseProcess(pid_t pid);      // SIGSTOP  freeze
SignalResult resumeProcess(pid_t pid);     // SIGCONT  unfreeze

// turns a result into text you can show in a message box
std::string resultToString(SignalResult r);

#endif
