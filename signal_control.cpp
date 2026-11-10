// signal_control.cpp
// Mariam's Part: Sending signals to processes

#include "signal_control.h"

#include <signal.h>
#include <errno.h>

static SignalResult sendSignal(pid_t pid, int sig) {

    // Protect important system processes
    if (pid <= 1) {
        return SignalResult::Error;
    }

    // Send requested signal
    if (kill(pid, sig) == 0) {
        return SignalResult::Ok;
    }

    // Handle errors
    if (errno == EPERM) {
        return SignalResult::NoPermission;
    }

    if (errno == ESRCH) {
        return SignalResult::NoSuchProcess;
    }

    return SignalResult::Error;
}

// Graceful termination
SignalResult terminateProcess(pid_t pid) {
    return sendSignal(pid, SIGTERM);
}

// Forceful termination
SignalResult killProcess(pid_t pid) {
    return sendSignal(pid, SIGKILL);
}

// Pause process
SignalResult pauseProcess(pid_t pid) {
    return sendSignal(pid, SIGSTOP);
}

// Resume process
SignalResult resumeProcess(pid_t pid) {
    return sendSignal(pid, SIGCONT);
}

// Convert result into readable text
std::string resultToString(SignalResult r) {

    switch (r) {

        case SignalResult::Ok:
            return "Signal sent successfully";

        case SignalResult::NoPermission:
            return "Permission denied";

        case SignalResult::NoSuchProcess:
            return "Process not found";

        case SignalResult::Error:
        default:
            return "Something went wrong";
    }
}