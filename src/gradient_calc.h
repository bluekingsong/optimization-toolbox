#ifndef _GRADIENT_CALC_H
#define _GRADIENT_CALC_H
#include "dataset.h"

typedef double Real;
typedef unsigned int uint32_t;

class GradientCalc {
  private:
	const Problem* data;
	Real *predictedProb;
	double funcVal;
	double bias;
	double l2;
  public:
	GradientCalc(const Problem* _data,double _bias = 0) : data(_data),bias(_bias){
		l2 = 0;
	}
	void set_bias(double _bias){
		bias = _bias;
	};
	void set_l2_para(double _l2){
		l2 = _l2;
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
