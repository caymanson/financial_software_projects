#ifndef YC_H
#define YC_H

#include "io.h"

// the yield curve object
class YieldCurve {

	public:

		//
		// filename: the yield curve file that is going to be read
		//
		YieldCurve(const char* filename) {
			yif = new InstrumentInputFile(filename);
			mtbs = yif->get_records(length);
		}

		~YieldCurve() {
			delete yif;
			delete[] mtbs;
		}

		//
		// return the Maturity Treasury Bond yield needed
		// term:  the maturity of the bond that is used to match
		// the curve
		//
		double getYield(int term);

		//cal DV01 according to the length of DV01
		double getDV01(int term);

	private:

		//
		// using InstrumentInputFile to read yield curve obj
		//
		InstrumentInputFile* yif;
		InstrumentFields* mtbs;//yc array
		int length;
};

#endif