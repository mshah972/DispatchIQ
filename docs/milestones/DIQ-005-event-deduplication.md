# DIQ-005: Event Deduplication

**Status:** Implemented

**Primary container:** `std::unordered_set`

## Business requirement

DispatchIQ may receive the same upstream event more than once when a sender retries after a
timeout. The system must identify repeated event IDs without storing duplicate event records or
depending on arrival order.

## Delivered behavior

- Owns processed event IDs in a hash-based set.
- Accepts positive event IDs.
- Rejects zero and negative IDs without changing the set.
- Accepts a previously unseen ID exactly once.
- Rejects a duplicate ID without changing the count.
- Reports whether an ID has already been processed.
- Forgets a stored ID and reports whether removal occurred.
- Leaves the set unchanged when removal targets a missing or invalid ID.
- Allows an ID to be recorded again after it is forgotten.
- Reports the current number of processed event IDs.

## Files

| File | Responsibility |
|---|---|
| `include/dispatchiq/event_deduplicator.hpp` | Deduplicator state and public contracts |
| `src/event_deduplicator.cpp` | Validation, insertion, membership, removal, and count behavior |
| `src/main.cpp` | Deterministic DIQ-005 acceptance scenario |
| `CMakeLists.txt` | Builds the deduplication implementation into the executable |

## Deduplication contract

- The event ID is the `std::unordered_set` key and stored value.
- Valid event IDs are greater than zero.
- `insert` performs storage and duplicate detection in one operation.
- A duplicate insertion returns `false` and leaves the set unchanged.
- `contains` reports membership without modifying the set.
- `erase` reports whether an event ID existed.
- Forgetting an ID allows a later insertion of that ID to succeed.
- No behavior depends on hash-table iteration order.

## Acceptance results

| Scenario | Expected result | Result |
|---|---|---|
| Inspect an empty deduplicator | Count is 0 and event 5001 is unseen | Pass |
| Record events 5001 through 5003 | All three are accepted | Pass |
| Inspect recorded events | Count is 3 and all three IDs are present | Pass |
| Record event 5002 again | Returns `false`; count remains 3 | Pass |
| Record event IDs 0 and -5 | Both return `false`; count remains 3 | Pass |
| Forget event 5002 | Returns `true`; count becomes 2 | Pass |
| Inspect state after forgetting 5002 | 5002 is absent; 5001 and 5003 remain | Pass |
| Forget event 5002 again | Returns `false`; count remains 2 | Pass |
| Forget event 0 | Returns `false`; count remains 2 | Pass |
| Record event 5002 again | Returns `true`; count returns to 3 | Pass |

## Complexity

| Operation | Average | Worst case |
|---|---:|---:|
| Validate event ID | O(1) | O(1) |
| Record event | O(1) | O(n) |
| Check event membership | O(1) | O(n) |
| Forget event | O(1) | O(n) |
| Count events | O(1) | O(1) |

The hash-table worst case occurs when many keys occupy the same bucket. In normal operation,
insertion, membership, and removal are expected to be constant time on average.

## Design trade-offs

- Storing only IDs minimizes state but cannot answer questions about event source or time.
- Fast membership does not preserve arrival or sorted order.
- The deduplicator is independent from the emergency-call queue and is not yet enforced during
  call intake.
- Processed IDs remain in memory until explicitly forgotten or the process exits.
- Forgetting an ID permits replay, so a production workflow would need authorization and
  retention rules.
- Boolean results do not distinguish invalid IDs from duplicates or missing removals.
- The executable remains a manual acceptance harness rather than an automated test target.

## Deferred work

- Integrate deduplication with emergency-call intake as one coordinated operation.
- Add event metadata or processing timestamps if retention policies require them.
- Add automatic expiration and persistence.
- Replace Boolean results with structured rejection reasons.
- Add automated unit tests and continuous integration.
- Benchmark hash behavior with representative event volumes.

## Concepts demonstrated

- Unique-key storage with `std::unordered_set`
- Hash-based insertion, membership, removal, and counting
- `insert` and its `std::pair<iterator, bool>` result
- C++20 `contains`
- `erase` count semantics
- Average versus worst-case complexity
- Duplicate-event protection
- Unspecified hash-table iteration order
