#include <cmath>
#include <iostream>
#include <cstring>
#include "assert.h"
#include "conjugate_gradient.h"
#include "vec_op.h"
#include "log.h"

void CGSolver::solve(Real *x, const CGPara& para,const Real *b, double bFactor){
  assert(para.maxIter != 0);
  std::memset(x,0,sizeof(Real) * n);
  vec_cpy(r,b,n,bFactor);
  vec_cpy(p,b,n,-bFactor);
  //vec_mul(p,n,-1);
  uint32_t k = 0;
  double rNorm = vec_dot(r,r,n);
  //std::cout<<"rNorm="<<rNorm<<std::endl;
  while( k++ < para.maxIter  && rNorm > para.errorNormKsi){
      (*matVecProduct)(p,n,y);
      double t = vec_dot(p,y,n);
      if( std::abs(t) < para.zeroEps ){
    	  Log::warn("CGSolver::solve","break on a 0 step size");
    	  break;
      }
      double alpha = rNorm / t;
      //std::cout<<"alpha="<<alpha<<std::endl;
      vec_add(x, x, p, n, 1, alpha);
      vec_add(r, r, y, n, 1, alpha);
      double rNormNew = vec_dot(r,r,n);
      double beta =  rNormNew / rNorm;
      rNorm = rNormNew;
      vec_add(p, r, p, n, -1.0, beta);
      //std::cout<<"cgsolver, iter="<<k<<" error norm="<<rNorm<<std::endl;
  }
  numIter = k;
};

void CGSolver::unittest(const HessianVecProduct *hessianMul){
	Log::raw("===========CGSolver::unittest===============");
	const GradientCalc *gradientCalc = hessianMul->get_gradientCalc();
	uint32_t n = gradientCalc->get_data()->n;
	CGSolver *cgSolver = new CGSolver(hessianMul,n);
	Real *mem = new Real[6 * n];
	std::memset(mem,0,sizeof(Real) * 5 * n);
	cgSolver->set_memory(mem,mem + n, mem + 2 * n);
	Real *w = mem + 3 * n;
	w[4] = 1;
	Real *g = mem + 4 * n;
	Real *d = mem + 5 * n;
	double f = (*const_cast<GradientCalc*>(gradientCalc))(w,g);
	CGPara para;
	para.maxIter = 10; para.errorNormKsi = 1e-12; para.zeroEps = 1e-12;
	cgSolver->solve(d,para,g,-1);
	double dec = vec_dot(g,d,n);
	assert(dec > 0);
	delete []mem;
	delete cgSolver;
	Log::raw("+++++++++++CGSolver::unittest+++++++++++++++");
}
/*
int main(){
    CGPara para;
    para.zeroEps = 1e-12;  para.maxIter = 100; para.errorNormKsi = 1e-12;
    uint32_t n = 100;
    Real *x = new Real[2 * n + 3 * n];
    Real *b = x + n;
    for(uint32_t i = 0; i < n; ++i){
        x[i] = 0;
        b[i] = 1;
    }
    std::cout<<"create solver."<<std::endl;
    HilbertProduct hilbertProduct;
    CGSolver cgSolver(&hilbertProduct, n);
    cgSolver.set_memory(x + 2 * n, x + 3 * n, x + 4 * n);
    cgSolver.solve(x,para,b);
};
*/
