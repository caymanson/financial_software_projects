#include "io.h"
#include <string.h>
#include <stdlib.h>

InstrumentInputFile::InstrumentInputFile(const char * filename)
{
	_file = fopen(filename, "r"); // open read-only
	if (!_file){
		fprintf(stderr,"ERROR couldn't open: %s\n", filename);
		throw std::runtime_error("file open failure");
	}
}

InstrumentInputFile::~InstrumentInputFile() 
{
	if (fclose(_file)) {
		fprintf(stderr,"fclose failed on file %s errno: %d\n", 
			_file, errno);
	}
}

int
InstrumentInputFile::get_record_count()
{
	// not including comments (first char is #) that is...
	// Comments interspersed in data lines is allowed
	int line_count_minus_comments = 0;
	int comment_count = 0;
	while(fgets(_line_buf,LINE_BUFFER_LENGTH,_file)){
		if('#' == _line_buf[0]) {
			comment_count++;
			continue; // skip through comments
		}
		++line_count_minus_comments;
	}

	//printf("comment count in file: %d\n", comment_count);
	rewind(_file);
	//fseek(_file, 0L, SEEK_SET);

	return line_count_minus_comments;
}

void
InstrumentInputFile::free_records()
{
	delete[] _fields_array;
}

InstrumentFields*
InstrumentInputFile::get_records(int& length) 
{
	length = get_record_count();

	_fields_array = new InstrumentFields[length];

	if(NULL == _fields_array){
		fprintf(stderr,"calloc failed - errno: %d\n", errno);
		return _fields_array;
	}
	int comment_count = 0;

	_line_buf[0] = ' ';
	int non_comments_line_count_read = 0; 
	char *token;
	while(fgets(_line_buf,LINE_BUFFER_LENGTH,_file)){

		//printf("SBB CHECK file line: %s\n", _line_buf);

		if('#' == _line_buf[0]) {
			continue; // skip through comments
			comment_count++;
		}

		// the line should be:
		// SecurityID Ticker SettlementDate Coupon MaturityDate Frequency RateType Rate Quality Amount

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
		//printf("Ticker: %s\n", token);
		strcpy(_fields_array[non_comments_line_count_read].Ticker, token);

		// SettlementDate
		token = strtok(NULL," ");
		//printf("SettlementDate: %s\n", token); 
		_fields_array[non_comments_line_count_read].SettlementDate = atoi(token);

		// CouponRate
		token = strtok(NULL," ");
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
		//printf("RateType: %s\n", token);
		strcpy(_fields_array[non_comments_line_count_read].RateType, token);

		// Rate
		token = strtok(NULL," ");
		if(NULL == token ) { 
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf); 
			exit(1); 
		}
		//printf("Rate is:  %s\n", token);
		_fields_array[non_comments_line_count_read].Rate = atof(token);

		// Quality
		token = strtok(NULL," ");
		//printf("Quality: %s\n", token);
		strcpy(_fields_array[non_comments_line_count_read].Quality, token);

		// Amount
		token = strtok(NULL," ");
		//printf("Amount: %s\n", token);
		_fields_array[non_comments_line_count_read].Amount = atoi(token);				

		non_comments_line_count_read++;
	}

	//printf("lines read: %d \n", non_comments_line_count_read);

	return _fields_array;
}

/////////////////////////////Historical file IO///////////////////////////////////
HistoricalInputFile::HistoricalInputFile(const char * filename)
{
	_file = fopen(filename, "r"); // open read-only
	if (!_file){
		fprintf(stderr,"ERROR couldn't open: %s\n", filename);
		throw std::runtime_error("file open failure");
	}
}

HistoricalInputFile::~HistoricalInputFile() 
{
	if (fclose(_file)) {
		fprintf(stderr,"fclose failed on file %s errno: %d\n", 
			_file, errno);
	}
}

int
HistoricalInputFile::get_record_count()
{
	// not including comments (first char is #) that is...
	// Comments interspersed in data lines is allowed
	int line_count_minus_comments = 0;
	int comment_count = 0;
	while(fgets(_line_buf,LINE_BUFFER_LENGTH,_file)){
		if('#' == _line_buf[0]) {
			comment_count++;
			continue; // skip through comments
		}
		++line_count_minus_comments;
	}
	//printf("Comment count in file: %d\n", comment_count);

	rewind(_file);
	//fseek(_file, 0L, SEEK_SET);

	return line_count_minus_comments;
}

void
HistoricalInputFile::free_records()
{
	delete[] _fields_array;
}

HistoricalFields*
HistoricalInputFile::get_records(int& length) 
{
	length = get_record_count();

	_fields_array = new HistoricalFields[length];

	if(NULL == _fields_array){
		fprintf(stderr,"calloc failed - errno: %d\n", errno);
		return _fields_array;
	}
	int comment_count = 0;

	_line_buf[0] = ' ';
	int non_comments_line_count_read = 0; 
	char *token;
	while(fgets(_line_buf,LINE_BUFFER_LENGTH,_file)){

		//printf("CHECK file line: %s\n", _line_buf);

		if('#' == _line_buf[0]) {
			continue; // skip through comments
			comment_count++;
		}

		// Date
		token = strtok(_line_buf," ");
		//printf("Date: %s\n", token); 
		_fields_array[non_comments_line_count_read].Date = atoi(token);

		// RateType
		token = strtok(NULL," ");
		//printf("RateType: %s\n", token);
		strcpy(_fields_array[non_comments_line_count_read].RateType, token);

		// RateValue
		token = strtok(NULL," ");
		if(NULL == token ) { 
			fprintf(stderr,"line parsing failed on buf: %s\n", _line_buf); 
			exit(1); 
		}
		//printf("Rate is:  %s\n", token);
		_fields_array[non_comments_line_count_read].RateValue = atof(token);

		// BmkTicker
		token = strtok(NULL," ");
		//printf("Ticker: %s\n", token);
		strcpy(_fields_array[non_comments_line_count_read].BmkTicker, token);			

		non_comments_line_count_read++;
	}

	//printf("lines read: %d \n", non_comments_line_count_read);

	return _fields_array;
}
