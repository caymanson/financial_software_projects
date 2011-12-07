#ifndef BOND_H
#define BOND_H
#include "YieldCurve.h"

#define RATETYPE_YIELD "YIELD"
#define RATETYPE_SPREAD "SPREAD"

// skeleton interface for a bond calculator
class BondCalculatorInterface {

	public:
		//
		// return price of the bond
		//
		virtual double getPrice()=0;

		//
		// return dvo1 of the bond
		//
		virtual double getDVO1()=0;

		//
		// return risk of the bond
		//
		virtual double getRisk()=0;

		//
		// return Loss Given Default of the bond
		//
		virtual double getLGD()=0;

		//
		// return shifted price
		//
		virtual double getShiftMktVal(int shift)=0;

		//
		// print the bond pricing info to stdout
		//
		virtual void show()=0;
};

// Skeleton implemention of BondCalculatorInterface
// Implement the interfaces that both Zero_Coupon_Bond and Coupon_Bearing_Bond shares
class AbstractBondCalculatorInterface : public BondCalculatorInterface {

	public:

		//
		// bond: is the InstrumentFields to calculate
		// yc  : is the yield curve that is going to use if the rate type is
		// SPREAD
		//
		AbstractBondCalculatorInterface(InstrumentFields* bond, YieldCurve* yc=NULL);
		double getPrice();
		double getDVO1();
		double getRisk();
		double getLGD();
		double getShiftMktVal(int shift);
		void show();
	protected:
		
		//
		// a simple method that makes getPrice() easier and make getDVO1() 
		// available now
		//
		virtual double getPrice(double y)=0;

		//
		// the instrument to price with
		//
		InstrumentFields *bond;

		//
		// total number of periods
		//
		int period;

		//
		// the yield rate the bond has
		//
		double yield;
};

// A simple implementation of Zero_Coupon_Bond
class ZeroCouponBondCalculator : public AbstractBondCalculatorInterface {
	public:
		ZeroCouponBondCalculator(InstrumentFields* bond, YieldCurve* yc=NULL) : AbstractBondCalculatorInterface(bond, yc){}
	protected:
		double getPrice(double y);
};

// A simple implementation of Coupon_Bearing_Bond
class CouponBearingBondCalculator : public AbstractBondCalculatorInterface {
	public:
		CouponBearingBondCalculator(InstrumentFields* bond, YieldCurve* yc=NULL) : AbstractBondCalculatorInterface(bond, yc){
			Coupon = bond->CouponRate;
		}
	protected:
		double getPrice(double y);
		
		//
		// Coupon_Bearing_Bond has a coupon rate
		//
		double Coupon;
};

#endif