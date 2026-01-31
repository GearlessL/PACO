#include <iostream>
#include "Graph.h"
#include <string>
#include <chrono>
#include "cmdline.h"
#include <fstream>

using namespace std;

int main(int argc, char *argv[]) {
    //input parameters
    cmdline::parser a;
    a.add<string>("file", 'f', "filename", true, "");
    a.add<int>("a", 'a', "algorithm", true);
  
    a.parse_check(argc, argv);

    //Read graph
    string filepath = a.get<string>("file");
    clock_t io_begin = clock();
    FILE* dFile = fopen(filepath.c_str(), "r");

    Graph g = Graph(dFile);

    clock_t io_end = clock();
    double io_secs = double(io_end - io_begin) / CLOCKS_PER_SEC;
    printf("io time: %.4f.\n", io_secs);


    int type = a.get<int>("a");
    

    auto begin = chrono::steady_clock::now();
    switch (type)
    {
    case 1:
        //basline decomposition
        g.Right_Decom();
        break;
    case 2:
        //advanced baseline
        g.Adv_Decom();
        break;
    case 3:
        //PACT
        g.PACT();
        break;
    case 4:
        //PACO
        g.PACO();
        break;
    default:
        printf("no such algorithm.\n");
        break;
    }
    auto end = chrono::steady_clock::now();
    double runtime = chrono::duration<double>(end - begin).count();
    printf("running time: %.4f sec.\n", runtime);
    
    return 0;
}