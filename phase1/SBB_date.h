#ifndef SBB_DATE_H
#define SBB_DATE_H

#include <stdio.h>
#include <iostream>
#include <string>

#define SBB_FIRST_VALID_YEAR 1900
#define SBB_ERROR -1

enum MonthName {
	NULL_MONTHNAME = 0,
	JANUARY,
	FEBRUARY,
	MARCH,
	APRIL,
	MAY,
	JUNE,
	JULY,
	AUGUST,
	SEPTEMBER,
	OCTOBER,
	NOVEMBER,
	DECEMBER
};

class SBB_date {
	public:
		SBB_date(){
			_month='0';
			_day='0';
			_year=0;
		}
		~SBB_date() {}

		// copy constructor
		SBB_date(const SBB_date& rhs){
			_month = rhs._month;
			_day = rhs._day;
			_year= rhs._year;
		}
		
		// assignment
		SBB_date& operator=(const SBB_date& rhs){
			_month = rhs._month;
			_day = rhs._day;
			_year= rhs._year;
			return *this;
		}

		// equality
		bool operator==(const SBB_date& rhs){
			return _month == rhs._month &&
				_day == rhs._day &&
				_year == rhs._year;	
		}

		// inequality
		bool operator!=(const SBB_date& rhs){
			return _month != rhs._month ||
				_day != rhs._day ||
				_year != rhs._year;	
		}

		void set_from_yyyymmdd(long yyyymmdd);

		long get_yyyymmdd(){ return _year * 10000 + _month * 100 + _day; }

		void show() {
			printf("SBB_date: day: %d month: %d year: %d readable: %d\n",
				_day, _month, _year, get_yyyymmdd() );
		}

		void add_months(int number); // -6 passed to go backwards a semi-annual period
		int is_valid();



	private:
		char _month;
		char _day;
		short _year;
};



int get_num_of_periods(int frequency, SBB_date from_dt, SBB_date to_dt);


#endif