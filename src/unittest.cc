#include <cstdlib>
#include <iostream>
#include "dataset.h"
#include "optimizer.h"
#include "inexact_newton.h"
#include "gradient_descent.h"
#include "log.h"


int main(){
	Problem data = read_problem("train.lbm.smp");
	OptimizePara para(200,1e-12);
	para.probBias = 0;
	para.l2 = 0.1;
	GradientDescent gd;
	gd.prepare_optimize(&data);
	gd.optimize(para);
	gd.post_optimize();
//	InexactNewton newton;
//	newton.prepare_optimize(&data);
/*	CGPara cgPara;
	cgPara.zeroEps = 1e-12, cgPara.maxIter = 10, cgPara.errorNormKsi = 1e-12;
	newton.set_cgPara(cgPara);
	newton.optimize(para);
	newton.post_optimize();
*/

	data.free_memory();
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
