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

	T2 = hif2.get_records(_historical_data_size);

        sprintf(buffer, "%s%s",filedir,"T5.txt");
        HistoricalInputFile hif5(buffer);
        T5 = hif5.get_records(_historical_data_size);
        sprintf(buffer, "%s%s",filedir,"T10.txt");
        HistoricalInputFile hif10(buffer);
        T10 = hif10.get_records(_historical_data_size);
        sprintf(buffer, "%s%s",filedir,"T30.txt");
        HistoricalInputFile hif30(buffer);
        T30 = hif30.get_records(_historical_data_size);
}

VarGenerator::~VarGenerator()
{
	delete [] T2;
	delete [] T5;
	delete [] T10;
	delete [] T30;
}

double
VarGenerator::getVaR()
{
	double (*PnL)[100] = new double[length][100];
	int _historical_data_size;

	for(int i=0;i<length;i++) {
		double price[101];
		
		char buffer[128];
		sprintf(buffer,"%s%s%s",filedir,records[i].SecurityID,".txt");
		HistoricalInputFile hif(buffer);
		HistoricalFields* hrecords = hif.get_records(_historical_data_size);

		if(strcmp(hrecords[0].RateType,"YIELD")==0) {
			for(int j=0;j<_historical_data_size;j++)
				price[j] = records_cal[i]->getPrice(hrecords[j].RateValue);
		}
		else if(strcmp(hrecords[0].RateType,"SPREAD")==0) {
			HistoricalFields* bmk;
			switch(hrecords[0].BmkTicker[1]) {
			case '2':
				bmk = T2; break;
			case '5':
				bmk = T5; break;
			case '1':
				bmk = T10; break;
			case '3':
				bmk = T30; break;
			default:
				break;
			}
			
			for(int j=0;j<_historical_data_size;j++) 
				price[j] = records_cal[i]->getPrice(HistoricalFields::getRate(&hrecords[j],&bmk[j]));
		}

		for(int j=1;j<_historical_data_size;j++)
			PnL[i][j-1] = (price[j]-price[j-1]) * records[i].Amount;
		
	}
	
	double *PnL_array = new double[_historical_data_size-1]();
	for(int m=0;m<_historical_data_size-1;m++) {
		for(int n=0;n<length;n++) {
			PnL_array[m] += PnL[n][m];
		}
	}

	double result = PnL_array[0];
	for(int m=1;m<_historical_data_size-1;m++) {
		result = result<PnL_array[m] ? result : PnL_array[m];
	}

	// free
	for(int m=0;m<length;m++)
		delete  PnL[m];
	delete [] PnL;
	delete [] PnL_array;

	return result/100;
}

