#ifndef YC_H
#define YC_H

#include "io.h"

// the yield curve object
class YieldCurve {

	public:
		YieldCurve(){}

		//
		// filename: the yield curve file that is going to be read
		//
		YieldCurve(const char* filename) {
			yif = new InstrumentInputFile(filename);
			mtbs = yif->get_records(length);
			shift_arr = new double[length]();
			
		}

		~YieldCurve() {
			delete yif;
			delete[] mtbs;
			delete [] shift_arr;
		}

		//
		// return the Maturity Treasury Bond yield needed
		// term:  the maturity of the bond that is used to match
		// the curve
		//
		double getYield(int term);
		int getID(int term);
		double getYieldUseID(int id);

		//cal DV01 according to the length of DV01
		double getDV01(int term);

		//set shift array
		void set_shift_arr(double s1, double s2, double s3, double s4);

	private:

		void getYieldID(int term, int& idt, double& yield);

		//
		// using InstrumentInputFile to read yield curve obj
		//
		InstrumentInputFile* yif;
		InstrumentFields* mtbs;//yc array
		int length;
		double *shift_arr;
};

#endif
