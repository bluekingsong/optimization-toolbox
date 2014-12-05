#include "dataset.h"
#include "optimizer.h"

Problem data = read_problem("data/train.lbm");
OptimizePara para;

void config(){
  para.maxIter = 100;
  para.gNormKsi = 1e-12;
  para.intercept = -2;
  para.fitIntercept = false;
  para.l2 = 0.5;
  para.LS_scaleRate = 0.7;
  para.LS_initStep = 1;
  para.LS_c1 = 1e-4;
  para.BFGS_m = 20;
  para.decRatio = 1e-6;
  para.maxTrainSeconds = 220;
  if(para.fitIntercept){
      ++data.n;
  }
}
