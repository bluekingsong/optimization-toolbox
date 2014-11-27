#ifndef _CONJUGATE_GRADIENT_H
#define _CONJUGATE_GRADIENT_H
#include "hessian_vec_product.h"

typedef double Real;
typedef unsigned int uint32_t;

struct CGPara{
   uint32_t maxIter;
   // ||Ax-b||
   double errorNormKsi;
   double zeroEps;
};
class CGSolver{
  public:
	CGSolver(const MatVecProduct *_matVecProduct, uint32_t vec_len):matVecProduct(_matVecProduct),n(vec_len)
  	{  }
    void solve(Real *x, const CGPara& cgPara, const Real *b, double bFactor = 1);
    void set_memory(Real *_r, Real *_p, Real *_y){
    	r = _r;
    	p = _p;
    	y = _y;
    }
    uint32_t get_iter_num()const{
    	return numIter;
    }
    static void unittest(const HessianVecProduct *hessianMul);
  private:
    Real *r;  // residual, r=Ax-b
    Real *p;  // conjugated gradient
    Real *y;  // temp vector of Matrix-Vector product, y=Ap
    uint32_t numIter;
    uint32_t n; // variable vector length
    const MatVecProduct *matVecProduct;
};

#endif
