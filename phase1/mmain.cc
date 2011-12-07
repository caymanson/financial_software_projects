#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "SBB_util.h"
#include "SBB_date.h"
#include "SBB_io.h"
#include <math.h>
#include "BondCalculator.h"
#include "Portfolio.h"

using namespace std;



int main (int argc, char** argv)
{
	SBB_util util_obj;
	util_obj.start_clock();
	//	for(int i=0; i<10000000; i++){}
	
	char data_input_file[128];
	char yc_input_file[128];
	strcpy(data_input_file, argv[1]);
	strcpy(yc_input_file, argv[2]);
	
	SBB_date from_dt;
	SBB_date to_dt;

	SBB_instrument_input_file yc_input(yc_input_file);
	int  yc_count;
	SBB_instrument_fields* yc_array;
	yc_array = yc_input.get_records(yc_count);
	


	int *maturity = new int[yc_count]();	//unit is in year

	for(int i=0;i<yc_count;i++)
	{
		from_dt.set_from_yyyymmdd((long)yc_array[i].SettlementDate);
		to_dt.set_from_yyyymmdd((long)yc_array[i].MaturityDate);
		maturity[i] = get_num_of_periods(yc_array[i].Frequency,from_dt,to_dt)/yc_array[i].Frequency;
	}


	SBB_instrument_input_file data_input(data_input_file);
	int data_count;	//number of data records
	SBB_instrument_fields* bond_array;
	bond_array = data_input.get_records(data_count);
	int total_line = data_input.get_line_count();	//number of lines, include # lines
	int comment_num = total_line - data_count;

	int *num_periods = new int[data_count]();
	double *ytm = new double[data_count]();

	Portfolio port(data_count,bond_array);

	//File pointer for read and write
	FILE * p_rw = fopen(data_input_file,"r+");
	if (p_rw == NULL) printf("Error opening file");

	char (*lines)[SBB_LINE_BUFFER_LENGTH] = new char[total_line][SBB_LINE_BUFFER_LENGTH];
	int *len_arr = new int[total_line]; //record the string length of each line
	for (int k=0; k<total_line; k++)
	{
		fgets(lines[k],SBB_LINE_BUFFER_LENGTH,p_rw);
		len_arr[k] = strlen(lines[k]);
		if (k>=comment_num)
			lines[k][len_arr[k]-1] = ' ';	//overwrite \n to be a space
	}

	for(int i=0;i<data_count;i++)
	{
		/*bond_array[i].show();
		cout<<endl;*/

		SBB_instrument_fields* bond_record_ptr = &bond_array[i];


		from_dt.set_from_yyyymmdd((long)bond_record_ptr->SettlementDate);
		to_dt.set_from_yyyymmdd((long)bond_record_ptr->MaturityDate);
		num_periods[i] = get_num_of_periods(bond_record_ptr->Frequency,from_dt,to_dt);

		if (0==strcmp(bond_record_ptr->RateType,"SPREAD"))
		{
			int mat_yrs = num_periods[i]/bond_record_ptr->Frequency;
			int min_index=0;
			int min_dis = abs(mat_yrs-maturity[0]);
			for (int k=0;k<yc_count;k++)
			{
				if (abs(mat_yrs-maturity[k])<min_dis)
				{
					min_dis = abs(mat_yrs-maturity[k]);
					min_index=k;
				}
				else if (abs(mat_yrs-maturity[k])==min_dis && maturity[k]<mat_yrs)
				{
					min_dis = abs(mat_yrs-maturity[k]);
					min_index=k;
				}
			}
			ytm[i] = yc_array[min_index].Rate + bond_record_ptr->Rate/100;
		}
		else
		{
			ytm[i] = bond_record_ptr->Rate;
		}

		AbstractBondCalculator * calculator_p;
		if (0 == bond_record_ptr->CouponRate)
		{
			calculator_p = new ZeroCouponCalculator(
				num_periods[i],
				bond_record_ptr,
				ytm[i]);
		}
		else
		{
			calculator_p = new CouponBearingCalculator(
				num_periods[i],
				bond_record_ptr,
				ytm[i]);
		}

		port.set_collection_elem(calculator_p,i);
		
		char append_str[128];
		sprintf(append_str,"%.3f %.3f %.3f %.3f\n", 
			calculator_p->cal_price(), 
			calculator_p->cal_dv01(), 
			calculator_p->cal_risk(), 
			calculator_p->cal_LGD());
		strcat(lines[comment_num+i],append_str);

		//cout<<"after change string read:"<<lines[comment_num+i]<< "length:"<<strlen(lines[comment_num+i])<<endl;
		//calculator_p->show();
	}
	rewind(p_rw);

	for (int n=0;n<total_line;n++)
	{
		fputs(lines[n],p_rw);
	}

	fclose (p_rw);

	FILE * p_wFile;
	p_wFile = fopen("results.txt","w");
	if (p_wFile==NULL)
	{
		printf("ERROR: create results.txt failed! \n");
	}
	fprintf(p_wFile,"%d\n%d\n%.3f\n%.3f\n", 
		port.largest_long_pos(), 
		port.largest_short_pos(), 
		port.most_risk_pos(), 
		port.total_risk());
	fclose (p_wFile);
	//port.show();

	delete [] lines;
	delete [] maturity;
	delete [] num_periods;
	delete [] ytm;
	yc_input.free_records();
	data_input.free_records();

	util_obj.end_clock();
	return 0;
}

