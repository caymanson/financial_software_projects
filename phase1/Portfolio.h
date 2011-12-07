#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include <stdio.h>
#include <BondCalculator.h>
#include <SBB_io.h>

class Portfolio
{
public:
	//initialize the colletion_p
	Portfolio(int data_count, SBB_instrument_fields* bonds);

	//deallocate the space in the heap
	~Portfolio();

	//returns the amount of the largest long position
	int largest_long_pos();
	
	//returns the amount of the largest short position
	int largest_short_pos();

	//return the risk of the position with most risk
	double most_risk_pos();

	//return the total risk of the whole portfolio
	double total_risk();

	void set_collection_elem(AbstractBondCalculator * p, int i);

	void show();
	
private:
	//pointer to pointer array, each array element points to a bond
	AbstractBondCalculator ** calculators_p;
	
	//pointer to array of amounts
	SBB_instrument_fields* bond_array;

	int length;
};

#endif