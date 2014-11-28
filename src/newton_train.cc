#include <cstdlib>
#include <iostream>
#include "dataset.h"
#include "optimizer.h"
#include "inexact_newton.h"
#include "gradient_descent.h"
#include "lbfgs.h"
#include "log.h"
#include "config.h"

int main(){
    config();
    CGPara cgPara;
    cgPara.maxIter = 5, cgPara.zeroEps = 1e-12, cgPara.errorNormKsi = 1e-12;
    Optimizer *optimizer = new InexactNewton();
    int iters[] = { 3,5,7,9,11};
    int n = 5;
    for(int i = 0; i < n; ++i){
    cgPara.maxIter = iters[i];
    ((InexactNewton*)optimizer)-> set_cgPara(cgPara);
    optimizer->set_parameter(para);
    optimizer->prepare_optimize(&data);
    optimizer->optimize();
    optimizer->post_optimize();
    }
    delete optimizer;
    data.free_memory();
    return 0;
}

