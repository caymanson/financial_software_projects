#include "vargenerator.h"
#include <string.h>

VarGenerator::VarGenerator(char* f,
			InstrumentFields* r, 
			BondCalculatorInterface** rc,
			int len)
{
	filedir = f;
	records = r;
	records_cal = rc;
	length = len;

	int _historical_data_size;
	char buffer[128];
	sprintf(buffer, "%s%s",filedir,"T2.txt");
	HistoricalInputFile hif2(buffer);
	T2_arr = hif2.get_records(_historical_data_size);
    sprintf(buffer, "%s%s",filedir,"T5.txt");
    HistoricalInputFile hif5(buffer);
    T5_arr = hif5.get_records(_historical_data_size);
    sprintf(buffer, "%s%s",filedir,"T10.txt");
    HistoricalInputFile hif10(buffer);
    T10_arr = hif10.get_records(_historical_data_size);
    sprintf(buffer, "%s%s",filedir,"T30.txt");
    HistoricalInputFile hif30(buffer);
    T30_arr = hif30.get_records(_historical_data_size);
}

VarGenerator::~VarGenerator()
{
	delete [] T2_arr;
	delete [] T5_arr;
	delete [] T10_arr;
	delete [] T30_arr;
}

void
VarGenerator::getVaR(double& VaR, double& VaR_CS, double& VaR_IR, double** PnL, int& size, double shift)
{
	*PnL = new double[100]();
	double *PnL_CS = new double[100]();
	double *PnL_IR = new double[100]();
	int _historical_data_size;

	for(int i=0;i<length;i++) {
		double rate_arr[101];
		double rate_CS[101];
		double rate_IR[101];
		double rate = records_cal[i]->getRate()+shift;
		double price = records_cal[i]->getPrice(rate);

		char buffer[128];
		sprintf(buffer,"%s%s%s",filedir,records[i].SecurityID,".txt");
		HistoricalInputFile hif(buffer);
		HistoricalFields* hrecords = hif.get_records(_historical_data_size);

		if(strcmp(hrecords[0].RateType,"YIELD")==0) {
			for(int j=0;j<_historical_data_size;j++) {
				rate_arr[j] = hrecords[j].RateValue;
				rate_CS[j] = 0.0;
				rate_IR[j] = hrecords[j].RateValue;
			}
		}
		else if(strcmp(hrecords[0].RateType,"SPREAD")==0) {
			HistoricalFields* bmk;
			switch(hrecords[0].BmkTicker[1]) {
			case '2':
				bmk = T2_arr; break;
			case '5':
				bmk = T5_arr; break;
			case '1':
				bmk = T10_arr; break;
			case '3':
				bmk = T30_arr; break;
			default:
				break;
			}
			
			for(int j=0;j<_historical_data_size;j++) {
				rate_arr[j] = HistoricalFields::getRate(&hrecords[j],&bmk[j]);
				rate_CS[j] = hrecords[j].RateValue / 100;
				rate_IR[j] = bmk[j].RateValue;
			}
		}

		for(int j=1;j<_historical_data_size;j++) {
			double r = rate + rate_arr[j] - rate_arr[j-1];
			double rc= rate + rate_CS[j] - rate_CS[j-1];
			double ri= rate + rate_IR[j] - rate_IR[j-1];
			(*PnL)[j-1] += (records_cal[i]->getPrice(r)-price) * records[i].Amount/100;
			PnL_CS[j-1]+=(records_cal[i]->getPrice(rc)-price)*records[i].Amount;
			PnL_IR[j-1]+=(records_cal[i]->getPrice(ri)-price)*records[i].Amount;
		}
		
	}
	
	double result = (*PnL)[0];
	double result_CS = PnL_CS[0];
	double result_IR = PnL_IR[0];
	for(int m=1;m<_historical_data_size-1;m++) {
		result = result<(*PnL)[m] ? result : (*PnL)[m];
		result_CS = result_CS<PnL_CS[m] ? result_CS : PnL_CS[m];
		result_IR = result_IR<PnL_IR[m] ? result_IR : PnL_IR[m];
	}

	VaR = result;
	VaR_CS = result_CS/100;
	VaR_IR = result_IR/100;
	size = _historical_data_size-1;

	// free
	delete [] PnL_CS;
	delete [] PnL_IR;
}

double
VarGenerator::getVaR()
{
	double r,r_CS,r_IR;
	double * PnL;
	int size;
	getVaR(r,r_CS,r_IR,&PnL,size);

	delete [] PnL;
	return r;
}
