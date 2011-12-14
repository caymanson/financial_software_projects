#ifndef PORTFOLIO_H
#define PORTFOLIO_H

using namespace std;

#include "bond.h"
#include "string.h"
#include <vector>
#include <map>

struct Position {
	string Ticker;
	int Amount;
	double Risk;
	double LGD;
};

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
		// deconstructor
		//
		~Portfolio();

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
		//calculate bucket risk and 2 year hedge amount
		//
		double cal_2yr_hedge_amount(vector<BondCalculatorInterface*> bucket, double &bucket_risk, double dv01, double shift);

		//
		//calculate bucket market value
		//
		double cal_bucket_mktval(vector<BondCalculatorInterface *> bucket, double shift);
		
		//
		//cal market value of portfolio
		//
		double cal_Market_Value(BondCalculatorInterface** records_calc,int item_count, double shift);

		//
		//
		//
		double getTotalLGD();

		//
		//
		//
		int getTotalAmount();

		//
		// get amount, risk, and LGD from bonds with various quality
		//
		int getAmount(vector<BondCalculatorInterface*>* bucket);
		double getRisk(vector<BondCalculatorInterface*>* bucket,double shift);
		double getLGD(vector<BondCalculatorInterface*>* bucket);

		//
		// get amount, risk, and LGF from bons with various ticker
		//
		Position* getPositions(double shift);

		//
		// calculate two portfolios' position
		//
		static char* DailyChangeByIssuer(Portfolio* p_c, Portfolio* p_e, double shift) {
			char* buffer = new char[1024];
			char* b = new char[2048];

			int n;

			Position* po_c = p_c->getPositions(shift);
			Position* po_e = p_e->getPositions(shift);

			for(int i=0;i<p_c->ticker_size;i++) {
				double n_c = po_c[i].Amount;
				double n_e = po_e[i].Amount;
				double r_c = po_c[i].Risk;
				double r_e = po_e[i].Risk;
				double l_c = po_c[i].LGD;
				double l_e = po_e[i].LGD;
				n = sprintf(buffer,"%s,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f;",po_c[i].Ticker.c_str(),n_c,r_c,l_c,n_e,r_e,l_e,n_e-n_c,r_e-r_c,l_e-l_c);
				strcat(b,buffer);
			}

			double n_ic=0,n_jc=0,r_ic=0,r_jc=0,l_ic=0,l_jc=0;
			double n_ie=0,n_je=0,r_ie=0,r_je=0,l_ie=0,l_je=0;
			double n_e=0,n_c=0,r_e=0,r_c=0,l_e=0,l_c=0;
			// AAA
			n_c = p_c->getAmount(&p_c->bucketAAA);
			n_e = p_e->getAmount(&p_e->bucketAAA);
			r_c = p_c->getRisk(&p_c->bucketAAA,shift);
			r_e = p_e->getRisk(&p_e->bucketAAA,shift);
			l_c = p_c->getLGD(&p_c->bucketAAA);
			l_e = p_e->getLGD(&p_e->bucketAAA);
			n = sprintf(buffer,"%s,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f;","AAA",n_c,r_c,l_c,n_e,r_e,l_e,n_e-n_c,r_e-r_c,l_e-l_c);
			strcat(b,buffer);
			n_ic+=n_c;
			n_ie+=n_e;
			r_ic+=r_c;
			r_ie+=r_e;
			l_ic+=l_c;
			l_ie+=l_e;
			
			// AA
			n_c = p_c->getAmount(&p_c->bucketAA);
			n_e = p_e->getAmount(&p_e->bucketAA);
			r_c = p_c->getRisk(&p_c->bucketAA,shift);
			r_e = p_e->getRisk(&p_e->bucketAA,shift);
			l_c = p_c->getLGD(&p_c->bucketAA);
			l_e = p_e->getLGD(&p_e->bucketAA);
			n = sprintf(buffer,"%s,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f;","AA",n_c,r_c,l_c,n_e,r_e,l_e,n_e-n_c,r_e-r_c,l_e-l_c);
			strcat(b,buffer);
			n_ic+=n_c;
			n_ie+=n_e;
			r_ic+=r_c;
			r_ie+=r_e;
			l_ic+=l_c;
			l_ie+=l_e;
			
			// A
			n_c = p_c->getAmount(&p_c->bucketA);
			n_e = p_e->getAmount(&p_e->bucketA);
			r_c = p_c->getRisk(&p_c->bucketA,shift);
			r_e = p_e->getRisk(&p_e->bucketA,shift);
			l_c = p_c->getLGD(&p_c->bucketA);
			l_e = p_e->getLGD(&p_e->bucketA);
			n = sprintf(buffer,"%s,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f;","A",n_c,r_c,l_c,n_e,r_e,l_e,n_e-n_c,r_e-r_c,l_e-l_c);
			strcat(b,buffer);
			n_ic+=n_c;
			n_ie+=n_e;
			r_ic+=r_c;
			r_ie+=r_e;
			l_ic+=l_c;
			l_ie+=l_e;
			
			// BBB
			n_c = p_c->getAmount(&p_c->bucketBBB);
			n_e = p_e->getAmount(&p_e->bucketBBB);
			r_c = p_c->getRisk(&p_c->bucketBBB,shift);
			r_e = p_e->getRisk(&p_e->bucketBBB,shift);
			l_c = p_c->getLGD(&p_c->bucketBBB);
			l_e = p_e->getLGD(&p_e->bucketBBB);
			n = sprintf(buffer,"%s,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f;","BBB",n_c,r_c,l_c,n_e,r_e,l_e,n_e-n_c,r_e-r_c,l_e-l_c);
			strcat(b,buffer);
			n_ic+=n_c;
			n_ie+=n_e;
			r_ic+=r_c;
			r_ie+=r_e;
			l_ic+=l_c;
			l_ie+=l_e;
			
			// BB
			n_c = p_c->getAmount(&p_c->bucketBB);
			n_e = p_e->getAmount(&p_e->bucketBB);
			r_c = p_c->getRisk(&p_c->bucketBB,shift);
			r_e = p_e->getRisk(&p_e->bucketBB,shift);
			l_c = p_c->getLGD(&p_c->bucketBB);
			l_e = p_e->getLGD(&p_e->bucketBB);
			n = sprintf(buffer,"%s,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f;","BB",n_c,r_c,l_c,n_e,r_e,l_e,n_e-n_c,r_e-r_c,l_e-l_c);
			strcat(b,buffer);
			n_jc+=n_c;
			n_je+=n_e;
			r_jc+=r_c;
			r_je+=r_e;
			l_jc+=l_c;
			l_je+=l_e;
			
			// B
			n_c = p_c->getAmount(&p_c->bucketB);
			n_e = p_e->getAmount(&p_e->bucketB);
			r_c = p_c->getRisk(&p_c->bucketB,shift);
			r_e = p_e->getRisk(&p_e->bucketB,shift);
			l_c = p_c->getLGD(&p_c->bucketB);
			l_e = p_e->getLGD(&p_e->bucketB);
			n = sprintf(buffer,"%s,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f;","B",n_c,r_c,l_c,n_e,r_e,l_e,n_e-n_c,r_e-r_c,l_e-l_c);
			strcat(b,buffer);
			n_jc+=n_c;
			n_je+=n_e;
			r_jc+=r_c;
			r_je+=r_e;
			l_jc+=l_c;
			l_je+=l_e;
			
			// CCC
			n_c = p_c->getAmount(&p_c->bucketCCC);
			n_e = p_e->getAmount(&p_e->bucketCCC);
			r_c = p_c->getRisk(&p_c->bucketCCC,shift);
			r_e = p_e->getRisk(&p_e->bucketCCC,shift);
			l_c = p_c->getLGD(&p_c->bucketCCC);
			l_e = p_e->getLGD(&p_e->bucketCCC);
			n = sprintf(buffer,"%s,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f;","CCC",n_c,r_c,l_c,n_e,r_e,l_e,n_e-n_c,r_e-r_c,l_e-l_c);
			strcat(b,buffer);
			n_jc+=n_c;
			n_je+=n_e;
			r_jc+=r_c;
			r_je+=r_e;
			l_jc+=l_c;
			l_je+=l_e;
			
			// CC
			n_c = p_c->getAmount(&p_c->bucketCC);
			n_e = p_e->getAmount(&p_e->bucketCC);
			r_c = p_c->getRisk(&p_c->bucketCC,shift);
			r_e = p_e->getRisk(&p_e->bucketCC,shift);
			l_c = p_c->getLGD(&p_c->bucketCC);
			l_e = p_e->getLGD(&p_e->bucketCC);
			n = sprintf(buffer,"%s,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f;","CC",n_c,r_c,l_c,n_e,r_e,l_e,n_e-n_c,r_e-r_c,l_e-l_c);
			strcat(b,buffer);
			n_jc+=n_c;
			n_je+=n_e;
			r_jc+=r_c;
			r_je+=r_e;
			l_jc+=l_c;
			l_je+=l_e;
			
			// C
			n_c = p_c->getAmount(&p_c->bucketC);
			n_e = p_e->getAmount(&p_e->bucketC);
			r_c = p_c->getRisk(&p_c->bucketC,shift);
			r_e = p_e->getRisk(&p_e->bucketC,shift);
			l_c = p_c->getLGD(&p_c->bucketC);
			l_e = p_e->getLGD(&p_e->bucketC);
			n = sprintf(buffer,"%s,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f;","C",n_c,r_c,l_c,n_e,r_e,l_e,n_e-n_c,r_e-r_c,l_e-l_c);
			strcat(b,buffer);
			n_jc+=n_c;
			n_je+=n_e;
			r_jc+=r_c;
			r_je+=r_e;
			l_jc+=l_c;
			l_je+=l_e;
			
			// D
			n_c = p_c->getAmount(&p_c->bucketD);
			n_e = p_e->getAmount(&p_e->bucketD);
			r_c = p_c->getRisk(&p_c->bucketD,shift);
			r_e = p_e->getRisk(&p_e->bucketD,shift);
			l_c = p_c->getLGD(&p_c->bucketD);
			l_e = p_e->getLGD(&p_e->bucketD);
			n = sprintf(buffer,"%s,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f;","D",n_c,r_c,l_c,n_e,r_e,l_e,n_e-n_c,r_e-r_c,l_e-l_c);
			strcat(b,buffer);
			n_jc+=n_c;
			n_je+=n_e;
			r_jc+=r_c;
			r_je+=r_e;
			l_jc+=l_c;
			l_je+=l_e;
			
			// Investment
			n = sprintf(buffer,"%s,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f;","Investment_Grade",n_ic,r_ic,l_ic,n_ie,r_ie,l_ie,n_ie-n_ic,r_ie-r_ic,l_ie-l_ic);
			strcat(b,buffer);
			// Junk
			n = sprintf(buffer,"%s,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f;","Junk",n_jc,r_jc,l_jc,n_je,r_je,l_je,n_je-n_jc,r_je-r_jc,l_je-l_jc);
			strcat(b,buffer);

			delete [] po_c;
			delete [] po_e;

			return b;
		}

		vector<BondCalculatorInterface*> bucket0_2;
		vector<BondCalculatorInterface*> bucket2_5;
		vector<BondCalculatorInterface*> bucket5_10;
		vector<BondCalculatorInterface*> bucket10_30;
		
		vector<BondCalculatorInterface*> bucketAAA;
		vector<BondCalculatorInterface*> bucketAA;
		vector<BondCalculatorInterface*> bucketA;
		vector<BondCalculatorInterface*> bucketBBB;
		vector<BondCalculatorInterface*> bucketBB;
		vector<BondCalculatorInterface*> bucketB;
		vector<BondCalculatorInterface*> bucketCCC;
		vector<BondCalculatorInterface*> bucketCC;
		vector<BondCalculatorInterface*> bucketC;
		vector<BondCalculatorInterface*> bucketD;

		vector<BondCalculatorInterface*> bucketInvestment;
		vector<BondCalculatorInterface*> bucketJunk;
		
		int ticker_size;
		map<string, vector<BondCalculatorInterface*>*> bucketTicker;

	protected:
		InstrumentFields* records;
		BondCalculatorInterface** records_calc;
		int length;
		int lAmt;
		int sAmt;
		double risk;
		double sum;

};

#endif
