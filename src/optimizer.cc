#include <iostream>
#include <sstream>
#include "optimizer.h"

std::string Optimizer::make_monitor_str() const{
	std::stringstream ss;
	ss<<"iter="<<numIter<<" funcVal="<<funcVal<<std::endl;
};
