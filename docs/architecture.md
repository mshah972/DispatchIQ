# DispatchIQ architecture

This document records the architecture of the current MVP and the reasoning behind each
data-structure choice. It should evolve with the implementation.

## Current component boundaries

```text
main.cpp
  ├── creates demonstration incidents and emergency calls
  ├── reports operation results
  ├── calls incident registry operations ──► std::vector<Incident>
  └── calls emergency intake operations ──► std::queue<EmergencyCall>
```

- `include/dispatchiq/incident.hpp` defines the `Incident` model and public operation contracts.
- `src/incident.cpp` owns validation, duplicate checking, searching, and registry presentation.
- `include/dispatchiq/emergency_call.hpp` defines the intake model and queue-operation contracts.
- `src/emergency_call.cpp` owns call validation, FIFO insertion, peeking, processing, and counting.
- `src/main.cpp` composes those operations into a deterministic acceptance scenario.
- All project-owned types and operations live in the `dispatchiq` namespace.

## Incident model

An incident currently contains:

| Field | Type | Meaning |
|---|---|---|
| `id` | `int` | Unique incident identifier |
| `category` | `std::string` | Incident classification, such as Fire or Medical |
| `severity` | `int` | Priority value from 1 through 5 |
| `location` | `std::pair<double, double>` | Latitude in `.first`, longitude in `.second` |

`std::pair` is intentionally used in the first milestone to teach two-value aggregation. A future
refactor may introduce a named `Location` type because `latitude` and `longitude` communicate
intent more clearly than `.first` and `.second`.

## Container decision: `std::vector`

The first registry uses `std::vector<Incident>` because it:

- preserves insertion order;
- provides contiguous storage and efficient iteration;
- supports amortized constant-time insertion at the end; and
- is a strong default container for a small, read-heavy sequence.

The current trade-off is linear duplicate detection and linear ID lookup. This is acceptable for
the learning MVP and creates a measurable baseline for a later `std::unordered_map` index.

### Complexity

| Operation | Complexity | Reason |
|---|---:|---|
| Severity validation | O(1) | Two comparisons |
| Add incident | O(n) | Linear duplicate scan, followed by amortized O(1) append |
| Find by ID | Best O(1), worst O(n) | Linear search in insertion order |
| Print incidents | O(n) | Visits every stored incident |

## Operation contracts

### `addIncident`

- Accepts severity values from 1 through 5, inclusive.
- Rejects an ID already present in the vector.
- Leaves the vector unchanged when validation fails.
- Appends the complete incident and returns `true` on success.
- Returns `false` on rejection.

### `findIncidentById`

- Does not modify the registry.
- Returns a pointer to the matching vector element.
- Returns `nullptr` when no matching ID exists.

The returned pointer is non-owning. It remains usable only while the vector exists and the element
has not been erased or invalidated. Operations such as `push_back` can reallocate the vector and
invalidate previously returned pointers.

### `printIncidents`

- Does not modify the registry.
- Prints a clear message for an empty registry.
- Prints incidents in insertion order.

## Emergency-call intake

An emergency call contains:

| Field | Type | Meaning |
|---|---|---|
| `callId` | `int` | Intake identifier |
| `category` | `std::string` | Reported emergency category |
| `reportedSeverity` | `int` | Caller-reported severity from 1 through 5 |
| `location` | `std::pair<double, double>` | Latitude and longitude |

### Container decision: `std::queue`

The intake workflow uses `std::queue<EmergencyCall>` because untriaged calls must be reviewed in
arrival order. The adapter deliberately exposes only the front and back of the sequence, which
prevents accidental random-access processing. Severity-based reordering belongs to the later
triage milestone and will use `std::priority_queue`.

| Operation | Complexity | Behavior |
|---|---:|---|
| Enqueue valid call | O(1) container operation | Adds the call at the back |
| Peek next call | O(1) | Reads the front without removal |
| Process next call | O(1) container operation | Copies and removes the front |
| Count pending calls | O(1) | Returns the queue size |

`peekNextCall` returns a non-owning pointer to the front element. That pointer must not be used
after `processNextCall` removes the element or after the queue is destroyed.

The intake functions reject severities outside 1 through 5 and leave the queue unchanged after a
rejection. Duplicate call IDs are intentionally deferred until the `std::unordered_set` milestone
because a queue does not provide a natural search operation.

## Validation and error reporting

The current milestones validate incident and call severity plus incident ID uniqueness. Coordinate
bounds, emergency-call ID uniqueness, and category rules are not yet enforced. Boolean results
keep the first APIs simple but cannot distinguish rejection reasons. A later milestone can
introduce result enums without coupling domain logic to console output.

## Growth path

The architecture will expand one behavior at a time:

1. `std::priority_queue` will prioritize accepted incidents by severity and arrival time.
2. Hash-based indexes will provide average constant-time lookup and duplicate detection.
3. Ordered containers will support stable operational reports.
4. `std::list` will support frequently edited routes, and `std::stack` will support undo.
5. Persistence, automated tests, and benchmarks will harden the completed simulator.
