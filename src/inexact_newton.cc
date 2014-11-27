#include <cstring>
#include "inexact_newton.h"
#include "vec_op.h"
#include "log.h"


void InexactNewton::prepare_optimize(const Problem* data){
	n = data->n;
	numData = data->l;
	Real *mem = new Real[3 * n + numData + 2 * n + 3 * n];
	// self memory=3*n[w,g,d]
	w = mem;
	std::memset(w,0,sizeof(Real) * n); // init
	g = mem + n;
	d = mem + 2 * n;
	// memory=numData +1 [ predict prob, funcVal] 
	gradientCalc = new GradientCalc(data);
	gradientCalc->set_memory(mem + 3 * n);
	// memory=2*n[xp,gp]
	linearSearch = new LinearSearch(gradientCalc);
	linearSearch->set_memory(mem + 3 * n + numData, mem + 3 * n + numData + n);
	hessianMul = new HessianVecProduct(gradientCalc);
	// memory=3*n[r,p,y]
	cgSolver = new CGSolver(hessianMul,n);
	cgSolver->set_memory(mem + 2 * n + numData + 2 * n,mem + 2 * n + numData + 3 * n, mem + 2 * n + numData + 4 * n);
}
void InexactNewton::post_optimize(){
	delete gradientCalc;
	delete linearSearch;
	delete hessianMul;
	delete cgSolver;
	delete []w;
}
void InexactNewton::optimize(const OptimizePara & optimizePara){
	(*gradientCalc)(w,g);
	funcVal = gradientCalc->get_funcVal();
	double gNorm = std::sqrt(vec_dot(g,g,n));
	numIter = 0;
	while( numIter++ < optimizePara.maxIter && gNorm > optimizePara.gNormKsi){
		cgSolver->solve(d,cgPara,g,-1);
		uint32_t cgIter = cgSolver->get_iter_num();
		double step_len = (*linearSearch)(w,g,d,-1);
		if( step_len < 0 ){
			Log::error("InexactNewton::do_optimize","can't find suitable step size at line-search");
			break;
		}
		vec_cpy(w,linearSearch->get_new_parameter(),n);
		vec_cpy(g,linearSearch->get_new_gradient(),n);
		funcVal = linearSearch->get_new_funcVal();
		gNorm = std::sqrt(vec_dot(g,g,n));
		//Log::info("performance",make_monitor_str());
		std::cout<<"iter="<<numIter<<" fVal="<<funcVal<<" gNorm="<<gNorm<<" cgIter="<<cgIter<<std::endl;
	}
};
void InexactNewton::unittest(){
	Log::raw("===========InexactNewton::unittest===============");
	Problem data = Problem::unittest();
	InexactNewton newton;
	newton.prepare_optimize(&data);
	std::memset(newton.w,0,sizeof(Real) * newton.n);
	newton.w[4] = 1;
	OptimizePara para(50,1e-12);
	newton.optimize(para);
	newton.post_optimize();
	Log::raw("+++++++++++InexactNewton::unittest+++++++++++++++");

}
