#ifndef _HESSIAN_VEC_PRODUCT_H
#define _HESSIAN_VEC_PRODUCT_H
#include "gradient_calc.h"
#include "mat_vec_product.h"

double InstanceDot(const FeatureNode* instance, const Real* v);
class HessianVecProduct : public MatVecProduct {
  protected:
	const GradientCalc* gradientCalc;
  public:
	explicit HessianVecProduct(const GradientCalc* _gradientCalc):gradientCalc(_gradientCalc){
	};
	virtual void operator()(const Real *x,const uint32_t n,Real *y)const;
	const GradientCalc* get_gradientCalc()const {
		return gradientCalc;
	}
	static void unittest(GradientCalc *gradientCalc);
};
#endif
