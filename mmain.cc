#include <stdio.h>
#include "SBB_util.h"
#include "portfolio.h"

#define RESULT_FILE "result.txt"

int main(int argc, char *argv[])
{
        SBB_util timer;
        timer.start_clock();

        char* data_input_file = argv[1];
        char* yc_input_file = argv[2];

	FILE* pFile = fopen (RESULT_FILE,"w");
	FILE* oFile = fopen (data_input_file, "r");
	FILE* nFile = fopen ("temp.txt", "w");

	InstrumentInputFile dif(data_input_file);
	YieldCurve yc(yc_input_file);
	int _bond_collection_size;
	InstrumentFields* records = dif.get_records(_bond_collection_size);
	BondCalculatorInterface** records_calc = new BondCalculatorInterface*[_bond_collection_size];
	
	for(int i=0;i<_bond_collection_size;i++){

		InstrumentFields* bond_record_ptr = &records[i];
		BondCalculatorInterface* bond_calc_ptr;

		if(0 == bond_record_ptr->CouponRate) 
			bond_calc_ptr = new ZeroCouponBondCalculator( bond_record_ptr, &yc);
		else
			bond_calc_ptr = new CouponBearingBondCalculator( bond_record_ptr, &yc);
	
		records_calc[i] = bond_calc_ptr;
	}
	char _line_buf[LINE_BUFFER_LENGTH];
	_line_buf[0]=' ';
	int count = 0;
	while(fgets(_line_buf,LINE_BUFFER_LENGTH,oFile)) {

		if('#' == _line_buf[0]) { 
			fprintf(nFile,_line_buf);
			continue;
		}
		
		fwrite(_line_buf,strlen(_line_buf)-1,1,nFile);
		fprintf(nFile," %.3f %.3f %.3f %.3f\n",
			records_calc[count]->getPrice(),
                        records_calc[count]->getDVO1(),
                        records_calc[count]->getRisk(),
                        records_calc[count]->getLGD());

		count++;
	}
	
	Portfolio portfolio(records, records_calc, _bond_collection_size);
	fprintf (pFile, "%d\n%d\n%.3f\n%.3f\n",
			portfolio.getLongest(),
			portfolio.getShortest(),
			portfolio.getMostRisky(),
			portfolio.getTotalRisk());

	delete[] records_calc;
	dif.free_records();
//	dif.~InstrumentInputFile();
	fclose(oFile);
	fclose(nFile);

	if(remove(data_input_file) == -1)
		fprintf(stderr,"Could not delete %s\n",data_input_file);
	else 
		rename("temp.txt",data_input_file);	
 
	timer.end_clock();
	timer.~SBB_util();

	return 0;
}
