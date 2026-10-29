#pragma once

#include "dispatchiq/incident.hpp"

#include <cstddef>
#include <map>
#include <set>
#include <string>

namespace dispatchiq {

struct OperationsReport {
    // Map each category string to its incident count.
    std::map<std::string, std::size_t> incidentsByCategory;

    // Store unique integer incident IDs in ascending order.
    std::set<int> recordedIncidentIds;
};

// Returns bool; takes OperationsReport& and const Incident&.
bool recordIncidentForReport(
    OperationsReport& report,
    const Incident& incident
);

// Returns void; takes const OperationsReport&.
void printOperationsReport(
    const OperationsReport& report
);

// Returns std::size_t; takes const OperationsReport&.
std::size_t reportedIncidentCount(
    const OperationsReport& report
);

}