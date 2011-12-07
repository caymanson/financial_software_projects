#ifndef BOND_CALCULATOR_H
#define BOND_CALCULATOR_H

#include <stdio.h>
#include <SBB_io.h>

class AbstractBondCalculator	//abstract class
{
public:
	AbstractBondCalculator(int num_periods, SBB_instrument_fields* bond_ptr, double ytm): 
	  Price(0.0), DV01(0.0), Risk(0.0), LGD(0.0), NumofPeriods(num_periods), BondRecordPtr(bond_ptr), YTM(ytm){};
	virtual ~AbstractBondCalculator(){};

	virtual double cal_price(double shift = 0.0)=0;	//different between zero coupon and coupon bearing

	double cal_dv01();	//[P(-1bp)-P(+1bp)]/2
	double cal_risk();	//dv01/100 * amount
	double cal_LGD();	//LGD_given_SnP_Fitch
	void show();

	double Price;
	double DV01;
	double Risk;
	double LGD;
protected:
	int NumofPeriods;
	SBB_instrument_fields * BondRecordPtr;
	double YTM;	//unit is percentage
};

class ZeroCouponCalculator: public AbstractBondCalculator
{
public:
	ZeroCouponCalculator(int num_periods, SBB_instrument_fields * bond_ptr, double ytm):AbstractBondCalculator(num_periods,bond_ptr,ytm){}
	double cal_price(double shift = 0.0);
};

class CouponBearingCalculator: public AbstractBondCalculator
{
public:
	CouponBearingCalculator(int num_periods, SBB_instrument_fields * bond_ptr, double ytm): AbstractBondCalculator(num_periods,bond_ptr,ytm){}
	double cal_price(double shift = 0.0);
};

#endif