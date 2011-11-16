#ifndef _TRADINGBOOK_COLLECTION_H_
#define _TRADINGBOOK_COLLECTION_H_

#include "SBB_io.h"
#include "SBB_date.h"
#include "SBB_ratings.h"
#include "bond_result.h"

class tradingbook_collection
{
	private:
		static const double bond_par_value = 100.0;
		static const double bond_bips = 0.01;
		static const char result_filename[];
		SBB_instrument_input_file *_bond_file;
		SBB_instrument_input_file *_yc_file;
		SBB_instrument_fields *_bond_records;
		SBB_instrument_fields *_yc_records;
		bond_result *_bond_results;
		int _bond_collection_size;
		int _yc_collection_size;
	protected:
		int num_periods_calc(
		SBB_date &settle_dt_obj,
	 	SBB_date &mat_dt_obj,
	 	int frequency);

		double bond_pv_calc(
		int num_periods,
		int frequency,
	 	double rate,
	 	double coupon = 0.0);

		int find_closest_treasury(SBB_instrument_fields &bond_record);
	public:
		tradingbook_collection(const char *bond_filename, const char *yc_filename);
		~tradingbook_collection();
		void tradingbook_calc();
		void show();
		void print_results();
		void output(const char *filename);
};

#endif

