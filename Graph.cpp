#include "Graph.h"
#include "defs.h"
#include <cassert>
#include <fstream>
#include <chrono>
#include <numeric>
#include <iomanip>


Graph::Graph(FILE* file) {
    clock_t begin = clock();

    if (!(std::fscanf(file, "%u%u", &n, &m) == 2)) {
        std::cerr << "Error reading graph parameters" << std::endl;
        return;
    }



    for (uint32_t i = 0; i < m; i++) {
        uint32_t u, v;
        if (std::fscanf(file, "%u%u", &u, &v) != 2) {
            std::cerr << "Error reading edge " << i << std::endl;
            break;
        }

        if(u == v) continue;
        add_edge(u, v);
    }

    for (const auto edge : edges) {
        ASSERT_MSG(edge.first < n&& edge.second < n, "invalid vertex ID");
    }

    m =  edges.size();

    std::sort(edges.begin(), edges.end());

    ASSERT_MSG(std::unique(edges.begin(), edges.end()) == edges.end(),
             "duplicate edges exist in the graph");


    // initialize adjacency arrays
    adj_out.resize(n);
    adj_in.resize(n);
    for (uint32_t eid = 0; eid < m; ++eid) {
        const uint32_t v1 = edges[eid].first;
        const uint32_t v2 = edges[eid].second;
        adj_out[v1].push_back({v2, eid});
        adj_in[v2].push_back({v1, eid});
    }

    for (uint32_t vid = 0; vid < n; ++vid) {
        adj_out[vid].shrink_to_fit();
        adj_in[vid].shrink_to_fit();

        std::sort(adj_out[vid].begin(), adj_out[vid].end(),
                [](const ArrayEntry& ae1, const ArrayEntry& ae2) {
                    return ae1.vid < ae2.vid;
                });
        std::sort(adj_in[vid].begin(), adj_in[vid].end(),
                [](const ArrayEntry& ae1, const ArrayEntry& ae2) {
                    return ae1.vid < ae2.vid;
                });
    }

    changed.clear();
    changed.resize(m,false);


    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    printf("graph construction: %.4f\n", elapsed_secs);
}

inline void Graph::add_edge(uint32_t u, uint32_t v) {
    edges.push_back({u, v});
}


//codes from Lei Chen

std::vector<uint32_t> Graph::intersection(std::vector<ArrayEntry>& nums1, std::vector<ArrayEntry>& nums2){
    if(nums1.empty() || nums2.empty()){
        return std::vector<uint32_t>();
    }

    std::vector<uint32_t> a, b;
    for(auto e : nums1) a.push_back(e.vid);
    for(auto e : nums2) b.push_back(e.vid);

    std::unordered_set<uint32_t> set{a.cbegin(), a.cend()};
    std::vector<uint32_t> intersections;

    for(auto n : b)
        if(set.erase(n) > 0)
            intersections.push_back(n);
    
    return intersections;

}

std::vector<uint32_t> Graph::intersectionQuali(std::vector<ArrayEntry>& nums1, std::vector<ArrayEntry>& nums2, std::vector<bool>& qualify){
    if(nums1.empty() || nums2.empty()){
        return std::vector<uint32_t>();
    }

    std::vector<uint32_t> a1, a2;
    for(auto e : nums1)
        if(qualify[e.eid]) 
            a1.push_back(e.vid);
    for(auto e : nums2) 
        if(qualify[e.eid])
            a2.push_back(e.vid);

    std::unordered_set<uint32_t> set{a1.cbegin(), a1.cend()};
    std::vector<uint32_t> intersections;

    for(auto n : a2)
        if(set.erase(n) > 0)
            intersections.push_back(n);
    
    return intersections;
}

std::vector<std::pair<uint32_t, uint32_t>> Graph::intersecedge(std::vector<ArrayEntry>& nums1, std::vector<ArrayEntry>& nums2){
    if(nums1.empty() || nums2.empty())
        return std::vector<std::pair<uint32_t, uint32_t>>();

    std::vector<uint32_t> a1, a2;
    for(auto e : nums1) a1.push_back(e.vid);
    for(auto e : nums2) a2.push_back(e.vid);
    std::unordered_set<uint32_t> set{a1.cbegin(), a1.cend()};
    std::vector<std::pair<uint32_t, uint32_t>> intersecedges;

    for(auto n : a2){
        if(set.erase(n) > 0){
            std::pair<uint32_t, uint32_t> arr;
            uint32_t ar1, ar2;
            for(auto e : nums1)
                if(n == e.vid) ar1 = e.eid;
            for(auto e : nums2)
                if(n == e.vid) ar2 = e.eid;
            arr = std::make_pair(ar1, ar2);
            intersecedges.push_back(arr);
        }
    }

    return intersecedges;
}

std::vector<std::pair<uint32_t, uint32_t>> Graph::intersecedgeQuali(std::vector<ArrayEntry>& nums1, std::vector<ArrayEntry>& nums2, std::vector<bool>& qualify){
    if(nums1.empty() || nums2.empty())
        return std::vector<std::pair<uint32_t, uint32_t>>();

    std::vector<uint32_t> a1, a2;
    for(auto e : nums1)
        if(qualify[e.eid]) 
            a1.push_back(e.vid);
    for(auto e : nums2)
        if(qualify[e.eid]) 
            a2.push_back(e.vid);

    std::unordered_set<uint32_t> set{a1.cbegin(), a1.cend()};
    std::vector<std::pair<uint32_t, uint32_t>> intersecedges;
    for(auto n : a2){
        if(set.erase(n) > 0){
            std::pair<uint32_t, uint32_t> arr;
            uint32_t ar1, ar2;
            for(auto e : nums1)
                if(n == e.vid) ar1 = e.eid;
            for(auto e : nums2)
                if(n == e.vid) ar2 = e.eid;
            arr = std::make_pair(ar1, ar2);
            intersecedges.push_back(arr);
        }
    }

    return intersecedges;
}

void Graph::cycleDecomp(std::vector<std::vector<ArrayEntry>> adj_in,
                        std::vector<std::vector<ArrayEntry>> adj_out,
                        std::vector<uint32_t> verts,
                        std::vector<std::uint32_t>& csup,
                        std::vector<std::uint32_t>& crem,
                        std::vector<std::uint32_t>& cts,
                        std::vector<std::uint32_t>& cord,
                        std::vector<bool>& qualify) 
{
    const uint32_t maxc = *max_element(csup.cbegin(), csup.cend());
    std::vector<uint32_t> cbin(maxc + 1, 0);
    std::vector<uint32_t> cpos(m);

    //initialize cbin, cord, and cpos
    for(uint32_t eid = 0; eid < m; ++eid){
        if(!qualify[eid]) continue;
        ++cbin[csup[eid]];
    }
    for(uint32_t i = 0, start = 0; i <= maxc; ++i){
        start += cbin[i];
        cbin[i] = start - cbin[i];
    }
    for(uint32_t eid = 0; eid < m; ++eid){
        if(!qualify[eid]) continue;
        cpos[eid] = cbin[csup[eid]];
        cord[cpos[eid]] = eid;
        ++cbin[csup[eid]];
    }
    std::rotate(cbin.rbegin(), cbin.rbegin() + 1, cbin.rend());
    cbin[0] = 0;

    //peeling
    std::vector<bool> cremove(m, false);
    uint32_t c = 0;

    for(uint32_t i = 0; i < m; ++i){
        const uint32_t eid = cord[i];
        if(!qualify[eid]) continue;
        c = std::max(c, csup[eid]);
        ++cbin[csup[eid]];
        cremove[eid] = true;

        //find triangles containing eid
        std::vector<std::pair<uint32_t, uint32_t>> ctris; {
            const uint32_t v1 = edges[eid].first;
            const uint32_t v2 = edges[eid].second;
            ctris = intersecedgeQuali(adj_in[v1], adj_out[v2], qualify); 
        }

        //update csuport
        for(const auto tri : ctris){
            const uint32_t e1 = tri.first;
            const uint32_t e2 = tri.second;
            if(!qualify[e1] || !qualify[e2]) continue;
            if(csup[e1] >= c && csup[e2] >= c) ++cts[eid];
            if(cremove[e1] || cremove[e2]) continue;
            ++crem[eid];

            for(const uint32_t e : {e1, e2}){
                if(csup[e] > c){
                    const uint32_t pe3 = cbin[csup[e]];
                    const uint32_t pe = cpos[e];
                    if(pe3 != pe){
                        const uint32_t e3 = cord[pe3];
                        cord[pe] = e3;
                        cpos[e3] = pe;
                        cord[pe3] = e;
                        cpos[e] = pe3;
                    }
                    ++cbin[csup[e]];
                    --csup[e];
                }
            }
        }

    }

}


void Graph::flowDecomp(std::vector<std::vector<ArrayEntry>> adj_in,
                        std::vector<std::vector<ArrayEntry>> adj_out,
                        std::vector<uint32_t> verts,
                        std::vector<std::uint32_t>& fsup,
                        std::vector<std::uint32_t>& ford,
                        std::uint32_t& maxf) {

    //count flow supports
    for (const uint32_t u : verts) {    
        for (const auto ae : adj_out[u]) {
            const uint32_t v = ae.vid;
            const uint32_t e = ae.eid;

            //3 cases of flow triangle 
            std::vector<uint32_t> W1,W2,W3;
            W1 = intersection(adj_out[u], adj_out[v]);
            W2 = intersection(adj_out[u], adj_in[v]);
            W3 = intersection(adj_in[u], adj_in[v]);

            fsup[e] = W1.size() + W2.size() + W3.size();
        }
    }

    maxf = *max_element(fsup.cbegin(), fsup.cend());

    // printf("max fsup: %d.\n", maxf);

    //decomposition: initialize bin, pos, and flow_order
    std::vector<uint32_t> fbin(maxf + 1, 0);
    std::vector<uint32_t> fpos(m);

    for(uint32_t eid = 0; eid < m; ++eid){
        ++fbin[fsup[eid]];
    }
    for(uint32_t i = 0, start = 0; i <= maxf; ++i){
        start += fbin[i];
        fbin[i] = start - fbin[i];
    }
    for (uint32_t eid = 0; eid < m; ++eid) {
        fpos[eid] = fbin[fsup[eid]];
        ford[fpos[eid]] = eid;
        ++fbin[fsup[eid]];
    }
    std::rotate(fbin.rbegin(), fbin.rbegin() + 1, fbin.rend());
    fbin[0] = 0;

    //decomposition: compute kf
    std::vector<bool> fremoved(m, false);
    uint32_t f = 0;

    for(uint32_t i = 0; i < m; ++i){
        const uint32_t eid = ford[i];
        f = std::max(f, fsup[ford[i]]);

        ++fbin[fsup[eid]];
        fremoved[eid] = true;

        //find triangles containing edge eid
        // std::vector<std::pair<uint32_t, uint32_t>> *ftris; 


        std::vector<std::vector<std::pair<uint32_t,uint32_t>>> ftris(3);

        // ftris = new std::vector<std::pair<uint32_t, uint32_t>>[3]; 
        {
            const uint32_t v1 = edges[eid].first;
            const uint32_t v2 = edges[eid].second;
            //case 1
            ftris[0] = intersecedge(adj_out[v1], adj_out[v2]);
            //case 2
            ftris[1] = intersecedge(adj_out[v1], adj_in[v2]);
            //case 3
            ftris[2] = intersecedge(adj_in[v1], adj_in[v2]);
        }

        //peeling
        for(int i = 0; i < 3; i++){
            for(const auto tri : ftris[i]){
                const uint32_t e1 = tri.first;
                const uint32_t e2 = tri.second;
                // if(fsup[e1] >= f && fsup[e2] >= f) ++fts[eid];
                if(fremoved[e1] || fremoved[e2]) continue;

                for(const uint32_t e : {e1, e2}){
                    if(fsup[e] > f){
                        const uint32_t pe3 = fbin[fsup[e]];
                        const uint32_t pe = fpos[e];
                        if(pe3 != pe){
                            const uint32_t e3 = ford[pe3];
                            ford[pe] = e3;
                            fpos[e3] = pe;
                            ford[pe3] = e;
                            fpos[e] = pe3;
                        }
                        ++fbin[fsup[e]];
                        --fsup[e];
                    }
                }
                
            }
        }

    }
    
}


//-----------------right baseline--------------------//

void Graph::baseCycleDecomp(std::vector<std::vector<ArrayEntry>> adj_in,
                        std::vector<std::vector<ArrayEntry>> adj_out,
                        std::vector<uint32_t> verts,
                        std::vector<std::uint32_t>& cs_,
                        std::vector<std::uint32_t>& crem_,
                        std::vector<std::uint32_t>& cts_,
                        std::vector<std::uint32_t>& cord_,
                        std::vector<bool>& qualify,
                        std::uint32_t kf)
{
    //initial fsup for all edges with qualify = true
    std::vector<uint32_t> fsup(m,0);
    uint32_t uiRmCnt = 0;
    std::vector<bool> removed(m, false);
    for (uint32_t eid = 0; eid < m; ++eid) {
        if(!qualify[eid]){
            uiRmCnt++;
            removed[eid] = true;
            csup[eid] = 0;
            continue;
        }
        const uint32_t v1 = edges[eid].first;
        const uint32_t v2 = edges[eid].second;
        std::vector<std::pair<uint32_t, uint32_t>> ftris;
        ftris = intersecedgeQuali(adj_out[v1], adj_in[v2], qualify); 

        for (const auto tri : ftris)
        {
            const uint32_t e1 = tri.first;
            const uint32_t e2 = tri.second;
            ++fsup[eid];++fsup[e1];++fsup[e2];
        }
    }

    // printf("kf = %d, removed edges: %d.\n", kf, uiRmCnt);

    // peeling
    std::vector<bool> vWaitRmFlag(m, false);
    uint32_t iK_c = 0;
    std::vector<uint32_t> vWait(m);
    std::vector<uint32_t> vWaitRmCache(m);
    // init cache
    vWaitRmCache.clear();

    

    while (uiRmCnt < m){

        //scan edges with csup <= kc and fsup < kf
        for (uint32_t eid = 0; eid < m; ++eid){
            if(removed[eid] || !qualify[eid]) continue;
            if (csup[eid] <= iK_c){
                vWaitRmFlag[eid] = true;
                vWaitRmCache.push_back(eid);
            }
            else if (fsup[eid] < kf){
                csup[eid] = iK_c;
                vWaitRmFlag[eid] = true;
                vWaitRmCache.push_back(eid);
            }
        }

        while(!vWaitRmCache.empty()){
            vWait.clear();
            vWait.swap(vWaitRmCache);

            for (auto eid : vWait){
                removed[eid] = true;
                ++uiRmCnt;

                //find cycle triangles and flow triangles contain eid
                std::vector<std::pair<uint32_t, uint32_t>> ftris, ftristmp1, ftristmp2, ctris;
                uint32_t v1 = edges[eid].first;
                uint32_t v2 = edges[eid].second;

                ctris = intersecedgeQuali(adj_in[v1], adj_out[v2], qualify); 

                ftris = intersecedgeQuali(adj_out[v1], adj_out[v2], qualify);
                ftristmp1 = intersecedgeQuali(adj_out[v1], adj_in[v2], qualify);
                ftristmp2 = intersecedgeQuali(adj_in[v1], adj_in[v2], qualify);
                for (auto e1 : ftristmp1) ftris.push_back(e1);
                for (auto e1 : ftristmp2) ftris.push_back(e1);
                ftris.erase(std::unique(ftris.begin(), ftris.end()), ftris.end());


            
                for (const auto tri : ftris){
                    const uint32_t e1 = tri.first;
                    const uint32_t e2 = tri.second;
                    if (removed[e1] || removed[e2]) continue;
                    if (!vWaitRmFlag[e1]){
                        --fsup[e1];
                        if (fsup[e1] < kf){
                            vWaitRmFlag[e1] = true;
                            vWaitRmCache.push_back(e1);
                            csup[e1] = iK_c;
                        }
                    }
                    if (!vWaitRmFlag[e2])
                    {
                        --fsup[e2];
                        if (fsup[e2] < kf){
                            vWaitRmFlag[e2] = true;
                            vWaitRmCache.push_back(e2);
                            csup[e2] = iK_c;
                        }
                    }
                }

                for (const auto tri : ctris){
                    const uint32_t e1 = tri.first;
                    const uint32_t e2 = tri.second;
                    if (removed[e1] || removed[e2]) continue;
                    if (!vWaitRmFlag[e1]){
                        --csup[e1];
                        if (csup[e1] <= iK_c){
                            vWaitRmFlag[e1] = true;
                            vWaitRmCache.push_back(e1);
                        }
                    }
                    if (!vWaitRmFlag[e2]){
                        --csup[e2];
                        if (csup[e2] <= iK_c){
                            vWaitRmFlag[e2] = true;
                            vWaitRmCache.push_back(e2);
                        }
                    }
                }
            }

        }

        iK_c++;

    }
}

void Graph::Right_Decom(){
    //initialize vertices
    std::vector<uint32_t> verts(n);
    std::iota(verts.begin(), verts.end(), 0);


    //initialize flow sup and compute kf
    uint32_t maxf= 0;
    fsup.resize(m, 0); ford.resize(m); frem.resize(m, 0); fts.resize(m, 0); 

    flowDecomp(adj_in, adj_out, verts, fsup, ford, maxf);

    maxf = *max_element(fsup.cbegin(), fsup.cend());


    printf("kf max: %d.\n", maxf);

    //initialize results
    std::vector<bool> quality(m, true);
    D_index.resize(maxf+1);
    Dord.resize(maxf+1); Drem.resize(maxf+1); Dts.resize(maxf+1);


    //compute kc for each kf
    for(uint32_t i = 0; i <= maxf; i++){
        //remove edges with kf < i
        uint32_t remain_count = 0;
        for(uint32_t eid = 0; eid < m; eid++)
            if(fsup[eid] < i){
                quality[eid] = false;
                remain_count++;
            }
        remain_count = m - remain_count;


                
               

        //intialize cycle structures
        csup.resize(m, 0); cord.resize(m); crem.resize(m, 0); cts.resize(m, 0);

        // auto begin1= std::chrono::steady_clock::now();
        //count csup
        for(const uint32_t u : verts){
            for(const auto ve : adj_out[u]){
                const uint32_t v = ve.vid;
                const uint32_t e = ve.eid;
                if(!quality[e]) continue;
                std::vector<uint32_t> W;
                W = intersectionQuali(adj_in[u], adj_out[v], quality);
                csup[e] += W.size();
            }
        }
        uint32_t maxc = *max_element(csup.cbegin(), csup.cend());
        // auto end1= std::chrono::steady_clock::now();

        if(!maxc){
            D_index[i] = csup; Drem[i] = crem; Dts[i] = cts; Dord[i] = cord;
            break;
        }

        //cycle decomposition, store the results
        if(i==0) cycleDecomp(adj_in, adj_out, verts, csup, crem, cts, cord, quality);
        else baseCycleDecomp(adj_in, adj_out, verts, csup, crem, cts, cord, quality, i);

        maxc = *max_element(csup.cbegin(), csup.cend());

        printf("i = %d, max csup: %d, remain edges: %d.\n", i, maxc,remain_count);

        //for i > 0, use the cycle decomposition for given kf
        D_index[i].resize(m, 0); Drem[i].resize(m, 0); Dts[i].resize(m, 0); Dord[i].resize(m);
        D_index[i] = csup; Drem[i] = crem; Dts[i] = cts; Dord[i] = cord;
        csup.clear(); crem.clear(); cts.clear(); cord.clear();
    }


    // DWriteToFile("res_exam.txt", maxf);
    

}


void Graph::baseCycleDecompO(std::vector<std::vector<ArrayEntry>> adj_in,
                        std::vector<std::vector<ArrayEntry>> adj_out,
                        std::vector<uint32_t> verts,
                        std::vector<std::uint32_t>& cs_,
                        std::vector<std::uint32_t>& crem_,
                        std::vector<std::uint32_t>& cts_,
                        std::vector<std::uint32_t>& cord_,
                        std::vector<bool>& qualify,
                        std::uint32_t kf)
{
    //initial fsup for all edges with qualify = true
    std::vector<uint32_t> fsup(m,0);
    uint32_t uiRmCnt = 0;
    std::vector<bool> removed(m, false);
    for (uint32_t eid = 0; eid < m; ++eid) {
        if(!qualify[eid]){
            uiRmCnt++;
            removed[eid] = true;
            csup[eid] = 0;
            continue;
        }
        const uint32_t v1 = edges[eid].first;
        const uint32_t v2 = edges[eid].second;
        std::vector<std::pair<uint32_t, uint32_t>> ftris;
        ftris = findNedgesQ(adj_out[v1], adj_in[v2], qualify); 

        for (const auto tri : ftris)
        {
            const uint32_t e1 = tri.first;
            const uint32_t e2 = tri.second;
            ++fsup[eid];++fsup[e1];++fsup[e2];
        }
    }

    // peeling
    std::vector<bool> vWaitRmFlag(m, false);
    uint32_t iK_c = 0;
    std::vector<uint32_t> vWait(m);
    std::vector<uint32_t> vWaitRmCache(m);
    // init cache
    vWaitRmCache.clear();

    

    while (uiRmCnt < m){

        //scan edges with csup <= kc and fsup < kf
        for (uint32_t eid = 0; eid < m; ++eid){
            if(removed[eid] || !qualify[eid]) continue;
            if (csup[eid] <= iK_c){
                vWaitRmFlag[eid] = true;
                vWaitRmCache.push_back(eid);
            }
            else if (fsup[eid] < kf){
                csup[eid] = iK_c;
                vWaitRmFlag[eid] = true;
                vWaitRmCache.push_back(eid);
            }
        }

        while(!vWaitRmCache.empty()){
            vWait.clear();
            vWait.swap(vWaitRmCache);

            for (auto eid : vWait){
                removed[eid] = true;
                ++uiRmCnt;

                //find cycle triangles and flow triangles contain eid
                std::vector<std::pair<uint32_t, uint32_t>> ftris, ftristmp1, ftristmp2, ctris;
                uint32_t v1 = edges[eid].first;
                uint32_t v2 = edges[eid].second;

                ctris = findNedgesQ(adj_in[v1], adj_out[v2], qualify); 

                ftris = findNedgesQ(adj_out[v1], adj_out[v2], qualify);
                ftristmp1 = findNedgesQ(adj_out[v1], adj_in[v2], qualify);
                ftristmp2 = findNedgesQ(adj_in[v1], adj_in[v2], qualify);
                for (auto e1 : ftristmp1) ftris.push_back(e1);
                for (auto e1 : ftristmp2) ftris.push_back(e1);
                ftris.erase(std::unique(ftris.begin(), ftris.end()), ftris.end());


            
                for (const auto tri : ftris){
                    const uint32_t e1 = tri.first;
                    const uint32_t e2 = tri.second;
                    if (removed[e1] || removed[e2]) continue;
                    if (!vWaitRmFlag[e1]){
                        --fsup[e1];
                        if (fsup[e1] < kf){
                            vWaitRmFlag[e1] = true;
                            vWaitRmCache.push_back(e1);
                            csup[e1] = iK_c;
                        }
                    }
                    if (!vWaitRmFlag[e2])
                    {
                        --fsup[e2];
                        if (fsup[e2] < kf){
                            vWaitRmFlag[e2] = true;
                            vWaitRmCache.push_back(e2);
                            csup[e2] = iK_c;
                        }
                    }
                }

                for (const auto tri : ctris){
                    const uint32_t e1 = tri.first;
                    const uint32_t e2 = tri.second;
                    if (removed[e1] || removed[e2]) continue;
                    if (!vWaitRmFlag[e1]){
                        --csup[e1];
                        if (csup[e1] <= iK_c){
                            vWaitRmFlag[e1] = true;
                            vWaitRmCache.push_back(e1);
                        }
                    }
                    if (!vWaitRmFlag[e2]){
                        --csup[e2];
                        if (csup[e2] <= iK_c){
                            vWaitRmFlag[e2] = true;
                            vWaitRmCache.push_back(e2);
                        }
                    }
                }
            }

        }

        iK_c++;

    }
}



//advanced baseline
void Graph::Adv_Decom(){
    //initialize vertices
    std::vector<uint32_t> verts(n);
    std::iota(verts.begin(), verts.end(), 0);

    std::vector<std::uint32_t> fbin;
    //initialize flow sup and compute kf
    uint32_t maxf= 0;
    fsup.resize(m, 0); ford.resize(m); fbin.resize(m, 0); fts.resize(m, 0); 

    // count flow support
    for (uint32_t eid = 0; eid < m; ++eid) {
        const uint32_t v1 = edges[eid].first;
        const uint32_t v2 = edges[eid].second;
        std::vector<std::pair<uint32_t, uint32_t>> ftris;
        ftris = findNedges(adj_out[v1], adj_in[v2]); 
        
        for (const auto tri : ftris)
        {
            const uint32_t e1 = tri.first;
            const uint32_t e2 = tri.second;
            ++fsup[eid];++fsup[e1];++fsup[e2];
        }
    }

    advFlowDecomp(adj_in, adj_out, verts, fsup, fbin, fts, ford, maxf);

    maxf = *max_element(fsup.cbegin(), fsup.cend());


    printf("kf max: %d.\n", maxf);

    //initialize results
    std::vector<bool> quality(m, true);
    D_index.resize(maxf+1);
    Dord.resize(maxf+1); Drem.resize(maxf+1); Dts.resize(maxf+1);


    //compute kc for each kf
    for(uint32_t i = 0; i <= maxf; i++){
        //remove edges with kf < i
        uint32_t remain_count = 0;
        for(uint32_t eid = 0; eid < m; eid++)
            if(fsup[eid] < i){
                quality[eid] = false;
                remain_count++;
            }
        remain_count = m - remain_count;


                
               

        //intialize cycle structures
        csup.resize(m, 0); cord.resize(m); crem.resize(m, 0); cts.resize(m, 0);

        // auto begin1= std::chrono::steady_clock::now();
        //count csup
        for(const uint32_t u : verts){
            for(const auto ve : adj_out[u]){
                const uint32_t v = ve.vid;
                const uint32_t e = ve.eid;
                if(!quality[e]) continue;
                std::vector<uint32_t> W;
                W = findNigbQ(adj_in[u], adj_out[v], quality);
                csup[e] += W.size();
            }
        }
        uint32_t maxc = *max_element(csup.cbegin(), csup.cend());
        // auto end1= std::chrono::steady_clock::now();

        // printf("max csup: %d, initial time: %.4f.\n", maxc,std::chrono::duration<double>(end1 - begin1).count());

        if(!maxc){
            D_index[i] = csup; Drem[i] = crem; Dts[i] = cts; Dord[i] = cord;
            break;
        }

        //cycle decomposition, store the results
        if(i==0) advCycleDecomp(adj_in, adj_out, verts, csup, crem, cts, cord);
        else baseCycleDecompO(adj_in, adj_out, verts, csup, crem, cts, cord, quality, i);

        maxc = *max_element(csup.cbegin(), csup.cend());

        printf("i = %d, max csup: %d, remain edges: %d.\n", i, maxc,remain_count);

        //for i > 0, use the cycle decomposition for given kf
        D_index[i].resize(m, 0); Drem[i].resize(m, 0); Dts[i].resize(m, 0); Dord[i].resize(m);
        D_index[i] = csup; Drem[i] = crem; Dts[i] = cts; Dord[i] = cord;
        csup.clear(); crem.clear(); cts.clear(); cord.clear();
    }


    // DWriteToFile("tres.txt", maxf);
    

}








////////////////////////////////--------new join approches--------////////////////////////////////////////////////////
std::vector<uint32_t> Graph::findNigb(std::vector<ArrayEntry>& nums1, std::vector<ArrayEntry>& nums2){
    if(nums1.empty() || nums2.empty()){
        return std::vector<uint32_t>();
    }
    
    std::vector<uint32_t> result;

    uint32_t p1 = 0, p2 = 0;
    while (p1 < nums1.size() && p2 < nums2.size()) {
        if (nums1[p1].vid == nums2[p2].vid) {
            result.push_back(nums1[p1].vid);
            ++p1; ++p2;
        } else if (nums1[p1].vid < nums2[p2].vid) {
            ++p1;
        } else {
            ++p2;
        }
    }

    return result;
}

std::vector<uint32_t> Graph::findNigbQ(std::vector<ArrayEntry>& nums1, std::vector<ArrayEntry>& nums2, std::vector<bool>& qualify){
    if(nums1.empty() || nums2.empty()){
        return std::vector<uint32_t>();
    }

    std::vector<uint32_t> intersections;
    
    uint32_t i = 0, j = 0;
    while(i < nums1.size() && j < nums2.size()) {
        if(!qualify[nums1[i].eid]) {
            i++;
            continue;
        }
        if(!qualify[nums2[j].eid]) {
            j++;
            continue;
        }

        if(nums1[i].vid < nums2[j].vid) {
            i++;
        } else if(nums1[i].vid > nums2[j].vid) {
            j++;
        } else {
            intersections.push_back(nums1[i].vid);
            i++;
            j++;
        }
    }

    return intersections;
}

std::vector<std::pair<uint32_t, uint32_t>> Graph::findNedges(std::vector<ArrayEntry>& nums1, std::vector<ArrayEntry>& nums2){
    
    std::vector<std::pair<uint32_t, uint32_t>> commonEdges;
    size_t p1 = 0, p2 = 0;
    while (p1 < nums1.size() && p2 < nums2.size()) {
        if (nums1[p1].vid == nums2[p2].vid) {
            commonEdges.push_back(std::pair<uint32_t, uint32_t>(nums1[p1].eid, nums2[p2].eid));
            ++p1; ++p2;
        } else if (nums1[p1].vid < nums2[p2].vid) {
            ++p1;
        } else {
            ++p2;
        }
    }

    return commonEdges;
}


void Graph::intersec(std::vector<ArrayEntry>& nums1, std::vector<ArrayEntry>& nums2, std::vector<std::pair<uint32_t, uint32_t>>& cedges){
    cedges.clear();
    size_t p1 = 0, p2 = 0;
    while (p1 < nums1.size() && p2 < nums2.size()) {
        if (nums1[p1].vid == nums2[p2].vid) {
            cedges.push_back(std::pair<uint32_t, uint32_t>(nums1[p1].eid, nums2[p2].eid));
            ++p1; ++p2;
        } else if (nums1[p1].vid < nums2[p2].vid) {
            ++p1;
        } else {
            ++p2;
        }
    }
}

void Graph::intercycle(std::vector<ArrayEntry>& nums1, std::vector<ArrayEntry>& nums2, uint32_t u, std::vector<std::pair<uint32_t, uint32_t>>& cedges){
    cedges.clear();
    size_t p1 = 0, p2 = 0;
    while (p1 < nums1.size() && p2 < nums2.size()) {
        if (nums1[p1].vid == nums2[p2].vid) {
            uint32_t w = nums1[p1].vid;
            if(w > u)
                cedges.push_back(std::pair<uint32_t, uint32_t>(nums1[p1].eid, nums2[p2].eid));
            ++p1; ++p2;
        } else if (nums1[p1].vid < nums2[p2].vid) {
            ++p1;
        } else {
            ++p2;
        }
    }
}

std::vector<std::pair<uint32_t, uint32_t>> Graph::findNedgesQ(std::vector<ArrayEntry>& nums1, std::vector<ArrayEntry>& nums2, std::vector<bool>& qualify){
    if(nums1.empty() || nums2.empty())
        return std::vector<std::pair<uint32_t, uint32_t>>();

    std::vector<std::pair<uint32_t, uint32_t>> intersecedges;

    uint32_t f1 =0, f2 = 0;
    
    uint32_t i = 0, j = 0;
    while(i < nums1.size() && j < nums2.size()) {
        if(!qualify[nums1[i].eid]) {
            i++;
            continue;
        }
        if(!qualify[nums2[j].eid]) {
            j++;
            continue;
        }
        
        if(nums1[i].vid < nums2[j].vid) {
            i++;
        } else if(nums1[i].vid > nums2[j].vid) {
            j++;
        } else {
            intersecedges.push_back(std::make_pair(nums1[i].eid, nums2[j].eid));
            i++;
            j++;
        }
    }

    return intersecedges;
}

//test for specific kf
void Graph::Peel(uint32_t kf)
{
    std::vector<uint32_t> fsup(m,0);
    for (uint32_t eid = 0; eid < m; ++eid) {
        const uint32_t v1 = edges[eid].first;
        const uint32_t v2 = edges[eid].second;
        std::vector<std::pair<uint32_t, uint32_t>> ftris;
        ftris = intersecedge(adj_out[v1], adj_in[v2]); 
        
        for (const auto tri : ftris)
        {
            const uint32_t e1 = tri.first;
            const uint32_t e2 = tri.second;
            ++fsup[eid];++fsup[e1];++fsup[e2];
        }
    }

    uint32_t maxf = *max_element(fsup.cbegin(), fsup.cend());

    // // auto end1 = std::chrono::steady_clock::now();

    printf("max fsup: %d.\n", maxf);

    // ///////////////peeling/////////////////////
    std::vector<bool> removed(m, false);
    std::vector<bool> vWaitRmFlag(m, false);
    uint32_t uiRmCnt = 0;
    std::vector<uint32_t> vWait(m);
    std::vector<uint32_t> vWaitRmCache(m);

    // uint32_t kf = 45;

    // init cache
    vWaitRmCache.clear();
    for (uint32_t eid = 0; eid < m; ++eid){
      if (fsup[eid] < kf){
        vWaitRmFlag[eid] = true;
        vWaitRmCache.push_back(eid);
      }
    }

    while (uiRmCnt < m){
        vWait.clear();
        if (vWaitRmCache.empty()){
            /* all remained are D-truss */
            break;
        }
        else{
            vWait.swap(vWaitRmCache);
        }

        for (auto eid : vWait){
            removed[eid] = true;
            ++uiRmCnt;
            //printf("start find triangles\n");
            std::vector<std::pair<uint32_t, uint32_t>> tris;
            const uint32_t v1 = edges[eid].first;
            const uint32_t v2 = edges[eid].second;
            
            std::vector<std::pair<uint32_t, uint32_t>> ftris, ftristmp1, ftristmp2; {
                const uint32_t v1 = edges[eid].first;
                const uint32_t v2 = edges[eid].second;
                ftris = intersecedge(adj_out[v1], adj_in[v2]); 
                ftristmp1 = intersecedge(adj_in[v1], adj_in[v2]); 
                ftristmp2 = intersecedge(adj_out[v1], adj_out[v2]); 
                for(auto e1: ftristmp1){
                    ftris.push_back(e1);
                }
                for(auto e1: ftristmp2){
                    ftris.push_back(e1);
                }
                ftris.erase(std::unique(ftris.begin(), ftris.end()), ftris.end());
            }

            for (const auto tri : ftris){
                const uint32_t e1 = tri.first;
                const uint32_t e2 = tri.second;
                if (removed[e1] || removed[e2]) continue;
                if (!vWaitRmFlag[e1]){
                    --fsup[e1];
                    if (fsup[e1] < kf){
                        vWaitRmFlag[e1] = true;
                        vWaitRmCache.push_back(e1);
                    }
                }
                if (!vWaitRmFlag[e2]){
                    --fsup[e2];
                    if (fsup[e2] < kf){
                        vWaitRmFlag[e2] = true;
                        vWaitRmCache.push_back(e2);
                    }
                }
            }
        }

    }

    std::vector<uint32_t> res;

    for (uint32_t eid = 0; eid < m; ++eid){
        if (!removed[eid]){
            res.push_back(eid);
        }
    }
    // printf("Peeling done!\n");
    // printf("number of edges: %d.\n",res.size());

}

void Graph::advFlowDecomp(std::vector<std::vector<ArrayEntry>> adj_in,
    std::vector<std::vector<ArrayEntry>> adj_out,
    std::vector<uint32_t> verts,
    std::vector<std::uint32_t>& fsup,
    std::vector<std::uint32_t>& fbin,
    std::vector<std::uint32_t>& fts,
    std::vector<std::uint32_t>& ford,
    std::uint32_t& maxf) 
{
    // auto begin = std::chrono::steady_clock::now();

    maxf = *max_element(fsup.cbegin(), fsup.cend());


    //flow decomposition: initialize bin, pos
    fbin.resize(maxf + 1,0);
    std::vector<uint32_t> fpos(m);

    for(uint32_t eid = 0; eid < m; ++eid){
        ++fbin[fsup[eid]];
    }
    for(uint32_t i = 0, start = 0; i <= maxf; ++i){
        start += fbin[i];
        fbin[i] = start - fbin[i];
    }
    for (uint32_t eid = 0; eid < m; ++eid) {
        fpos[eid] = fbin[fsup[eid]];
        ford[fpos[eid]] = eid;
        ++fbin[fsup[eid]];
    }
    std::rotate(fbin.rbegin(), fbin.rbegin() + 1, fbin.rend());
    fbin[0] = 0;

    //decomposition: compute kf
    std::vector<bool> fremoved(m, false);
    uint32_t f = 0;

    for(uint32_t i = 0; i < m; ++i){
        const uint32_t eid = ford[i];
        f = std::max(f, fsup[ford[i]]);

        ++fbin[fsup[eid]];
        fremoved[eid] = true;
        std::vector<std::pair<uint32_t, uint32_t>> ftris;{
            const uint32_t v1 = edges[eid].first;
            const uint32_t v2 = edges[eid].second;
            ftris = findNedges(adj_out[v1], adj_in[v2]); 
            auto tmp1  = findNedges(adj_in[v1], adj_in[v2]);
            auto tmp2  = findNedges(adj_out[v1], adj_out[v2]);
            ftris.insert(ftris.end(), tmp1.begin(), tmp1.end());
            ftris.insert(ftris.end(), tmp2.begin(), tmp2.end());
        }

        //peeling
        for(const auto tri : ftris){
            const uint32_t e1 = tri.first;
            const uint32_t e2 = tri.second;
            // if(fsup[e1] >= f && fsup[e2] >= f) ++fts[eid];
            if(fremoved[e1] || fremoved[e2]) continue;
            // ++frem[eid];

            for(const uint32_t e : {e1, e2}){
                if(fsup[e] > f){
                    const uint32_t pe3 = fbin[fsup[e]];
                    const uint32_t pe = fpos[e];
                    if(pe3 != pe){
                        const uint32_t e3 = ford[pe3];
                        ford[pe] = e3;
                        fpos[e3] = pe;
                        ford[pe3] = e;
                        fpos[e] = pe3;
                    }
                    ++fbin[fsup[e]];
                    --fsup[e];
                }
            }
        }
    }

    // auto end = std::chrono::steady_clock::now();

    // printf("flow decomposition time: %.4f.\n", std::chrono::duration<double>(end - begin).count());
}



void Graph::advCycleDecomp(std::vector<std::vector<ArrayEntry>> adj_in,
    std::vector<std::vector<ArrayEntry>> adj_out,
    std::vector<uint32_t> verts,
    std::vector<std::uint32_t>& csup,
    std::vector<std::uint32_t>& crem,
    std::vector<std::uint32_t>& cts,
    std::vector<std::uint32_t>& cord) 
{
    // auto begin = std::chrono::steady_clock::now();

    uint32_t maxc = *max_element(csup.cbegin(), csup.cend());

    if(maxc){
        const uint32_t maxc = *max_element(csup.cbegin(), csup.cend());
        std::vector<uint32_t> cbin(maxc + 1, 0);
        std::vector<uint32_t> cpos(m);

        //initialize cbin, cord, and cpos
        for(uint32_t eid = 0; eid < m; ++eid){
            // if(!qualify[eid]) continue;
            ++cbin[csup[eid]];
        }
        for(uint32_t i = 0, start = 0; i <= maxc; ++i){
            start += cbin[i];
            cbin[i] = start - cbin[i];
        }
        for(uint32_t eid = 0; eid < m; ++eid){
            // if(!qualify[eid]) continue;
            cpos[eid] = cbin[csup[eid]];
            cord[cpos[eid]] = eid;
            ++cbin[csup[eid]];
        }
        std::rotate(cbin.rbegin(), cbin.rbegin() + 1, cbin.rend());
        cbin[0] = 0;

        //peeling
        std::vector<bool> cremove(m, false);
        uint32_t c = 0;

        for(uint32_t i = 0; i < m; ++i){
            const uint32_t eid = cord[i];
            // if(!qualify[eid]) continue;
            c = std::max(c, csup[eid]);
            ++cbin[csup[eid]];
            cremove[eid] = true;

            //find triangles containing eid
            std::vector<std::pair<uint32_t, uint32_t>> ctris; {
                const uint32_t v1 = edges[eid].first;
                const uint32_t v2 = edges[eid].second;
                ctris = findNedges(adj_in[v1], adj_out[v2]); 
            }

            //update csuport
            for(const auto tri : ctris){
                const uint32_t e1 = tri.first;
                const uint32_t e2 = tri.second;
                // if(!qualify[e1] || !qualify[e2]) continue;
                if(csup[e1] >= c && csup[e2] >= c) ++cts[eid];
                if(cremove[e1] || cremove[e2]) continue;
                ++crem[eid];

                for(const uint32_t e : {e1, e2}){
                    if(csup[e] > c){
                    const uint32_t pe3 = cbin[csup[e]];
                    const uint32_t pe = cpos[e];
                    if(pe3 != pe){
                        const uint32_t e3 = cord[pe3];
                        cord[pe] = e3;
                        cpos[e3] = pe;
                        cord[pe3] = e;
                        cpos[e] = pe3;
                    }
                    ++cbin[csup[e]];
                    --csup[e];
                    }
                }
            }

        }
    }

    // for(uint32_t i = 0; i < m; ++i){
    //     printf("%d, (%d, %d): %d.\n", i, edges[i].first, edges[i].second, csup[i]);
    // }

    // auto end = std::chrono::steady_clock::now();

    // printf("cycle decomposition time: %.4f.\n", std::chrono::duration<double>(end - begin).count());
}


void Graph::DWriteToFile(const std::string& file_name, uint32_t maxf) const {
    std::vector<std::vector<uint32_t>> res;
    res.resize(m);

    std::ofstream outFile(file_name);
    
    if (!outFile) {
        std::cerr << "无法创建文件！" << std::endl;
    }

    for(uint32_t i = 0; i <=  maxf; i++){
        auto cres = D_index[i];
        for (uint32_t eid = 0; eid < m; ++eid)
        {
            res[eid].push_back(cres[eid]);
        }
    }

    for (uint32_t eid = 0; eid < m; ++eid){
        outFile << eid << " ";
        for(uint32_t i = 0; i < res[eid].size(); i++){
            outFile << res[eid][i];
            if (i < res[eid].size() - 1) {
                outFile << " ";
            }
        }
        outFile << std::endl;
    }

    outFile.close();
}



void Graph::recordCycleDecomp(std::vector<std::vector<ArrayEntry>> adj_in,
                        std::vector<std::vector<ArrayEntry>> adj_out,
                        std::vector<std::uint32_t>& cs_,
                        std::vector<std::uint32_t>& cts_,
                        std::vector<std::uint32_t>& fts_,
                        std::vector<std::uint32_t>& cord_,
                        std::vector<std::vector<std::pair<uint32_t, uint32_t>>> ctriIndex)
{
    // auto begin = std::chrono::steady_clock::now();

    uint32_t maxc = *max_element(csup.cbegin(), csup.cend());

    // printf("done.\n");

    if(maxc){
        const uint32_t maxc = *max_element(csup.cbegin(), csup.cend());
        std::vector<uint32_t> cbin(maxc + 1, 0);
        std::vector<uint32_t> cpos(m);

        //initialize cbin, cord, and cpos
        for(uint32_t eid = 0; eid < m; ++eid){
            // if(!qualify[eid]) continue;
            ++cbin[csup[eid]];
        }
        for(uint32_t i = 0, start = 0; i <= maxc; ++i){
            start += cbin[i];
            cbin[i] = start - cbin[i];
        }
        for(uint32_t eid = 0; eid < m; ++eid){
            // if(!qualify[eid]) continue;
            cpos[eid] = cbin[csup[eid]];
            cord[cpos[eid]] = eid;
            ++cbin[csup[eid]];
        }
        std::rotate(cbin.rbegin(), cbin.rbegin() + 1, cbin.rend());
        cbin[0] = 0;

        //peeling
        std::vector<bool> cremove(m, false);
        uint32_t c = 0;

        for(uint32_t i = 0; i < m; ++i){
            const uint32_t eid = cord[i];
            // if(!qualify[eid]) continue;
            c = std::max(c, csup[eid]);
            ++cbin[csup[eid]];
            cremove[eid] = true;

            //find cycle and flow triangles containing eid
            const auto& ctris = ctriIndex[eid];
            const auto& ftris = ftriIndex[eid];

            //update csuport
            for(const auto tri : ctris){
                const uint32_t e1 = tri.first;
                const uint32_t e2 = tri.second;
                // if(!qualify[e1] || !qualify[e2]) continue;
                if(csup[e1] >= c && csup[e2] >= c) ++cts[eid];
                if(cremove[e1] || cremove[e2]) continue;
                // ++crem[eid];

                for(const uint32_t e : {e1, e2}){
                    if(csup[e] > c){
                    const uint32_t pe3 = cbin[csup[e]];
                    const uint32_t pe = cpos[e];
                    if(pe3 != pe){
                        const uint32_t e3 = cord[pe3];
                        cord[pe] = e3;
                        cpos[e3] = pe;
                        cord[pe3] = e;
                        cpos[e] = pe3;
                    }
                    ++cbin[csup[e]];
                    --csup[e];
                    }
                }
            }

            // count fct
            for(const auto tri : ftris){
                const uint32_t e1 = tri.first;
                const uint32_t e2 = tri.second;
                if(csup[e1] >= c && csup[e2] >= c) ++fts[eid];
            }

        }
    }

    // auto end = std::chrono::steady_clock::now();

    // printf("cycle decomposition time: %.4f.\n", std::chrono::duration<double>(end - begin).count());

}



//------------advanced approach----------------------//

void Graph::PACT(){
    //initialize vertices
    std::vector<uint32_t> verts(n);
    std::iota(verts.begin(), verts.end(), 0);

    fsup.resize(m, 0); ford.resize(m);
    csup.resize(m, 0);
    fct.resize(m, 0); 
    ftriIndex.resize(m); ctriIndex.resize(m);
    changed.resize(m,false);


    //1. compute all fsup and csup, use neighbor index
    auto begin_index = std::chrono::steady_clock::now();
    bildnbIndex(ftriIndex, ctriIndex, csup, fsup);
    auto end_index = std::chrono::steady_clock::now();
    double runtime_index = std::chrono::duration<double>(end_index - begin_index).count();
    printf("triangle computation time: %.4f sec.\n", runtime_index);

    //2. flow decomposition, obtain fsup with kc = 0
    auto begin_fd = std::chrono::steady_clock::now();
    uint32_t maxf = 0; std::vector<uint32_t> fbin;
    newFlowDecomp(adj_in, adj_out, fsup, ford, fbin, ftriIndex);
    auto end_fd = std::chrono::steady_clock::now();
    double runtime_fd = std::chrono::duration<double>(end_fd - begin_fd).count();
    printf("fd time: %.4f sec.\n", runtime_fd);


    maxf = *max_element(fsup.cbegin(), fsup.cend());

    double num_edges = 0;

    printf("kf max: %d.\n", maxf);

    cord.resize(m);
    D_index.resize(maxf+1);

    //3. cycle decomposition, obtain csup with kf = 0;
    auto begin_cd = std::chrono::steady_clock::now();
    newCycleDecomp(adj_in, adj_out, csup, cord, ctriIndex);
    auto end_cd = std::chrono::steady_clock::now();
    double runtime_cd = std::chrono::duration<double>(end_cd - begin_cd).count();
    printf("cd time: %.4f sec.\n", runtime_cd);

    uint32_t maxc;

    maxc = *max_element(csup.cbegin(), csup.cend());

    printf("kc max: %d.\n", maxc);


    auto begin_ac = std::chrono::steady_clock::now();


    //4. anchored cycle truss computation: compute cycle trussness from 1 to maxf

    //initialize results
    std::vector<bool> quality(m, true);
    D_index[0].resize(m, 0);
    D_index[0] = csup;

    uint32_t iteration = 0;

    //edges to be removed because of fsup < i
    for(uint32_t i = 1; i <= maxf; i++){

        // printf("iter = %d.\n",i);
        //mark edges with fsup < current kf as removed
        uint32_t start = (i == 1)? 0 : fbin[i-2];
        uint32_t end = fbin[i-1];

        for (uint32_t j = start; j < end; j++) {
            uint32_t eid = ford[j];
            // if (quality[eid]) re.push_back(eid);
            quality[eid] = false;
            csup[eid] = 0;
        }

        //mark all edges as changed
        for(uint32_t j = fbin[i-1]; j < fbin[maxf]; j++){
            uint32_t eid = ford[j];
            if(changed[eid]) continue;
            changed[eid] = true;
        }


        //comput cycle truss number corresponding to kf(i)
        bool flag = true;
        while(flag){
            flag = false;
            iteration++;
            for(uint32_t j = fbin[i-1]; j < fbin[maxf]; j++){
                uint32_t eid = ford[j];
                if(changed[eid]){
                    num_edges++;
                    changed[eid] = false;
                    if(refineCsup(eid,i,csup,quality,changed)){
                        flag = true;
                    }
                }
            }
        }
        
        D_index[i].resize(m, 0);
        D_index[i] = csup;

        changed.resize(m,false);

    }

    auto end_ac = std::chrono::steady_clock::now();
    double runtime_ac = std::chrono::duration<double>(end_ac - begin_ac).count();
    printf("ac time: %.4f sec.\n", runtime_ac);

    printf("processed edges: %f.\n", num_edges);


    double runtime = std::chrono::duration<double>(end_ac - begin_index).count();
    // printf("all time: %.4f sec.\n", runtime);


    // results
    std::ofstream outFile("pact.txt", std::ios::app);

    if (!outFile) {
        std::cerr << "Unable to create file!" << std::endl;
        throw std::runtime_error("Unable to create the file pact.txt");
    }

    outFile << "index build: " << runtime_index << std::endl;
    outFile << "flow decomposition: " << runtime_fd << std::endl;
    outFile << "cycle decomposition: " << runtime_cd << std::endl;
    outFile << "anchored cycle trussness: " << runtime_ac << std::endl;
    // outFile << "overall time: " << runtime << std::endl;
    outFile << "iterations: " << iteration << std::endl;
    outFile << "processed edges: " << num_edges << std::endl;

    outFile << std::endl;
    
    outFile.close();

}


void Graph::bildnbIndex(std::vector<std::vector<std::pair<uint32_t, uint32_t>>>& ftriIndex,
                        std::vector<std::vector<std::pair<uint32_t, uint32_t>>>& ctriIndex,
                        std::vector<std::uint32_t>& csup,
                        std::vector<std::uint32_t>& fsup)
{   
    // initialize vertices
    std::vector<std::pair<uint32_t, uint32_t>> cedges;

    for (uint32_t u = 0; u < n; u++) { 
        for (const auto ae : adj_out[u]) {
            const uint32_t v = ae.vid;
            const uint32_t e = ae.eid;

            //flow triangles 
            intersec(adj_out[u],adj_out[v],cedges);

            fsup[e] += cedges.size();

            for(auto edge: cedges){
                fsup[edge.first]++;
                fsup[edge.second]++;

                //index
                ftriIndex[e].push_back({edge.first,edge.second});
                ftriIndex[edge.first].push_back({e, edge.second});
                ftriIndex[edge.second].push_back({e, edge.first});
            }

            //cycle triangles
            if(u < v){
                intercycle(adj_in[u],adj_out[v], u, cedges);
                csup[e] += cedges.size();
                for(auto edge: cedges){
                    csup[edge.first]++;
                    csup[edge.second]++;

                    //index
                    ctriIndex[e].push_back({edge.first,edge.second});
                    ctriIndex[edge.first].push_back({e, edge.second});
                    ctriIndex[edge.second].push_back({e, edge.first});
                }
            }
        }
    }
    
}


void Graph::newFlowDecomp(std::vector<std::vector<ArrayEntry>> adj_in,
    std::vector<std::vector<ArrayEntry>> adj_out,
    std::vector<std::uint32_t>& fsup,
    std::vector<std::uint32_t>& ford,
    std::vector<std::uint32_t>& fbin,
    std::vector<std::vector<std::pair<uint32_t, uint32_t>>> ftriIndex) 
{

    uint32_t maxf = *max_element(fsup.cbegin(), fsup.cend());


    printf("fmax: %d.\n",maxf);

    // auto begin = std::chrono::steady_clock::now();

    //flow decomposition: initialize bin, pos, and flow_order
    fbin.resize(maxf + 1,0);
    std::vector<uint32_t> fpos(m);

    for(uint32_t eid = 0; eid < m; ++eid){
        ++fbin[fsup[eid]];
    }
    for(uint32_t i = 0, start = 0; i <= maxf; ++i){
        start += fbin[i];
        fbin[i] = start - fbin[i];
    }
    for (uint32_t eid = 0; eid < m; ++eid) {
        fpos[eid] = fbin[fsup[eid]];
        ford[fpos[eid]] = eid;
        ++fbin[fsup[eid]];
    }
    std::rotate(fbin.rbegin(), fbin.rbegin() + 1, fbin.rend());
    fbin[0] = 0;

    //decomposition: compute kf
    std::vector<bool> fremoved(m, false);
    uint32_t f = 0;

    for(uint32_t i = 0; i < m; ++i){
        const uint32_t eid = ford[i];
        f = std::max(f, fsup[ford[i]]);

        ++fbin[fsup[eid]];
        fremoved[eid] = true;

        //flow triangles containing edge eid
        std::vector<std::pair<uint32_t, uint32_t>> ftris = ftriIndex[eid];

        //peeling
        for(const auto tri : ftris){
            const uint32_t e1 = tri.first;
            const uint32_t e2 = tri.second;
            // if(fsup[e1] >= f && fsup[e2] >= f) ++fts[eid];
            if(fremoved[e1] || fremoved[e2]) continue;
            // ++frem[eid];

            for(const uint32_t e : {e1, e2}){
                if(fsup[e] > f){
                    const uint32_t pe3 = fbin[fsup[e]];
                    const uint32_t pe = fpos[e];
                    if(pe3 != pe){
                        const uint32_t e3 = ford[pe3];
                        ford[pe] = e3;
                        fpos[e3] = pe;
                        ford[pe3] = e;
                        fpos[e] = pe3;
                    }
                    ++fbin[fsup[e]];
                    --fsup[e];
                }
            }
        }
    }

    // auto end = std::chrono::steady_clock::now();

    // printf("flow decomposition time: %.4f.\n", std::chrono::duration<double>(end - begin).count());

}

void Graph::newCycleDecomp(std::vector<std::vector<ArrayEntry>> adj_in,
                        std::vector<std::vector<ArrayEntry>> adj_out,
                        std::vector<std::uint32_t>& csup,
                        std::vector<std::uint32_t>& cord,
                        std::vector<std::vector<std::pair<uint32_t, uint32_t>>> ctriIndex)
{
    uint32_t maxc = *max_element(csup.cbegin(), csup.cend());

    if(maxc == 0) return;

    // auto begin = std::chrono::steady_clock::now();

    std::vector<uint32_t> cbin(maxc + 1, 0);
    std::vector<uint32_t> cpos(m);


    //initialize cbin, cord, and cpos
    for(uint32_t eid = 0; eid < m; ++eid){
        ++cbin[csup[eid]];
    }
    for(uint32_t i = 0, start = 0; i <= maxc; ++i){
        start += cbin[i];
        cbin[i] = start - cbin[i];
    } 
    for(uint32_t eid = 0; eid < m; ++eid){
        cpos[eid] = cbin[csup[eid]];
        cord[cpos[eid]] = eid;
        ++cbin[csup[eid]];
    }
    std::rotate(cbin.rbegin(), cbin.rbegin() + 1, cbin.rend());
    cbin[0] = 0;


    //peeling
    std::vector<bool> cremove(m, false);
    uint32_t c = 0;

    for(uint32_t i = 0; i < m; ++i){
        const uint32_t eid = cord[i];
        c = std::max(c, csup[eid]);
        ++cbin[csup[eid]];
        cremove[eid] = true;

        //cycle triangles containing eid
        const auto& ctris = ctriIndex[eid];

        //update csuport
        for(const auto tri : ctris){
            const uint32_t e1 = tri.first;
            const uint32_t e2 = tri.second;
            if(cremove[e1] || cremove[e2]) continue;

            for(const uint32_t e : {e1, e2}){
                if(csup[e] > c){
                const uint32_t pe3 = cbin[csup[e]];
                const uint32_t pe = cpos[e];
                if(pe3 != pe){
                    const uint32_t e3 = cord[pe3];
                    cord[pe] = e3;
                    cpos[e3] = pe;
                    cord[pe3] = e;
                    cpos[e] = pe3;
                }
                ++cbin[csup[e]];
                --csup[e];
                }
            }
        }
    }

    // auto end = std::chrono::steady_clock::now();

    // printf("cycle decomposition time: %.4f.\n", std::chrono::duration<double>(end - begin).count());
}


bool Graph::refineCsup(uint32_t eid, uint32_t kf, std::vector<std::uint32_t>& csup, std::vector<bool>& quality, std::vector<bool>& changed){
    
    if(csup[eid] == 0){ // remove unnecessary step
        return false;
    }

    //initial
    uint32_t orginalC = csup[eid];
    auto& ctris = ctriIndex[eid];
    auto& ftris = ftriIndex[eid];
    std::vector <uint32_t> bucket(orginalC + 1);



    //handle cycle neighbor and flow neighbor
    //1. compute h-index for c-neighbor
    uint32_t estimateC = 0, estimateF = 0;
    
    uint32_t idx = 0;  // remove triangles
    for(const auto& tri : ctris){
        const uint32_t e1 = tri.first;
        const uint32_t e2 = tri.second;
        if(!quality[e1] || !quality[e2]) continue;

        ctris[idx++] = tri;

        uint32_t x = std::min(csup[e1], csup[e2]);
        if(x >= orginalC){
            bucket[orginalC]++;
        }else{
            bucket[x]++;
        }
    }
    ctris.resize(idx);

    uint32_t cnt = 0;
    for(int32_t i = orginalC; i >= 0; i--){
        cnt += bucket[i];
        if(cnt >= i){
            estimateC = i;
            break;
        }  
    }

    
    
    //need update csup[eid]
    if(estimateC == 0){
        changed[eid] = false;
        quality[eid] = false;
        //set neighbors as changed
        for(const auto& tri : ctris){
            const uint32_t e1 = tri.first;
            const uint32_t e2 = tri.second;

            if(!quality[e1] || !quality[e2]) continue;

            changed[e1] = true;
            changed[e2] = true;
                
        }
        for(const auto& tri : ftris){
            const uint32_t e1 = tri.first;
            const uint32_t e2 = tri.second;

            if(!quality[e1] || !quality[e2]) continue;

            changed[e1] = true;
            changed[e2] = true;
        }

        csup[eid] = 0;

        return true;
    }



    //2. compute estimate csup for f-neighbor
    std::fill(bucket.begin(), bucket.end(), 0);
    // std::vector <uint32_t> neighs(orginalC + 1);
    uint32_t geq = 0;
    idx = 0;
    for(const auto& tri : ftris){
        const uint32_t e1 = tri.first;
        const uint32_t e2 = tri.second;
        if(!quality[e1] || !quality[e2]) continue;

        //remove triangles
        ftris[idx++] = tri;

        uint32_t x = std::min(csup[e1], csup[e2]);
        if(x >= orginalC){
            geq++;
            bucket[orginalC]++;
        }else{
            bucket[x]++;
        }
    }
    ftris.resize(idx);

    if(geq == kf){
        estimateF = orginalC;
        fct[eid] = geq;
    }
    else{
        cnt = 0;
        for(int32_t i = orginalC; i >= 0; i--){
            cnt += bucket[i];
            if(cnt >= kf) {
                estimateF = i;
                fct[eid] = cnt;
                break;
            }
        }

        if(estimateF == 0){
            changed[eid] = false;
            quality[eid] = false;
            //set neighbors as changed
            for(const auto& tri : ctris){
                const uint32_t e1 = tri.first;
                const uint32_t e2 = tri.second; 

                changed[e1] = true;
                changed[e2] = true;
                
            }
            for(const auto& tri : ftris){
                const uint32_t e1 = tri.first;
                const uint32_t e2 = tri.second;

                changed[e1] = true;
                changed[e2] = true;
            }

            csup[eid] = 0;
            return true;
        }
    }


    //3. if update, set changed for neighbors of eid
    uint32_t updated = std::min(estimateC, estimateF);

    if(updated < orginalC){
        changed[eid] = true;
        csup[eid] = updated;

        //set cycle neighbors
        for(const auto& tri : ctris){
            const uint32_t e1 = tri.first;
            const uint32_t e2 = tri.second;

            changed[e1] = true;
            changed[e2] = true;
        }

        //set flow neighbors
        for(const auto& tri : ftris){
            const uint32_t e1 = tri.first;
            const uint32_t e2 = tri.second;

            changed[e1] = true;
            changed[e2] = true;
        }

        return true;
    }

    return false;
}


bool Graph::refineCsupO(uint32_t eid, uint32_t kf, std::vector<std::uint32_t>& csup, std::vector<bool>& quality, std::vector<bool>& changed){    
    if(csup[eid] == 0){ // remove unnecessary step
        return false;
    }

    //initial
    uint32_t orginalC = csup[eid];
    auto& ctris = ctriIndex[eid];
    auto& ftris = ftriIndex[eid];
    std::vector <uint32_t> bucket(orginalC + 1);



    //handle cycle neighbor and flow neighbor
    //1. compute h-index for c-neighbor
    uint32_t estimateC = 0, estimateF = 0;
    
    uint32_t idx = 0;  // remove triangles
    for(const auto& tri : ctris){
        const uint32_t e1 = tri.first;
        const uint32_t e2 = tri.second;
        if(!quality[e1] || !quality[e2]) continue;

        ctris[idx++] = tri;

        uint32_t x = std::min(csup[e1], csup[e2]);
        if(x >= orginalC){
            bucket[orginalC]++;
        }else{
            bucket[x]++;
        }
    }
    ctris.resize(idx);

    uint32_t cnt = 0;
    for(int32_t i = orginalC; i >= 0; i--){
        cnt += bucket[i];
        if(cnt >= i){
            estimateC = i;
            cts[eid] = cnt;
            break;
        }  
    }
    
    
    //need update csup[eid]
    if(estimateC == 0){
        changed[eid] = false;
        quality[eid] = false;
        //set neighbors as changed
        for(const auto& tri : ctris){
            const uint32_t e1 = tri.first;
            const uint32_t e2 = tri.second;

            if(!quality[e1] || !quality[e2]) continue;

            const uint32_t minCsup = std::min({orginalC, csup[e1], csup[e2]});

            if (minCsup >= csup[e1] && csup[e1] > 0 && --cts[e1] < csup[e1]) changed[e1] = true;
            if (minCsup >= csup[e2] && csup[e2] > 0 && --cts[e2] < csup[e2]) changed[e2] = true;
                
        }
        for(const auto& tri : ftris){
            const uint32_t e1 = tri.first;
            const uint32_t e2 = tri.second;

            if(!quality[e1] || !quality[e2]) continue;

            const uint32_t minCsup = std::min({orginalC, csup[e1], csup[e2]});
        
            if (minCsup >= csup[e1] && csup[e1] > 0 && --fts[e1] < kf) changed[e1] = true;
            if (minCsup >= csup[e2] && csup[e2] > 0 && --fts[e2] < kf) changed[e2] = true;

        }

        csup[eid] = 0;

        return true;
    }



    //2. compute estimate csup for f-neighbor
    std::fill(bucket.begin(), bucket.end(), 0);
    // std::vector <uint32_t> neighs(orginalC + 1);
    uint32_t geq = 0;
    idx = 0;
    for(const auto& tri : ftris){
        const uint32_t e1 = tri.first;
        const uint32_t e2 = tri.second;
        if(!quality[e1] || !quality[e2]) continue;

        //remove triangles
        ftris[idx++] = tri;

        uint32_t x = std::min(csup[e1], csup[e2]);
        if(x >= orginalC){
            geq++;
            bucket[orginalC]++;
        }else{
            bucket[x]++;
        }
    }
    ftris.resize(idx);

    if(geq == kf){
        estimateF = orginalC;
        fts[eid] = geq;
    }
    else{
        cnt = 0;
        for(int32_t i = orginalC; i >= 0; i--){
            cnt += bucket[i];
            if(cnt >= kf) {
                estimateF = i;
                fts[eid] = cnt;
                break;
            }
        }

        if(estimateF == 0){
            changed[eid] = false;
            quality[eid] = false;
            //set neighbors as changed
            for(const auto& tri : ctris){
                const uint32_t e1 = tri.first;
                const uint32_t e2 = tri.second; 

                const uint32_t minCsup = std::min({orginalC, csup[e1], csup[e2]});

                if (minCsup >= csup[e1] && csup[e1] > 0 && --cts[e1] < csup[e1]) changed[e1] = true;
                if (minCsup >= csup[e2] && csup[e2] > 0 && --cts[e2] < csup[e2]) changed[e2] = true;
                
            }
            for(const auto& tri : ftris){
                const uint32_t e1 = tri.first;
                const uint32_t e2 = tri.second;

                const uint32_t minCsup = std::min({orginalC, csup[e1], csup[e2]});
        
                if (minCsup >= csup[e1] && csup[e1] > 0 && --fts[e1] < kf) changed[e1] = true;
                if (minCsup >= csup[e2] && csup[e2] > 0 && --fts[e2] < kf) changed[e2] = true;
            }

            csup[eid] = 0;
            return true;
        }
    }


    //3. if update, set changed for neighbors of eid satisfy order condition
    uint32_t updated = std::min(estimateC, estimateF);


    if(updated < orginalC){
        changed[eid] = true;
        csup[eid] = updated;

        //set cycle neighbors
        for(const auto& tri : ctris){
            const uint32_t e1 = tri.first;
            const uint32_t e2 = tri.second;

            const uint32_t minCsup = std::min({orginalC, csup[e1], csup[e2]});

            if (minCsup >= csup[e1] && csup[e1] > updated && --cts[e1] < csup[e1]) changed[e1] = true;
            if (minCsup >= csup[e2] && csup[e2] > updated && --cts[e2] < csup[e2]) changed[e2] = true;


        }

        //set flow neighbors
        for(const auto& tri : ftris){
            const uint32_t e1 = tri.first;
            const uint32_t e2 = tri.second;

            const uint32_t minCsup = std::min({orginalC, csup[e1], csup[e2]});
        
            if (minCsup >= csup[e1] && csup[e1] > updated && --fts[e1] < kf) changed[e1] = true;
            if (minCsup >= csup[e2] && csup[e2] > updated && --fts[e2] < kf) changed[e2] = true;
            
        }

        return true;
    }

    return false;
}






//test for the given kc and kf
void Graph::Peeling(uint32_t iK_c, uint32_t iK_f)
{
    std::vector<uint32_t> verts(n);
    std::iota(verts.begin(), verts.end(), 0);
    // peeling for D-truss
    // 1. compute the support of each edge by triangle listing
    std::vector<uint32_t> fsup(m,0);
    std::vector<uint32_t> csup(m,0);

    for(const uint32_t u : verts){
        for(const auto ve : adj_out[u]){
            const uint32_t v = ve.vid;
            const uint32_t e = ve.eid;
            std::vector<uint32_t> W;
            W = intersection(adj_in[u], adj_out[v]);
            csup[e] += W.size();
        }
    }

    for (uint32_t eid = 0; eid < m; ++eid) {
        const uint32_t v1 = edges[eid].first;
        const uint32_t v2 = edges[eid].second;
        std::vector<std::pair<uint32_t, uint32_t>> ftris;
        ftris = intersecedge(adj_out[v1], adj_in[v2]); 
        
        for (const auto tri : ftris)
        {
            const uint32_t e1 = tri.first;
            const uint32_t e2 = tri.second;
            ++fsup[eid];++fsup[e1];++fsup[e2];
        }
    }


    // ///////////////peeling/////////////////////
    std::vector<bool> removed(m, false);
    std::vector<bool> vWaitRmFlag(m, false);
    uint32_t uiRmCnt = 0;
    std::vector<uint32_t> vWait(m);
    std::vector<uint32_t> vWaitRmCache(m);

    // init cache
    vWaitRmCache.clear();
    for (uint32_t eid = 0; eid < m; ++eid)
    {
      /*printf("(%d, %d) c_sup: %d f_sup: %d bool: %d %d\n", edges_[eid].first, edges_[eid].second,
             m_Sup_c[eid], m_Sup_f[eid],
             m_Sup_c[eid] < iK_c, m_Sup_f[eid] < iK_f);*/
      if (csup[eid] < iK_c)
      {
        vWaitRmFlag[eid] = true;
        vWaitRmCache.push_back(eid);
      }
      else if (fsup[eid] < iK_f)
      {
        vWaitRmFlag[eid] = true;
        vWaitRmCache.push_back(eid);
      }
    }
    while (uiRmCnt < m)
    {
        vWait.clear();
        if (vWaitRmCache.empty())
        {
            /* all remained are D-truss */
            break;
        }
        else
        {
            vWait.swap(vWaitRmCache);
        }

        for (auto eid : vWait)
        {
            removed[eid] = true;
            ++uiRmCnt;
            //printf("start find triangles\n");

            //printf("start find triangles\n");
            std::vector<std::pair<uint32_t, uint32_t>> ctris;
            const uint32_t v1 = edges[eid].first;
            const uint32_t v2 = edges[eid].second;
            
            std::vector<std::pair<uint32_t, uint32_t>> ftris, ftristmp1, ftristmp2; {
                const uint32_t v1 = edges[eid].first;
                const uint32_t v2 = edges[eid].second;
                ftris = intersecedge(adj_out[v1], adj_in[v2]); 
                ftristmp1 = intersecedge(adj_in[v1], adj_in[v2]); 
                ftristmp2 = intersecedge(adj_out[v1], adj_out[v2]); 
                for(auto e1: ftristmp1){
                    ftris.push_back(e1);
                }
                for(auto e1: ftristmp2){
                    ftris.push_back(e1);
                }
                ftris.erase(std::unique(ftris.begin(), ftris.end()), ftris.end());
            }


            for (const auto tri : ftris){
                const uint32_t e1 = tri.first;
                const uint32_t e2 = tri.second;
                if (removed[e1] || removed[e2]) continue;
                if (!vWaitRmFlag[e1])
                {
                    --fsup[e1];
                    if (fsup[e1] < iK_f)
                  {
                    vWaitRmFlag[e1] = true;
                    vWaitRmCache.push_back(e1);
                  }
                }
                if (!vWaitRmFlag[e2])
                {
                    --fsup[e2];
                    if (fsup[e2] < iK_f)
                  {
                    vWaitRmFlag[e2] = true;
                    vWaitRmCache.push_back(e2);
                  }
                }
            }
            // find triangles containing the edge with ID eid, cycle
            ctris = intersecedge(adj_in[v1], adj_out[v2]); 
            for (const auto tri : ctris)
            {
                const uint32_t e1 = tri.first;
                const uint32_t e2 = tri.second;
                if (removed[e1] || removed[e2]) continue;
                if (!vWaitRmFlag[e1])
                {
                    --csup[e1];
                    if (csup[e1] < iK_c)
                  {
                    vWaitRmFlag[e1] = true;
                    vWaitRmCache.push_back(e1);
                  }
                }
                if (!vWaitRmFlag[e2])
                {
                    --csup[e2];
                    if (csup[e2] < iK_c)
                  {
                    vWaitRmFlag[e2] = true;
                    vWaitRmCache.push_back(e2);
                  }
                }
            }
        }

    }


    // keep result
    std::unordered_set<uint32_t> res;
    for (uint32_t eid = 0; eid < m; ++eid)
    {
        if (!removed[eid])
        {
            auto v1 = edges[eid].first;
            auto v2 = edges[eid].second;
            res.insert(v1);
            res.insert(v2);

        }
    }
    // sort(res.begin(), res.end());

    std::ofstream outFile("dtruss.txt");
    
    if (!outFile) {
        std::cerr << "Unable to create file!" << std::endl;
    }
    
    for (const auto& vertex :res) {
        outFile << vertex << " ";
    }

    outFile.close();

}



void Graph::PACO(){
    //initialize vertices
    std::vector<uint32_t> verts(n);
    std::iota(verts.begin(), verts.end(), 0);

    fsup.resize(m, 0); ford.resize(m);
    csup.resize(m, 0);
    ftriIndex.resize(m); ctriIndex.resize(m);

    //auxiliary structure
    cts.resize(m,0); fts.resize(m,0);

    quality.resize(m,true);

    uint32_t iteration = 0;


    //1. compute all fsup and csup, use neighbor index
    auto begin_index = std::chrono::steady_clock::now();
    bildnbIndex(ftriIndex, ctriIndex, csup, fsup);
    auto end_index = std::chrono::steady_clock::now();
    double runtime_index = std::chrono::duration<double>(end_index - begin_index).count();
    printf("index time: %.4f sec.\n", runtime_index);

    //2. flow decomposition, obtain fsup with kc = 0
    auto begin_fd = std::chrono::steady_clock::now();
    uint32_t maxf = 0; std::vector<uint32_t> fbin;
    newFlowDecomp(adj_in, adj_out, fsup, ford, fbin, ftriIndex);
    auto end_fd = std::chrono::steady_clock::now();
    double runtime_fd = std::chrono::duration<double>(end_fd - begin_fd).count();
    printf("fd time: %.4f sec.\n", runtime_fd);


    maxf = *max_element(fsup.cbegin(), fsup.cend());

    double num_edges = 0;

    printf("kf max: %d.\n", maxf);

    cord.resize(m);
    D_index.resize(maxf+1);

    //3. cycle decomposition, obtain csup with kf = 0;
    auto begin_cd = std::chrono::steady_clock::now();
    recordCycleDecomp(adj_in, adj_out, csup, cts, fts, cord, ctriIndex);
    auto end_cd = std::chrono::steady_clock::now();
    double runtime_cd = std::chrono::duration<double>(end_cd - begin_cd).count();
    printf("cd time: %.4f sec.\n", runtime_cd);

    uint32_t maxc;

    maxc = *max_element(csup.cbegin(), csup.cend());

    printf("kc max: %d.\n", maxc);


    auto begin_ac = std::chrono::steady_clock::now();

    //4. anchored cycle truss computation
    //compute cycle trussness from 1 to maxf

    //initialize results
    D_index[0].resize(m, 0);
    D_index[0] = csup;

    changed.resize(m,false);

    //edges to be removed because of fsup < i
    std::vector<uint32_t> re;
    uint32_t remian_edges = m;
    for(uint32_t i = 1; i <= maxf; i++){

        // printf("iter = %d.\n",i);
        //mark edges with fsup < current kf as removed
        uint32_t start = (i == 1)? 0 : fbin[i-2];
        uint32_t end = fbin[i-1];

        for (uint32_t j = start; j < end; j++) {
            uint32_t eid = ford[j];
            quality[eid] = false;

            //mark cycle neighbors
            for(const auto& [e1, e2] : ctriIndex[eid]){
                if(!quality[e1] || !quality[e2]) continue;
                const uint32_t minCsup = std::min({csup[eid], csup[e1], csup[e2]});

                if (minCsup >= csup[e1] && --cts[e1] < csup[e1]) changed[e1] = true;
                if (minCsup >= csup[e2] && --cts[e2] < csup[e2]) changed[e2] = true;
            }

            //mark flow neighbors
            for(const auto& [e1, e2] : ftriIndex[eid]){
                if(!quality[e1] || !quality[e2]) continue;
                const uint32_t minCsup = std::min({csup[eid], csup[e1], csup[e2]});
        
                if (quality[e1] && minCsup >= csup[e1] && --fts[e1] < i) changed[e1] = true;
                if (quality[e2] && minCsup >= csup[e2] && --fts[e2] < i) changed[e2] = true;
            }

            csup[eid] = 0;
        }

        remian_edges -= (end - start); // 直接计算减少的边数
        // printf("kf = %d, number of edges: %d.\n", i, remian_edges);

        //mark edges with fts < kf as changed
        for(uint32_t j = fbin[i-1]; j < fbin[maxf]; j++){
            uint32_t eid = ford[j];
            if(changed[eid]) continue;
            if(fts[eid] < i) changed[eid] = true;
        }

        //comput cycle truss number corresponding to kf(i)
        bool flag = true;
        while(flag){
            iteration++;
            flag = false;
            for(uint32_t j = fbin[i-1]; j < fbin[maxf]; j++){
                uint32_t eid = ford[j];
                //refine cycle trusssness number of eid
                if(changed[eid]){
                    num_edges++;
                    changed[eid] = false;
                    if(refineCsupO(eid,i,csup,quality,changed)){
                        flag = true;
                    }
                }
            }
        }

        
        D_index[i].resize(m, 0);
        D_index[i] = csup;
        changed.resize(m,false);

    }

    auto end_ac = std::chrono::steady_clock::now();
    double runtime_ac = std::chrono::duration<double>(end_ac - begin_ac).count();
    printf("ac time: %.4f sec.\n", runtime_ac);

    printf("processed edges: %f.\n", num_edges);


    double runtime = std::chrono::duration<double>(end_ac - begin_index).count();
    // printf("all time: %.4f sec.\n", runtime);

    std::ofstream outFile("paco.txt", std::ios::app);

    if (!outFile) {
        std::cerr << "Unable to create file!" << std::endl;
        throw std::runtime_error("Unable to create file res.txt");
    }

    outFile << "index build: " << runtime_index << std::endl;
    outFile << "flow decomposition: " << runtime_fd << std::endl;
    outFile << "cycle decomposition: " << runtime_cd << std::endl;
    outFile << "anchored cycle trussness: " << runtime_ac << std::endl;
    // outFile << "overall time: " << runtime << std::endl;
    outFile << "iterations: " << iteration << std::endl;
    outFile << "processed edges: " << num_edges << std::endl;

    outFile << std::endl;
    
    outFile.close();

}