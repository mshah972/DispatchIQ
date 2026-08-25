# DispatchIQ architecture

This document records the architecture of the current MVP and the reasoning behind each
data-structure choice. It should evolve with the implementation.

## Current component boundaries

```text
main.cpp
  ├── creates demonstration incidents
  ├── reports operation results
  └── calls incident registry operations
            │
            ▼
incident.cpp
  ├── addIncident
  ├── findIncidentById
  └── printIncidents
            │
            ▼
std::vector<Incident>
```

- `include/dispatchiq/incident.hpp` defines the `Incident` model and public operation contracts.
- `src/incident.cpp` owns validation, duplicate checking, searching, and registry presentation.
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

## Validation and error reporting

The MVP validates severity and ID uniqueness. Coordinate bounds and category rules are not yet
enforced. `addIncident` currently returns a Boolean, which keeps the first API simple but cannot
distinguish rejection reasons. A later milestone can introduce a result enum such as accepted,
duplicate ID, or invalid severity without coupling registry logic to console output.

## Growth path

The architecture will expand one behavior at a time:

1. `std::queue` will model FIFO emergency-call intake.
2. `std::priority_queue` will prioritize accepted incidents by severity and arrival time.
3. Hash-based indexes will provide average constant-time lookup and duplicate detection.
4. Ordered containers will support stable operational reports.
5. `std::list` will support frequently edited routes, and `std::stack` will support undo.
6. Persistence, automated tests, and benchmarks will harden the completed simulator.
