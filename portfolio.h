#include "bond.h"

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

	protected:
		int lAmt;
		int sAmt;
		double risk;
		double sum;
};
