# Building KNN Graph for Billion High Dimensional Vectors
## Overview
KNN refers to “K Nearest Neighbors”, which is a basic and popular topic in data mining and machine learning areas. The KNN graph is a graph in which two vertices p and q are connected by an edge, if the distance between p and q is among the K-th smallest distances.[2] Given different similarity measure of these vectors, the pairwise distance can be Hamming distance, Cosine distance, Euclidean distance and so on. We take Euclidean distance as the way to measure similarity between vectors in this paper. The KNN Graph data structure has many advantages in data mining. For example, for a billion-level dataset, prebuilding a KNN graph offline as an index is much better than doing KNN search online many times. 

The study case of this work is as follows: Alibaba has more than one billion products in Taobao platform, and each product contains many features: product name, image, description, tags and so on. For each product, it has already been represented to a float vector with 1024 dimensions by learning algorithm. There are two main challenges on this situation: (1) how to construct a KNN graph quickly, and (2) how to answer a KNN query, which means given a query point, search in the KNN graph index to find its k nearest neighbors as accurate and fast as possible. In this work, we mainly focus on the first challenge, which aims to propose an algorithm to build the KNN graph for the billion-scale products in Taobao efficiently.


## METHODOLOGY
A novel approach named KGraph proposed by Wei Dong et al. at 2011 [3] is basic framework for KNN Graph Construction problem. This algorithm is simple but efficient. We take it as the baseline. After the raised of KGraph, more and more graph-based method based on the idea of KGraph were put forward. To the best of our knowledge, EFANNA[4] is one of the state of the art to build a KNN graph with the high recall. In this section, we will first introduce this KGraph algorithm in detail, and then state the idea of our approach to build a KNN graph efficiently.
### Basic Idea

Our goal is to make some improvements based on the KGraph algorithm in two aspects: First, start the algorithm with an approximate KNN graph instead of random initialize graph. An approximate KNN can be generated by many methods, for example, the kd-tree or LSH algorithm, which takes an acceptable extra time but get much more accurate result of initialization. Good initialization can reduce the times of iteration, which can save time further. Second, we want to optimize the distance computation process in NN-descent iterations. In each iteration, each vector need to check all its neighbors’ neighbor, and compute distance with them, which is O(n 2 d) complexity. In a billion-level dataset, the time cost in distance computation becomes the bottleneck. It is better to find a way using lower cost to achieve the same result. We list some optimization to speed up the distance computation cost in optimization part.
### Algorithm
<img src="https://github.com/lengyyy/KNN-Graph/blob/master/pictures/Algorithm1.png" width="750">
<img src="https://github.com/lengyyy/KNN-Graph/blob/master/pictures/Algorithm2.png" width="750">


### Optimization
- Changeable length code of LSH
- The state flag for vector
- Euclidean distance pruning

## EXPERIMENT
### Dataset
We conduct our experiments in two popular real world data set for nearest neighbor problem. The detailed information are listed below.

<img src="https://github.com/lengyyy/KNN-Graph/blob/master/pictures/dataset.png" width="400">

### Evaluation Metrics
We apply the same evaluation metrics with the 2016 survey paper[11].
For the efficiency, we use the speedup as metrics, speedup of an algorithm is t BF /t, where t BF is the brute force time of searching a query, and t is the average search time of a query by using the algorithm.
For the accuracy, we use the recall, recall = |X ∩ KNN(q)|/k, where X is the set of neighbors given by the search algorithm, and KNN(q) is the true nearest neighbors for query vector.

In addition, we also measure the index construction time and index size for KNN graph construction.

#### Compared with graph-based Method
<img src="https://github.com/lengyyy/KNN-Graph/blob/master/pictures/Experiment1.png" width="600">

#### CLSH compared with learning-based Method
<img src="https://github.com/lengyyy/KNN-Graph/blob/master/pictures/Experiment2.png" width="600">



Please check [report](https://github.com/lengyyy/KNN-Graph/blob/master/report.pdf) for details.
