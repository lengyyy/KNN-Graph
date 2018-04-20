//
// Copyright (c) 2017 ZJULearning. All rights reserved.
//
// This source code is licensed under the MIT license.
//

#ifndef EFANNA2E_GRAPH_H
#define EFANNA2E_GRAPH_H

#include <cstddef>
#include <vector>
#include <mutex>
#include <efanna2e/index.h>
#include <set>

namespace efanna2e {
    extern long long compare_times;

struct Neighbor {
    unsigned id;
    float distance;
    bool flag;

    Neighbor() = default;
    Neighbor(unsigned id, float distance, bool f) : id{id}, distance{distance}, flag(f) {}

    inline bool operator<(const Neighbor &other) const {
        return distance < other.distance;
    }
    inline bool operator>(const Neighbor &other) const {
      return distance > other.distance;
    }
};

    struct id_distance {
        unsigned id;
        float distance;

        id_distance() = default;
        id_distance(unsigned id, float distance) : id{id}, distance{distance}{}

        inline bool operator<(const id_distance &other) const {
          return distance < other.distance;
        }
        inline bool operator>(const id_distance &other) const {
          return distance > other.distance;
        }
    };

    struct id_lowbound {
        unsigned id;
        float lowbound;

        id_lowbound() = default;
        id_lowbound(unsigned id, float lowbound) : id{id}, lowbound{lowbound}{}

        inline bool operator<(const id_lowbound &other) const {
            if (id==other.id) return false;
            return lowbound < other.lowbound;
        }
        inline bool operator>(const id_lowbound &other) const {
            if (id==other.id) return false;
            return lowbound > other.lowbound;
        }
    };


typedef std::lock_guard<std::mutex> LockGuard;
struct nhood{
  std::mutex lock;
  std::vector<Neighbor> pool;
  unsigned M;
    unsigned count_rnew;
    unsigned count_rold;

  std::vector<unsigned> nn_old;
  std::vector<unsigned> nn_new;
  std::vector<unsigned> rnn_old;
  std::vector<unsigned> rnn_new;

    std::vector<id_distance> nn_old2;
    std::vector<id_distance> nn_new2;
    std::vector<id_distance> rnn_old2;
    std::vector<id_distance> rnn_new2;
    std::set<id_lowbound> pool_lb;
  
  nhood(){}
  nhood(unsigned l, unsigned s, std::mt19937 &rng, unsigned N){
    count_rnew=0;
    count_rold=0;
    M = s;
//    nn_new.resize(s * 2);
//    GenRandom(rng, &nn_new[0], (unsigned)nn_new.size(), N);
    nn_new.reserve(s * 2);
    pool.reserve(l);
   //   pool_lb.reserve(6000);
  }

  nhood(const nhood &other){
    count_rnew=other.count_rnew;
    count_rold=other.count_rold;
    M = other.M;
    std::copy(other.nn_new.begin(), other.nn_new.end(), std::back_inserter(nn_new));
    nn_new.reserve(other.nn_new.capacity());
    pool.reserve(other.pool.capacity());
     // pool_lb.reserve(other.pool_lb.capacity());
  }

    void insert (unsigned id, float dist) {
      LockGuard guard(lock);
      for(unsigned i=0; i<pool.size(); i++){
        if(id == pool[i].id)return;
      }
      if(pool.size() < pool.capacity()){
        pool.push_back(Neighbor(id, dist, true));
        std::push_heap(pool.begin(), pool.end());
      }else{
        std::pop_heap(pool.begin(), pool.end());
        pool[pool.size()-1] = Neighbor(id, dist, true);
        std::push_heap(pool.begin(), pool.end());
      }

    }

//    void insert_lb (unsigned id, float lb) {
//        LockGuard guard(lock);
//        for(unsigned i=0; i<pool_lb.size(); i++){
//            if(id == pool_lb[i].id ){
//                if(lb>pool_lb[i].lowbound) pool_lb[i].lowbound=lb;
//                return;
//            }
//        }
//        pool_lb.push_back(id_lowbound(id, lb));
//    }

  void insert3 (unsigned id, float dist) {
    LockGuard guard(lock);

    for(unsigned i=0; i<pool.size(); i++){
      if(id == pool[i].id)return;//如果已经存在这个ID就不加了
    }
    if(pool.size() < pool.capacity()){
      pool.push_back(Neighbor(id, dist, true));
      std::push_heap(pool.begin(), pool.end(),greater<Neighbor>());
    }else{
      std::pop_heap(pool.begin(), pool.end(),greater<Neighbor>());
      pool[pool.size()-1] = Neighbor(id, dist, true);
      std::push_heap(pool.begin(), pool.end(),greater<Neighbor>());
    }

  }

  template <typename C>
  void join (C callback) const {
    for (unsigned const i: nn_new) {
      for (unsigned const j: nn_new) {
        if (i < j) {
          callback(i, j);
          compare_times++;
        }
      }
      for (unsigned j: nn_old) {
        callback(i, j);
        compare_times++;
      }
    }
  }

    template <typename C>
    void join12 (C callback) const {
        for (id_distance const i: nn_new2) {
            for (id_distance const j: nn_new2) {
                if (i.id < j.id) {
                    callback(i, j);
                }
            }
            for (id_distance j: nn_old2) {
                callback(i, j);
            }
        }
    }

};

struct LockNeighbor{
  std::mutex lock;
  std::vector<Neighbor> pool;
};

static inline int InsertIntoPool (Neighbor *addr, unsigned K, Neighbor nn) {
  // find the location to insert
  int left=0,right=K-1;
  if(addr[left].distance>nn.distance){
    memmove((char *)&addr[left+1], &addr[left],K * sizeof(Neighbor));
    addr[left] = nn;
    return left;
  }
  if(addr[right].distance<nn.distance){
    addr[K] = nn;
    return K;
  }
  while(left<right-1){
    int mid=(left+right)/2;
    if(addr[mid].distance>nn.distance)right=mid;
    else left=mid;
  }
  //check equal ID

  while (left > 0){
    if (addr[left].distance < nn.distance) break;
    if (addr[left].id == nn.id) return K + 1;
    left--;
  }
  if(addr[left].id == nn.id||addr[right].id==nn.id)return K+1;
  memmove((char *)&addr[right+1], &addr[right],(K-right) * sizeof(Neighbor));
  addr[right]=nn;
  return right;
}

}

#endif //EFANNA2E_GRAPH_H
