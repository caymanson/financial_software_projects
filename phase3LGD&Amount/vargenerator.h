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
	//
	//
	double getVaR();

private:
	char* filedir;
	InstrumentFields* records;
	BondCalculatorInterface** records_cal;
	int length;
	HistoricalFields* T2;
	HistoricalFields* T5;
	HistoricalFields* T10;
	HistoricalFields* T30; 
};

#endif
