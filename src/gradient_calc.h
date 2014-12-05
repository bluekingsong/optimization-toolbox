#ifndef _GRADIENT_CALC_H
#define _GRADIENT_CALC_H
#include "dataset.h"

typedef double Real;
typedef unsigned int uint32_t;

// the Object Function is f(w)=sum(fi(w,xi))/data.l + 0.5*l2*||w||^2
class GradientCalc {
  private:
    const Problem* data;
    Real *predictedProb;
    double funcVal;
    double intercept;
    bool fitIntercept;  // if fitIntercept = true, then the w[data.n] is the intercept
    double l2;
  public:
    GradientCalc(const Problem* _data,double _intercept = 0) : data(_data),intercept(_intercept){
        l2 = 0;
        fitIntercept = false;
    }
    void set_intercept(double _intercept,bool _fitIntercept){
        intercept = _intercept;
        fitIntercept = _fitIntercept;
    };
    void set_l2_para(double _l2){
        l2 = _l2;
    }
    double get_l2_para()const{
      return l2;
    }
    void set_memory(Real *mem){
        predictedProb = mem;
    };
    virtual double operator()(const Real *w, Real * g);
    double get_prediction(uint32_t index)const{
       return predictedProb[index];
    };
    double get_funcVal()const{
        return funcVal;
    };
    const Problem* get_data()const{
        return data;
    }
    static GradientCalc* unittest(const Problem* _data);
};
#endif
