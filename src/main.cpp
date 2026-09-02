#include "dispatchiq/incident_index.hpp"

#include <cstddef>
#include <iostream>

int main() {
    std::cout << "=======FAST INCIDENT INDEX=======\n\n";

    bool allTestsPassed{true};
    dispatchiq::IncidentIndex incidentIndex{};

    const std::size_t initialCount =
        dispatchiq::indexedIncidentCount(incidentIndex);

    if (initialCount == 0) {
        std::cout << "Initial count test passed.\n";
    } else {
        std::cout << "Initial count test failed: expected 0, received "
                  << initialCount << ".\n";
        allTestsPassed = false;
    }

    if (const dispatchiq::Incident* missingIncident =
            dispatchiq::findIncidentInIndexById(incidentIndex, 9999);
        missingIncident == nullptr) {
        std::cout << "Empty-index lookup test passed.\n";
    } else {
        std::cout << "Empty-index lookup test failed: expected nullptr, received incident "
                  << missingIncident->id << ".\n";
        allTestsPassed = false;
    }

    const dispatchiq::Incident incident4001 {
        .id = 4001,
        .category = "Fire",
        .severity = 5,
        .location = {29.7604, -95.3698}
    };

    const dispatchiq::Incident incident4002 {
        .id = 4002,
        .category = "Medical",
        .severity = 4,
        .location = {32.7767, -96.7970}
    };

    const dispatchiq::Incident incident4003 {
        .id = 4003,
        .category = "Police",
        .severity = 3,
        .location = {30.2672, -97.7431}
    };

    const dispatchiq::Incident duplicateIncident4001 {
        .id = 4001,
        .category = "Traffic",
        .severity = 1,
        .location = {31.7234, -92.3698}
    };

    const dispatchiq::Incident invalidIncident4004 {
        .id = 4004,
        .category = "Medical",
        .severity = 8,
        .location = {31.7245, -96.7321}
    };

    if (dispatchiq::addIncidentToIndex(incidentIndex, incident4001)) {
        std::cout << "Incident " << incident4001.id << " accepted.\n";
    } else {
        std::cout << "Incident " << incident4001.id
                  << " was unexpectedly rejected.\n";
        allTestsPassed = false;
    }

    if (dispatchiq::addIncidentToIndex(incidentIndex, incident4002)) {
        std::cout << "Incident " << incident4002.id << " accepted.\n";
    } else {
        std::cout << "Incident " << incident4002.id
                  << " was unexpectedly rejected.\n";
        allTestsPassed = false;
    }

    if (dispatchiq::addIncidentToIndex(incidentIndex, incident4003)) {
        std::cout << "Incident " << incident4003.id << " accepted.\n";
    } else {
        std::cout << "Incident " << incident4003.id
                  << " was unexpectedly rejected.\n";
        allTestsPassed = false;
    }

    if (dispatchiq::addIncidentToIndex(incidentIndex, duplicateIncident4001)) {
        std::cout << "Duplicate incident " << duplicateIncident4001.id
                  << " was unexpectedly accepted.\n";
        allTestsPassed = false;
    } else {
        std::cout << "Duplicate incident " << duplicateIncident4001.id
                  << " was rejected as expected.\n";
    }

    if (dispatchiq::addIncidentToIndex(incidentIndex, invalidIncident4004)) {
        std::cout << "Invalid incident " << invalidIncident4004.id
                  << " was unexpectedly accepted.\n";
        allTestsPassed = false;
    } else {
        std::cout << "Invalid incident " << invalidIncident4004.id
                  << " was rejected as expected.\n";
    }

    const std::size_t afterInsertionCount =
        dispatchiq::indexedIncidentCount(incidentIndex);

    if (afterInsertionCount == 3) {
        std::cout << "Insertion count test passed.\n";
    } else {
        std::cout << "Insertion count test failed: expected 3, received "
                  << afterInsertionCount << ".\n";
        allTestsPassed = false;
    }

    if (const dispatchiq::Incident* foundIncident4002 =
            dispatchiq::findIncidentInIndexById(incidentIndex, 4002);
        foundIncident4002 == nullptr) {
        std::cout << "Existing incident lookup test failed: expected incident 4002.\n";
        allTestsPassed = false;
    } else {
        const bool fieldsMatch =
            foundIncident4002->id == incident4002.id &&
            foundIncident4002->category == incident4002.category &&
            foundIncident4002->severity == incident4002.severity &&
            foundIncident4002->location == incident4002.location;

        if (fieldsMatch) {
            std::cout << "Existing incident lookup test passed.\n";
        } else {
            std::cout << "Existing incident lookup test failed: stored fields differ.\n";
            allTestsPassed = false;
        }
    }

    if (const dispatchiq::Incident* missingIncident =
            dispatchiq::findIncidentInIndexById(incidentIndex, 9999);
        missingIncident == nullptr) {
        std::cout << "Missing incident lookup test passed.\n";
    } else {
        std::cout << "Missing incident lookup test failed: expected nullptr, received incident "
                  << missingIncident->id << ".\n";
        allTestsPassed = false;
    }

    if (const dispatchiq::Incident* storedIncident4001 =
            dispatchiq::findIncidentInIndexById(incidentIndex, 4001);
        storedIncident4001 == nullptr) {
        std::cout << "Duplicate protection test failed: incident 4001 is missing.\n";
        allTestsPassed = false;
    } else {
        const bool originalFieldsPreserved =
            storedIncident4001->id == incident4001.id &&
            storedIncident4001->category == incident4001.category &&
            storedIncident4001->severity == incident4001.severity &&
            storedIncident4001->location == incident4001.location;

        if (originalFieldsPreserved) {
            std::cout << "Duplicate protection test passed.\n";
        } else {
            std::cout << "Duplicate protection test failed: original data was overwritten.\n";
            allTestsPassed = false;
        }
    }

    if (dispatchiq::removeIncidentFromIndex(incidentIndex, 4002)) {
        std::cout << "Existing incident removal test passed.\n";
    } else {
        std::cout << "Existing incident removal test failed.\n";
        allTestsPassed = false;
    }

    const std::size_t afterRemovalCount =
        dispatchiq::indexedIncidentCount(incidentIndex);

    if (afterRemovalCount == 2) {
        std::cout << "Post-removal count test passed.\n";
    } else {
        std::cout << "Post-removal count test failed: expected 2, received "
                  << afterRemovalCount << ".\n";
        allTestsPassed = false;
    }

    if (const dispatchiq::Incident* removedIncident =
            dispatchiq::findIncidentInIndexById(incidentIndex, 4002);
        removedIncident == nullptr) {
        std::cout << "Post-removal lookup test passed.\n";
    } else {
        std::cout << "Post-removal lookup test failed: incident "
                  << removedIncident->id << " is still indexed.\n";
        allTestsPassed = false;
    }

    if (dispatchiq::removeIncidentFromIndex(incidentIndex, 9999)) {
        std::cout << "Missing incident removal test failed: expected false, received true.\n";
        allTestsPassed = false;
    } else {
        std::cout << "Missing incident removal test passed.\n";
    }

    const std::size_t afterMissingRemovalCount =
        dispatchiq::indexedIncidentCount(incidentIndex);

    if (afterMissingRemovalCount == 2) {
        std::cout << "Missing-removal count test passed.\n";
    } else {
        std::cout << "Missing-removal count test failed: expected 2, received "
                  << afterMissingRemovalCount << ".\n";
        allTestsPassed = false;
    }

    if (dispatchiq::addIncidentToIndex(incidentIndex, incident4002)) {
        std::cout << "Reinsertion test passed.\n";
    } else {
        std::cout << "Reinsertion test failed: expected true, received false.\n";
        allTestsPassed = false;
    }

    const std::size_t finalCount =
        dispatchiq::indexedIncidentCount(incidentIndex);

    if (finalCount == 3) {
        std::cout << "Final count test passed.\n";
    } else {
        std::cout << "Final count test failed: expected 3, received "
                  << finalCount << ".\n";
        allTestsPassed = false;
    }

    if (allTestsPassed) {
        std::cout << "\nDIQ-004 fast incident index tests passed.\n";
    } else {
        std::cout << "\nDIQ-004 fast incident index tests failed.\n";
    }

    return allTestsPassed ? 0 : 1;
}
