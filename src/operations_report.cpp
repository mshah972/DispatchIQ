//
// Created by Moksh Shah on 9/12/26.
//
#include "dispatchiq/operations_report.hpp"

#include <iostream>

namespace dispatchiq {

bool recordIncidentForReport(OperationsReport& report, const Incident& incident) {
    // - ID must be positive.
    // - Severity must be between 1 and 5.
    // - Category must not be empty.
    // Return false before modifying the report.
    const bool invalidIncidentData = incident.id <= 0 || incident.severity < 1 || incident.severity > 5 || incident.category.empty();

    if (invalidIncidentData) {
        return false;
    }

    // Capture the insertion result.
    const auto insertIdResult = report.recordedIncidentIds.emplace(incident.id);

    // Do not change the category count.
    if (!insertIdResult.second) {
        return false;
    }

    // in incidentsByCategory.
    ++report.incidentsByCategory[incident.category];

    return true;
}

void printOperationsReport(const OperationsReport& report) {
    // If empty:
    // - Print "No incidents have been recorded for this report."
    // - Return early.
    if (report.recordedIncidentIds.empty()) {
        std::cout << "No incidents have been recorded for this report.\n";
        return;
    }

    std::cout << "Incident counts by category: \n";

    // Print each category and its count as:
    // Fire: 2
    for (const auto& incident : report.incidentsByCategory) {
        std::cout << incident.first << ": " << incident.second << "\n";
    }

    std::cout << "\nRecorded incident IDs: \n";

    // Print each ID on its own line.
    for (const int& incidentId : report.recordedIncidentIds) {
        std::cout << incidentId << "\n";
    }
}

std::size_t reportedIncidentCount(const OperationsReport& report) {
    return report.recordedIncidentIds.size();
}

} // namespace dispatchiq