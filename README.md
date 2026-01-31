# The code of algorithms
* **Peeling**: The state-of-the-art sequential D-truss decomposition algorithm. We use the authors' implementation: [Peeling](https://github.com/TestCodeHouse/DTrussMain/tree/main/ddecomp).
* **RePeel**: Originally proposed for D-truss retrieval in streaming graphs. We adapt it to static full D-truss decomposition and use the authors' released implementation: [Repeel](https://github.com/hkbudb/streaming-dtruss/tree/main).
* PACT: our proposed D-truss decomposition method;
* PACO: our proposed D-truss decomposition method incorporates the order-based refinement algorithm.

# Environment
## Ubuntu 24.04 LTS
## g++ 13.3.0

# Compiling and Running
## Compiling the program
```
g++ ${fileDirname}/*.cpp -O3 -o ${FILE_NAME}
```

For example:
```
g++ Graph.cpp main.cpp -O3 -o dtruss
```


## Running the program:
```
./${FILE_NAME} -f ${GRAPH_FILE} -a ${ALGORITHM}
```

For example:
```
./dtruss -f ./example/em.txt -a 1 
```

For simply, we can run the program via a script ```bash run.sh```.




## Input format
* GRAPH_FILE:
The first line is consited of # of nodes and # of directed edges in the graph, which is denoted as following.
```
${NODES} ${EDGE}
``` 
Remains line represents a directed edge from node u to node v, which is presented as following.
```
${u} ${v}
```


* ALGORITHM:
The id of algorithm will be running, which the id is presented as following.

| id | algorithms |
| :----: | :----: |
| 1 | Peeling |
| 2 | RePeel |
| 3 | PACT |
| 4 | PACO |

# Other
* The ten datasets used in paper are availabe from:


<div style="text-align:center">
<table>
    <tbody>
    <tr>
        <th>Name</th>
        <th>Abbr.</th>
        <th>Source</th>
    </tr>
    <tr>
        <td>OpenFlights</td>
        <td>OF</td>
        <td rowspan="10">
            https://snap.stanford.edu/data/index.htm<br>
            https://law.di.unimi.it/index.php<br>
            http://konect.cc/networks/
        </td>
    </tr>
    <tr>
        <td>Advogato</td>
        <td>AD</td>
    </tr>
    <tr>
        <td>Email-EuAll</td>
        <td>EM</td>
    </tr>
    <tr>
        <td>Slashdot</td>
        <td>SD</td>
    </tr>
    <tr>
        <td>Amazon</td>
        <td>AM</td>
    </tr>
    <tr>
        <td>BerkStan</td>
        <td>BS</td>
    </tr>
    <tr>
        <td>Wikipedia-Link</td>
        <td>WL</td>
    </tr>
    <tr>
        <td>Pokec</td>
        <td>PO</td>
    </tr>
    <tr>
        <td>Live Journal</td>
        <td>LJ</td>
    </tr>
    <tr>
        <td>Enwiki-2013</td>
        <td>EW</td>
    </tr>
    </tbody>
</table>
</div>

* We give an example of Email-EuAll in ```./example/```, and running time of all algorithms in ```./result/```.
