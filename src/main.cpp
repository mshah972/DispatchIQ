#include "dispatchiq/operations_report.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

int main() {
    std::cout << "=======ORDERED OPERATIONS REPORT=======\n\n";

    bool allTestsPassed{true};

    const auto check = [&allTestsPassed](bool passed, std::string_view testName) {
        if (passed) {
            std::cout << testName << " passed.\n";
        } else {
            std::cout << testName << " failed.\n";
            allTestsPassed = false;
        }
    };

    dispatchiq::OperationsReport report{};

    check(
        dispatchiq::reportedIncidentCount(report) == 0 &&
            report.incidentsByCategory.empty() &&
            report.recordedIncidentIds.empty(),
        "Initial empty-state test"
    );

    std::cout << "\nEmpty report output:\n";
    dispatchiq::printOperationsReport(report);
    std::cout << "\n";

    const std::array<dispatchiq::Incident, 4> incidents{{
        {6003, "Fire", 5, {29.7604, -95.3698}},
        {6001, "Medical", 4, {32.7767, -96.7970}},
        {6004, "Police", 1, {30.2672, -97.7431}},
        {6002, "Fire", 3, {29.7610, -95.3700}},
    }};

    bool allValidIncidentsAccepted{true};
    for (const dispatchiq::Incident& incident : incidents) {
        const bool accepted = dispatchiq::recordIncidentForReport(report, incident);
        std::cout << "Incident " << incident.id
                  << (accepted ? " accepted.\n" : " unexpectedly rejected.\n");
        if (!accepted) {
            allValidIncidentsAccepted = false;
        }
    }
    check(allValidIncidentsAccepted, "Valid incident recording test");

    check(
        dispatchiq::reportedIncidentCount(report) == 4,
        "Reported incident count test"
    );
    check(
        report.incidentsByCategory.size() == 3,
        "Unique category count test"
    );

    const auto categoryCountIs = [&report](
        const std::string& category,
        std::size_t expectedCount
    ) {
        const auto entry = report.incidentsByCategory.find(category);
        return entry != report.incidentsByCategory.end() &&
               entry->second == expectedCount;
    };

    check(
        categoryCountIs("Fire", 2) &&
            categoryCountIs("Medical", 1) &&
            categoryCountIs("Police", 1),
        "Per-category incident counts test"
    );

    bool allExpectedIdsPresent{true};
    for (const int expectedId : {6001, 6002, 6003, 6004}) {
        if (report.recordedIncidentIds.find(expectedId) ==
            report.recordedIncidentIds.end()) {
            allExpectedIdsPresent = false;
        }
    }
    check(allExpectedIdsPresent, "Recorded incident ID membership test");

    const auto categoriesBeforeDuplicate = report.incidentsByCategory;
    const auto idsBeforeDuplicate = report.recordedIncidentIds;
    const dispatchiq::Incident duplicateIncident{
        6003,
        "Traffic",
        2,
        {31.7234, -92.3698}
    };
    check(
        !dispatchiq::recordIncidentForReport(report, duplicateIncident),
        "Duplicate incident rejection test"
    );
    check(
        report.incidentsByCategory == categoriesBeforeDuplicate &&
            report.recordedIncidentIds == idsBeforeDuplicate,
        "Duplicate leaves report unchanged test"
    );
    check(
        categoryCountIs("Fire", 2) &&
            report.incidentsByCategory.find("Traffic") ==
                report.incidentsByCategory.end(),
        "Duplicate category-count protection test"
    );

    const std::array<dispatchiq::Incident, 5> invalidIncidents{{
        {0, "Medical", 3, {32.7767, -96.7970}},
        {-5, "Medical", 3, {32.7767, -96.7970}},
        {6005, "Medical", 0, {32.7767, -96.7970}},
        {6006, "Medical", 6, {32.7767, -96.7970}},
        {6007, "", 3, {32.7767, -96.7970}},
    }};

    bool allInvalidIncidentsRejectedWithoutMutation{true};
    for (const dispatchiq::Incident& invalidIncident : invalidIncidents) {
        const auto categoriesBeforeAttempt = report.incidentsByCategory;
        const auto idsBeforeAttempt = report.recordedIncidentIds;

        const bool accepted =
            dispatchiq::recordIncidentForReport(report, invalidIncident);
        const bool reportUnchanged =
            report.incidentsByCategory == categoriesBeforeAttempt &&
            report.recordedIncidentIds == idsBeforeAttempt;

        std::cout << "Invalid incident " << invalidIncident.id
                  << ((!accepted && reportUnchanged)
                          ? " correctly rejected without changing the report.\n"
                          : " test failed.\n");

        if (accepted || !reportUnchanged) {
            allInvalidIncidentsRejectedWithoutMutation = false;
        }
    }
    check(
        allInvalidIncidentsRejectedWithoutMutation,
        "Invalid incident validation test"
    );

    const std::vector<std::string> expectedCategories{
        "Fire",
        "Medical",
        "Police"
    };
    std::vector<std::string> actualCategories;
    for (const auto& [category, count] : report.incidentsByCategory) {
        (void)count;
        actualCategories.push_back(category);
    }
    check(
        actualCategories == expectedCategories,
        "Map category ordering test"
    );

    const std::vector<int> expectedIds{6001, 6002, 6003, 6004};
    const std::vector<int> actualIds(
        report.recordedIncidentIds.begin(),
        report.recordedIncidentIds.end()
    );
    check(actualIds == expectedIds, "Set incident ID ordering test");

    const auto categoriesBeforePrinting = report.incidentsByCategory;
    const auto idsBeforePrinting = report.recordedIncidentIds;
    std::cout << "\nPopulated operations report:\n";
    dispatchiq::printOperationsReport(report);
    check(
        report.incidentsByCategory == categoriesBeforePrinting &&
            report.recordedIncidentIds == idsBeforePrinting,
        "Printing leaves report unchanged test"
    );

    if (allTestsPassed) {
        std::cout << "\nDIQ-006 ordered operations report tests passed.\n";
        return 0;
    }

    std::cout << "\nDIQ-006 ordered operations report tests failed.\n";
    return 1;
}
