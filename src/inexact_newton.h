#ifndef _INEXACT_NEWTON_H
#define _INEXACT_NEWTON_H
#include "optimizer.h"
#include "conjugate_gradient.h"
#include "gradient_calc.h"
#include "hessian_vec_product.h"

class InexactNewton : public Optimizer {
  public:
	virtual void optimize(const OptimizePara& optPara);
	virtual void prepare_optimize(const Problem* data);
	virtual void post_optimize();
	void set_cgPara(const CGPara& para){
		cgPara = para;
	}
	static void unittest();
  private:
	HessianVecProduct *hessianMul;
	CGSolver *cgSolver;
	CGPara cgPara;
};

#endif
