---

# D-truss Decomposition Algorithms

This repository provides implementations and experimental evaluation of several
D-truss decomposition algorithms for directed graphs.

---

## Algorithms

We evaluate the following algorithms:

* **Peeling**:
  The state-of-the-art sequential D-truss decomposition algorithm.
  Implementation (original authors):
  [https://github.com/TestCodeHouse/DTrussMain/tree/main/ddecomp](https://github.com/TestCodeHouse/DTrussMain/tree/main/ddecomp)

* **RePeel**:
  Originally proposed for D-truss retrieval in streaming graphs.
  We adapt it to static full D-truss decomposition.
  Implementation (original authors):
  [https://github.com/hkbudb/streaming-dtruss/tree/main](https://github.com/hkbudb/streaming-dtruss/tree/main)

* **PACT**:
  Our proposed progressive D-truss decomposition method.

* **PACO**:
  Our proposed method incorporates order-based refinement into PACT.

---

## Environment

The code is tested under the following environment:

* **Operating System**: Ubuntu 24.04 LTS
* **Compiler**: g++ 13.3.0

---

## Compiling and Running

### Compilation

Compile all source files in the current directory:

```
g++ *.cpp -O3 -o ${FILE_NAME}
```

Example:

```
g++ Graph.cpp main.cpp -O3 -o dtruss
```

---

### Running

Run the program using the following command:

```
./${FILE_NAME} -f ${GRAPH_FILE} -a ${ALGORITHM}
```

Example:

```
./dtruss -f ./example/em.txt -a 1
```

For convenience, all experiments can also be executed using the provided script:

```
bash run.sh
```

---

## Input Format

### Graph File (GRAPH_FILE)

The first line contains the number of nodes and the number of directed edges:

```
<NODES> <EDGES>
```

Each of the remaining lines represents a directed edge from node u to node v:

```
<u> <v>
```

---

### Algorithm Selection (ALGORITHM)

Specify the algorithm to run using the following ID:

|  ID | Algorithm |
| :-: | :-------: |
|  1  |  Peeling  |
|  2  |   RePeel  |
|  3  |    PACT   |
|  4  |    PACO   |

---

## Datasets

The ten datasets used in the paper are publicly available from:

* [https://snap.stanford.edu/data/index.htm](https://snap.stanford.edu/data/index.htm)
* [https://law.di.unimi.it/index.php](https://law.di.unimi.it/index.php)
* [http://konect.cc/networks/](http://konect.cc/networks/)

### Dataset List

| Dataset Name   | Abbreviation |
| -------------- | ------------ |
| OpenFlights    | OF           |
| Advogato       | AD           |
| Email-EuAll    | EM           |
| Slashdot       | SD           |
| Amazon         | AM           |
| BerkStan       | BS           |
| Wikipedia-Link | WL           |
| Pokec          | PO           |
| LiveJournal    | LJ           |
| Enwiki-2013    | EW           |

---

## Directory Structure

```
.
├── example/        # Example input graphs
├── result/         # Running time results of all algorithms
├── *.cpp           # Source files
├── run.sh          # Script for running experiments
└── README.md
```

---

## Notes

* An example input for **Email-EuAll** is provided in `./example/`.
* The running time results of all algorithms are provided in `./result/`.

---
