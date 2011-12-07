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

	// using settle date, maturity date and frequency to get periods
	settleDtObj.set_from_yyyymmdd(bond->SettlementDate);
	matDtObj.set_from_yyyymmdd(bond->MaturityDate);
	period = Term::getNumberOfPeriods(settleDtObj, matDtObj, bond->Frequency);

	// using yield curve yc if necessary, to get yield
	if(!strcmp(bond->RateType, RATETYPE_YIELD))
		yield = bond->Rate;
	else {
		if(NULL == yc){
			printf("no yield curve input file!!!"); 
			exit(1);
		}
		yield = yc->getYield(period/bond->Frequency) + bond->Rate/100;
	}
}

double
AbstractBondCalculatorInterface::getPrice(){
	return getPrice(yield);
}

double
AbstractBondCalculatorInterface::getDVO1(){
	double pd, pu;

	pd = getPrice(yield-0.01);
	pu = getPrice(yield+0.01);

	return (pd-pu)/2;
}

double
AbstractBondCalculatorInterface::getRisk(){
	return getDVO1()*bond->Amount/100;
}

double
AbstractBondCalculatorInterface::getLGD(){
	SBB_bond_ratings br;
	return br.LGD_given_SnP_Fitch(bond->Quality);
}

double
AbstractBondCalculatorInterface::getShiftMktVal(int shift) {
	return bond->Amount*this->getPrice(yield+shift/100);
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

