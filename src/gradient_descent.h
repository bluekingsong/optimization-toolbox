#ifndef _GRADIENT_DESCENT_H
#define _GRADIENT_DESCENT_H
#include "optimizer.h"
#include "dataset.h"

class GradientDescent : public Optimizer{
  public:
	virtual void optimize(const OptimizePara& optPara);
	virtual void prepare_optimize(const Problem* data);
	virtual void post_optimize();
  protected:
	double guessInitStep()const;
};
#endif
