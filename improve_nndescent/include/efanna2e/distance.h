//
// Created by 付聪 on 2017/6/21.
//

#ifndef EFANNA2E_DISTANCE_H
#define EFANNA2E_DISTANCE_H

#include <x86intrin.h>
#include <iostream>
#include <math.h>

namespace efanna2e{
  enum Metric{
    L2 = 0,
    INNER_PRODUCT = 1,
    FAST_L2 = 2,
    PQ = 3
  };
    class Distance {
    public:
        virtual float compare(const float* a, const float* b, unsigned length) const = 0;
        virtual float compare2(const float* a, const float* b, unsigned length) {} ;
        virtual float compare3(const float* a, const float* b, unsigned size,float boundary, unsigned &hasDim,
                               unsigned delta) {} ;
        virtual float compare3_rank(const float* a, const float* b, unsigned size,float boundary, unsigned &hasDim,
                               unsigned delta,std::vector<unsigned > &rank) {} ;
        virtual ~Distance() {}
    };

    class DistanceL2 : public Distance{
    public:
        float compare(const float* a, const float* b, unsigned size) const {
            float result = 0;

#ifdef __GNUC__
#ifdef __AVX__

  #define AVX_L2SQR(addr1, addr2, dest, tmp1, tmp2) \
      tmp1 = _mm256_loadu_ps(addr1);\
      tmp2 = _mm256_loadu_ps(addr2);\
      tmp1 = _mm256_sub_ps(tmp1, tmp2); \
      tmp1 = _mm256_mul_ps(tmp1, tmp1); \
      dest = _mm256_add_ps(dest, tmp1);

      __m256 sum;
      __m256 l0, l1;
      __m256 r0, r1;
      unsigned D = (size + 7) & ~7U;
      unsigned DR = D % 16;
      unsigned DD = D - DR;
      const float *l = a;
      const float *r = b;
      const float *e_l = l + DD;
      const float *e_r = r + DD;
      float unpack[8] __attribute__ ((aligned (32))) = {0, 0, 0, 0, 0, 0, 0, 0};

      sum = _mm256_loadu_ps(unpack);
      if(DR){AVX_L2SQR(e_l, e_r, sum, l0, r0);}

      for (unsigned i = 0; i < DD; i += 16, l += 16, r += 16) {
      	AVX_L2SQR(l, r, sum, l0, r0);
      	AVX_L2SQR(l + 8, r + 8, sum, l1, r1);
      }
      _mm256_storeu_ps(unpack, sum);
      result = unpack[0] + unpack[1] + unpack[2] + unpack[3] + unpack[4] + unpack[5] + unpack[6] + unpack[7];

#else
#ifdef __SSE2__
  #define SSE_L2SQR(addr1, addr2, dest, tmp1, tmp2) \
          tmp1 = _mm_load_ps(addr1);\
          tmp2 = _mm_load_ps(addr2);\
          tmp1 = _mm_sub_ps(tmp1, tmp2); \
          tmp1 = _mm_mul_ps(tmp1, tmp1); \
          dest = _mm_add_ps(dest, tmp1);

  __m128 sum;
  __m128 l0, l1, l2, l3;
  __m128 r0, r1, r2, r3;
  unsigned D = (size + 3) & ~3U;
  unsigned DR = D % 16;
  unsigned DD = D - DR;
  const float *l = a;
  const float *r = b;
  const float *e_l = l + DD;
  const float *e_r = r + DD;
  float unpack[4] __attribute__ ((aligned (16))) = {0, 0, 0, 0};

  sum = _mm_load_ps(unpack);
  switch (DR) {
      case 12:
      SSE_L2SQR(e_l+8, e_r+8, sum, l2, r2);
      case 8:
      SSE_L2SQR(e_l+4, e_r+4, sum, l1, r1);
      case 4:
      SSE_L2SQR(e_l, e_r, sum, l0, r0);
    default:
      break;
  }
  for (unsigned i = 0; i < DD; i += 16, l += 16, r += 16) {
      SSE_L2SQR(l, r, sum, l0, r0);
      SSE_L2SQR(l + 4, r + 4, sum, l1, r1);
      SSE_L2SQR(l + 8, r + 8, sum, l2, r2);
      SSE_L2SQR(l + 12, r + 12, sum, l3, r3);
  }
  _mm_storeu_ps(unpack, sum);
  result += unpack[0] + unpack[1] + unpack[2] + unpack[3];

//nomal distance
#else

      float diff0, diff1, diff2, diff3;
      const float* last = a + size;
      const float* unroll_group = last - 3;

      /* Process 4 items with each loop for efficiency. */
      while (a < unroll_group) {
          diff0 = a[0] - b[0];
          diff1 = a[1] - b[1];
          diff2 = a[2] - b[2];
          diff3 = a[3] - b[3];
          result += diff0 * diff0 + diff1 * diff1 + diff2 * diff2 + diff3 * diff3;
          a += 4;
          b += 4;
      }
      /* Process last 0-3 pixels.  Not needed for standard vector lengths. */
      while (a < last) {
          diff0 = *a++ - *b++;
          result += diff0 * diff0;
      }
#endif
#endif
#endif

            //return result;
            return sqrt(result);
        }
        float compare3(const float* a, const float* b, unsigned size, float boundary, unsigned &hasDim,
                       unsigned delta) {
            //hasDim是刚好超过boundary的dim,或者如果没有超过boundar就是128+16
            float result = 0;

#ifdef __GNUC__
#ifdef __AVX__

#define AVX_L2SQR(addr1, addr2, dest, tmp1, tmp2) \
      tmp1 = _mm256_loadu_ps(addr1);\
      tmp2 = _mm256_loadu_ps(addr2);\
      tmp1 = _mm256_sub_ps(tmp1, tmp2); \
      tmp1 = _mm256_mul_ps(tmp1, tmp1); \
      dest = _mm256_add_ps(dest, tmp1);

            __m256 sum;
            __m256 l0, l1;
            __m256 r0, r1;
            unsigned D = (size + 7) & ~7U;
            unsigned DR = D % 16;
            unsigned DD = D - DR;
            const float *l = a;
            const float *r = b;
            const float *e_l = l + DD;
            const float *e_r = r + DD;
            float unpack[8] __attribute__ ((aligned (32))) = {0, 0, 0, 0, 0, 0, 0, 0};

            sum = _mm256_loadu_ps(unpack);

            hasDim += 16;
            for (unsigned i = 0; i < DD; i += 16, l += 16, r += 16, hasDim+=16) {
                AVX_L2SQR(l, r, sum, l0, r0);
                AVX_L2SQR(l + 8, r + 8, sum, l1, r1);
                if (hasDim%delta==0){
                    _mm256_storeu_ps(unpack, sum);
                    result = unpack[0] + unpack[1] + unpack[2] + unpack[3] + unpack[4] + unpack[5] + unpack[6] + unpack[7];
                    if (result>=boundary) return  result;
                }

            }
            if(DR){AVX_L2SQR(e_l, e_r, sum, l0, r0);}

            _mm256_storeu_ps(unpack, sum);
            result = unpack[0] + unpack[1] + unpack[2] + unpack[3] + unpack[4] + unpack[5] + unpack[6] + unpack[7];

#else
            #ifdef __SSE2__
  #define SSE_L2SQR(addr1, addr2, dest, tmp1, tmp2) \
          tmp1 = _mm_load_ps(addr1);\
          tmp2 = _mm_load_ps(addr2);\
          tmp1 = _mm_sub_ps(tmp1, tmp2); \
          tmp1 = _mm_mul_ps(tmp1, tmp1); \
          dest = _mm_add_ps(dest, tmp1);

  __m128 sum;
  __m128 l0, l1, l2, l3;
  __m128 r0, r1, r2, r3;
  unsigned D = (size + 3) & ~3U;
  unsigned DR = D % 16;
  unsigned DD = D - DR;
  const float *l = a;
  const float *r = b;
  const float *e_l = l + DD;
  const float *e_r = r + DD;
  float unpack[4] __attribute__ ((aligned (16))) = {0, 0, 0, 0};

  sum = _mm_load_ps(unpack);
  switch (DR) {
      case 12:
      SSE_L2SQR(e_l+8, e_r+8, sum, l2, r2);
      case 8:
      SSE_L2SQR(e_l+4, e_r+4, sum, l1, r1);
      case 4:
      SSE_L2SQR(e_l, e_r, sum, l0, r0);
    default:
      break;
  }
  for (unsigned i = 0; i < DD; i += 16, l += 16, r += 16) {
      SSE_L2SQR(l, r, sum, l0, r0);
      SSE_L2SQR(l + 4, r + 4, sum, l1, r1);
      SSE_L2SQR(l + 8, r + 8, sum, l2, r2);
      SSE_L2SQR(l + 12, r + 12, sum, l3, r3);
  }
  _mm_storeu_ps(unpack, sum);
  result += unpack[0] + unpack[1] + unpack[2] + unpack[3];

//nomal distance
#else

      float diff0, diff1, diff2, diff3;
      const float* last = a + size;
      const float* unroll_group = last - 3;

      /* Process 4 items with each loop for efficiency. */
      while (a < unroll_group) {
          diff0 = a[0] - b[0];
          diff1 = a[1] - b[1];
          diff2 = a[2] - b[2];
          diff3 = a[3] - b[3];
          result += diff0 * diff0 + diff1 * diff1 + diff2 * diff2 + diff3 * diff3;
          a += 4;
          b += 4;
      }
      /* Process last 0-3 pixels.  Not needed for standard vector lengths. */
      while (a < last) {
          diff0 = *a++ - *b++;
          result += diff0 * diff0;
      }
#endif
#endif
#endif

            return result;
        }
        float compare3_rank(const float* a, const float* b, unsigned size,float boundary, unsigned &hasDim,
                            unsigned delta,std::vector<unsigned > &rank)  {
            //按照rank中的dim数值遍历
            float result = 0;

#ifdef __GNUC__
#ifdef __AVX__

#define AVX_L2SQR(addr1, addr2, dest, tmp1, tmp2) \
      tmp1 = _mm256_loadu_ps(addr1);\
      tmp2 = _mm256_loadu_ps(addr2);\
      tmp1 = _mm256_sub_ps(tmp1, tmp2); \
      tmp1 = _mm256_mul_ps(tmp1, tmp1); \
      dest = _mm256_add_ps(dest, tmp1);

            __m256 sum;
            __m256 l0, l1;
            __m256 r0, r1;
            unsigned D = (size + 7) & ~7U;//D是8的倍数
           // unsigned DR = D % 16;
            //unsigned DD = D - DR;
            const float *l = a;
            const float *r = b;
            float unpack[8] __attribute__ ((aligned (32))) = {0, 0, 0, 0, 0, 0, 0, 0};

            sum = _mm256_loadu_ps(unpack);

            hasDim += 8;
            for (unsigned i = 0; i < D; i += 8, hasDim+=8) {
                float p[8] __attribute__ ((aligned (32))) = {l[rank[i]],l[rank[i+1]], l[rank[i+2]], l[rank[i+3]], l[rank[i+4]], l[rank[i+5]], l[rank[i+6]], l[rank[i+7]]};
                float q[8] __attribute__ ((aligned (32))) = {r[rank[i]],r[rank[i+1]], r[rank[i+2]], r[rank[i+3]], r[rank[i+4]], r[rank[i+5]], r[rank[i+6]], r[rank[i+7]]};
                AVX_L2SQR(p, q, sum, l0, r0);
                if (hasDim%delta==0){
                    _mm256_storeu_ps(unpack, sum);
                    result = unpack[0] + unpack[1] + unpack[2] + unpack[3] + unpack[4] + unpack[5] + unpack[6] + unpack[7];
                    if (result>=boundary) return  result;
                }

            }

            _mm256_storeu_ps(unpack, sum);
            result = unpack[0] + unpack[1] + unpack[2] + unpack[3] + unpack[4] + unpack[5] + unpack[6] + unpack[7];

#else
            #ifdef __SSE2__
  #define SSE_L2SQR(addr1, addr2, dest, tmp1, tmp2) \
          tmp1 = _mm_load_ps(addr1);\
          tmp2 = _mm_load_ps(addr2);\
          tmp1 = _mm_sub_ps(tmp1, tmp2); \
          tmp1 = _mm_mul_ps(tmp1, tmp1); \
          dest = _mm_add_ps(dest, tmp1);

  __m128 sum;
  __m128 l0, l1, l2, l3;
  __m128 r0, r1, r2, r3;
  unsigned D = (size + 3) & ~3U;
  unsigned DR = D % 16;
  unsigned DD = D - DR;
  const float *l = a;
  const float *r = b;
  const float *e_l = l + DD;
  const float *e_r = r + DD;
  float unpack[4] __attribute__ ((aligned (16))) = {0, 0, 0, 0};

  sum = _mm_load_ps(unpack);
  switch (DR) {
      case 12:
      SSE_L2SQR(e_l+8, e_r+8, sum, l2, r2);
      case 8:
      SSE_L2SQR(e_l+4, e_r+4, sum, l1, r1);
      case 4:
      SSE_L2SQR(e_l, e_r, sum, l0, r0);
    default:
      break;
  }
  for (unsigned i = 0; i < DD; i += 16, l += 16, r += 16) {
      SSE_L2SQR(l, r, sum, l0, r0);
      SSE_L2SQR(l + 4, r + 4, sum, l1, r1);
      SSE_L2SQR(l + 8, r + 8, sum, l2, r2);
      SSE_L2SQR(l + 12, r + 12, sum, l3, r3);
  }
  _mm_storeu_ps(unpack, sum);
  result += unpack[0] + unpack[1] + unpack[2] + unpack[3];

//nomal distance
#else

      float diff0, diff1, diff2, diff3;
      const float* last = a + size;
      const float* unroll_group = last - 3;

      /* Process 4 items with each loop for efficiency. */
      while (a < unroll_group) {
          diff0 = a[0] - b[0];
          diff1 = a[1] - b[1];
          diff2 = a[2] - b[2];
          diff3 = a[3] - b[3];
          result += diff0 * diff0 + diff1 * diff1 + diff2 * diff2 + diff3 * diff3;
          a += 4;
          b += 4;
      }
      /* Process last 0-3 pixels.  Not needed for standard vector lengths. */
      while (a < last) {
          diff0 = *a++ - *b++;
          result += diff0 * diff0;
      }
#endif
#endif
#endif

            return result;
        }
    };

  class DistanceInnerProduct : public Distance{
  public:
    float compare(const float* a, const float* b, unsigned size) const {
      float result = 0;
#ifdef __GNUC__
#ifdef __AVX__
      #define AVX_DOT(addr1, addr2, dest, tmp1, tmp2) \
          tmp1 = _mm256_loadu_ps(addr1);\
          tmp2 = _mm256_loadu_ps(addr2);\
          tmp1 = _mm256_mul_ps(tmp1, tmp2); \
          dest = _mm256_add_ps(dest, tmp1);

	  __m256 sum;
   	  __m256 l0, l1;
   	  __m256 r0, r1;
      unsigned D = (size + 7) & ~7U;
      unsigned DR = D % 16;
      unsigned DD = D - DR;
   	  const float *l = a;
   	  const float *r = b;
      const float *e_l = l + DD;
   	  const float *e_r = r + DD;
      float unpack[8] __attribute__ ((aligned (32))) = {0, 0, 0, 0, 0, 0, 0, 0};

      sum = _mm256_loadu_ps(unpack);
      if(DR){AVX_DOT(e_l, e_r, sum, l0, r0);}

      for (unsigned i = 0; i < DD; i += 16, l += 16, r += 16) {
	    AVX_DOT(l, r, sum, l0, r0);
	    AVX_DOT(l + 8, r + 8, sum, l1, r1);
      }
      _mm256_storeu_ps(unpack, sum);
      result = unpack[0] + unpack[1] + unpack[2] + unpack[3] + unpack[4] + unpack[5] + unpack[6] + unpack[7];

#else
#ifdef __SSE2__
      #define SSE_DOT(addr1, addr2, dest, tmp1, tmp2) \
          tmp1 = _mm128_loadu_ps(addr1);\
          tmp2 = _mm128_loadu_ps(addr2);\
          tmp1 = _mm128_mul_ps(tmp1, tmp2); \
          dest = _mm128_add_ps(dest, tmp1);
      __m128 sum;
      __m128 l0, l1, l2, l3;
      __m128 r0, r1, r2, r3;
      unsigned D = (size + 3) & ~3U;
      unsigned DR = D % 16;
      unsigned DD = D - DR;
      const float *l = a;
      const float *r = b;
      const float *e_l = l + DD;
      const float *e_r = r + DD;
      float unpack[4] __attribute__ ((aligned (16))) = {0, 0, 0, 0};

      sum = _mm_load_ps(unpack);
      switch (DR) {
          case 12:
          SSE_DOT(e_l+8, e_r+8, sum, l2, r2);
          case 8:
          SSE_DOT(e_l+4, e_r+4, sum, l1, r1);
          case 4:
          SSE_DOT(e_l, e_r, sum, l0, r0);
        default:
          break;
      }
      for (unsigned i = 0; i < DD; i += 16, l += 16, r += 16) {
          SSE_DOT(l, r, sum, l0, r0);
          SSE_DOT(l + 4, r + 4, sum, l1, r1);
          SSE_DOT(l + 8, r + 8, sum, l2, r2);
          SSE_DOT(l + 12, r + 12, sum, l3, r3);
      }
      _mm_storeu_ps(unpack, sum);
      result += unpack[0] + unpack[1] + unpack[2] + unpack[3];
#else

      float dot0, dot1, dot2, dot3;
      const float* last = a + size;
      const float* unroll_group = last - 3;

      /* Process 4 items with each loop for efficiency. */
      while (a < unroll_group) {
          dot0 = a[0] * b[0];
          dot1 = a[1] * b[1];
          dot2 = a[2] * b[2];
          dot3 = a[3] * b[3];
          result += dot0 + dot1 + dot2 + dot3;
          a += 4;
          b += 4;
      }
      /* Process last 0-3 pixels.  Not needed for standard vector lengths. */
      while (a < last) {
          result += *a++ * *b++;
      }
#endif
#endif
#endif
      return result;
    }

      float compare2(const float* a, const float* b, unsigned size) {
          if (size == 0)
              return 0;
          float result = 0;
#ifdef __GNUC__
#ifdef __AVX__
#define AVX_DOT(addr1, addr2, dest, tmp1, tmp2) \
          tmp1 = _mm256_loadu_ps(addr1);\
          tmp2 = _mm256_loadu_ps(addr2);\
          tmp1 = _mm256_mul_ps(tmp1, tmp2); \
          dest = _mm256_add_ps(dest, tmp1);

          __m256 sum,sum0;
          __m256 l0, l1;
          __m256 r0, r1;
          unsigned D = (size + 7) & ~7U;
          unsigned DR = D % 16;
          unsigned DD = D - DR;
          const float *l = a;
          const float *r = b;
          const float *e_l = l + DD;
          const float *e_r = r + DD;
          float unpack[8] __attribute__ ((aligned (32))) = {0, 0, 0, 0, 0, 0, 0, 0};

          sum = _mm256_loadu_ps(unpack);

          if(DR){
              unsigned res = 8-D+ size;
              if (res==8){
                  AVX_DOT(e_l, e_r, sum, l0, r0);
              }else{
                  float unpack0[8] __attribute__ ((aligned (32))) = {0, 0, 0, 0, 0, 0, 0, 0};
                  sum0 = _mm256_loadu_ps(unpack0);
                  AVX_DOT(e_l, e_r, sum0, l0, r0);
                  _mm256_storeu_ps(unpack0, sum0);
                  for (int  i=0;i<res;i++){
                      result += unpack0[i];
                  }
              }
          } else{
              unsigned res = 8-D+ size;
              if(res!=8){
                  float unpack0[8] __attribute__ ((aligned (32))) = {0, 0, 0, 0, 0, 0, 0, 0};
                  sum0 = _mm256_loadu_ps(unpack0);
                  AVX_DOT(e_l-16, e_r-16, sum, l0, r0);
                  AVX_DOT(e_l-8, e_r-8, sum0, l0, r0);
                  _mm256_storeu_ps(unpack0, sum0);

                  for (int  i=0;i<res;i++){
                      result += unpack0[i];
                  }
                  DD-=16;
              }
          }

          for (unsigned i = 0; i < DD; i += 16, l += 16, r += 16) {
              AVX_DOT(l, r, sum, l0, r0);
              AVX_DOT(l + 8, r + 8, sum, l1, r1);
          }
          _mm256_storeu_ps(unpack, sum);
          result += unpack[0] + unpack[1] + unpack[2] + unpack[3] + unpack[4] + unpack[5] + unpack[6] + unpack[7];

#else
          #ifdef __SSE2__
      #define SSE_DOT(addr1, addr2, dest, tmp1, tmp2) \
          tmp1 = _mm128_loadu_ps(addr1);\
          tmp2 = _mm128_loadu_ps(addr2);\
          tmp1 = _mm128_mul_ps(tmp1, tmp2); \
          dest = _mm128_add_ps(dest, tmp1);
      __m128 sum;
      __m128 l0, l1, l2, l3;
      __m128 r0, r1, r2, r3;
      unsigned D = (size + 3) & ~3U;
      unsigned DR = D % 16;
      unsigned DD = D - DR;
      const float *l = a;
      const float *r = b;
      const float *e_l = l + DD;
      const float *e_r = r + DD;
      float unpack[4] __attribute__ ((aligned (16))) = {0, 0, 0, 0};

      sum = _mm_load_ps(unpack);
      switch (DR) {
          case 12:
          SSE_DOT(e_l+8, e_r+8, sum, l2, r2);
          case 8:
          SSE_DOT(e_l+4, e_r+4, sum, l1, r1);
          case 4:
          SSE_DOT(e_l, e_r, sum, l0, r0);
        default:
          break;
      }
      for (unsigned i = 0; i < DD; i += 16, l += 16, r += 16) {
          SSE_DOT(l, r, sum, l0, r0);
          SSE_DOT(l + 4, r + 4, sum, l1, r1);
          SSE_DOT(l + 8, r + 8, sum, l2, r2);
          SSE_DOT(l + 12, r + 12, sum, l3, r3);
      }
      _mm_storeu_ps(unpack, sum);
      result += unpack[0] + unpack[1] + unpack[2] + unpack[3];
#else

      float dot0, dot1, dot2, dot3;
      const float* last = a + size;
      const float* unroll_group = last - 3;

      /* Process 4 items with each loop for efficiency. */
      while (a < unroll_group) {
          dot0 = a[0] * b[0];
          dot1 = a[1] * b[1];
          dot2 = a[2] * b[2];
          dot3 = a[3] * b[3];
          result += dot0 + dot1 + dot2 + dot3;
          a += 4;
          b += 4;
      }
      /* Process last 0-3 pixels.  Not needed for standard vector lengths. */
      while (a < last) {
          result += *a++ * *b++;
      }
#endif
#endif
#endif
          return result;
      }



  };
  class DistanceFastL2 : public DistanceInnerProduct{
   public:
      float norm(const float* a, unsigned size) const{
          float result = 0;
#ifdef __GNUC__
#ifdef __AVX__
#define AVX_L2NORM(addr, dest, tmp) \
    tmp = _mm256_loadu_ps(addr); \
    tmp = _mm256_mul_ps(tmp, tmp); \
    dest = _mm256_add_ps(dest, tmp);

          __m256 sum;
          __m256 l0, l1;
          unsigned D = (size + 7) & ~7U;
          unsigned DR = D % 16;
          unsigned DD = D - DR;
          const float *l = a;
          const float *e_l = l + DD;
          float unpack[8] __attribute__ ((aligned (32))) = {0, 0, 0, 0, 0, 0, 0, 0};

          sum = _mm256_loadu_ps(unpack);
          if(DR){AVX_L2NORM(e_l, sum, l0);}
          for (unsigned i = 0; i < DD; i += 16, l += 16) {
              AVX_L2NORM(l, sum, l0);
              AVX_L2NORM(l + 8, sum, l1);
          }
          _mm256_storeu_ps(unpack, sum);
          result = unpack[0] + unpack[1] + unpack[2] + unpack[3] + unpack[4] + unpack[5] + unpack[6] + unpack[7];
#else
          #ifdef __SSE2__
#define SSE_L2NORM(addr, dest, tmp) \
    tmp = _mm128_loadu_ps(addr); \
    tmp = _mm128_mul_ps(tmp, tmp); \
    dest = _mm128_add_ps(dest, tmp);

    __m128 sum;
    __m128 l0, l1, l2, l3;
    unsigned D = (size + 3) & ~3U;
    unsigned DR = D % 16;
    unsigned DD = D - DR;
    const float *l = a;
    const float *e_l = l + DD;
    float unpack[4] __attribute__ ((aligned (16))) = {0, 0, 0, 0};

    sum = _mm_load_ps(unpack);
    switch (DR) {
        case 12:
        SSE_L2NORM(e_l+8, sum, l2);
        case 8:
        SSE_L2NORM(e_l+4, sum, l1);
        case 4:
        SSE_L2NORM(e_l, sum, l0);
      default:
        break;
    }
    for (unsigned i = 0; i < DD; i += 16, l += 16) {
        SSE_L2NORM(l, sum, l0);
        SSE_L2NORM(l + 4, sum, l1);
        SSE_L2NORM(l + 8, sum, l2);
        SSE_L2NORM(l + 12, sum, l3);
    }
    _mm_storeu_ps(unpack, sum);
    result += unpack[0] + unpack[1] + unpack[2] + unpack[3];
#else
    float dot0, dot1, dot2, dot3;
    const float* last = a + size;
    const float* unroll_group = last - 3;

    /* Process 4 items with each loop for efficiency. */
    while (a < unroll_group) {
        dot0 = a[0] * a[0];
        dot1 = a[1] * a[1];
        dot2 = a[2] * a[2];
        dot3 = a[3] * a[3];
        result += dot0 + dot1 + dot2 + dot3;
        a += 4;
    }
    /* Process last 0-3 pixels.  Not needed for standard vector lengths. */
    while (a < last) {
        result += (*a) * (*a);
        a++;
    }
#endif
#endif
#endif
          return result;
      }

    float norm2(const float* a, unsigned size) const{
        if (size == 0)
            return 0;
      float result = 0;
#ifdef __GNUC__
#ifdef __AVX__
#define AVX_L2NORM(addr, dest, tmp) \
    tmp = _mm256_loadu_ps(addr); \
    tmp = _mm256_mul_ps(tmp, tmp); \
    dest = _mm256_add_ps(dest, tmp);

    __m256 sum, sum0;
   	__m256 l0, l1;
    unsigned D = (size + 7) & ~7U;
    unsigned DR = D % 16;
    unsigned DD = D - DR;
    const float *l = a;
    const float *e_l = l + DD;
    float unpack[8] __attribute__ ((aligned (32))) = {0, 0, 0, 0, 0, 0, 0, 0};
    sum = _mm256_loadu_ps(unpack);

    if(DR){
        unsigned res = 8-D+ size;
        if (res==8){
            AVX_L2NORM(e_l, sum, l0);
        }else{
            float unpack0[8] __attribute__ ((aligned (32))) = {0, 0, 0, 0, 0, 0, 0, 0};
            sum0 = _mm256_loadu_ps(unpack0);
            AVX_L2NORM(e_l, sum0, l0);
            _mm256_storeu_ps(unpack0, sum0);
            for (int  i=0;i<res;i++){
                result += unpack0[i];
            }
        }
    } else{
        unsigned res = 8-D+ size;
        if(res!=8){
            float unpack0[8] __attribute__ ((aligned (32))) = {0, 0, 0, 0, 0, 0, 0, 0};
            sum0 = _mm256_loadu_ps(unpack0);
            AVX_L2NORM(e_l-16, sum, l0);
            AVX_L2NORM(e_l-8, sum0, l1);
            _mm256_storeu_ps(unpack0, sum0);

            for (int  i=0;i<res;i++){
                result += unpack0[i];
            }
            DD-=16;
        }
    }

	for (unsigned i = 0; i < DD; i += 16, l += 16) {
      AVX_L2NORM(l, sum, l0);
      AVX_L2NORM(l + 8, sum, l1);
    }
    _mm256_storeu_ps(unpack, sum);
   result += unpack[0] + unpack[1] + unpack[2] + unpack[3] + unpack[4] + unpack[5] + unpack[6] + unpack[7];
#else
#ifdef __SSE2__
#define SSE_L2NORM(addr, dest, tmp) \
    tmp = _mm128_loadu_ps(addr); \
    tmp = _mm128_mul_ps(tmp, tmp); \
    dest = _mm128_add_ps(dest, tmp);

    __m128 sum;
    __m128 l0, l1, l2, l3;
    unsigned D = (size + 3) & ~3U;
    unsigned DR = D % 16;
    unsigned DD = D - DR;
    const float *l = a;
    const float *e_l = l + DD;
    float unpack[4] __attribute__ ((aligned (16))) = {0, 0, 0, 0};

    sum = _mm_load_ps(unpack);
    switch (DR) {
        case 12:
        SSE_L2NORM(e_l+8, sum, l2);
        case 8:
        SSE_L2NORM(e_l+4, sum, l1);
        case 4:
        SSE_L2NORM(e_l, sum, l0);
      default:
        break;
    }
    for (unsigned i = 0; i < DD; i += 16, l += 16) {
        SSE_L2NORM(l, sum, l0);
        SSE_L2NORM(l + 4, sum, l1);
        SSE_L2NORM(l + 8, sum, l2);
        SSE_L2NORM(l + 12, sum, l3);
    }
    _mm_storeu_ps(unpack, sum);
    result += unpack[0] + unpack[1] + unpack[2] + unpack[3];
#else
    float dot0, dot1, dot2, dot3;
    const float* last = a + size;
    const float* unroll_group = last - 3;

    /* Process 4 items with each loop for efficiency. */
    while (a < unroll_group) {
        dot0 = a[0] * a[0];
        dot1 = a[1] * a[1];
        dot2 = a[2] * a[2];
        dot3 = a[3] * a[3];
        result += dot0 + dot1 + dot2 + dot3;
        a += 4;
    }
    /* Process last 0-3 pixels.  Not needed for standard vector lengths. */
    while (a < last) {
        result += (*a) * (*a);
        a++;
    }
#endif
#endif
#endif
      return result;
    }


    using DistanceInnerProduct::compare;
    float compare(const float* a, const float* b, float norm, unsigned size) const {//not implement
      float result = -2 * DistanceInnerProduct::compare(a, b, size);
      result += norm;
      return result;
    }
  };
}



#endif //EFANNA2E_DISTANCE_H
