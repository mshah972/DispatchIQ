# pragma once

#include <utility>
#include <vector>
#include <string>
using namespace std;

namespace dispatchiq {

    struct Incident {
        int id;
        string category;
        int severity;
        pair<double, double> location;
    };

    bool addIncident(
        vector<Incident>& incidents,
        const Incident& incident
    );

    const Incident* findIncidentById(
        const vector<Incident>& incident,
        int id
    );

    void printIncidents(
        const vector<Incident>& incidents
    );
}