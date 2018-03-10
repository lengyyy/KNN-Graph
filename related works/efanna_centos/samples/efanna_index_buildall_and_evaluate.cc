#include <efanna.hpp>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include<stdio.h>  
#include<string.h>  
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
  data = (float*)memalign(KGRAPH_MATRIX_ALIGN, num * cols * sizeof(float));
if(dim!=cols)cout<<"data align to dimension "<<cols<<" for avx2 inst"<<endl;

  in.seekg(0,ios::beg);
  for(size_t i = 0; i < num; i++){
    in.seekg(4,ios::cur);
    in.read((char*)(data+i*cols),dim*4);
  }
  in.close();
}

void load_datae(char* filename, int*& data, int& num,int& dim){// load data with sift10K pattern
  ifstream in(filename, ios::binary);
  if(!in.is_open()){cout<<"open file error"<<endl;return;}
  in.read((char*)&dim,4);
  in.seekg(0,ios::end);
  ios::pos_type ss = in.tellg();
  int fsize = (int)ss;
  num = fsize / (dim+1) / 4;
  data = new int[num*dim];

  in.seekg(0,ios::beg);
  for(int i = 0; i < num; i++){
    in.seekg(4,ios::cur);
    in.read((char*)(data+i*dim),dim*4);
  }
  in.close();
}

int main(int argc, char** argv){
  MyDB db;
  db.initDB("120.24.163.35", "lengyue", "123456", "experiment");
  if(argc!=13){cout<< argv[0] << " data_file save_graph_file trees level epoch L K kNN S" <<endl; exit(-1);}
  float* data_load = NULL;
  //float* query_load = NULL;
  size_t points_num;
  int dim;
  load_data(argv[1], data_load, points_num,dim);
  myInfo["dataset"]=argv[1];
  myInfo["n"]=to_string(points_num);
  myInfo["d"]=to_string(dim);
  //size_t q_num;
  //int qdim;cd
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
  FIndex<float> index(dataset, new L2DistanceAVX<float>(), efanna::KDTreeUbIndexParams(true, trees ,mlevel ,epochs,checkK,L, kNN, trees, S));
  clock_t s,f;
  s = clock();
  index.buildIndex();
  
  myInfo["memory_tree"]=to_string(memoryInfo["memory_after_tree"]-memoryInfo["memory_before_tree"]);
  myInfo["memory_graph"]=to_string(memoryInfo["memory_after_graph"]-memoryInfo["memory_before_graph"]);

  f = clock();
  int t_total = (f-s)*1.0/CLOCKS_PER_SEC;
  cout<<"Index building time : "<<t_total<<" seconds"<<endl; myInfo["t_build_total"]=to_string(t_total);
  index.saveGraph(argv[2]);
  index.saveTrees(argv[3]);
  float size_dataset=file_size(argv[1]);
  float size_graph=file_size(argv[2]);
  float size_tree=file_size(argv[3]);
  cout<<"index size graph : "<<size_graph<<" MB"<<endl;
  cout<<"index size tree : "<<size_tree<<" MB"<<endl;
  myInfo["size_graph"]=to_string(size_graph);
  myInfo["size_tree"]=to_string(size_tree);
  myInfo["size_dataset"]=to_string(size_dataset);

  time_t date = time(0);
  char tmpBuf[255];
  strftime(tmpBuf, 255, "%Y%m%d%H%M", localtime(&date));
  cout<<tmpBuf<<endl; myInfo["date"]=tmpBuf;

  std::cout<<"memory_end : "<<getMemory()<<std::endl;

  //evaluate

  int *gt = NULL;
  int *gt_ = NULL;
  int p1,p2;
  int dim1,dim2;
  load_datae(argv[2], gt,  p1,dim1);
  load_datae(argv[12], gt_, p2,dim2);

  if(p1 != p2){
      cout << "p1:" << p1 <<endl;
      cout << "p2:" << p2 <<endl;
      cout<< "result file and groundtruth don't match" <<endl;
      exit(-1);
  }

  if(kNN > dim1){cout<< "result file not enough for k="<<kNN <<endl; exit(-1);}

  set<int> result;
  for(int i=0; i < p1; i++){
    result.clear();
    for(int j=0; j<kNN;j++){
      result.insert(gt[i*dim1+j]);
    }
    //if (result.size()< (unsigned int) kNN)
    // cout<< "query " << i <<" result index not unique!"<< endl;

  }

  int cnt=0;
  for(int i = 0; i < p1; i++){
    for(int j=0; j< kNN; j++){
      int k=0;
      for(; k<kNN; k++){
        if(gt[i*dim1+j]==gt_[i*dim2+k])break;
      }
      if(k==kNN)cnt++;
    }
  }
  float accuracy = 1-(float)cnt/(p1*kNN);
  cout<<kNN <<"NN accuracy: "<<accuracy<<endl; myInfo["accuracy"]=to_string(accuracy);
  if(atoi(argv[11])==1) db.addRecord("KNNG",myInfo);

 //  for(auto key : myInfo){
 //     cout<<key.first<<":"<<key.second<<endl;
 // }
  return 0;
}
