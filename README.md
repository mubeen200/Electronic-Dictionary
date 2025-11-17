
# Electronic Dictionary (C++, DSA)

**Console-based lexical lookup tool** supporting part-of-speech grouped meanings, POS-specific synonyms & antonyms, exact-meaning lookup, a searchable history (stack), and a random *Word of the Day*. Designed for fast single-word lookup across large datasets using hashing and an AVL Tree.

---

## Features
- Fast single-word lookup using an **AVL Tree** (O(log n)).
- Word entry table built with **hashing** (`unordered_map`) for fast construction and access.
- POS-grouped storage using **vectors** for meanings, synonyms and antonyms.
- Search history implemented as a **stack-like** linked list.
- Exact-meaning reverse lookup (linear scan).
- Random **Word of the Day** sampled from loaded entries.
- Robust parsing of plaintext data sources: `merged_dictionary.txt`, `synonym.txt`, `antonym.txt`.

---

## Repository structure (suggested)
```

/
├─ DSA Project
│  └─  DSA project.sln         # Block format: Word:, POS:, Antonyms:
│  └─  DSA project.vcxproj
│  └─  DSA project.vcxproj.filters
│  └─  DSA project.vcxproj.user
│  └─  FileName.cpp
│  └─  antonym.txt
│  └─  merged_dictionary.txt
│  └─  synonym.txt
├─ README.md
└─ .gitignore

```

---

## Data formats & examples

### `merged_dictionary.txt`
Each line contains a word, a POS in parentheses, then `:` and the meaning:
```

apple (n): A round fruit with red or green skin.
run (v): Move at a speed faster than a walk.

```

### `synonym.txt` and `antonym.txt`
Plaintext blocks separated by blank lines. Each block contains labeled lines starting with the field name.

Example `synonym.txt` block:
```

Word: apple
POS: n
Synonyms: pomaceous fruit; pome|orchard fruit

```

Example `antonym.txt` block:
```

Word: happy
POS: adj
Antonyms: sad; unhappy

````

Notes:
- Synonym/antonym lists may use `;` or `|` separators inside entries; the parser trims and deduplicates tokens.

---

## Build & Run

**Requirements**
- C++ compiler with C++17 support (`g++`, `clang++`, etc.).
- Sufficient memory for large datasets (200k entries may require hundreds of MB).

**Build**
```bash
g++ -std=c++17 -O2 -pipe -o electronic_dictionary src/main.cpp
````

**Run** (ensure `data/` files are present or adjust paths in source)

```bash
./electronic_dictionary
```

On startup the program loads `merged_dictionary.txt`, `synonym.txt`, and `antonym.txt`, builds in-memory structures, and presents a CLI menu.

---

## CLI Usage

Menu options:

1. Meaning — lookup word meanings grouped by POS (top 2 definitions shown).
2. Synonyms — POS-specific synonyms (deduplicated, truncated output).
3. Antonyms — POS-specific antonyms (deduplicated, truncated output).
4. History — show recent lookups (stack order).
5. New Word of the Day — resample a random word.
6. Lookup by Exact Meaning — find words matching an exact meaning string.
7. Exit

Example:

```
Choice: 1
Enter word: apple
apple (n)
  1. A round fruit with red or green skin.
```

---

## Implementation highlights (keywords)

* **AVL Tree** — balanced BST for fast word lookup.
* **Hashing** (`unordered_map`) — map words → Node* during load.
* **Vectors** — POS-aligned storage for meanings/synonyms/antonyms.
* **Stack (linked list)** — recent-search history.
* **unordered_set** — deduplicate synonym/antonym tokens.
* Parsing helpers: `trim()`, `split()`, `parseBlocks()`.

---

## Performance & limits

* Scales to large datasets (tested with ~200k entries); memory usage grows with dataset size.
* Single-word lookups: O(log n) via AVL tree.
* Exact-meaning lookups: O(n) linear scan — suitable for occasional use; consider an inverted index for faster reverse lookup.
* Startup parsing is single-threaded and can take time depending on dataset size.

---

## Limitations & Future Work

* Add an **inverted index** for fast reverse-meaning lookup.
* Persist history to disk and support pagination for large histories.
* Export selected entries to JSON/CSV.
* Add unit tests and CI for parsing and tree invariants.
* Provide a GUI or web interface for broader usability.

---

## Contributing

1. Fork the repository.
2. Create a branch: `git checkout -b feature/your-feature`.
3. Commit and push changes, then open a pull request describing the change.

---

## Author

**Mubeen Haider** — github: https://github.com/mubeen200

**Ashar Ali** — github: https://github.com/AsharAli-042

**Abbas Fakhruddin** — github: https://github.com/Abbas-coding

**Rahoul Kumar** — github: https://github.com/CodeWithRahoul
