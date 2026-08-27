#include "dispatchiq/incident.hpp"
#include "dispatchiq/emergency_call.hpp"
#include <iostream>
#include <vector>

int main() {
    // DIQ-001: Incident Registry MVP
    std::vector<dispatchiq::Incident> incidents;

    dispatchiq::printIncidents(incidents);

    dispatchiq::Incident fireIncident {
       1001,
        "Fire",
        5,
{29.7604, -95.3698}
    };

    dispatchiq::Incident medicalIncident {
        1002,
        "Medical",
        4,
{32.7767, -96.7970}
    };

    dispatchiq::Incident duplicateIncident {
        1001,
        "Fire",
        5,
        {100.101010101, 101.101010101}
    };

    dispatchiq::Incident invalidSeverityIncident {
        1003,
        "Police",
        8,
        {0.0000, 0.0000}
    };

    if (dispatchiq::addIncident(incidents, fireIncident)) {
        std::cout << "Incident " << fireIncident.id << " accepted.\n";
    } else {
        std::cout << "Incident " << fireIncident.id << " rejected.\n";
    }

    if (dispatchiq::addIncident(incidents, medicalIncident)) {
        std::cout << "Incident " << medicalIncident.id << " accepted.\n";
    } else {
        std::cout << "Incident " << medicalIncident.id << " rejected.\n";
    }

    if (dispatchiq::addIncident(incidents, duplicateIncident)) {
        std::cout << "Incident " << duplicateIncident.id << " accepted.\n";
    } else {
        std::cout << "Incident " << duplicateIncident.id << " rejected.\n";
    }

    if (dispatchiq::addIncident(incidents, invalidSeverityIncident)) {
        std::cout << "Incident " << invalidSeverityIncident.id << " accepted.\n";
    } else {
        std::cout << "Incident " << invalidSeverityIncident.id << " rejected.\n";
    }

    dispatchiq::printIncidents(incidents);

    const int searchId1 = 1002;
    const dispatchiq::Incident* foundIncident1 = dispatchiq::findIncidentById(incidents, searchId1);

    if (foundIncident1 != nullptr) {
        std::cout << "Incident " << foundIncident1->id << " found.\n";
    } else {
        std::cout << "Incident " << searchId1 << " not found.\n";
    }

    const int searchId2 = 9999;
    const dispatchiq::Incident* foundIncident2 = dispatchiq::findIncidentById(incidents, searchId2);

    if (foundIncident2 != nullptr) {
        std::cout << "Incident " << foundIncident2->id << " found.\n";
    } else {
        std::cout << "Incident " << searchId2 << " not found.\n";
    }

    // DIQ-002: Emergency Call Intake
    std::cout << "\n=======EMERGENCY CALL INTAKE=======\n\n";
    // TODO 2: Create an empty queue of EmergencyCall objects.
    std::queue<dispatchiq::EmergencyCall> emergencyCalls;
    // TODO 3: Verify the initial count is zero and peeking returns nullptr.
    if (const std::size_t initialCount = dispatchiq::pendingCallCount(emergencyCalls);
        initialCount == 0) {
        std::cout << "Initial count test passed: expected 0, received " << initialCount << ".\n";
    } else {
        std::cout << "Initial count test failed: expected 0, received " << initialCount << ".\n";
    }

    if (dispatchiq::peekNextCall(emergencyCalls) == nullptr) {
        std::cout << "Peek test passed: queue is empty.\n";
    }
    // TODO 4: Create calls 2001, 2002, 2003, and invalid call 2004.
    dispatchiq::EmergencyCall fireCall {
        .callId = 2001,
        .category = "Fire",
        .reportedSeverity = 5,
        .location = {29.7604, -95.3698}
    };

    dispatchiq::EmergencyCall medicalCall {
        .callId = 2002,
        .category = "Medical",
        .reportedSeverity = 3,
        .location = {32.7767, -96.7970}
    };

    dispatchiq::EmergencyCall trafficCall {
        .callId = 2003,
        .category = "Traffic",
        .reportedSeverity = 2,
        .location = {30.2672, -97.7431}
    };

    dispatchiq::EmergencyCall invalidCall {
        .callId = 2004,
        .category = "Police",
        .reportedSeverity = 7,
        .location = {41.8781, -87.6298}
    };
    // TODO 5: Enqueue all four calls and report whether each was accepted.
    if (dispatchiq::enqueueCall(emergencyCalls, fireCall)) {
        std::cout << "Enqueue " << fireCall.callId << " accepted.\n";
    } else {
        std::cout << "Enqueue " << fireCall.callId << " rejected.\n";
    }

    if (dispatchiq::enqueueCall(emergencyCalls, medicalCall)) {
        std::cout << "Enqueue " << medicalCall.callId << " accepted.\n";
    } else {
        std::cout << "Enqueue " << medicalCall.callId << " rejected.\n";
    }

    if (dispatchiq::enqueueCall(emergencyCalls, trafficCall)) {
        std::cout << "Enqueue " << trafficCall.callId << " accepted.\n";
    } else {
        std::cout << "Enqueue " << trafficCall.callId << " rejected.\n";
    }

    if (dispatchiq::enqueueCall(emergencyCalls, invalidCall)) {
        std::cout << "Enqueue " << invalidCall.callId << " accepted.\n";
    } else {
        std::cout << "Enqueue " << invalidCall.callId << " rejected.\n";
    }
    // TODO 6: Verify that three calls are pending and call 2001 is at the front.
    if (const std::size_t pendingCalls = dispatchiq::pendingCallCount(emergencyCalls);
        pendingCalls == 3) {
        std::cout << "Pending calls test passed.\n";
    } else {
        std::cout << "Pending calls test failed.\n";
    }

    if (const dispatchiq::EmergencyCall* frontCall = dispatchiq::peekNextCall(emergencyCalls);
        frontCall != nullptr && frontCall->callId == 2001) {
        std::cout << "Front of queue test passed.\n";
    } else {
        std::cout << "Front of queue test failed.\n";
    }

    if (const std::size_t pendingCalls = dispatchiq::pendingCallCount(emergencyCalls);
        pendingCalls == 3) {
        std::cout << "Pending calls test after peek passed.\n";
    } else {
        std::cout << "Pending calls test after peek failed.\n";
    }
    // TODO 7: Process the calls and confirm FIFO order: 2001, 2002, then 2003.
    const std::vector<int> expectedOrder {
        2001,
        2002,
        2003
    };

    dispatchiq::EmergencyCall processedCall {};
    std::size_t processedCount{0};
    bool fifoPassed{true};

    for (const int expectedId : expectedOrder) {
        // TODO: Process the next call into processedCall.
        const bool processed = dispatchiq::processNextCall(emergencyCalls, processedCall);

        // TODO: If processing returned false:
        // - Report that the queue became empty unexpectedly.
        // - Stop the loop with break.
        if (!processed) {
            std::cout << "Queue became empty unexpectedly.\n";
            fifoPassed = false;
            break;
        }

        ++processedCount;

        // TODO: Compare processedCall.callId with expectedId.
        // - Print a pass message when they match.
        // - Print the expected and actual IDs when they differ.
        if (processedCall.callId == expectedId) {
            std::cout << "FIFO test passed for call "
                      << processedCall.callId << ".\n";
        } else {
            std::cout << "FIFO test failed: expected call "
                      << expectedId << ", received "
                      << processedCall.callId << ".\n";

            fifoPassed = false;
        }

        // TODO: Print the number of calls remaining.
        const std::size_t expectedRemaining =
        expectedOrder.size() - processedCount;

        const std::size_t actualRemaining =
            dispatchiq::pendingCallCount(emergencyCalls);

        if (actualRemaining == expectedRemaining) {
            std::cout << "Queue-size test passed: "
                      << actualRemaining << " call(s) remaining.\n";
        } else {
            std::cout << "Queue-size test failed: expected "
                      << expectedRemaining << ", received "
                      << actualRemaining << ".\n";

            fifoPassed = false;
        }
    }
    // TODO: Verify the queue count is now zero
    const std::size_t finalPendingCount =
    dispatchiq::pendingCallCount(emergencyCalls);

    if (fifoPassed &&
        processedCount == expectedOrder.size() &&
        finalPendingCount == 0) {
        std::cout << "DIQ-002 FIFO processing test passed.\n";
    } else {
        std::cout << "DIQ-002 FIFO processing test failed.\n";
    }
    // TODO 8: Attempt one more process and confirm it returns false.
    const bool processedExtraCall = dispatchiq::processNextCall(emergencyCalls, processedCall);

    const std::size_t countAfterFailedProcess = dispatchiq::pendingCallCount(emergencyCalls);

    if (!processedExtraCall && countAfterFailedProcess == 0) {
        std::cout << "Empty-queue processing test passed.\n";
    } else {
        std::cout << "Empty-queue processing test failed.\n";
    }

    return 0;
}
