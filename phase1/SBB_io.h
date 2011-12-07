#ifndef SBB_IO_H
#define SBB_IO_H
#include <sys/errno.h>
#include <stdio.h>
#include <stdexcept>
#include <string.h>

#define SBB_LINE_BUFFER_LENGTH 256
#define SBB_INSTRUMENT_ID_LENGTH 32
#define SBB_TICKER_LENGTH 8
#define SBB_RATE_TYPE_LENGTH 16
#define QUALITY_LENGTH 8

// wide open class for loading data per instrument
//# SecurityID Ticker SettlementDate Coupon MaturityDate Frequency RateType Rate Quality Amount
class SBB_instrument_fields {
	public:
		SBB_instrument_fields(){
			SecurityID[0]=0;
			Ticker[0] = 0;
			SettlementDate=0;
			CouponRate=0.0;
			MaturityDate=0;
			Frequency=0;
			RateType[0]=0;
			Rate = 0.0;
			Quality[0]=0;
			Amount = 0;
		} 
		~SBB_instrument_fields() {};

		void show(){
			printf("ID: %s Ticker: %s SettleDate: %d Coupon: %.2f MatDate: %d Frequency: %d RateType: %s Rate: %.3f Quality: %s Amount: %d",
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

		char SecurityID[SBB_INSTRUMENT_ID_LENGTH];
		char Ticker[SBB_TICKER_LENGTH];
		int SettlementDate;
		double CouponRate; //in percentage
		int MaturityDate;
		short Frequency;
		char RateType[SBB_RATE_TYPE_LENGTH];
		double Rate;	//if Yield then YTM if Spread then this is in BPs
		char Quality[8];
		int Amount;
};



// simple class for interfacing with data file that contains instrument desriptive data 
// (and yield)
class SBB_instrument_input_file {

	// Resource (file handle) acquisition is initialization version - "RAII"
	// destructor closes file...
	public:
		SBB_instrument_input_file(const char* filename);
		~SBB_instrument_input_file();

		//
		// once his object is constructed (and the file is open), returns number of items in it
		//
		int get_record_count();
	
		//
		// returns a heap allocated array of file contents
		//
		SBB_instrument_fields* get_records(int& length);
			
		//
		// As a convenience to the caller memory is allocated inside this class
		// Call this to free it
		//
		void free_records();

		int get_line_count();
	private:
		FILE * _file;
		//std::FILE * _file;
		
		char _line_buf[SBB_LINE_BUFFER_LENGTH];

		int line_count;
		SBB_instrument_fields *_fields_array;
	 
		//
		// prevent copying
		// 
		SBB_instrument_input_file (const SBB_instrument_input_file &);
		
		//
		// prevent assignment
		//
		SBB_instrument_input_file & operator= (const SBB_instrument_input_file &);

		//
		// disable the default constructor
		// force construction with filename param only...
		//
		SBB_instrument_input_file();
};


//instrument_fields with 4 additional fields to associate with new tradingbook.txt
class Expanded_instrument_fields: public SBB_instrument_fields{
public:
	Expanded_instrument_fields():SBB_instrument_fields(){
		Price = 0;
		DV01 = 0;
		Risk = 0;
		LGD = 0;
	} 
	~Expanded_instrument_fields() {};

	void show()
	{
		SBB_instrument_fields::show();
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

class Expanded_instrument_input_file: public SBB_instrument_input_file{
public:
	Expanded_instrument_input_file(const char* filename);
	~Expanded_instrument_input_file();

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

	int get_line_count();
private:
	FILE * _file;
	//std::FILE * _file;

	char _line_buf[SBB_LINE_BUFFER_LENGTH];

	int line_count;
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