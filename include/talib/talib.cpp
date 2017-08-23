#include"talib.h"

std::vector<double>TALIB::MA(std::vector<double>price_vector, TA_MAType MAType, int InTimePeriod)
{
	TA_Integer outBeg;
	TA_Integer outNbElement;
	double *result = new double[price_vector.size()];
	TA_RetCode retcode = TA_MA(
		0,//开始
		price_vector.size() - 1,//结束
		&price_vector[0],//开始位置指针
		InTimePeriod, /* From 1 to 100000 */
		MAType,
		&outBeg,
		&outNbElement,
		result);
	std::vector<double>MA_vector;
	for (int i = 0; i < outNbElement; i++)
	{
		//
		MA_vector.push_back(result[i]);
	}
	delete[] result;
	return MA_vector;
};

std::vector<double>TALIB::EMA(std::vector<double>price_vector, int InTimePeriod)
{
	TA_Integer outBeg;
	TA_Integer outNbElement;
	double *result = new double[price_vector.size()];
	TA_RetCode retcode = TA_EMA(
		0,//开始
		price_vector.size() - 1,//结束
		&price_vector[0],//开始位置指针
		InTimePeriod, /* From 1 to 100000 */
		&outBeg,
		&outNbElement,
		result);
	std::vector<double>MA_vector;
	for (int i = 0; i < outNbElement; i++)
	{
		//
		MA_vector.push_back(result[i]);
	}
	delete[] result;
	return MA_vector;
}

STOCH_struct TALIB::STOCH(std::vector<double>high_vector, std::vector<double>low_vector, std::vector<double>close_vector,
				int fastK_period, int slowK_period, int slowD_period,TA_MAType slowK_MAType,TA_MAType slowD_MAType)
{

	int outBegIdx;
	int outNbElement;
	double *slowK_array = new double[high_vector.size()];
	double *slowD_array = new double[high_vector.size()];
	TA_RetCode retCode = TA_STOCH(0,
		high_vector.size()-1,
		&high_vector[0],
		&low_vector[0],
		&close_vector[0],
		fastK_period, /* From 1 to 100000 */
		slowK_period, /* From 1 to 100000 */
		slowK_MAType,
		slowD_period, /* From 1 to 100000 */
		slowD_MAType,
		&outBegIdx,
		&outNbElement,
		slowK_array,
		slowD_array);
	STOCH_struct stoch_to_return;
	for (int i = 0; i < outNbElement; i++)
	{
		//
		stoch_to_return.K_vector.push_back(slowK_array[i]);
		stoch_to_return.D_vector.push_back(slowD_array[i]);
	}
	delete[] slowK_array;
	delete[] slowD_array;
	return stoch_to_return;

};


std::vector<double>TALIB::RSI(std::vector<double>price_vector, int timePeriod)
{
	int outBegIdx;
	int outNbElement;
	double *RSI_array = new double[price_vector.size()];
	TA_RetCode retCode = TA_RSI(0,
		price_vector.size()-1,
		&price_vector[0],
		timePeriod, /* From 2 to 100000 */
		&outBegIdx,
		&outNbElement,
		&RSI_array[0]);

	std::vector<double>RSI_vector;
	for (int i=0; i < outNbElement; i++)
	{
		RSI_vector.push_back(RSI_array[i]);
	}
	delete[] RSI_array;
	return RSI_vector;
};




std::vector<double>TALIB::ATR(std::vector<double>high_vector, std::vector<double>low_vector,
	std::vector<double>close_vector, int timePeriod)
{
	int outBegIdx;
	int outNbElement;
	double* ATR_array = new double[high_vector.size()];

	TA_RetCode retCode = TA_ATR(0,
		close_vector.size()-1,
		&high_vector[0],
		&low_vector[0],
		&close_vector[0],
		timePeriod, /* From 1 to 100000 */
		&outBegIdx,
		&outNbElement,
		&ATR_array[0]);

	std::vector<double>ATR_vector;
	for (int i = 0; i < outNbElement; i++)
	{
		ATR_vector.push_back(ATR_array[i]);
	}
	delete[] ATR_array;
	return ATR_vector;
};



std::vector<double>TALIB::ADX(std::vector<double>high_vector, std::vector<double>low_vector,
	std::vector<double>close_vector, int timePeriod)
{
	int outBegIdx;
	int outNbElement;
	double* ADX_array = new double[high_vector.size()];
	TA_RetCode retCode =  TA_ADX(0,
		high_vector.size()-1,
		&high_vector[0],
		&low_vector[0],
		&close_vector[0],
		timePeriod, /* From 2 to 100000 */
		&outBegIdx,
		&outNbElement,
		&ADX_array[0]);
	std::vector<double>ADX_vector;
	for (int i = 0; i < outNbElement; i++)
	{
		ADX_vector.push_back(ADX_array[i]);
	}
	delete[] ADX_array;
	return ADX_vector;
}


std::vector<double>TALIB::KAMA(std::vector<double>close_vector, int timePerios)
{
	TA_Integer outBegIdx;
	TA_Integer outNbElement;
	double *KAMA_array = new double[close_vector.size()];
	TA_RetCode retcode = TA_MA(0,
		close_vector.size() - 1,
		&close_vector[0],
		timePerios, /* From 2 to 100000 */
		TA_MAType_KAMA,
		&outBegIdx,
		&outNbElement,
		&KAMA_array[0]);
	std::vector<double>KAMA_vector;
	for (int i = 0; i < outNbElement; i++)
	{
		KAMA_vector.push_back(KAMA_array[i]);
	}
	delete[] KAMA_array;
	return KAMA_vector;
}

std::vector<double>TALIB::VAR(std::vector<double>close_vector, int timePeriods, double optInNbDev)
{
	TA_Integer outBegIdx;
	TA_Integer outNbElement;
	double *VAR_array = new double[close_vector.size()];
	TA_RetCode retcode = TA_VAR(0,
		close_vector.size() - 1,
		&close_vector[0],
		timePeriods, /* From 2 to 100000 */
		optInNbDev, /* From TA_REAL_MIN to TA_REAL_MAX */
		&outBegIdx,
		&outNbElement,
		&VAR_array[0]);
	std::vector<double>VAR_vector;
	for (int i = 0; i < outNbElement; i++)
	{
		VAR_vector.push_back(VAR_array[i]);
	}
	delete[] VAR_array;
	return VAR_vector;

}


std::vector<double>TALIB::STDDEV(std::vector<double>close_vector, int timePeriods, double optInNbDev)
{
	TA_Integer outBegIdx;
	TA_Integer outNbElement;
	double *STDDEV_array = new double[close_vector.size()];
	TA_RetCode retcode = TA_STDDEV(0,
		close_vector.size() - 1,
		&close_vector[0],
		timePeriods, /* From 2 to 100000 */
		optInNbDev, /* From TA_REAL_MIN to TA_REAL_MAX */
		&outBegIdx,
		&outNbElement,
		&STDDEV_array[0]);
	std::vector<double>STDDEV_vector;
	for (int i = 0; i < outNbElement; i++)
	{
		STDDEV_vector.push_back(STDDEV_array[i]);
	}
	delete[] STDDEV_array;
	return STDDEV_vector;

}

std::vector<std::vector<double>>TALIB::BOLLING(std::vector<double>close_vector, int timePeriods, double optInNbDevUp, double optInNbDevDn, TA_MAType optInMAType)
{
	TA_Integer outBegIdx;
	TA_Integer outNbElement;
	double *UPPER_array = new double[close_vector.size()];
	double *MID_array = new double[close_vector.size()];
	double *LOWER_array = new double[close_vector.size()];
	TA_RetCode retcode = TA_BBANDS(0,
		close_vector.size() - 1,
		&close_vector[0],
		timePeriods, /* From 2 to 100000 */
		optInNbDevUp, /* From TA_REAL_MIN to TA_REAL_MAX */
		optInNbDevDn, /* From TA_REAL_MIN to TA_REAL_MAX */
		optInMAType,
		&outBegIdx,
		&outNbElement,
		&UPPER_array[0],
		&MID_array[0],
		&LOWER_array[0]);

	std::vector<std::vector<double>>BOLLINGER;
	for (int i = 0; i < outNbElement; i++)
	{
		std::vector<double>UML;
		UML.push_back(UPPER_array[i]);
		UML.push_back(MID_array[i]);
		UML.push_back(LOWER_array[i]);
		BOLLINGER.push_back(UML);
	}
	delete[] UPPER_array, MID_array, LOWER_array;
	return BOLLINGER;
}

std::vector<double>TALIB::SAR(std::vector<double>high_vector, std::vector<double>low_vector, double Acceleration, double Maximum)
{
	TA_Integer outBeg;
	TA_Integer outNbElement;
	double *result = new double[high_vector.size()];
	TA_RetCode retcode = TA_SAR(
		0,//开始
		high_vector.size() - 1,//结束
		&high_vector[0],
		&low_vector[0],
		Acceleration, /* From 1 to 100000 */
		Maximum,
		&outBeg,
		&outNbElement,
		result);
	std::vector<double>sar_vector;
	for (int i = 0; i < outNbElement; i++)
	{
		//
		sar_vector.push_back(result[i]);
	}
	delete[] result;
	return sar_vector;
}