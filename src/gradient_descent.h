#ifndef _GRADIENT_DESCENT_H
#define _GRADIENT_DESCENT_H
#include "optimizer.h"
#include "dataset.h"

class GradientDescent : public Optimizer{
  public:
    virtual void optimize();
    virtual void prepare_optimize(const Problem* data);
    static double guessInitStep(const Real *g,uint32_t n,uint32_t numIter);
};
#endif
