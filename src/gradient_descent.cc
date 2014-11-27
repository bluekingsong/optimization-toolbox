#include <cmath>
#include <cstring>
#include "vec_op.h"
#include "gradient_descent.h"
#include "log.h"

void GradientDescent::optimize(const OptimizePara& optimizePara){
	funcVal = (*gradientCalc)(w,g);
	double gNorm = std::sqrt(vec_dot(g,g,n));
	numIter = 0;
	while( numIter++ < optimizePara.maxIter && gNorm > optimizePara.gNormKsi){
		double initStep = guessInitStep();
		linearSearch->set_init_step(initStep);
		double step_len = (*linearSearch)(w,g,g,-1);
		if( step_len < 0 ){
			Log::error("InexactNewton::do_optimize","can't find suitable step size at line-search");
			break;
		}
		vec_cpy(w,linearSearch->get_new_parameter(),n);
		vec_cpy(g,linearSearch->get_new_gradient(),n);
		funcVal = linearSearch->get_new_funcVal();
		gNorm = std::sqrt(vec_dot(g,g,n));
		//Log::info("performance",make_monitor_str());
		std::cout<<"iter="<<numIter<<" fVal="<<funcVal<<" gNorm="<<gNorm<<std::endl;
	}
}
double GradientDescent::guessInitStep()const{
	double result = std::abs(g[0]);
	for(uint32_t i = 1; i < n; ++i){
		if(std::abs(g[i]) > result)    result = std::abs(g[i]);
	}
	return 1.0 / result * std::pow(0.9,numIter);
}
void GradientDescent::prepare_optimize(const Problem* data){
	n = data->n;
	numData = data->l;
	Real *mem = new Real[2 * n + numData + 2 * n];
	// self memory=3*n[w,g,d] + l
	w = mem;
	std::memset(w,0,sizeof(Real) * n); // init
	g = mem + n;
	d = 0;
	// memory=numData +1 [ predict prob, funcVal]
	gradientCalc = new GradientCalc(data);
	gradientCalc->set_memory(mem + 2 * n);
	// memory=2*n[xp,gp]
	linearSearch = new LinearSearch(gradientCalc);
	linearSearch->set_memory(mem + 2 * n + numData, mem + 2 * n + numData + n);
}
void GradientDescent::post_optimize(){
	delete linearSearch;
	delete gradientCalc;
	delete []w;
}
