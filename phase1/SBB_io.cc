#include "SBB_io.h"
#include <cstdlib>
#include <errno.h>

SBB_instrument_input_file::SBB_instrument_input_file(const char * filename)
{
	_file = fopen(filename, "r"); // open read-only
	if (!_file){
		fprintf(stderr,"ERROR couldn't open: %s\n", filename);
		throw std::runtime_error("file open failure");
	}
	line_count =0;
}

SBB_instrument_input_file::~SBB_instrument_input_file() 
{
	if (fclose(_file)) {
		fprintf(stderr,"fclose failed on file %s errno: %d\n", 
			_file, errno);
	}
}

int SBB_instrument_input_file::get_record_count()
{
	// not including comments (first char is #) that is...
	// Comments interspersed in data lines is allowed
	int line_count_minus_comments = 0;
	int comment_count = 0;
	
	//read a new line, or until maxnum of characters, whichever comes first
	while(fgets(_line_buf,SBB_LINE_BUFFER_LENGTH,_file)){		
		if('#' == _line_buf[0]) {
			comment_count++;
			continue; // skip through comments
		}
		++line_count_minus_comments;
	}
	printf("comment count in file: %d\n", comment_count);

	//Set position indicator to the beginning
	rewind(_file);
	//fseek(_file, 0L, SEEK_SET);
	line_count = line_count_minus_comments+comment_count;

	return line_count_minus_comments;
}

int SBB_instrument_input_file::get_line_count()
{
	return line_count;
}

void SBB_instrument_input_file::free_records()
{
	delete[] _fields_array;
}

SBB_instrument_fields* SBB_instrument_input_file::get_records(int& length) 
{
	length = get_record_count();

	_fields_array = new SBB_instrument_fields[length];

	if(NULL == _fields_array){
		fprintf(stderr,"calloc failed - errno: %d\n", errno);
		return _fields_array;
	}
	int comment_count = 0;

	_line_buf[0] = ' ';
	int non_comments_line_count_read = 0; 
	char *token;
	while(fgets(_line_buf,SBB_LINE_BUFFER_LENGTH,_file)){

		//printf("SBB CHECK file line: %s\n", _line_buf);

		if('#' == _line_buf[0]) {
			continue; // skip through comments
			comment_count++;
		}

		// the line should be:
//# SecurityID Ticker SettlementDate Coupon MaturityDate Frequency RateType Rate Quality Amount

		// SecurityID
		token = strtok(_line_buf," ");
		if(NULL == token) { 
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf); 
			exit(1); 
		} 
		//printf("SecurityID: %s\n", token);
		strcpy(_fields_array[non_comments_line_count_read].SecurityID, token);

		// Ticker
		token = strtok(NULL," ");
		if(NULL == token) { 
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf); 
			exit(1); 
		} 
		//printf("Ticker: %s\n", token);
		strcpy(_fields_array[non_comments_line_count_read].Ticker, token);

		// SettlementDate
		token = strtok(NULL," ");
		if(NULL == token) { 
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf); 
			exit(1); 
		}
		//printf("SettlementDate: %s\n", token); 
		_fields_array[non_comments_line_count_read].SettlementDate = atoi(token);

		// CouponRate
		token = strtok(NULL," ");
		if(NULL == token ) { 
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf); 
			exit(1); 
		}
		//printf("CouponRate: %s\n", token);
		_fields_array[non_comments_line_count_read].CouponRate = atof(token);

		// MaturityDate
		token = strtok(NULL," ");
		//printf("MaturityDate: %s\n", token);
		_fields_array[non_comments_line_count_read].MaturityDate = atoi(token);				

		// Frequency
		token = strtok(NULL," ");
		//printf("Frequency: %s\n", token);
		_fields_array[non_comments_line_count_read].Frequency = atoi(token);				

		// RateType
		token = strtok(NULL," ");
		if(NULL == token) { 
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf); 
			exit(1); 
		} 
		//printf("RateType: %s\n", token);
		strcpy(_fields_array[non_comments_line_count_read].RateType, token);

		// Rate
		token = strtok(NULL," ");
		if(NULL == token) { 
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf); 
			exit(1); 
		} 
		//printf("Rate: %s\n", token);
		_fields_array[non_comments_line_count_read].Rate = atof(token);

		// Quality
		token = strtok(NULL," ");
		if(NULL == token) { 
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf); 
			exit(1); 
		} 
		//printf("Quality: %s\n", token);
		strcpy(_fields_array[non_comments_line_count_read].Quality, token);

		// Amount
		token = strtok(NULL," ");
		if(NULL == token) { 
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf); 
			exit(1); 
		} 
		//printf("Amount: %s\n", token);
		_fields_array[non_comments_line_count_read].Amount = atoi(token);

		//// Yield (later we will load in spread and a string token of an OTR. Also will have a separate file

		//token = strtok(NULL," ");
		//if(NULL == token ) { 
		//	fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf); 
		//	exit(1); 
		//}
		////printf("YieldRate is:  %s\n", token);
		//_fields_array[non_comments_line_count_read].YieldRate = atof(token);

		non_comments_line_count_read++;
	}

	//printf("SBB lines read: %d \n", non_comments_line_count_read);

	return _fields_array;
}



//////////////////////////////////////////////////////////////////
Expanded_instrument_input_file::Expanded_instrument_input_file(const char * filename)
{
	_file = fopen(filename, "r"); // open read-only
	if (!_file){
		fprintf(stderr,"ERROR couldn't open: %s\n", filename);
		throw std::runtime_error("file open failure");
	}
	line_count =0;
}

Expanded_instrument_input_file::~Expanded_instrument_input_file() 
{
	if (fclose(_file)) {
		fprintf(stderr,"fclose failed on file %s errno: %d\n", 
			_file, errno);
	}
}

int Expanded_instrument_input_file::get_record_count()
{
	// not including comments (first char is #) that is...
	// Comments interspersed in data lines is allowed
	int line_count_minus_comments = 0;
	int comment_count = 0;

	//read a new line, or until maxnum of characters, whichever comes first
	while(fgets(_line_buf,SBB_LINE_BUFFER_LENGTH,_file)){		
		if('#' == _line_buf[0]) {
			comment_count++;
			continue; // skip through comments
		}
		++line_count_minus_comments;
	}
	printf("comment count in file: %d\n", comment_count);

	//Set position indicator to the beginning
	rewind(_file);
	//fseek(_file, 0L, SEEK_SET);
	line_count = line_count_minus_comments+comment_count;

	return line_count_minus_comments;
}

int Expanded_instrument_input_file::get_line_count()
{
	return line_count;
}

void Expanded_instrument_input_file::free_records()
{
	delete[] _fields_array;
}

Expanded_instrument_fields* Expanded_instrument_input_file::get_records(int& length) 
{
	length = get_record_count();

	_fields_array = new Expanded_instrument_fields[length];

	if(NULL == _fields_array){
		fprintf(stderr,"calloc failed - errno: %d\n", errno);
		return _fields_array;
	}
	int comment_count = 0;

	_line_buf[0] = ' ';
	int non_comments_line_count_read = 0; 
	char *token;
	while(fgets(_line_buf,SBB_LINE_BUFFER_LENGTH,_file)){

		//printf("SBB CHECK file line: %s\n", _line_buf);

		if('#' == _line_buf[0]) {
			continue; // skip through comments
			comment_count++;
		}

		// the line should be:
		//# SecurityID Ticker SettlementDate Coupon MaturityDate
		//Frequency RateType Rate Quality Amount Price DV01 Risk LGD

		// SecurityID
		token = strtok(_line_buf," ");
		if(NULL == token) { 
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf); 
			exit(1); 
		} 
		//printf("SecurityID: %s\n", token);
		strcpy(_fields_array[non_comments_line_count_read].SecurityID, token);

		// Ticker
		token = strtok(NULL," ");
		if(NULL == token) { 
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf); 
			exit(1); 
		} 
		//printf("Ticker: %s\n", token);
		strcpy(_fields_array[non_comments_line_count_read].Ticker, token);

		// SettlementDate
		token = strtok(NULL," ");
		if(NULL == token) { 
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf); 
			exit(1); 
		}
		//printf("SettlementDate: %s\n", token); 
		_fields_array[non_comments_line_count_read].SettlementDate = atoi(token);

		// CouponRate
		token = strtok(NULL," ");
		if(NULL == token ) { 
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf); 
			exit(1); 
		}
		//printf("CouponRate: %s\n", token);
		_fields_array[non_comments_line_count_read].CouponRate = atof(token);

		// MaturityDate
		token = strtok(NULL," ");
		//printf("MaturityDate: %s\n", token);
		_fields_array[non_comments_line_count_read].MaturityDate = atoi(token);				

		// Frequency
		token = strtok(NULL," ");
		//printf("Frequency: %s\n", token);
		_fields_array[non_comments_line_count_read].Frequency = atoi(token);				

		// RateType
		token = strtok(NULL," ");
		if(NULL == token) { 
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf); 
			exit(1); 
		} 
		//printf("RateType: %s\n", token);
		strcpy(_fields_array[non_comments_line_count_read].RateType, token);

		// Rate
		token = strtok(NULL," ");
		if(NULL == token) { 
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf); 
			exit(1); 
		} 
		//printf("Rate: %s\n", token);
		_fields_array[non_comments_line_count_read].Rate = atof(token);

		// Quality
		token = strtok(NULL," ");
		if(NULL == token) { 
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf); 
			exit(1); 
		} 
		//printf("Quality: %s\n", token);
		strcpy(_fields_array[non_comments_line_count_read].Quality, token);

		// Amount
		token = strtok(NULL," ");
		if(NULL == token) { 
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf); 
			exit(1); 
		} 
		//printf("Amount: %s\n", token);
		_fields_array[non_comments_line_count_read].Amount = atoi(token);

		// Price
		token = strtok(NULL," ");
		if(NULL == token) { 
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf); 
			exit(1); 
		} 
		//printf("Price: %s\n", token);
		_fields_array[non_comments_line_count_read].Price = atof(token);

		// DV01
		token = strtok(NULL," ");
		if(NULL == token) { 
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf); 
			exit(1); 
		} 
		//printf("DV01: %s\n", token);
		_fields_array[non_comments_line_count_read].DV01 = atof(token);

		// Risk
		token = strtok(NULL," ");
		if(NULL == token) { 
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf); 
			exit(1); 
		} 
		//printf("Risk: %s\n", token);
		_fields_array[non_comments_line_count_read].Risk = atof(token);

		// LGD
		token = strtok(NULL," ");
		if(NULL == token) { 
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf); 
			exit(1); 
		} 
		//printf("LGD: %s\n", token);
		_fields_array[non_comments_line_count_read].LGD = atof(token);

		non_comments_line_count_read++;
	}

	//printf("SBB lines read: %d \n", non_comments_line_count_read);

	return _fields_array;
}