#include <iostream>
#include <cstdio>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cmath>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <ctime>


class Graph {
public:
    explicit Graph(FILE* file);

    // 禁用拷贝
    Graph(const Graph&) = delete;
    Graph& operator=(const Graph&) = delete;

    void Check();

    void Right_Decom();

    //baseline with advanced triangle computation
    void Adv_Decom();

    //progress anchored cycle truss computation
    void PACT(); 

    void PACO(); 

    void Peeling(uint32_t kc, uint32_t kf); 

    


private:
    uint32_t n;
    uint32_t m;

    // adjacency array entry type
    typedef struct final {
        uint32_t vid;
        uint32_t eid;
    } ArrayEntry;

    // the adjacency array representation
    std::vector<std::vector<ArrayEntry>> adj;
    std::vector<std::vector<ArrayEntry>> adj_in;
    std::vector<std::vector<ArrayEntry>> adj_out;

    // the cycle support and flow support
    std::vector<std::uint32_t> csup;
    std::vector<std::uint32_t> fsup;

    // the remaining supports
    std::vector<std::uint32_t> rem;
    std::vector<std::uint32_t> crem;
    std::vector<std::uint32_t> frem;
    std::vector<std::vector<std::uint32_t>> Drem;

    // the triangle supports
    std::vector<std::uint32_t> ts;
    std::vector<std::uint32_t> cts;
    std::vector<std::uint32_t> fts;
    // std::vector<std::uint32_t> fct;
    std::vector<std::vector<std::uint32_t>> Dts;

    // the D-trussness
    std::vector<std::vector<std::uint32_t>> D_index;

    //neighbor index
    std::vector<std::vector<std::pair<uint32_t, uint32_t>>> ftriIndex;
    std::vector<std::vector<std::pair<uint32_t, uint32_t>>> ctriIndex;

    // the truss numbers
    std::vector<std::uint32_t> tn;

    // the edge peeling order
    std::vector<std::uint32_t> ord;
    std::vector<std::uint32_t> cord;
    std::vector<std::uint32_t> ford;
    std::vector<std::vector<std::uint32_t>> Dord;
    std::vector<std::vector<std::uint32_t>> Dord_o;

    // the set of edges
    std::vector<std::pair<std::uint32_t, std::uint32_t>> edges;
    std::vector<std::pair<std::uint32_t, std::uint32_t>> edges_out;
    std::vector<std::pair<std::uint32_t, std::uint32_t>> edges_in;

    //the status of each edge
    std::vector<bool> changed;

    //ancored cycle trussness of kf
    std::vector<std::uint32_t> fct;

    //the status of each edge
    std::vector<bool> quality;




    void add_edge(uint32_t u, uint32_t v);

    //join
    std::vector<uint32_t> findNigb(std::vector<ArrayEntry>& nums1, std::vector<ArrayEntry>& nums2);
    std::vector<std::pair<uint32_t, uint32_t>> findNedges(std::vector<ArrayEntry>& nums1, std::vector<ArrayEntry>& nums2);

    std::vector<uint32_t> findNigbQ(std::vector<ArrayEntry>& nums1, std::vector<ArrayEntry>& nums2, std::vector<bool>& qualify);
    std::vector<std::pair<uint32_t, uint32_t>> findNedgesQ(std::vector<ArrayEntry>& nums1, std::vector<ArrayEntry>& nums2, std::vector<bool>& qualify);

    //find
    void intersec(std::vector<ArrayEntry>& nums1, std::vector<ArrayEntry>& nums2, std::vector<std::pair<uint32_t, uint32_t>>& cedges);

    void intercycle(std::vector<ArrayEntry>& nums1, std::vector<ArrayEntry>& nums2, uint32_t u, std::vector<std::pair<uint32_t, uint32_t>>& cedges);

    //join edges
    std::vector<uint32_t> intersection(std::vector<ArrayEntry>& nums1, std::vector<ArrayEntry>& nums2);
    std::vector<uint32_t> intersectionQuali(std::vector<ArrayEntry>& nums1, std::vector<ArrayEntry>& nums2, std::vector<bool>& qualify);
    std::vector<std::pair<uint32_t, uint32_t>> intersecedge(std::vector<ArrayEntry>& nums1, std::vector<ArrayEntry>& nums2);
    std::vector<std::pair<uint32_t, uint32_t>> intersecedgeQuali(std::vector<ArrayEntry>& nums1, std::vector<ArrayEntry>& nums2, std::vector<bool>& qualify);

    //initial index
    void bildnbIndex(std::vector<std::vector<std::pair<uint32_t, uint32_t>>>& ftriIndex,
                        std::vector<std::vector<std::pair<uint32_t, uint32_t>>>& ctriIndex,
                        std::vector<std::uint32_t>& csup,
                        std::vector<std::uint32_t>& fsup);

    void optIndex(std::vector<std::vector<std::pair<uint32_t, uint32_t>>>& ftriIndex,
                        std::vector<std::vector<std::pair<uint32_t, uint32_t>>>& ctriIndex,
                        std::vector<std::uint32_t>& csup,
                        std::vector<std::uint32_t>& fsup);

    //decomposition
    void flowDecomp(std::vector<std::vector<Graph::ArrayEntry>> adj_in,
                        std::vector<std::vector<ArrayEntry>> adj_out,
                        std::vector<uint32_t> verts,
                        std::vector<std::uint32_t>& fs_,
                        std::vector<std::uint32_t>& ford_,
                        std::uint32_t& maxf); 
    
    void cycleDecomp(std::vector<std::vector<ArrayEntry>> adj_in,
                        std::vector<std::vector<ArrayEntry>> adj_out,
                        std::vector<uint32_t> verts,
                        std::vector<std::uint32_t>& cs_,
                        std::vector<std::uint32_t>& crem_,
                        std::vector<std::uint32_t>& cts_,
                        std::vector<std::uint32_t>& cord_,
                        std::vector<bool>& qualify);
    
    void newCycleDecomp(std::vector<std::vector<ArrayEntry>> adj_in,
                        std::vector<std::vector<ArrayEntry>> adj_out,
                        std::vector<std::uint32_t>& cs_,
                        std::vector<std::uint32_t>& cord_,
                        std::vector<std::vector<std::pair<uint32_t, uint32_t>>> ctriIndex);

    void advFlowDecomp(std::vector<std::vector<Graph::ArrayEntry>> adj_in,
                        std::vector<std::vector<ArrayEntry>> adj_out,
                        std::vector<uint32_t> verts,
                        std::vector<std::uint32_t>& fs_,
                        std::vector<std::uint32_t>& fbin,
                        std::vector<std::uint32_t>& fts_,
                        std::vector<std::uint32_t>& ford_,
                        std::uint32_t& maxf); 


    void newFlowDecomp(std::vector<std::vector<Graph::ArrayEntry>> adj_in,
                        std::vector<std::vector<ArrayEntry>> adj_out,
                        std::vector<std::uint32_t>& fs_,
                        std::vector<std::uint32_t>& ford_,
                        std::vector<std::uint32_t>& fbin,
                        std::vector<std::vector<std::pair<uint32_t, uint32_t>>> ftriIndex); 

    void advCycleDecomp(std::vector<std::vector<ArrayEntry>> adj_in,
                        std::vector<std::vector<ArrayEntry>> adj_out,
                        std::vector<uint32_t> verts,
                        std::vector<std::uint32_t>& cs_,
                        std::vector<std::uint32_t>& crem_,
                        std::vector<std::uint32_t>& cts_,
                        std::vector<std::uint32_t>& cord_);
    
    void recordCycleDecomp(std::vector<std::vector<ArrayEntry>> adj_in,
                        std::vector<std::vector<ArrayEntry>> adj_out,
                        std::vector<std::uint32_t>& cs_,
                        std::vector<std::uint32_t>& cts_,
                        std::vector<std::uint32_t>& fts_,
                        std::vector<std::uint32_t>& cord_,
                        std::vector<std::vector<std::pair<uint32_t, uint32_t>>> ctriIndex);

    void baseCycleDecomp(std::vector<std::vector<ArrayEntry>> adj_in,
                        std::vector<std::vector<ArrayEntry>> adj_out,
                        std::vector<uint32_t> verts,
                        std::vector<std::uint32_t>& cs_,
                        std::vector<std::uint32_t>& crem_,
                        std::vector<std::uint32_t>& cts_,
                        std::vector<std::uint32_t>& cord_,
                        std::vector<bool>& qualify,
                        std::uint32_t kf);
    
    void baseCycleDecompO(std::vector<std::vector<ArrayEntry>> adj_in,
                        std::vector<std::vector<ArrayEntry>> adj_out,
                        std::vector<uint32_t> verts,
                        std::vector<std::uint32_t>& cs_,
                        std::vector<std::uint32_t>& crem_,
                        std::vector<std::uint32_t>& cts_,
                        std::vector<std::uint32_t>& cord_,
                        std::vector<bool>& qualify,
                        std::uint32_t kf);

    void Peel(uint32_t kf); 
    
    //local compute
    bool refineCsup(uint32_t eid, uint32_t kf, std::vector<std::uint32_t>& csup, std::vector<bool>& quality, std::vector<bool>& changed);
    bool refineCsupO(uint32_t eid, uint32_t kf, std::vector<std::uint32_t>& csup, std::vector<bool>& quality, std::vector<bool>& changed);


    //写入结果
    void DWriteToFile(const std::string& file_name, uint32_t maxf) const;
};
