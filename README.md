````markdown
# Algorithms

We evaluate the following D-truss decomposition algorithms:

- **Peeling**  
  State-of-the-art sequential D-truss decomposition algorithm.  
  Implementation: https://github.com/TestCodeHouse/DTrussMain/tree/main/ddecomp

- **RePeel**  
  Originally proposed for D-truss retrieval in streaming graphs.  
  We adapt it to static full D-truss decomposition.  
  Implementation: https://github.com/hkbudb/streaming-dtruss/tree/main

- **PACT**  
  Our proposed D-truss decomposition method.

- **PACO**  
  Our proposed method that incorporates order-based refinement into PACT.

---

# Environment

- **Operating System**: Ubuntu 24.04 LTS  
- **Compiler**: g++ 13.3.0  

---

# Compiling and Running

## Compilation

Compile all source files under the current directory:

```bash
g++ *.cpp -O3 -o ${FILE_NAME}
````

Example:

```bash
g++ Graph.cpp main.cpp -O3 -o dtruss
```

## Execution

Run the program with the following command:

```bash
./${FILE_NAME} -f ${GRAPH_FILE} -a ${ALGORITHM}
```

Example:

```bash
./dtruss -f ./example/em.txt -a 1
```

For convenience, you can also run all experiments using the provided script:

```bash
bash run.sh
```

---

# Input Format

## Graph File (`GRAPH_FILE`)

* The first line contains the number of nodes and the number of directed edges:

```text
<NODES> <EDGES>
```

* Each subsequent line represents a directed edge from node `u` to node `v`:

```text
<u> <v>
```

## Algorithm Selection (`ALGORITHM`)

Specify the algorithm to run using the following IDs:

|  ID | Algorithm |
| :-: | :-------: |
|  1  |  Peeling  |
|  2  |   RePeel  |
|  3  |    PACT   |
|  4  |    PACO   |

---

# Datasets

The ten datasets used in the paper are publicly available from the following sources:

* [https://snap.stanford.edu/data/index.htm](https://snap.stanford.edu/data/index.htm)
* [https://law.di.unimi.it/index.php](https://law.di.unimi.it/index.php)
* [http://konect.cc/networks/](http://konect.cc/networks/)

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

# Additional Notes

* An example input for **Email-EuAll** is provided in `./example/`.
* The running time results of all algorithms are reported in `./result/`.

```
```
