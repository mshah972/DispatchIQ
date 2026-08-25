#include "dispatchiq/incident.hpp"
#include <iostream>
#include <vector>

int main() {
    std::vector<dispatchiq::Incident> incidents;

    // Step 1: Verify the empty-registry behavior.
    dispatchiq::printIncidents(incidents);

    // Step 2: Create four Incident objects:
    // - Valid fire incident
    // - Valid medical incident
    // - Duplicate-ID incident
    // - Invalid-severity incident
    //
    // Aggregate: initialization shape:
    // dispatchiq::Incident name {
    //      id,
    //      "category",
    //      severity,
    //      {latitude, longitude}
    // };

    // Fire Incident
    dispatchiq::Incident fireIncident {
       1001,
        "Fire",
        5,
{29.7604, -95.3698}
    };

    // Medical Incident
    dispatchiq::Incident medicalIncident {
        1002,
        "Medical",
        4,
{32.7767, -96.7970}
    };

    // Duplicate-ID
    dispatchiq::Incident duplicateIncident {
        1001,
        "Fire",
        5,
        {100.101010101, 101.101010101}
    };

    // Invalid-Severity
    dispatchiq::Incident invalidSeverityIncident {
        1003,
        "Police",
        8,
        {0.0000, 0.0000}
    };



    // Step 3: Pass each incident to addIncident().
    // Print whether each addition was accepted or rejected.
    if (bool fireInc = dispatchiq::addIncident(incidents, fireIncident)) {
        std::cout << "Incident " << fireIncident.id << " accepted.\n";
    } else {
        std::cout << "Incident " << fireIncident.id << " rejected.\n";
    }

    if (bool medInc = dispatchiq::addIncident(incidents, medicalIncident)) {
        std::cout << "Incident " << medicalIncident.id << " accepted.\n";
    } else {
        std::cout << "Incident " << medicalIncident.id << " rejected.\n";
    }

    if (bool dupInc = dispatchiq::addIncident(incidents, duplicateIncident)) {
        std::cout << "Incident " << duplicateIncident.id << " accepted.\n";
    } else {
        std::cout << "Incident " << duplicateIncident.id << " rejected.\n";
    }

    if (bool invInc = dispatchiq::addIncident(incidents, invalidSeverityIncident)) {
        std::cout << "Incident " << invalidSeverityIncident.id << " accepted.\n";
    } else {
        std::cout << "Incident " << invalidSeverityIncident.id << " rejected.\n";
    }

    // Step 4: Print all accepted incidents.
    dispatchiq::printIncidents(incidents);

    // Step 5: Search for incident 1002.
    // Check the returned pointer before accessing the incident.
    const int searchId1 = 1002;
    const dispatchiq::Incident* foundIncident1 = dispatchiq::findIncidentById(incidents, searchId1);

    if (foundIncident1 != nullptr) {
        std::cout << "Incident " << foundIncident1->id << " found.\n";
    } else {
        std::cout << "Incident " << searchId1 << " not found.\n";
    }

    // Step 6: Search for incident 9999.
    // Confirm that the returned pointer is nullptr.
    const int searchId2 = 9999;
    const dispatchiq::Incident* foundIncident2 = dispatchiq::findIncidentById(incidents, searchId2);

    if (foundIncident2 != nullptr) {
        std::cout << "Incident " << foundIncident2->id << " found.\n";
    } else {
        std::cout << "Incident " << searchId2 << " not found.\n";
    }

    return 0;
}
