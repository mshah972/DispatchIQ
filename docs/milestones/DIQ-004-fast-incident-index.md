# DIQ-004: Fast Incident Index

**Status:** Implemented

**Primary container:** `std::unordered_map`

## Business requirement

Dispatchers must retrieve active incidents by ID without scanning the complete incident history.
Duplicate IDs must be rejected without replacing the incident that is already active.

## Delivered behavior

- Owns active incidents in an ID-keyed hash table.
- Accepts severity values from 1 through 5.
- Rejects invalid severity without changing the index.
- Rejects duplicate IDs without overwriting the original incident.
- Finds an incident by ID and reports a missing ID with `nullptr`.
- Removes an existing incident and reports whether removal occurred.
- Leaves the index unchanged when removal targets a missing ID.
- Allows an ID to be inserted again after its previous entry is removed.
- Reports the current number of indexed incidents.

## Files

| File | Responsibility |
|---|---|
| `include/dispatchiq/incident_index.hpp` | Index state and public operation contracts |
| `src/incident_index.cpp` | Validation, insertion, lookup, removal, and count behavior |
| `src/main.cpp` | Deterministic DIQ-004 acceptance scenario |
| `CMakeLists.txt` | Builds the index implementation into the executable |

## Index contract

- The incident ID is the `std::unordered_map` key.
- The complete `Incident` is the mapped value.
- `emplace` performs insertion and duplicate detection in one operation.
- A duplicate insertion returns `false` and preserves the original mapped value.
- No behavior depends on hash-table iteration order.

## Acceptance results

| Scenario | Expected result | Result |
|---|---|---|
| Inspect an empty index | Count 0 and missing lookup returns `nullptr` | Pass |
| Add incidents 4001 through 4003 | All three are accepted | Pass |
| Add a different incident with ID 4001 | Rejected; original Fire data remains | Pass |
| Add incident 4004 with severity 8 | Rejected; count remains 3 | Pass |
| Find incident 4002 | Every stored field matches the input | Pass |
| Find incident 9999 in a populated index | Returns `nullptr` | Pass |
| Remove incident 4002 | Returns `true`; count becomes 2 | Pass |
| Remove incident 9999 | Returns `false`; count remains 2 | Pass |
| Reinsert incident 4002 | Returns `true`; count returns to 3 | Pass |

## Complexity

| Operation | Average | Worst case |
|---|---:|---:|
| Validate severity | O(1) | O(1) |
| Insert incident | O(1) | O(n) |
| Find incident | O(1) | O(n) |
| Remove incident | O(1) | O(n) |
| Count incidents | O(1) | O(1) |

The worst case occurs when many keys occupy the same bucket. Copying an `Incident` may also
depend on the length of its string fields.

## Pointer lifetime

The pointer returned by `findIncidentInIndexById` is non-owning. Insertion and rehashing do not
invalidate pointers or references to existing elements, although rehashing does invalidate
iterators. The pointer must not be used after its incident is erased, the index is cleared or
destroyed, or the underlying map is replaced.

## Design trade-offs

- Fast ID access does not preserve arrival or sorted order.
- The index owns incident copies independently from the vector registry and triage queue.
- Boolean results do not explain whether insertion failed because of severity or a duplicate ID.
- The executable remains a manual acceptance harness rather than an automated test target.

## Deferred work

- Coordinate registry, triage, and index mutations through one application service.
- Replace Boolean validation results with structured rejection reasons.
- Validate latitude and longitude ranges.
- Add automated unit tests and continuous integration.
- Benchmark vector linear search against hash lookup with representative datasets.

## Concepts demonstrated

- Key-value storage with `std::unordered_map`
- Hash-based insertion, lookup, removal, and counting
- `emplace` and its `std::pair<iterator, bool>` result
- Iterator access through `first` and `second`
- Average versus worst-case complexity
- Duplicate-key protection without overwriting
- Non-owning pointers and element lifetime
