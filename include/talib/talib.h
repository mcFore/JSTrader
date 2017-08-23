#ifndef TA_LIB_H
#define TA_LIB_H


#include "ta_func.h"
#include <vector>
#include <fstream>
struct STOCH_struct
{
	std::vector<double> K_vector;
	std::vector<double> D_vector;
	std::vector<double> J_vector;
};
class TALIB{
public:
	static std::vector<double>MA(std::vector<double>price_vector, TA_MAType MAType, int InTimePeriod);
	static std::vector<double>EMA(std::vector<double>price_vector, int InTimePeriod);
	static std::vector<double>RSI(std::vector<double>price_vector, int timePeriod);
	static std::vector<double>ATR(std::vector<double>high_vector, std::vector<double>low_vector,
		std::vector<double>close_vector, int timePeriod);
	static std::vector<double>ADX(std::vector<double>high_vector, std::vector<double>low_vector,
		std::vector<double>close_vector, int timePeriod);
	static STOCH_struct STOCH(std::vector<double>high_vector, std::vector<double>low_vector, std::vector<double>close_vector,
		int fastK_period, int slowK_period, int slowD_period, TA_MAType slowK_MAType, TA_MAType slowD_MAType);
	static std::vector<double>KAMA(std::vector<double>close_vector, int timePeriods);
	static std::vector<double>VAR(std::vector<double>close_vector, int timePeriods, double optInNbDev);
	static std::vector<double>STDDEV(std::vector<double>close_vector, int timePeriods, double optInNbDev);
	static std::vector<std::vector<double>>BOLLING(std::vector<double>close_vector, int timePeriodsdouble, double optInNbDevUp, double optInNbDevDn, TA_MAType optInMAType);
	static std::vector<double>SAR(std::vector<double>high_vector, std::vector<double>low_vector, double Acceleration, double Maximum);
};
#endif