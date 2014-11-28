#include "dataset.h"
#include "optimizer.h"

Problem data = read_problem("data/train.lbm");
OptimizePara para;

void config(){
  para.maxIter = 2000;
  para.gNormKsi = 1e-12;
  para.probBias = -1;
  para.l2 = 1;
  para.LS_scaleRate = 0.7;
  para.LS_initStep = 1;
  para.LS_c1 = 1e-4;
  para.BFGS_m = 10;
  para.decRatio = 1e-6;
}
