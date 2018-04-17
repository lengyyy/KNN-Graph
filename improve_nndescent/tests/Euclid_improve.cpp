
#include <cblas.h>
#include <efanna2e/index_graph.h>
#include <efanna2e/index_random.h>
#include <efanna2e/util.h>
#include <efanna2e/index_kdtree.h>
#include <mysql/MyDB.h>
#ifdef linux
#include<gperftools/profiler.h>
#endif


void load_data(char *filename, float *&data, unsigned &num, unsigned &dim) {// load data with sift10K pattern
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open()) {
        std::cout << "open file error" << std::endl;
        exit(-1);
    }
    in.read((char *) &dim, 4);
    std::cout << "data dimension: " << dim << std::endl;
    in.seekg(0, std::ios::end);
    std::ios::pos_type ss = in.tellg();
    size_t fsize = (size_t) ss;
    num = (unsigned) (fsize / (dim + 1) / 4);
    data = new float[num * dim * sizeof(float)];

    in.seekg(0, std::ios::beg);
    for (size_t i = 0; i < num; i++) {
        in.seekg(4, std::ios::cur);
        in.read((char *) (data + i * dim), dim * 4);
    }
    in.close();
}

void load_datai(char *filename, int *&data, unsigned &num, unsigned &dim) {// load graph by int
    ifstream in(filename, ios::binary);
    if (!in.is_open()) {
        cout << "open file error" << endl;
        return;
    }
    in.read((char *) &dim, 4);
    in.seekg(0, ios::end);
    ios::pos_type ss = in.tellg();
    int fsize = (int) ss;
    num = fsize / (dim + 1) / 4;
    data = new int[num * dim];

    in.seekg(0, ios::beg);
    for (int i = 0; i < num; i++) {
        in.seekg(4, ios::cur);
        in.read((char *) (data + i * dim), dim * 4);
    }
    in.close();
}

int main(int argc, char **argv) {
    unordered_map<string,string> myInfo;

    if (argc != 12) {
        std::cout << argv[0] << " data_file graph_truth nTress mLevel iter L S R K improve_level mysql" << std::endl;
        exit(-1);
    }
    float *data_load = NULL;
    unsigned points_num, dim;
    load_data(argv[1], data_load, points_num, dim);


    char profname[30];
    sprintf(profname,"%s_%s","Euclid_improve",argv[10]);
#ifdef linux
    ProfilerStart(profname);
#endif
    auto s_init = std::chrono::high_resolution_clock::now();

    char *graph_truth_file = argv[2];
    unsigned nTrees = (unsigned) atoi(argv[3]);
    unsigned mLevel = (unsigned) atoi(argv[4]);
    unsigned iter = (unsigned) atoi(argv[5]);
    unsigned L = (unsigned) atoi(argv[6]);
    unsigned S = (unsigned) atoi(argv[7]);
    unsigned R = (unsigned) atoi(argv[8]);
    unsigned K = (unsigned) atoi(argv[9]);
    unsigned pl = (unsigned) atoi(argv[10]);

    efanna2e::Parameters paras;
    paras.Set<unsigned>("K", K);
    paras.Set<unsigned>("L", L);
    paras.Set<unsigned>("iter", iter);
    paras.Set<unsigned>("S", S);
    paras.Set<unsigned>("R", R);
    paras.Set<unsigned>("nTrees", nTrees);
    paras.Set<unsigned>("mLevel", mLevel);

    float *projection_data = NULL;
    if (pl==10){
        //JL降维
        auto jl_start = std::chrono::high_resolution_clock::now();
        float e = 0.5;
        unsigned newd = 20*log10(points_num)/(e*e);
        std::cout << "newd: " <<newd<< endl;
        float * projection_matrix = new float[dim * newd];
        std::default_random_engine generator;
        std::normal_distribution<float> distribution(0.0, 1.0);
#pragma omp parallel for
        for (int i = 0; i < newd * dim; i++) {
            projection_matrix[i] = distribution(generator);
        }
        projection_data = new float[points_num * newd* sizeof(float)];
        cblas_sgemm(CblasRowMajor, CblasNoTrans,CblasNoTrans, points_num, newd, dim,
                    1, data_load, dim, projection_matrix, newd, 0, projection_data, newd);
        delete[] data_load;
        delete[] projection_matrix;
        auto jl_end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> jl_time = jl_end - jl_start;
        std::cout << "JL time : " << jl_time.count() << "s\n";

        dim = newd;
        projection_data = efanna2e::data_align(projection_data, points_num, dim);//one must align the data before build
    } else{
        data_load = efanna2e::data_align(data_load, points_num, dim);//one must align the data before build

    }


    efanna2e::IndexKDtree init_index(dim, points_num, efanna2e::L2, nullptr);


    vector<vector<unsigned >> rank;
    if (pl == 0 ){
        init_index.Build(points_num, data_load, paras);

    }else if (pl==1){
        auto sort_start = std::chrono::high_resolution_clock::now();

        rank.reserve(points_num);
        vector<unsigned > myrank;
        myrank.reserve(dim);
        for (int i=0;i<dim;i++){
            myrank.push_back(i);
        }
        for (size_t i = 0; i < points_num; i++) {
            sort(myrank.begin(),myrank.end(),[&](const unsigned index1, const unsigned index2){
                return data_load[i*dim+index1]>data_load[i*dim+index2];
            });
            rank.push_back(myrank);
        }
//        for (size_t i = 0; i < points_num; i++){
//            for (auto j:rank[i])printf("%f ",data_load[i*dim+j]);
//            printf("\n");
//        }
        auto sort_end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> sort_time = sort_end - sort_start;
        std::cout << "Sort time : " << sort_time.count() << "s\n";

        init_index.Build11(points_num, data_load, paras, rank);

    } else if (pl==10){
        init_index.Build(points_num, projection_data, paras);
    }

    auto e_init = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff_init = e_init - s_init;
    std::cout << "Init time : " << diff_init.count() << "s\n";


    efanna2e::IndexGraph index(dim, points_num, efanna2e::L2, (efanna2e::Index *) (&init_index));
    index.final_graph_ = init_index.final_graph_; //pass the init graph without Save and Load
    auto s = std::chrono::high_resolution_clock::now();
    if (pl == 0 ){
        index.RefineGraph(data_load, paras);
    } else if (pl==1){
        index.RefineGraph11(data_load, paras,rank);
    } else if (pl==10){
        index.RefineGraph(projection_data, paras);
    }
    auto e = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = e - s;
    std::cout << "Refine time: " << diff.count() << "s\n";

    map<unsigned ,unsigned >::iterator it;
    for (it=index.Euclid_dim.begin();it!=index.Euclid_dim.end();it++){
        printf("[%u] : %u times\n",it->first,it->second);
    }


#ifdef linux
    ProfilerStop();
#endif

    int *graph_truth = NULL;
    vector<std::vector<unsigned> > &final_result = index.final_graph_;
    load_datai(graph_truth_file, graph_truth, points_num, dim);
    int cnt = 0;
    for (unsigned i = 0; i < points_num; i++) {
        for (unsigned j = 0; j < K; j++) {
            unsigned k = 0;
            for (; k < K; k++) {
                if (graph_truth[i * dim + j] == final_result[i][k]) break;
            }

            if (k == K)cnt++;
        }
    }
    float accuracy = 1 - (float) cnt / (points_num * K);
    cout << K << "NN accuracy: " << accuracy << endl;

    if(atoi(argv[11])!=0){
        MyDB db;
        db.initDB("120.24.163.35", "lengyue", "123456", "experiment");
        myInfo["type"]=profname;
        myInfo["init_time"]=to_string(diff_init.count());
        myInfo["refine_time"]=to_string(diff.count());
        myInfo["accuracy"]=to_string(accuracy);
        time_t date = time(0);
        char tmpBuf[255];
        strftime(tmpBuf, 255, "%Y%m%d%H%M", localtime(&date));
        myInfo["date"]=tmpBuf;
        myInfo["exp_group"]=argv[10];
        db.addRecord("KNNG_purn2",myInfo);
    }
    return 0;
}
