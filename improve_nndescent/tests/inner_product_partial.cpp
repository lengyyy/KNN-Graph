//
// Created by markz on 2018-03-16.
//

#include <efanna2e/index_graph.h>
#include <efanna2e/index_random.h>
#include <efanna2e/util.h>
#include <efanna2e/index_kdtree.h>
#ifdef linux
#include<gperftools/profiler.h>
#endif

//purn_level
// 1. 只把第一项提出来准确算的ubber bound, 剪枝效果不到5%。  "4_p"
// 2. 同上，但加入了单调性，只改了树部分的代码。时间慢了很多，失去了对称性.  "4_purn"
// 3. 除了提出第一项，还提出了left项（16的倍数）准确算的upper bound "5"

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

    if (argc != 12) {
        std::cout << argv[0] << " data_file graph_truth nTress mLevel iter L S R K purn_level division" << std::endl;
        exit(-1);
    }
    float *data_load = NULL;
    unsigned points_num, dim;
    load_data(argv[1], data_load, points_num, dim);


    char profname[30];
    sprintf(profname,"%s_%s","ip.prof",argv[10]);
#ifdef linux
    ProfilerStart(profname);
#endif
    auto s_init = std::chrono::high_resolution_clock::now();

//    char* graph_filename = argv[3];
    unsigned nTrees = (unsigned) atoi(argv[3]);
    unsigned mLevel = (unsigned) atoi(argv[4]);
    unsigned iter = (unsigned) atoi(argv[5]);
    char *graph_truth_file = argv[2];
    unsigned L = (unsigned) atoi(argv[6]);
    unsigned S = (unsigned) atoi(argv[7]);
    unsigned R = (unsigned) atoi(argv[8]);
    unsigned K = (unsigned) atoi(argv[9]);
    unsigned pl = (unsigned) atoi(argv[10]);
    unsigned div = (unsigned) atoi(argv[11]);//如果div=0，就是情况purn level =1

    efanna2e::Parameters paras;
    paras.Set<unsigned>("K", K);
    paras.Set<unsigned>("L", L);
    paras.Set<unsigned>("iter", iter);
    paras.Set<unsigned>("S", S);
    paras.Set<unsigned>("R", R);
    paras.Set<unsigned>("nTrees", nTrees);
    paras.Set<unsigned>("mLevel", mLevel);
    paras.Set<unsigned>("div", div);

    data_load = efanna2e::data_align(data_load, points_num, dim);//one must align the data before build

    std::vector<float> p_square(points_num);
    std::vector<float> p_size(points_num);
    std::vector<float> p_right_size(points_num);
    std::vector<float> p_square2(points_num);
    std::vector<float> p_right_size2(points_num);
//    vector<float>  p_bar(points_num);
//    vector<float>  q_bar(points_num);
    efanna2e::DistanceFastL2* distance_norm = new efanna2e::DistanceFastL2();

    if (pl == 3){
        for (size_t i = 0; i < points_num; i++) {
            float right = distance_norm->norm2(data_load+i*dim+div,  dim-div);
            p_right_size[i]=sqrt(right);
            p_square[i] = distance_norm->norm2(data_load+i*dim,  div)+right;
        }
    } else{
        for (size_t i = 0; i < points_num; i++) {
            float ps = distance_norm->norm(data_load+i*dim, dim);
            p_square[i]=ps;
            p_size[i] = sqrt(ps);
            //q_bar[i] = sqrt(1+4*ps);
            //p_bar[i] = sqrt(ps*ps+ps);
        }
    }
//    efanna2e::IndexRandom init_index(dim, points_num);
    efanna2e::IndexKDtree init_index(dim, points_num, efanna2e::INNER_PRODUCT, nullptr);
    init_index.init_times();


    //init_index.Build2(points_num, data_load, paras, p_square,p_bar,q_bar);

    if (pl==1){
        init_index.Build4_p(points_num, data_load, paras, p_square,p_size);
    }else if (pl==2){
        init_index.Build4_purn(points_num, data_load, paras, p_square,p_size);
    }else if (pl==3){
        init_index.Build5(points_num, data_load, paras, p_square,p_right_size);
    }

    init_index.print_times();
    auto e_init = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff_init = e_init - s_init;
    std::cout << "Init time : " << diff_init.count() << "s\n";


    efanna2e::IndexGraph index(dim, points_num, efanna2e::INNER_PRODUCT, (efanna2e::Index *) (&init_index));
    index.init_times();
    index.final_graph_ = init_index.final_graph_; //pass the init graph without Save and Load
//    index.Load(init_graph_filename);
    auto s = std::chrono::high_resolution_clock::now();
    if (pl==1){
        index.RefineGraph4_p(data_load, paras,p_square,p_size);
    }else if (pl==2){
        index.RefineGraph4_p(data_load, paras,p_square,p_size);
        //还没有实现index.RefineGraph4_purn(data_load, paras,p_square,p_size);
    }else if (pl==3){
        index.RefineGraph5(data_load, paras,p_square,p_right_size);
    }

    //index.RefineGraph4_purn(data_load, paras,p_square,p_size);
    index.print_times();
    auto e = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = e - s;
    std::cout << "Refine time: " << diff.count() << "s\n";
//    index.Save(graph_filename);
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
    return 0;
}
