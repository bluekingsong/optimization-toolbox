#ifndef _DATASET_H_
#define _DATASET_H_
#include <cmath>
#include <string>
#include <cstdlib>

struct FeatureNode{
    int index; // -1 means end of line, lowest index is 0 no matter what the low index of the data file
    double value;
};

struct Problem{
    int n,l;  // number of features & number of instances
    double *y; // label
    struct FeatureNode **x;
    void free_memory(){
        std::free(y);
        std::free(*x);
    };
    static Problem unittest(const std::string& filename = std::string("train.lbm.smp"));
};

Problem read_problem(const char *filename);

#endif
