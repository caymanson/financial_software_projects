#ifndef VARGENERATOR_H

#define VARGENERATOR_H

using namespace std;

#include "io.h"
#include "bond.h"
#include "string.h"

class VarGenerator {
public:
	VarGenerator(char* f,
			InstrumentFields* r,
			BondCalculatorInterface** rc,
			int len);
	~VarGenerator();

	//
	// calculate total VaR, CS VaR and IR VaR
	//
	void getVaR(double& VaR,double& VaR_CS,double& VaR_IR,double**PnL,int& size, double shift=0);

	//
	// calculate total VaR and return it
	//
	double getVaR();

	static char* DailyChangeByIssuer(VarGenerator* vo, VarGenerator* vc, double shift)
	{
		char buffer[256];
		char*p = new char[2048];
		memset(p,0,sizeof(p));

		double o,c,ocs,oir,ccs,cir;
		double *po;
		double *pc;
		int size;
		vo->getVaR(o,ocs,oir,&po,size,shift);
		vc->getVaR(c,ccs,cir,&pc,size,shift);
		sprintf(buffer,"VaR-CreditSpread,%.3f,%.3f,%.3f;",ocs,ccs,ccs-ocs);
		strcat(p,buffer);
		sprintf(buffer,"VaR-InterestRate,%.3f,%.3f,%.3f;",oir,cir,cir-oir);
		strcat(p,buffer);
		sprintf(buffer,"VaR-Total,%.3f,%.3f,%.3f;",o,c,c-o);
		strcat(p,buffer);

		for(int i=0;i<size;i++) {
			sprintf(buffer,"%.3f,",pc[i]);
			strcat(p,buffer);
		}

		delete [] po;
		delete [] pc;
		return p;
	}

private:
	char* filedir;
	InstrumentFields* records;
	BondCalculatorInterface** records_cal;
	int length;
	HistoricalFields* T2_arr;
	HistoricalFields* T5_arr;
	HistoricalFields* T10_arr;
	HistoricalFields* T30_arr; 
};

#endif
