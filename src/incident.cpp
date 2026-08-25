//
// Created by Moksh Shah on 8/24/26.
//
#include "dispatchiq/incident.hpp"
#include <iostream>

namespace dispatchiq {

/* addIncident:
 *   Rejects invalid severity values.
 *   Rejects duplicates IDs
 *   Preserves the complete incident
 *   Returns the correct success status.
 *   Runs in *O(n)* because of duplicate searching.
 */
bool addIncident(
    std::vector<Incident>& incidents,
    const Incident& incident
) {
    if (incident.severity < 1 || incident.severity > 5) {
        return false;
    }

    for (const auto& existing : incidents) {
        if (existing.id == incident.id) {
            return false;
        }
    }

    incidents.push_back(incident);

    return true;
}
/*  findIncidentById:
 *    Best case: O(1)
 *    Worst case: O(n)
 */
const Incident* findIncidentById(
    const std::vector<Incident>& incidents,
    int id
) {
    for (const auto& incident : incidents) {
        if (incident.id == id) {
            return &incident;
        }
    }
    return nullptr;
}

void printIncidents(
    const std::vector<Incident>& incidents
) {
    if (incidents.empty()) {
        std::cout << "No incidents have been recorded.\n";
        return;
    }

    for (const auto& incident: incidents) {
        std::cout << incident.id << ". " << incident.category << " - " << incident.severity
        << " - (" << incident.location.first << ", " << incident.location.second << ")" << "\n";
    }
}
}
