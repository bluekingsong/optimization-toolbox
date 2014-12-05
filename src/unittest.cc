#include <cstdlib>
#include <iostream>
#include "dataset.h"
#include "optimizer.h"
#include "inexact_newton.h"
#include "gradient_descent.h"
#include "lbfgs.h"
#include "log.h"

int unittest();

int main(){
    unittest();
    return 0;
}


int unittest(){
    Problem prob = Problem::unittest();
    uint32_t n = prob.n;
    uint32_t l = prob.l;
    Real *mem = new Real[l];
    GradientCalc *gradientCalc = GradientCalc::unittest(&prob);
    gradientCalc->set_memory(mem);
    LinearSearch::unittest(gradientCalc);
    HessianVecProduct::unittest(gradientCalc);
    HessianVecProduct *hessianMul = new HessianVecProduct(gradientCalc);
    CGSolver::unittest(hessianMul);
    delete hessianMul;
    delete gradientCalc;
    prob.free_memory();
    delete[] mem;
    InexactNewton::unittest();
    return 0;
}
