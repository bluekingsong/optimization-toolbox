#ifndef _LBFGS_H
#define _LBFGS_H
#include "optimizer.h"
#include "dataset.h"

class LBFGS : public Optimizer{
  public:
    virtual void optimize();
    virtual void prepare_optimize(const Problem* data);
    virtual void set_parameter(const OptimizePara& optPara);
  protected:
    Real *s; // delta(x)
    Real *y; // delta(g);
    Real *rho;
    Real *alpha;
    uint32_t m;
    void calc_bfgs_direction(uint32_t k,double gamma);
};
#endif
