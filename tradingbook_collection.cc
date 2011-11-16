#include "tradingbook_collection.h"
#include <stdio.h>
#include <cmath>
#include <string.h>
#include "treasury_records.h"
//#include "SBB_util.h"

const char tradingbook_collection::result_filename[] = "result.txt";

int tradingbook_collection::num_periods_calc(
			SBB_date &from_dt_obj,
			SBB_date &to_dt_obj,
			int frequency)
{
	if (12 % frequency != 0) {
					fprintf(stderr, "%d is invalid frenquency\n", frequency);
					return -1;
	}
	int num_periods = 0;
	while (from_dt_obj != to_dt_obj) {
		from_dt_obj.add_months(12 / frequency);
		num_periods++;
	}
	return num_periods;
}

double tradingbook_collection::bond_pv_calc(
					int num_periods,
					int frequency,
					double rate,
					double coupon)
{
	double adjusted_rate = rate / 100 / frequency;
	double adjusted_coupon = coupon / 100 / frequency;
	double pv_factor = 1 / pow(adjusted_rate + 1, num_periods);
//	printf("adjusted rate: %f adjusted_coupon: %f pv factor: %f ",
//						adjusted_rate,
//						adjusted_coupon,
//						pv_factor);
	if (coupon == 0) {
//		printf("\n");
		return bond_par_value * pv_factor;
	}
	double coupon_factor = (1 - pv_factor) /  adjusted_rate;
//	printf("coupon factor: %f\n", coupon_factor);
	return bond_par_value * (adjusted_coupon * coupon_factor + pv_factor);
}

tradingbook_collection::tradingbook_collection(const char *bond_filename, const char *yc_filename)
{
//	SBB_util util;
//	START_TIMER(util);
	_bond_file = new SBB_instrument_input_file(bond_filename);
	_yc_file = new SBB_instrument_input_file(yc_filename);
	_bond_records = _bond_file->get_records(_bond_collection_size);
	_yc_records = _yc_file->get_records(_yc_collection_size);
	_bond_results = new bond_result[_bond_collection_size];
//	END_TIMER(util);
}

tradingbook_collection::~tradingbook_collection()
{
	delete [] _bond_results;
}

void tradingbook_collection::tradingbook_calc()
{
	SBB_instrument_fields *bond_record_ptr = NULL;
	SBB_instrument_fields *yc_record_ptr = NULL;
	bond_result *bond_result_ptr = NULL;
	SBB_date settle_dt_obj;
	SBB_date mat_dt_obj;
	int num_periods;
	SBB_bond_ratings bond_ratings;
	treasury_records t_records(_yc_records, _yc_collection_size);
	double treasury_rate;
	double adjusted_rate;

	int llp = 0; // largest long position
//	int llp_index;
	int lsp = 0; // largest short position
//	int lsp_index;
	double most_risk = 0.0;
//	int most_risk_index;
	double total_risk = 0.0;

	for (int i = 0; i < _bond_collection_size; i++) {
		bond_record_ptr = &_bond_records[i];
		bond_result_ptr = &_bond_results[i];
		settle_dt_obj.set_from_yyyymmdd(bond_record_ptr->SettlementDate);
		mat_dt_obj.set_from_yyyymmdd(bond_record_ptr->MaturityDate);
		num_periods = num_periods_calc(
										settle_dt_obj,
									 	mat_dt_obj,
									 	bond_record_ptr->Frequency);
//		printf("number of periods: %d\n", num_periods);

		if (!strcmp(bond_record_ptr->RateType, "YIELD")) {
			adjusted_rate = bond_record_ptr->Rate;
		} else {
			// must be SPREAD - only two types
			treasury_rate = t_records.find_closest_treasury(num_periods / bond_record_ptr->Frequency);
			adjusted_rate = bond_record_ptr->Rate / 100 + treasury_rate;
//			printf("treasury rate: %f\n", treasury_rate);
		}
		bond_result_ptr->Price = bond_pv_calc(
															num_periods,
														 	bond_record_ptr->Frequency,
															adjusted_rate,
															bond_record_ptr->CouponRate);
		bond_result_ptr->dv01 = (fabs(bond_result_ptr->Price
																	- bond_pv_calc(
																			num_periods,
																			bond_record_ptr->Frequency,
																			adjusted_rate + bond_bips,
																			bond_record_ptr->CouponRate))
													+ fabs(bond_result_ptr->Price
																	- bond_pv_calc(
																			num_periods,
																			bond_record_ptr->Frequency,
																			adjusted_rate - bond_bips,
																			bond_record_ptr->CouponRate))) / 2;
		bond_result_ptr->Risk = bond_record_ptr->Amount / 100
					 								* bond_result_ptr->dv01;
		bond_result_ptr->LGD = bond_record_ptr->Amount
													* bond_ratings.LGD_given_SnP_Fitch(bond_record_ptr->Quality);

		if (bond_record_ptr->Amount > llp) {
			llp = bond_record_ptr->Amount;
//			llp_index = i;
		} else if (bond_record_ptr->Amount < lsp) {
			lsp = bond_record_ptr->Amount;
//			lsp_index = i;
		}
		if (abs(bond_result_ptr->Risk) > abs(most_risk)) {
			most_risk = bond_result_ptr->Risk;
//			most_risk_index = i;
		}
		total_risk += bond_result_ptr->Risk;
	}
	FILE *pFile = fopen(result_filename, "w");
	if (NULL == pFile) {
		fprintf(stderr, "ERROR couldn't open: %s\n", result_filename);
		exit(1);
	}
/*
	fprintf(pFile, "# The largest long position\n%d\n", llp);
	fprintf(pFile, "# The largest short position\n%d\n", lsp);
	fprintf(pFile, "# Position with most risk\n%.3f\n", most_risk);
	fprintf(pFile, "# Total risk\n%.3f\n", total_risk);
*/
	fprintf(pFile, "%d\n%d\n%.3f\n%.3f\n", llp, lsp, most_risk, total_risk);
	fclose(pFile);
}

void tradingbook_collection::show()
{
	SBB_instrument_fields *bond_record_ptr = NULL;
	for (int i = 0; i < _bond_collection_size; i++) {
		bond_record_ptr = &_bond_records[i];
		bond_record_ptr->show();
	}
}

void tradingbook_collection::print_results()
{
	bond_result *bond_result_ptr = NULL;
	for (int i = 0; i < _bond_collection_size; i++) {
		bond_result_ptr = &_bond_results[i];
		printf("%.3f %.3f %.3f %.3f\n",
						bond_result_ptr->Price,
						bond_result_ptr->dv01,
						bond_result_ptr->Risk,
						bond_result_ptr->LGD);
	}
}

void tradingbook_collection::output(const char *filename)
{
	FILE *pFile = fopen(filename, "r+");
	if (NULL == pFile) {
		fprintf(stderr, "ERROR couldn't open: %s\n", filename);
		exit(1);
	}
	SBB_instrument_fields *bond_record_ptr = NULL;
	bond_result *bond_result_ptr = NULL;
	char buf[SBB_LINE_BUFFER_LENGTH];
	// skip comments
	while (fgets(buf, SBB_LINE_BUFFER_LENGTH, pFile)) {
		if (buf[0] != '#') {
			fseek(pFile, -strlen(buf), SEEK_CUR);
			break;
		}
	}
	for (int i = 0; i < _bond_collection_size; i++) {
		bond_record_ptr = &_bond_records[i];
		bond_result_ptr = &_bond_results[i];
		fprintf(pFile, "%s %s %d %.1f %d %d %s %.1f %s %ld %.3f %.3f %.3f %.3f\n",
							bond_record_ptr->SecurityID,
							bond_record_ptr->Ticker,
							bond_record_ptr->SettlementDate,
							bond_record_ptr->CouponRate,
							bond_record_ptr->MaturityDate,
							bond_record_ptr->Frequency,
							bond_record_ptr->RateType,
							bond_record_ptr->Rate,
							bond_record_ptr->Quality,
							bond_record_ptr->Amount,
							bond_result_ptr->Price,
							bond_result_ptr->dv01,
							bond_result_ptr->Risk,
							bond_result_ptr->LGD);
		//fputs(buf, pFile);
	}
	fclose(pFile);
}

