#ifndef _TREASURY_RECORDS_H_
#define _TREASURY_RECORDS_H_

#include <ext/hash_map>
#include "SBB_io.h"
#include "SBB_date.h"

class treasury_records {
	private:
		__gnu_cxx::hash_map<int, double> _treasury_map;

	protected:
		int maturity_calc(SBB_date &from_dt_obj, SBB_date &to_dt_obj);

	public:
		treasury_records(SBB_instrument_fields *yc_record, int size);
		double find_closest_treasury(int bond_maturity);
};

#endif

