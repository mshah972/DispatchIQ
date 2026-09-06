# DispatchIQ

DispatchIQ is a C++20 emergency-response dispatch simulator built to demonstrate practical
data-structure selection. The project begins with an in-memory incident registry and will grow
into a dispatch workflow with intake queues, priority-based triage, responder lookup, route
management, reporting, and undo support.

## Current status

Milestones **DIQ-001: Incident Registry MVP** through **DIQ-005: Event Deduplication** are
implemented. The project can:

- represent incident coordinates with `std::pair<double, double>`;
- preserve incident arrival order in `std::vector<Incident>`;
- accept incidents whose severity is between 1 and 5;
- reject duplicate incident IDs;
- locate an incident with a linear ID search;
- print accepted incidents and handle an empty registry;
- accept validated emergency calls into a `std::queue`;
- inspect the oldest pending call without removing it;
- process calls in first-in, first-out order while handling an empty queue safely;
- prioritize incidents by severity with a `std::priority_queue`;
- preserve arrival order when incidents have equal severity;
- safely peek, dispatch, and count prioritized incidents;
- index active incidents by ID with `std::unordered_map<int, Incident>`;
- reject duplicate index entries without overwriting the original incident;
- find and remove indexed incidents with average constant-time operations;
- track processed event IDs with `std::unordered_set<int>`;
- reject duplicate and non-positive event IDs without changing deduplication state; and
- query, forget, and re-record event IDs with average constant-time operations.

The command-line program is currently a deterministic DIQ-005 acceptance harness. Interactive
input, persistence, and automated tests are planned for later milestones.

## Requirements

- A C++20-compatible compiler such as Apple Clang, Clang, GCC, or MSVC
- CMake 3.20 or newer for the recommended build workflow

## Build and run

### CMake

From the project root:

```bash
cmake -S . -B build
cmake --build build
./build/dispatchiq
```

Run tests after test targets are introduced:

```bash
ctest --test-dir build --output-on-failure
```

### Compiler-only fallback

If CMake is not available on the command line:

```bash
mkdir -p build
clang++ -std=c++20 -Wall -Wextra -Wpedantic \
    -Iinclude \
    src/main.cpp src/incident.cpp src/emergency_call.cpp src/incident_triage.cpp \
    src/incident_index.cpp src/event_deduplicator.cpp \
    -o build/dispatchiq
./build/dispatchiq
```

## Example output

```text
=======EVENT DEDUPLICATION=======

Initial count test passed.
Unseen event test passed.
Event 5001 accepted.
Event 5002 accepted.
Event 5003 accepted.
Unique-event count test passed.
Recorded-event membership test passed.
Duplicate event rejection test passed.
Duplicate-attempt count test passed.
Duplicate preservation test passed.
Invalid event rejection test passed.
Invalid-attempt count test passed.
Invalid-event membership test passed.
Existing event removal test passed.
Post-removal count test passed.
Forgotten-event lookup test passed.
Remaining-event preservation test passed.
Missing event removal test passed.
Missing-removal count test passed.
Invalid event removal test passed.
Invalid-removal count test passed.
Event reinsertion test passed.
Final count test passed.
Re-recorded event lookup test passed.

DIQ-005 event deduplication tests passed.
```

## Project layout

```text
DispatchIQ/
├── CMakeLists.txt
├── README.md
├── data/                       # Small sample datasets
├── docs/
│   ├── architecture.md         # Design and container decisions
│   ├── learning-roadmap.md     # Milestone sequence
│   └── milestones/             # Completed work records
├── include/dispatchiq/
│   ├── emergency_call.hpp       # Emergency-call model and queue operations
│   ├── event_deduplicator.hpp   # Processed-event set and operation contracts
│   ├── incident_index.hpp       # Hash index model and operation contracts
│   ├── incident.hpp             # Incident model and public operations
│   └── incident_triage.hpp      # Priority-triage model and operation contracts
├── src/
│   ├── emergency_call.cpp       # FIFO intake implementation
│   ├── event_deduplicator.cpp   # Event uniqueness and membership operations
│   ├── incident_index.cpp       # Hash-based incident lookup and removal
│   ├── incident.cpp             # Incident registry implementation
│   ├── incident_triage.cpp      # Severity and arrival-order triage
│   └── main.cpp                 # Current DIQ-005 acceptance harness
└── tests/                      # Future automated tests
```

## Data-structure roadmap

| Structure | Planned responsibility | Status |
|---|---|---|
| `std::pair` | Latitude and longitude | Implemented |
| `std::vector` | Incident history in arrival order | Implemented |
| `std::queue` | FIFO emergency-call intake | Implemented |
| `std::priority_queue` | Severity-based triage | Implemented |
| `std::unordered_map` | Fast lookup by incident or responder ID | Implemented |
| `std::unordered_set` | Duplicate-event detection | Implemented |
| `std::map` | Ordered operational reports | Next |
| `std::set` | Sorted unique skills and service regions | Next |
| `std::list` | Frequently edited route stops | Planned |
| `std::stack` | Undo history for dispatch actions | Planned |

## Documentation

- [Architecture and design decisions](docs/architecture.md)
- [Learning roadmap](docs/learning-roadmap.md)
- [DIQ-001 milestone record](docs/milestones/DIQ-001-incident-registry.md)
- [DIQ-002 milestone record](docs/milestones/DIQ-002-emergency-call-intake.md)
- [DIQ-003 milestone record](docs/milestones/DIQ-003-incident-triage.md)
- [DIQ-004 milestone record](docs/milestones/DIQ-004-fast-incident-index.md)
- [DIQ-005 milestone record](docs/milestones/DIQ-005-event-deduplication.md)

## Current limitations

- Records exist only for the lifetime of the process.
- The original vector registry still uses linear duplicate detection and lookup; the active
  incident index provides average constant-time alternatives.
- Event deduplication is not yet wired into the emergency-call intake queue.
- Duplicate IDs are not yet detected when incidents enter triage.
- Indexed incidents are independent copies, so synchronization with the registry and triage is
  not yet coordinated by a single workflow.
- Hash-table iteration order is intentionally unspecified and must not be used for reporting.
- Processed event IDs have no expiration policy and, unless explicitly forgotten, remain for the
  process lifetime.
- Latitude and longitude ranges are not yet validated.
- Validation operations report success or failure but not the precise rejection reason.
- Verification is currently manual; automated tests have not yet been added.
