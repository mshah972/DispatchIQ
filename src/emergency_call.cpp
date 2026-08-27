//
// Created by Moksh Shah on 8/26/26.
//
#include "dispatchiq/emergency_call.hpp"
#include <iostream>

namespace dispatchiq {
bool enqueueCall(
    std::queue<EmergencyCall>& pendingCalls,
    const EmergencyCall& call
) {
    if (call.reportedSeverity < 1 || call.reportedSeverity > 5) {
        return false;
    }

    pendingCalls.push(call);
    return true;
}

const EmergencyCall* peekNextCall(
    const std::queue<EmergencyCall>& pendingCalls
) {
    if (pendingCalls.empty()) {
        return nullptr;
    }

    return &pendingCalls.front();
}

bool processNextCall(
    std::queue<EmergencyCall>& pendingCalls,
    EmergencyCall& processedCall
) {
    if (pendingCalls.empty()) {
        return false;
    }

    processedCall = pendingCalls.front();
    pendingCalls.pop();
    return true;
}

std::size_t pendingCallCount(
    const std::queue<EmergencyCall>& pendingCalls
) {
    return pendingCalls.size();
}
}