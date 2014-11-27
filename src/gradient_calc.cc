#include <cstring>
#include <cmath>
#include <cstdlib>
#include <assert.h>
#include <iostream>
#include <cstdio>
#include "gradient_calc.h"
#include "vec_op.h"
#include "log.h"

double GradientCalc::operator()(const Real *w, Real * g){
		vec_cpy(g,w,data->n,l2);  // l2 regularization
		double NLL = 0.0;  // obj-function value
		for(uint32_t i = 0; i < data->l; ++i){
			const FeatureNode *instance=data->x[i];
			double yi = data->y[i];
			double t = bias;
			int j=0;
			while(-1 != instance[j].index){
				t += w[instance[j].index] * instance[j].value;
				++j;
			}
			double ui = 1.0 / (1.0 + std::exp(-t));
			j=0;
			while(-1 != instance[j].index){
				g[instance[j].index] += (ui - yi) * instance[j].value; // gradient, X'*(U-Y)
				++j;
			}
			predictedProb[i] = ui;
			if(yi)	NLL -= log(ui);
			else	NLL -= log(1-ui);
		}
		funcVal = NLL;
		return NLL;
}
GradientCalc* GradientCalc::unittest(const Problem* _data){
/*
 * w[0,1,2,3]=0, w[4]=1
 *   x[1],x[2],yi,ti,ui, gi,     g[0], g[1], g[2], g[3], g[4],   ui*(1-ui)
 i=0,  0,  1,  1, 0,0.5,-0.5,    -0.5, -0.5,     ,     ,     ,  0.25
 i=1,  1,  2,  1, 0,0.5,-0.5,        , -0.5, -0.5,     ,     ,  0.25
 i=2,  2,  3,  0, 0,0.5, 0.5,        ,     ,   0.5,  0.5,    ,  0.25
 i=3,  3,  4,  0, 1,s(1),s(1),     ,     ,      , s(1), s(1)  , s(1)*(1-s(1))
 i=4,  4,  0,  0, 1,s(1),s(1), s(1),    ,      ,      , s(1)  , s(1)*(1-s(1))
 i=5,  0,  2,  0, 0,0.5,0.5,       0.5,    ,   0.5,       ,   , 0.25
 i=6,  0,  3,  0, 0,0.5,0.5,       0.5,    ,      ,    0.5,   , 0.25
 i=7,  0,  4,  0, 1,s(1),s(1),    s(1),   ,      ,       , s(1),s(1)*(1-s(1))
 i=8,  1,  3,  0, 0,0.5,0.5,          , 0.5,     ,    0.5,     ,0.25
 i=9,  1,  4,  0, 1,s(1),s(1),       , s(1),    ,       , s(1) ,s(1)*(1-s(1))
 g[0]=0.5+2*s(1)
 g[1]=-0.5+s(1)
 g[2]=0.5
 g[3]=1.5+s(1)
 g[4]=4*s(1)
 a=0.25, b=s(1)*(1-s(1))
 Hessian
    0,    1,   2,    3,   4
0 3a+2b,
1   a,  3a+b
2   a,   a,   3a
3   a,   a,   a,   3a+b
4  2b,   b,   0,    b,   4b
 */
	Log::raw("===========GradientCalc::unittest===============");
	GradientCalc *gradientCalc = new GradientCalc(_data,0);
	uint32_t n = _data->n;
	Real *mem = new Real[2 * n + _data->l];
	Real *p = mem + 2 * n;
	gradientCalc->set_memory(p);
	std::memset(mem,0,sizeof(Real) * n);
	mem[4] = 1.0;
	double f = (*gradientCalc)(mem,mem + n);
	Real *g = mem + n;
	double u = 1.0 / (1.0 + std::exp(-1));
	double required = -6 * std::log(0.5) - 4 * std::log(1 - u);
	//std::cout<<f<<" "<<required<<std::endl;
	assert(std::abs(f - required < 1e-12));
	double t = 1.0 / (1.0 + std::exp(-1));
	double targets[] = { 0.5 + 2 * t, -0.5 + t, 0.5, 1.5 + t, 4 * t }; // 1.9621    0.2311    0.5000    2.2311    2.9242
	for(int i = 0; i < 5; ++i){
		assert(std::abs(g[i] - targets[i]) < 1e-12);
	}
	double p_target[] = {0.5,0.5,0.5,t,t,0.5,0.5,t,0.5,t};
	for(int i = 0; i < _data->l; ++i){
		assert(std::abs(p_target[i] - p[i]) < 1e-6);
	}
	delete []mem;
	Log::raw("+++++++++++GradientCalc::unittest+++++++++++++++");
	return gradientCalc;
}
