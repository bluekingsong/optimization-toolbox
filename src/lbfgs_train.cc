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
    Optimizer *optimizer = new LBFGS();
    double bias_vec[] = {-20,-15,-10,-5,-1,0,1,5,10};
    int mems[] = { 10,20,30,40,50,60 } ;
    int n = 6;
    for(int i = 0; i < n; ++i){
    //para.probBias = bias_vec[i];
    para.BFGS_m = mems[i];
    optimizer->set_parameter(para);
    optimizer->prepare_optimize(&data);
    optimizer->optimize();
    optimizer->post_optimize();
    }
    delete optimizer;
    data.free_memory();
    return 0;
}

