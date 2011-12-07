#include <Portfolio.h>
#include <stdio.h>

Portfolio::Portfolio(int data_count, SBB_instrument_fields* bonds)
{
	length = data_count;
	calculators_p = new AbstractBondCalculator* [data_count];
	bond_array = bonds;
}

Portfolio::~Portfolio()
{
	for (int i =0; i< length; i++ )
	{
		delete calculators_p[i];
	}
	delete [] calculators_p;
}

void Portfolio::set_collection_elem(AbstractBondCalculator * p, int i)
{
	calculators_p[i]=p;
}

int Portfolio::largest_long_pos()
{
	int max_long = bond_array[0].Amount;
	for (int i =0; i< length; i++ )
	{
		if (bond_array[i].Amount>max_long)
		{
			max_long = bond_array[i].Amount;
		}
	}

	return max_long;
}

int Portfolio::largest_short_pos()
{
	int max_short = bond_array[0].Amount;
	for (int i =0; i< length; i++ )
	{
		if (bond_array[i].Amount<max_short)
		{
			max_short = bond_array[i].Amount;
		}
	}

	return max_short;
}

double Portfolio::most_risk_pos()
{
	double max_risk = calculators_p[0]->Risk;
	for (int i=0; i<length; i++)
	{
		if (calculators_p[i]->Risk>max_risk)
		{
			max_risk = calculators_p[i]->Risk;
		}
	}
	
	return max_risk;
}

double Portfolio::total_risk()
{
	double total = 0; 
	for (int i=0; i<length; i++)
	{
		total += calculators_p[i]->Risk;
	} 
	
	return total;
}

void Portfolio::show()
{
	printf(" largest_long: %d\n largest_short: %d\n most_risk: %.3f\n total_risk: %.3f\n", 
		this->largest_long_pos(), 
		this->largest_short_pos(), 
		this->most_risk_pos(), 
		this->total_risk());
}