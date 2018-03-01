#include <efanna.hpp>
#include <iostream>
#include <fstream>
#include <ctime>
#include <stdlib.h>
#include "mysql/MyDB.h"
#include "../general/matrix.hpp"
#include "../algorithm/kdtreeub_index.hpp"

using namespace efanna;
using namespace std;


void load_data(char* filename, float*& data, size_t& num,int& dim){// load data with sift10K pattern
  ifstream in(filename, ios::binary);
  if(!in.is_open()){cout<<"open file error"<<endl;exit(-1);}
  in.read((char*)&dim,4);
  cout<<"data dimension: "<<dim<<endl;
  in.seekg(0,ios::end);
  ios::pos_type ss = in.tellg();
  size_t fsize = (size_t)ss;
  num = fsize / (dim+1) / 4;
  int cols = (dim + 7)/8*8;
  data = (float*)malloc(num * cols * sizeof(float));
if(dim!=cols)cout<<"data align to dimension "<<cols<<" for avx2 inst"<<endl;

  in.seekg(0,ios::beg);
  for(size_t i = 0; i < num; i++){
    in.seekg(4,ios::cur);
    in.read((char*)(data+i*cols),dim*4);
  }
  in.close();
}
int main(int argc, char** argv){
  MyDB db;
  db.initDB("120.24.163.35", "lengyue", "123456", "experiment");
  unordered_map<string, string> myInfo;

  if(argc!=12){cout<< argv[0] << " data_file save_graph_file trees level epoch L K kNN S" <<endl; exit(-1);}
  float* data_load = NULL;
  //float* query_load = NULL;
  size_t points_num;
  int dim;
  load_data(argv[1], data_load, points_num,dim);
  myInfo["dataset"]=argv[1];
  myInfo["n"]=to_string(points_num);
  myInfo["d"]=to_string(dim);
  //size_t q_num;
  //int qdim;
  //load_data(argv[3], query_load, q_num,qdim);
  Matrix<float> dataset(points_num,dim,data_load);
  //Matrix<float> query(q_num,qdim,query_load);

  unsigned int trees = atoi(argv[4]); myInfo["trees_num"]=argv[4];
  int mlevel = atoi(argv[5]); myInfo["conquer_depth"]=argv[5];
  unsigned int epochs = atoi(argv[6]); myInfo["epochs"]=argv[6];
  int L = atoi(argv[7]); myInfo["L"]=argv[7];
  int checkK = atoi(argv[8]); myInfo["checkK"]=argv[8];
  int kNN = atoi(argv[9]); myInfo["k"]=argv[9];
  int S = atoi(argv[10]); myInfo["S"]=argv[10];

  //srand(time(NULL));
  FIndex<float> index(dataset, new L2Distance<float>(), efanna::KDTreeUbIndexParams(true, trees ,mlevel ,epochs,checkK,L, kNN, trees, S));
  clock_t s,f;
  s = clock();
  index.buildIndex();

  f = clock();
  cout<<"Index building time : "<<(f-s)*1.0/CLOCKS_PER_SEC<<" seconds"<<endl; myInfo["t_build_total"]=((f-s)*1.0/CLOCKS_PER_SEC);
  index.saveGraph(argv[2]);
  index.saveTrees(argv[3]);

  //if(argv[11]=="y")
  //db.addRecord("KNNG",myInfo);
  for(auto key : myInfo){
    cout<<key.first<<": "<<key.second;
  }

  return 0;
}
