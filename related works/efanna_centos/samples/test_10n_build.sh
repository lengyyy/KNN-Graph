./efanna_index_buildall sift_base.fvecs sift.graph sift.trees 8 8 0 20 10 10 10 1
./efanna_search sift_base.fvecs sift.trees sift.graph sift_base.fvecs sift_query_allgraph.results 8 4 120 20 10
./evaluate sift_query_allgraph.results sift_graphtruth.ivecs 10