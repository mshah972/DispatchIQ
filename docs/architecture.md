# DispatchIQ architecture

This document records the architecture of the current MVP and the reasoning behind each
data-structure choice. It should evolve with the implementation.

## Current component boundaries

```text
main.cpp
  ├── creates deterministic event-retry scenarios
  ├── reports operation results
  └── calls deduplication operations ──► EventDeduplicator
                                               │
                                               ▼
                                  std::unordered_set<int>
```

- `include/dispatchiq/incident.hpp` defines the `Incident` model and public operation contracts.
- `src/incident.cpp` owns validation, duplicate checking, searching, and registry presentation.
- `include/dispatchiq/emergency_call.hpp` defines the intake model and queue-operation contracts.
- `src/emergency_call.cpp` owns call validation, FIFO insertion, peeking, processing, and counting.
- `include/dispatchiq/event_deduplicator.hpp` defines processed-event state and contracts.
- `src/event_deduplicator.cpp` owns event validation, insertion, lookup, removal, and counting.
- `include/dispatchiq/incident_triage.hpp` defines triage entries, ordering, state, and contracts.
- `src/incident_triage.cpp` owns severity ordering, arrival tie-breaking, peeking, and dispatching.
- `include/dispatchiq/incident_index.hpp` defines the active incident index and its contracts.
- `src/incident_index.cpp` owns hash-based insertion, lookup, removal, and counting.
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

The current trade-off is linear duplicate detection and linear ID lookup. This remains a useful
baseline for comparing the `std::unordered_map` index introduced in DIQ-004.

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
rejection. DIQ-005 provides a separate `std::unordered_set` deduplicator, but it is not yet wired
into call intake. A queue does not provide a natural membership operation, so integration must
coordinate the queue and deduplicator as one workflow.

## Incident triage

Accepted incidents enter an `IncidentTriage` state containing a priority queue and the next
arrival sequence number. Each `TriageEntry` combines the original `Incident` with the sequence
assigned when it entered triage.

### Container decision: `std::priority_queue`

Triage must expose the most operationally urgent incident without sorting the entire collection
after every insertion. A `std::priority_queue` maintains the highest-priority entry at `top()`
while supporting logarithmic insertion and removal.

The `IncidentPriority` comparator applies two ordering rules:

1. Higher severity has higher priority.
2. Equal severities are ordered by smaller arrival sequence, preserving first-arrived-first-out
   behavior within the severity level.

The comparator returns `true` when its left operand has lower priority than its right operand.
This inverted-looking contract is required by `std::priority_queue`.

| Operation | Complexity | Behavior |
|---|---:|---|
| Add valid incident | O(log n) | Assigns a sequence and restores heap order |
| Peek highest priority | O(1) | Reads `top()` without removal |
| Dispatch highest priority | O(log n) | Copies and removes `top()` |
| Count pending incidents | O(1) | Returns the priority queue size |

Severity validation occurs before insertion. Rejected incidents leave both the queue and arrival
counter unchanged, so accepted sequence numbers remain contiguous. Sequence numbers are
monotonic and are not reused after dispatch.

`peekHighestPriorityIncident` returns a non-owning pointer to the incident inside the top entry.
Any priority-queue mutation can reorder or reallocate storage, so the pointer must not be used
after adding or dispatching an incident or after the triage state is destroyed.

## Fast incident index

The active incident index owns incidents in an
`std::unordered_map<int, Incident>`. Each incident ID is both the uniqueness boundary and the
hash-table key. This provides direct access by ID without scanning the insertion-order registry.

### Container decision: `std::unordered_map`

The index uses `std::unordered_map` because it:

- associates each unique integer ID with one complete incident;
- detects duplicate keys during insertion without a separate scan;
- supports average constant-time insertion, lookup, and removal; and
- does not impose an ordering cost that the lookup workflow does not require.

Hash-table iteration order is unspecified. Ordered displays must continue to use an ordered
sequence or a later reporting container rather than depending on the index's iteration order.

| Operation | Average | Worst case | Behavior |
|---|---:|---:|---|
| Add valid incident | O(1) | O(n) | Uses `emplace`; duplicate keys leave existing data unchanged |
| Find by ID | O(1) | O(n) | Returns a pointer to the mapped incident or `nullptr` |
| Remove by ID | O(1) | O(n) | Erases one matching key and reports whether it existed |
| Count indexed incidents | O(1) | O(1) | Returns the hash table size |

`addIncidentToIndex` validates severity before insertion. A severity outside 1 through 5 or an
existing ID returns `false` without modifying the index. Removing a missing ID also returns
`false` and leaves the index unchanged.

`findIncidentInIndexById` returns a non-owning pointer to the mapped incident. Rehashing
invalidates iterators but does not invalidate pointers or references to stored elements. The
pointer becomes invalid when its incident is erased, the index is cleared or destroyed, or the
map is replaced.

The index currently owns copies independently from the vector registry and priority triage.
Coordinating those views behind one workflow is deferred until the application layer is expanded.

## Event deduplication

External systems may deliver an event repeatedly when a sender retries after a timeout.
`EventDeduplicator` records the integer ID of each processed event so DispatchIQ can distinguish
a new event from a retry without storing a second copy of the event payload.

### Container decision: `std::unordered_set`

The deduplicator uses `std::unordered_set<int>` because it:

- stores each event ID at most once;
- answers membership questions without an associated mapped value;
- detects duplicates as part of insertion; and
- supports average constant-time insertion, lookup, removal, and counting.

An unordered set is preferable to an unordered map here because the deduplication decision needs
only the event ID. If DispatchIQ later needs metadata such as processing time or source system,
the component can be redesigned around a map.

| Operation | Average | Worst case | Behavior |
|---|---:|---:|---|
| Record event | O(1) | O(n) | Inserts a positive ID and reports whether it was new |
| Check event | O(1) | O(n) | Uses C++20 `contains` to test membership |
| Forget event | O(1) | O(n) | Erases one ID and reports whether it existed |
| Count events | O(1) | O(1) | Returns the number of tracked IDs |

`recordEvent` rejects zero and negative IDs before insertion. A positive duplicate returns
`false` and leaves the set unchanged. `hasProcessedEvent` and `forgetEvent` also return
`false` for non-positive IDs.

`forgetEvent` deliberately allows an ID to be processed again. This models a controlled replay
or expiration operation; production integration would restrict when that operation is permitted.
The current component has no retention window, persistence, or automatic expiration policy.

Hash-table iteration order is unspecified, and no deduplication behavior depends on it. The
`processedEventIds` member is currently public to match the project's simple state-object
pattern, although callers should use the operation functions so validation is not bypassed.

## Validation and error reporting

The current milestones validate registry incidents, indexed incidents, emergency calls, and
triage incidents by severity. The vector registry and hash index enforce incident ID uniqueness,
and the event deduplicator accepts only positive, previously unseen event IDs. Coordinate bounds,
emergency-call and triage ID uniqueness, and category rules are not yet enforced. Boolean results
keep the first APIs simple but cannot distinguish rejection reasons. A later milestone can
introduce result enums without coupling domain logic to console output.

## Growth path

The architecture will expand one behavior at a time:

1. Ordered containers will support stable operational reports.
2. `std::list` will support frequently edited routes.
3. `std::stack` will support undo.
4. Persistence, automated tests, and benchmarks will harden the completed simulator.
