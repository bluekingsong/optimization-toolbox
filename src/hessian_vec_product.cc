#include <cstring>
#include <iostream>
#include <assert.h>
#include "hessian_vec_product.h"
#include "dataset.h"
#include "gradient_calc.h"
#include "log.h"

double InstanceDot(const FeatureNode* instance, const Real* v){
	const FeatureNode *feature = instance;
	double r = 0;
	while( feature->index >= 0 ){
		r += feature->value * v[feature->index];
		feature += 1;
	}
	return r;
}
void HessianVecProduct::operator()(const Real *x,const uint32_t n,Real *y)const{
	std::memset(y,0,sizeof(Real) * n);
	const Problem& problemData = *(gradientCalc->get_data());
	for(uint32_t i = 0; i < problemData.l; ++i){
		double p = gradientCalc->get_prediction(i);
		FeatureNode* instance = problemData.x[i];
		double dot = InstanceDot(instance,x);
		FeatureNode *feature = instance;
		while (feature->index >= 0){
			y[feature->index] += feature->value * dot * p * (1-p);
			feature += 1;
		}
	}
};
void HessianVecProduct::unittest(GradientCalc *gradientCalc){
	Log::raw("===========HessianVecProduct::unittest===============");
	uint32_t n = gradientCalc->get_data()->n;
	Real *mem = new Real[4 * n];
	std::memset(mem,0,sizeof(Real) * 2 * n);
	Real *w = mem; Real *g = mem + n; Real *y = mem + 2 * n;
	mem[4] = 1;
	(*gradientCalc)(w,g);
	HessianVecProduct *hessianMul = new HessianVecProduct(gradientCalc);
	Real *ones = mem + 3 * n;
	for(int i = 0; i < n; ++i)    ones[i] = 1;
	(*hessianMul)(ones,n,y);
	Real one_target[] = {2.2864,1.8932,1.5,1.8932,1.5728};
	for(int i = 0; i < 5; ++i){
		//std::cout<<"target="<<one_target[i]<<" y="<<y[i]<<"   ";
	}
	//std::cout<<std::endl;
	for(int i = 0; i < 5; ++i){
		assert(std::abs(one_target[i] - y[i]) < 1e-3);
	}
	(*hessianMul)(g,n,y);
	Real target[] = {4.1334,1.9669,1.4811,3.3602,3.5552};
	for(int i = 0; i < 5; ++i){
		//std::cout<<y[i]<<" ";
	}
	//std::cout<<std::endl;
	for(int i = 0; i < 5; ++i){
		assert(std::abs(target[i] - y[i]) < 1e-3);
	}
	delete hessianMul;
	delete []mem;
	Log::raw("+++++++++++HessianVecProduct::unittest+++++++++++++++");
}
