#include "bond_result.h"
#include "SBB_io.h"

bond_result::bond_result()
	: Price(0.0), dv01(0.0), Risk(0.0), LGD(0.0)
{}

void bond_result::print()
{
	fprintf(stderr, "price: %.3f, dv01: %.3f, risk: %.3f, LGD: %.3f",
						Price,
					 	dv01,
					 	Risk,
					 	LGD);
}

void bond_result::tostr(char *buf, int length)
{
	snprintf(buf, length, "%.3f %.3f %.3f %.3f",
						Price,
					 	dv01,
					 	Risk,
					 	LGD);
}

