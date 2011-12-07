#include <stdio.h>
#include "BondCalculator.h"
#include "math.h"
#include "SBB_ratings.h"

//set and return dv01
double AbstractBondCalculator::cal_dv01()
{
	double dv01 = (this->cal_price(-0.01)-this->cal_price(0.01))/2;
	this->DV01 = dv01;
	return dv01;
}

//set and return risk
double AbstractBondCalculator::cal_risk()
{
	double risk = this->cal_dv01()/100*this->BondRecordPtr->Amount;
	this->Risk = risk;
	return risk;
}

//set and return LGD
double AbstractBondCalculator::cal_LGD()
{
	SBB_bond_ratings ratings;
	double lgd = ratings.LGD_given_SnP_Fitch((this->BondRecordPtr)->Quality) * 1000;
	this->LGD = lgd;
	return lgd;
}

void AbstractBondCalculator::show()
{
	printf("Price: %.3f DVO1: %.3f Risk: %.3f LGD: %.3f\n", 
		this->cal_price(), 
		this->cal_dv01(), 
		this->cal_risk(), 
		this->cal_LGD());
}

double ZeroCouponCalculator::cal_price(double shift)
{
	double ytm = this->YTM + shift;
	double face_value = 100.0;
	double yield_adj = (ytm/100)/BondRecordPtr->Frequency;
	double pv_factor = 1/pow(1+yield_adj,this->NumofPeriods);
	double present_value = face_value*pv_factor;

	if (0==shift)
	{
		this->Price = present_value;	//set price
	}
	return present_value;
}

double CouponBearingCalculator::cal_price(double shift)
{
	double ytm = this->YTM + shift;
	double face_value = 100.0;
	//yield adjusted to frequency
	double yield_adj = (ytm/100)/BondRecordPtr->Frequency;
	//cout<<"yield adjusted is: "<<yield_adj<<endl;;

	//M*Cr/F
	double coupon_payment_adj = face_value * (BondRecordPtr->CouponRate/100)/BondRecordPtr->Frequency;
	//cout<<"coupon_payment_adj is: "<<coupon_payment_adj<<endl;

	double pv_factor = 1/pow(1+yield_adj,this->NumofPeriods);
	//cout<<"pv_factor is: "<<pv_factor<<endl;

	double present_value = coupon_payment_adj*(1-pv_factor)/yield_adj + face_value*pv_factor;
	
	if (0==shift)
	{
		this->Price = present_value;
	}
	return present_value;
}