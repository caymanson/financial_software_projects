#ifndef VARGENERATOR_H
#define VARGENERATOR_H

#include "io.h"
#include "bond.h"

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
	void getVaR(double& VaR,double& VaR_CS,double& VaR_IR);

	//
	// calculate total VaR and return it
	//
	double getVaR();

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
