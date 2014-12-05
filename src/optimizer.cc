#include <iostream>
#include <sstream>
#include <ctime>
#include "optimizer.h"
#include "vec_op.h"
#include "log.h"

std::string Optimizer::make_monitor_str() const{
    std::stringstream ss;
    time_t now = time(0);
    double wNorm = vec_dot(w,w,n);
    if(optimizePara.fitIntercept)   wNorm -= w[n - 1] * w[n - 1];
    ss<<"iter="<<numIter<<" time="<<now - beginTime<<" numData="<<numAccessData<<" funcVal="<<
        funcVal<<" gNorm="<<gNorm<<" squareWNorm="<<wNorm<<" numLS="<<numLS;
    if(optimizePara.fitIntercept)   ss<<" intercept="<<w[n-1];
    return ss.str();
};
void Optimizer::set_parameter(const OptimizePara& optPara){
    optimizePara = optPara;
    beginTime = time(0);
    numLS = numAccessData = 0;
    if(gradientCalc != 0){
        gradientCalc->set_intercept(optPara.intercept,optPara.fitIntercept);
        gradientCalc->set_l2_para(optPara.l2);
    }
    if(linearSearch != 0){
        linearSearch->setPara(optPara.LS_scaleRate,optPara.LS_initStep,optPara.LS_c1);
    }
}
void Optimizer::common_update(uint32_t dataAccessFactor){
    
}
void Optimizer::post_optimize(){
    std::cout<<"[Report]:"<<make_monitor_str()<<std::endl;
    delete linearSearch;
    delete gradientCalc;
    delete []w;
}
bool Optimizer::check_stop_condition(double fNew){
  ++numIter;
  double _funcVal = funcVal;
  funcVal = linearSearch->get_new_funcVal();
  if(numIter < 3)    return false;
  double decRatio = (_funcVal - fNew) / _funcVal;
  time_t now = time(0);
  if(numIter < optimizePara.maxIter && gNorm > optimizePara.gNormKsi
     && decRatio > optimizePara.decRatio && (now - beginTime) <= optimizePara.maxTrainSeconds){
      return false;
  }
  Log::raw(make_monitor_str());
  return true;
}
