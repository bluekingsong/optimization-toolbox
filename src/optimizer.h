#ifndef _OPTIMIZER_H
#define _OPTIMIZER_H
#include <string>
#include "dataset.h"
#include "linear_search.h"
#include "gradient_calc.h"

struct OptimizePara {
	uint32_t maxIter;
	double gNormKsi;
	double probBias;
	double l2;
	OptimizePara(uint32_t _maxIter,double _gNormKsi): maxIter(_maxIter),gNormKsi(_gNormKsi)	{
		probBias = l2 = 0;
	}
};
class Optimizer {
  public:
	virtual void optimize(const OptimizePara&)=0;
	virtual void prepare_optimize(const Problem*)=0;
	virtual void post_optimize()=0;
	
  protected:
	GradientCalc *gradientCalc;
	LinearSearch *linearSearch;
	uint32_t n; // parameter vector length
	uint32_t numData;
	Real *w;  // current parameter vec
	Real *g; // current gradient vec
	Real *d; // current search direction
	double funcVal;
	uint32_t numIter;
	// member functions
	std::string make_monitor_str()const;
};

#endif
