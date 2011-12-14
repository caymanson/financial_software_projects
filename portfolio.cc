#include <stdlib.h>
#include "SBB_date.h"
#include "portfolio.h"

using namespace std;

Portfolio::Portfolio(InstrumentFields r[],
		     BondCalculatorInterface* rc[],
		     int len) {
	lAmt = 0;
	sAmt = 0;
	risk = 0;
	sum = 0;
	ticker_size = 0;

	records = r;
	records_calc = rc;
	length = len;

	SBB_date from_dt;
	SBB_date to_dt;

	for(int i=0;i<length;i++) {
		from_dt.set_from_yyyymmdd(records[i].SettlementDate);
		to_dt.set_from_yyyymmdd(records[i].MaturityDate);
		int T = Term::getNumberOfPeriods(from_dt, to_dt, 1);

		if (T>0 && T<=2)
			bucket0_2.push_back(records_calc[i]);
		else if(T>2 && T<=5)
			bucket2_5.push_back(records_calc[i]);
		else if(T>5 && T<=10)
			bucket5_10.push_back(records_calc[i]);
		else if(T>10 && T<=30)
			bucket10_30.push_back(records_calc[i]);

		if (strcmp(records[i].Quality,"AAA")==0) {
			bucketAAA.push_back(records_calc[i]);
			bucketInvestment.push_back(records_calc[i]);
		}
		else if (strcmp(records[i].Quality,"AA")==0) {
			bucketAA.push_back(records_calc[i]);
			bucketInvestment.push_back(records_calc[i]);
		}
		else if (strcmp(records[i].Quality,"A")==0) {
			bucketA.push_back(records_calc[i]);
			bucketInvestment.push_back(records_calc[i]);
		}
		else if (strcmp(records[i].Quality,"BBB")==0) {
			bucketBBB.push_back(records_calc[i]);
			bucketInvestment.push_back(records_calc[i]);
		}
		else if (strcmp(records[i].Quality,"BB")==0) {
			bucketBB.push_back(records_calc[i]);
			bucketJunk.push_back(records_calc[i]);
		}
		else if (strcmp(records[i].Quality,"B")==0) {
			bucketJunk.push_back(records_calc[i]);
			bucketB.push_back(records_calc[i]);
		}
		else if (strcmp(records[i].Quality,"CCC")==0) {
			bucketCCC.push_back(records_calc[i]);
			bucketJunk.push_back(records_calc[i]);
		}
		else if (strcmp(records[i].Quality,"CC")==0) {
			bucketCC.push_back(records_calc[i]);
			bucketJunk.push_back(records_calc[i]);
		}
		else if (strcmp(records[i].Quality,"C")==0) {
			bucketC.push_back(records_calc[i]);
			bucketJunk.push_back(records_calc[i]);
		}
		else if (strcmp(records[i].Quality,"D")==0) {
			bucketD.push_back(records_calc[i]);
			bucketJunk.push_back(records_calc[i]);
		}

		map<string, vector<BondCalculatorInterface *>*>::iterator it;
		it = bucketTicker.find(records[i].Ticker);
		if(it != bucketTicker.end()) {
			it->second->push_back(records_calc[i]);
		}
		else {
			vector<BondCalculatorInterface*> *vec = new vector<BondCalculatorInterface *>();
			vec->push_back(records_calc[i]);
			bucketTicker.insert(make_pair(records[i].Ticker, vec)); 
			ticker_size++;
		}
	}
}

Portfolio::~Portfolio() {
	map<string, vector<BondCalculatorInterface *>*>::iterator it;
	for (it = bucketTicker.begin(); it != bucketTicker.end(); it++) {
		delete it->second;
	}

}

int
Portfolio::getLongest() {
	for (int i=0;i<length;i++)
	{
		int amt = records[i].Amount;
		if(amt > lAmt)
			lAmt = amt;
	}
	return lAmt;
}

int
Portfolio::getShortest() {
	for (int i=0;i<length;i++)
	{
		int amt = records[i].Amount;
		if(amt < sAmt)
			sAmt = amt;
	}
	return sAmt;
}

double
Portfolio::getMostRisky() {
	for (int i=0;i<length;i++)
	{
		double tRisk = records_calc[i]->getRisk();
		if(abs(tRisk) > abs(risk))
			risk = tRisk;
	}
	return risk;
}

double
Portfolio::getTotalRisk() {
	sum = 0;
	for(int i=0;i<length;i++){
	double tRisk = records_calc[i]->getRisk();
	sum+=tRisk;	
	}
	return sum;
}

void
Portfolio::show() {
	printf("Longest: %d Shortest: %d Risky: %.3f Total: %.3f\n",
		lAmt, sAmt, risk, sum);
}

double
Portfolio::cal_amount10_30(double dvo1)
{
	double bucket_risk=0;
	for (unsigned int i=0; i<bucket10_30.size(); i++)
	{
		bucket_risk+=bucket10_30[i]->getRisk();
	}

	return bucket_risk/dvo1;
}

//calculate bucket risk and 2 year hedge amount of a bucket
double
Portfolio::cal_2yr_hedge_amount(vector<BondCalculatorInterface*> bucket, double &bucket_risk, double dv01, double shift)   
{
	bucket_risk=getRisk(&bucket,shift);
	return bucket_risk/dv01;
}

//calculate bucket market value
double 
Portfolio::cal_bucket_mktval(vector<BondCalculatorInterface*> bucket,double shift)
{
	double bucket_mktval = 0;
	for (unsigned int i=0; i<bucket.size();i++)
	{
		bucket_mktval += bucket[i]->getShiftMktVal(shift);	
	}
	return bucket_mktval;
}

//calculate market value of portfolio
double
Portfolio::cal_Market_Value(BondCalculatorInterface** records_calc,int item_count, double shift)
{
	double market_value = 0;
	for (int i=0; i< item_count; i++)
	{
		market_value += records_calc[i]->getShiftMktVal(shift);
		//cout<<i<<": shift:"<<shift<<" shiftedMV:"<<records_calc[i]->getShiftMktVal(shift)<<endl;
	}
	return market_value;
}

double
Portfolio::getTotalLGD()
{
	double total_LGD = 0;
	for(int i=0; i<length; i++) {
		total_LGD += records_calc[i]->getLGD() * records[i].Amount;
	}
	return total_LGD;
}

int
Portfolio::getTotalAmount()
{
	int total_amount=0;
	for (int i=0; i<length; i++)
	{
		total_amount += records[i].Amount;
	}
	return total_amount;
}

int
Portfolio::getAmount(vector<BondCalculatorInterface*>* bucket)
{
	int amt = 0;
	for(unsigned int i=0;i<bucket->size();i++) {
		amt += (*bucket)[i]->getAmount();
	}
	return amt;
}

double
Portfolio::getRisk(vector<BondCalculatorInterface*>* bucket,double shift)
{
    double risk = 0;
	for(unsigned int i=0;i<bucket->size();i++) {
		risk += (*bucket)[i]->getRisk(shift);
	}
	return risk;
}

double
Portfolio::getLGD(vector<BondCalculatorInterface*>* bucket)
{
	double lgd = 0;
	for(unsigned int i=0;i<bucket->size();i++) {
		lgd += (*bucket)[i]->getLGD();
	}
	return lgd;
}

Position*
Portfolio::getPositions(double shift)
{
	Position* p_array = new Position[ticker_size]();

	map<string,vector<BondCalculatorInterface*>*>::iterator it=bucketTicker.begin();
	for(int i=0;i<ticker_size;i++) {
		vector<BondCalculatorInterface*>* bucket = it->second;
		Position *p = &p_array[i];
		p->Ticker = it->first;
		it++;
		p->Amount = getAmount(bucket);
		p->Risk = getRisk(bucket,shift);
		p->LGD = getLGD(bucket);
	}
	return p_array;
}
