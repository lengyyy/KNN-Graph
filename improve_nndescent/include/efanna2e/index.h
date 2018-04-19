//
// Copyright (c) 2017 ZJULearning. All rights reserved.
//
// This source code is licensed under the MIT license.
//

#ifndef EFANNA2E_INDEX_H
#define EFANNA2E_INDEX_H
#include <map>

#include <cstddef>
#include <string>
#include <vector>
#include <fstream>
#include "distance.h"
#include "parameters.h"
extern long long compare_times;
namespace efanna2e {

    struct id_and_square {
        size_t row_id;
        float square;
        id_and_square(const size_t row_id, const float square): row_id(row_id), square(square) { }

        bool operator >(const id_and_square& rhs) const {
            if (this->square == rhs.square) {
                return this->row_id > rhs.row_id;
            }
            return this->square > rhs.square;
        }
        bool operator <(const id_and_square& rhs) const {
            if (this->square == rhs.square) {
                return this->row_id < rhs.row_id;
            }
            return this->square < rhs.square;
        }
    };

class Index {
 public:
    long long purn_times;
    long long ip_times;
    std::map<unsigned ,unsigned > Euclid_dim;
    std::vector<unsigned > calcul_times;

  explicit Index(const size_t dimension, const size_t n, Metric metric);


  virtual ~Index();

  virtual void Build(size_t n, const float *data, const Parameters &parameters) = 0;

  virtual void Search(
      const float *query,
      const float *x,
      size_t k,
      const Parameters &parameters,
      unsigned *indices) = 0;

  virtual void Save(const char *filename) = 0;

  virtual void Load(const char *filename) = 0;

  inline bool HasBuilt() const { return has_built; }

  inline size_t GetDimension() const { return dimension_; };

  inline size_t GetSizeOfDataset() const { return nd_; }

  inline const float *GetDataset() const { return data_; }


    inline void init_times()  { purn_times=0;ip_times=0;}
    inline void init_calcul_times()  {
        calcul_times.resize(nd_);
    }
    inline void print_times()  {
        printf("purn_times:%lld\nip_times:%lld\n",purn_times,ip_times);
    }

    typedef std::vector<std::vector<unsigned> > CompactGraph;
    CompactGraph final_graph_;

 protected:
  const size_t dimension_;
  const float *data_;
  size_t nd_;
  bool has_built;
  Distance* distance_;

};

}

#endif //EFANNA2E_INDEX_H
