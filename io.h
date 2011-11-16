#ifndef IO_H
#define IO_H
#include <errno.h>
#include <stdio.h>
#include <stdexcept>

#define LINE_BUFFER_LENGTH 256
#define INSTRUMENT_ID_LENGTH 32
#define TICKER_LENGTH 32
#define RATETYPE_LENGTH 32
#define QUALITY_LENGTH 32

// wide open class for loading data per instrument
class InstrumentFields {
	public:
		InstrumentFields(){
			SecurityID[0]=0;
			Ticker[0]=0;
			SettlementDate=0;
			CouponRate=0.0;
			MaturityDate=0;
			Frequency=0;
			RateType[0]=0;
			Rate=0.0;
			Quality[0]=0;
			Amount=0;
		} 
		~InstrumentFields() {};

		void show(){
			printf("ID: %s Ticker: %s SettleDate: %d Coupon: %.2f MatDate: %d Frequency: %d RateType: %s Rate: %.3f Quality: %s Amount: %d\n",
				SecurityID,
				Ticker,
				SettlementDate,
				CouponRate,
				MaturityDate,
				Frequency,
				RateType,
				Rate,
				Quality,
				Amount);
		}

		char SecurityID[INSTRUMENT_ID_LENGTH];
		char Ticker[TICKER_LENGTH];
		int SettlementDate;
		double CouponRate;
		int MaturityDate;
		short Frequency;
		char RateType[RATETYPE_LENGTH];
		double Rate;
		char Quality[QUALITY_LENGTH];
		int Amount;
};

// simple class for interfacing with data file that contains instrument desriptive data 
// (and yield)
class InstrumentInputFile {

	// Resource (file handle) acquisition is initialization version - "RAII"
	// destructor closes file...
	public:
		InstrumentInputFile(const char* filename);
		~InstrumentInputFile();

		//
		// once his object is constructed (and the file is open), returns number of items in it
		//
		int get_record_count();
	
		//
		// returns a heap allocated array of file contents
		//
		InstrumentFields* get_records(int& length);

		//
		// As a convenience to the caller memory is allocated inside this class
		// Call this to free it
		//
		void free_records();

	protected:
		FILE* _file;
		char _line_buf[LINE_BUFFER_LENGTH];
		
	 
	private:
		InstrumentFields *_fields_array;
		//
		// prevent copying
		// 
		InstrumentInputFile (const InstrumentInputFile &);
		
		//
		// prevent assignment
		//
		InstrumentInputFile & operator= (const InstrumentInputFile &);

		//
		// disable the default constructor
		// force construction with filename param only...
		//
		InstrumentInputFile();
};

//instrument_fields with 4 additional fields to associate with new tradingbook.txt
class Expanded_instrument_fields: public InstrumentFields{
public:
	Expanded_instrument_fields():InstrumentFields(){
		Price = 0;
		DV01 = 0;
		Risk = 0;
		LGD = 0;
	} 
	~Expanded_instrument_fields() {};

	void show()
	{
		InstrumentFields::show();
		printf("Price: %.3f DV01: %.3f Risk: %.3f LGD: %.3f",
			Price,
			DV01,
			Risk,
			LGD);

	}
	double Price;
	double DV01;
	double Risk;
	double LGD;
};

class Expanded_instrument_input_file: public InstrumentInputFile{
public:
	Expanded_instrument_input_file(const char* filename):InstrumentInputFile(filename){}
	~Expanded_instrument_input_file(){}

	//
	// once his object is constructed (and the file is open), returns number of items in it
	//
	int get_record_count();

	//
	// returns a heap allocated array of file contents
	//
	Expanded_instrument_fields* get_records(int& length);

	//
	// As a convenience to the caller memory is allocated inside this class
	// Call this to free it
	//
	void free_records();

private:
	Expanded_instrument_fields *_fields_array;
	//
	// prevent copying
	// 
	Expanded_instrument_input_file (const Expanded_instrument_input_file &);

	//
	// prevent assignment
	//
	Expanded_instrument_input_file & operator= (const Expanded_instrument_input_file &);

	//
	// disable the default constructor
	// force construction with filename param only...
	//
	Expanded_instrument_input_file();
};

#endif