#include "signal_control.h"
#include <signal.h>
#include <errno.h>

static SignalResult sendSignal(pid_t pid, int sig) {
    if (pid <= 1) return SignalResult::Error;   // never touch PID 0 or 1 (init)
    if (kill(pid, sig) == 0) return SignalResult::Ok;
    if (errno == EPERM) return SignalResult::NoPermission;
    if (errno == ESRCH) return SignalResult::NoSuchProcess;
    return SignalResult::Error;
}

SignalResult terminateProcess(pid_t pid) { return sendSignal(pid, SIGTERM); }
SignalResult killProcess(pid_t pid)      { return sendSignal(pid, SIGKILL); }
SignalResult pauseProcess(pid_t pid)     { return sendSignal(pid, SIGSTOP); }
SignalResult resumeProcess(pid_t pid)    { return sendSignal(pid, SIGCONT); }

std::string resultToString(SignalResult r) {
    switch (r) {
        case SignalResult::Ok:            return "Done";
        case SignalResult::NoPermission:  return "Permission denied (not your process)";
        case SignalResult::NoSuchProcess: return "Process not found";
        default:                          return "Something went wrong";
    }
}
