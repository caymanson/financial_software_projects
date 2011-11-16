#include "treasury_records.h"
#include <cmath>

int treasury_records::maturity_calc(
			SBB_date &from_dt_obj,
			SBB_date &to_dt_obj)
{
	int maturity = 0;
	while (from_dt_obj != to_dt_obj) {
		from_dt_obj.add_months(12);
		maturity++;
	}
	return maturity;
}

treasury_records::treasury_records(SBB_instrument_fields *yc_record, int size)
{
	SBB_instrument_fields *yc_record_ptr = NULL;
	SBB_date from_dt_obj;
	SBB_date to_dt_obj;
	int maturity;

	for (int i = 0; i < size; i++) {
		yc_record_ptr = yc_record + i;
		from_dt_obj.set_from_yyyymmdd(yc_record_ptr->SettlementDate);
		to_dt_obj.set_from_yyyymmdd(yc_record_ptr->MaturityDate);
		maturity = maturity_calc(from_dt_obj, to_dt_obj);
		_treasury_map.insert(std::pair<int, double>(maturity, yc_record_ptr->Rate));
	}
}

double treasury_records::find_closest_treasury(int bond_maturity)
{
	__gnu_cxx::hash_map<int, double>::iterator find_it = _treasury_map.find(bond_maturity);
	if (find_it == _treasury_map.end()) {
	// maturity not found, try to find the closet
		int maturity = 10000; // large enough for valid dates
		float diff = 10000.0;
		double rate = 0.0;
		for (__gnu_cxx::hash_map<int, double>::iterator it = _treasury_map.begin(); it != _treasury_map.end(); it++) {
//			printf("%d %f\n", it->first, it->second);
			float temp = abs(it->first - bond_maturity);
			if (temp < diff || temp == diff && it->first < maturity) {
				maturity = it->first;
				diff = temp;
				rate = it->second;
			}
		}
		return rate;
	}
	return find_it->second;
}

