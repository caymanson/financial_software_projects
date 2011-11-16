#include <stdlib.h>
#include "portfolio.h"

Portfolio::Portfolio(InstrumentFields records[],
		     BondCalculatorInterface* records_calc[],
		     int length) {
	lAmt = 0;
	sAmt = 0;
	risk = 0;
	sum = 0;

	for(int i=0;i<length;i++) {
		int amt = records[i].Amount;
		double tRisk = records_calc[i]->getRisk();

		if(amt > lAmt)
			lAmt = amt;
		if(amt < sAmt)
			sAmt = amt;
		if(abs(tRisk) > abs(risk))
			risk = tRisk;
		sum+=tRisk;	
	}
}

int
Portfolio::getLongest() {
	return lAmt;
}

int
Portfolio::getShortest() {
	return sAmt;
}

double
Portfolio::getMostRisky() {
	return risk;
}

double
Portfolio::getTotalRisk() {
	return sum;
}

void
Portfolio::show() {
	printf("Longest: %d Shortest: %d Risky: %.3f Total: %.3f\n",
		lAmt, sAmt, risk, sum);
}

