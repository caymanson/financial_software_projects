#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include "bond.h"
#include <vector>

using namespace std;

// the class of portfolio object
class Portfolio {

	public:
	
		//
		// using both instrument array and calculator array to
		// construct the object
		// records	: instrument array
		// records_calc	: calculator array
		// length	: the length of both array (should be the same)
		//
		Portfolio(InstrumentFields records[],
			  BondCalculatorInterface* records_calc[], 
			  int length);

		//
		// return the largest long position amount
		//
		int getLongest();

		//
		// return the largest short position amount
		//
		int getShortest();

		// 
		// return the most risky position risk
		//
		double getMostRisky();

		//
		// return the total risk of the portfolio
		//
		double getTotalRisk();

		//
		// print the the calculate result to stdout
		//
		void show();

		//
		// calculate bucket10_30 amount
		//
		double cal_amount10_30(double dvo1);

		//
		//
		//
		double cal_Market_Value(BondCalculatorInterface** records_calc,int item_count, int shift);

		//
		//
		//
		double getTotalLGD();

		//
		//
		//
		int getTotalAmount();

	protected:
		InstrumentFields* records;
		BondCalculatorInterface** records_calc;
		int length;
		int lAmt;
		int sAmt;
		double risk;
		double sum;
		vector<BondCalculatorInterface*> bucket0_2;
		vector<BondCalculatorInterface*> bucket2_5;
		vector<BondCalculatorInterface*> bucket5_10;
		vector<BondCalculatorInterface*> bucket10_30;
};

#endif
