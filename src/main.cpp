#include "dispatchiq/incident_triage.hpp"

#include <cstddef>
#include <iostream>
#include <vector>

int main() {
    std::cout << "=======INCIDENT TRIAGE=======\n\n";

    bool allTestsPassed{true};
    dispatchiq::IncidentTriage triage{};

    const std::size_t initialCount = dispatchiq::pendingTriageCount(triage);
    if (initialCount == 0) {
        std::cout << "Initial count test passed.\n";
    } else {
        std::cout << "Initial count test failed: expected 0, received "
                  << initialCount << ".\n";
        allTestsPassed = false;
    }

    if (dispatchiq::peekHighestPriorityIncident(triage) == nullptr) {
        std::cout << "Highest-priority peek test passed.\n";
    } else {
        std::cout << "Highest-priority peek test failed: expected nullptr.\n";
        allTestsPassed = false;
    }

    const dispatchiq::Incident incident3001 {
        .id = 3001,
        .category = "Police",
        .severity = 2,
        .location = {0.0, 0.0}
    };

    const dispatchiq::Incident incident3002 {
        .id = 3002,
        .category = "Medical",
        .severity = 5,
        .location = {0.0, 0.0}
    };

    const dispatchiq::Incident incident3003 {
        .id = 3003,
        .category = "Traffic",
        .severity = 3,
        .location = {0.0, 0.0}
    };

    const dispatchiq::Incident incident3004 {
        .id = 3004,
        .category = "Fire",
        .severity = 5,
        .location = {0.0, 0.0}
    };

    const dispatchiq::Incident incident3005 {
        .id = 3005,
        .category = "Traffic",
        .severity = 8,
        .location = {0.0, 0.0}
    };

    if (dispatchiq::addIncidentToTriage(triage, incident3001)) {
        std::cout << "Incident " << incident3001.id << " was accepted.\n";
    } else {
        std::cout << "Incident " << incident3001.id << " was rejected.\n";
        allTestsPassed = false;
    }

    if (dispatchiq::addIncidentToTriage(triage, incident3002)) {
        std::cout << "Incident " << incident3002.id << " was accepted.\n";
    } else {
        std::cout << "Incident " << incident3002.id << " was rejected.\n";
        allTestsPassed = false;
    }

    if (dispatchiq::addIncidentToTriage(triage, incident3003)) {
        std::cout << "Incident " << incident3003.id << " was accepted.\n";
    } else {
        std::cout << "Incident " << incident3003.id << " was rejected.\n";
        allTestsPassed = false;
    }

    if (dispatchiq::addIncidentToTriage(triage, incident3004)) {
        std::cout << "Incident " << incident3004.id << " was accepted.\n";
    } else {
        std::cout << "Incident " << incident3004.id << " was rejected.\n";
        allTestsPassed = false;
    }

    if (dispatchiq::addIncidentToTriage(triage, incident3005)) {
        std::cout << "Incident " << incident3005.id << " was accepted.\n";
        allTestsPassed = false;
    } else {
        std::cout << "Incident " << incident3005.id << " was rejected.\n";
    }

    const std::size_t afterInsertionCount = dispatchiq::pendingTriageCount(triage);
    if (afterInsertionCount == 4) {
        std::cout << "Insertion count test passed.\n";
    } else {
        std::cout << "Insertion count test failed: expected 4, received "
                  << afterInsertionCount << ".\n";
        allTestsPassed = false;
    }

    if (triage.nextArrivalSequence == 4) {
        std::cout << "Arrival-sequence test passed.\n";
    } else {
        std::cout << "Arrival-sequence test failed: expected 4, received "
                  << triage.nextArrivalSequence << ".\n";
        allTestsPassed = false;
    }

    const dispatchiq::Incident* highestPriorityIncident =
        dispatchiq::peekHighestPriorityIncident(triage);

    if (highestPriorityIncident == nullptr) {
        std::cout << "Highest-priority incident test failed: expected incident 3002.\n";
        allTestsPassed = false;
    } else if (highestPriorityIncident->id == 3002) {
        std::cout << "Highest-priority incident test passed.\n";
    } else {
        std::cout << "Highest-priority incident test failed: expected 3002, received "
                  << highestPriorityIncident->id << ".\n";
        allTestsPassed = false;
    }

    const std::size_t afterPeekCount = dispatchiq::pendingTriageCount(triage);
    if (afterPeekCount == 4) {
        std::cout << "Peek count test passed.\n";
    } else {
        std::cout << "Peek count test failed: expected 4, received "
                  << afterPeekCount << ".\n";
        allTestsPassed = false;
    }

    const std::vector<int> expectedIds {
        3002, 3004, 3003, 3001
    };

    dispatchiq::Incident outputIncident{};
    std::size_t dispatchedCount{};
    bool dispatchTestsPassed{true};

    for (const int expectedId : expectedIds) {
        const bool dispatched =
            dispatchiq::dispatchHighestPriorityIncident(triage, outputIncident);

        if (!dispatched) {
            std::cout << "Dispatch failed: triage became empty unexpectedly.\n";
            dispatchTestsPassed = false;
            break;
        }

        ++dispatchedCount;

        if (outputIncident.id != expectedId) {
            std::cout << "Dispatch order test failed: expected " << expectedId
                      << ", received " << outputIncident.id << ".\n";
            dispatchTestsPassed = false;
        }

        const std::size_t expectedRemaining = expectedIds.size() - dispatchedCount;
        const std::size_t actualRemaining = dispatchiq::pendingTriageCount(triage);

        if (actualRemaining != expectedRemaining) {
            std::cout << "Dispatch count test failed: expected " << expectedRemaining
                      << ", received " << actualRemaining << ".\n";
            dispatchTestsPassed = false;
        }
    }

    if (dispatchTestsPassed && dispatchedCount == expectedIds.size()) {
        std::cout << "Dispatch priority test passed.\n";
    } else {
        std::cout << "Dispatch priority test failed.\n";
        allTestsPassed = false;
    }

    const std::size_t finalCount = dispatchiq::pendingTriageCount(triage);
    if (finalCount == 0) {
        std::cout << "Final empty-state test passed.\n";
    } else {
        std::cout << "Final empty-state test failed: expected 0, received "
                  << finalCount << ".\n";
        allTestsPassed = false;
    }

    if (dispatchiq::peekHighestPriorityIncident(triage) == nullptr) {
        std::cout << "Final peek test passed.\n";
    } else {
        std::cout << "Final peek test failed: expected nullptr.\n";
        allTestsPassed = false;
    }

    dispatchiq::Incident finalIncident{};
    const bool finalDispatch =
        dispatchiq::dispatchHighestPriorityIncident(triage, finalIncident);
    const std::size_t countAfterFinalDispatch = dispatchiq::pendingTriageCount(triage);

    if (!finalDispatch && countAfterFinalDispatch == 0) {
        std::cout << "Final dispatch test passed.\n";
    } else {
        std::cout << "Final dispatch test failed.\n";
        allTestsPassed = false;
    }

    if (allTestsPassed) {
        std::cout << "\nDIQ-003 incident triage tests passed.\n";
    } else {
        std::cout << "\nDIQ-003 incident triage tests failed.\n";
    }

    return allTestsPassed ? 0 : 1;
}
