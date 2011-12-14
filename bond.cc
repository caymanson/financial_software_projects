#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "bond.h"
#include "SBB_date.h"
#include "SBB_ratings.h"

AbstractBondCalculatorInterface::AbstractBondCalculatorInterface(InstrumentFields* b, YieldCurve* yc) {
	SBB_date settleDtObj, matDtObj;

	// set bond
	bond = b;
	this->yc = yc;

	// using settle date, maturity date and frequency to get periods
	settleDtObj.set_from_yyyymmdd(bond->SettlementDate);
	matDtObj.set_from_yyyymmdd(bond->MaturityDate);
	period = Term::getNumberOfPeriods(settleDtObj, matDtObj, bond->Frequency);

	if(strcmp(bond->RateType, RATETYPE_SPREAD)==0){
		if(NULL == yc){
			printf("no yield curve input file!!!"); 
			exit(1);
		}
		id = yc->getID(period/bond->Frequency);
	}
	else 
		id = -1;

}

double
AbstractBondCalculatorInterface::getRate() {
	double yield;

	// using yield curve yc if necessary, to get yield
	if(id==-1)
		yield = bond->Rate;
	else {
		yield = yc->getYieldUseID(id) + bond->Rate/100;
	}

	return yield;
}

double
AbstractBondCalculatorInterface::getPrice(){
	return getPrice(getRate());
}

double
AbstractBondCalculatorInterface::getDVO1(double shift){
	double pd, pu;

	pd = getPrice(getRate()+shift-0.01);
	pu = getPrice(getRate()+shift+0.01);

	return (pd-pu)/2;
}

double
AbstractBondCalculatorInterface::getRisk(double shift){
	return getDVO1(shift)*bond->Amount/100;
}

double
AbstractBondCalculatorInterface::getLGD(){
	SBB_bond_ratings br;
	return br.LGD_given_SnP_Fitch(bond->Quality);
}

double
AbstractBondCalculatorInterface::getShiftMktVal(double shift) {
	return bond->Amount*this->getPrice(getRate()+shift);
}

void
AbstractBondCalculatorInterface::show() {
	bond->show();
	printf("Price: %.3f dvo1: %.3f risk: %.3f LGD: %.3f\n",
		getPrice(),
		getDVO1(),
		getRisk(),
		getLGD());
}

double
ZeroCouponBondCalculator::getPrice(double y) {
	return 100 * pow(1+y/100/bond->Frequency, -period);
}

double
CouponBearingBondCalculator::getPrice(double y) {
	double PV =  pow(1+y/100/bond->Frequency, -period);
	return 100*PV + Coupon/bond->Frequency * (1-PV)/(y/100/bond->Frequency);
}

