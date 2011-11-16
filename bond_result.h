#ifndef _BOND_RESULT_H_
#define _BOND_RESULT_H_

class bond_result {
	public:
		double Price;
		double dv01;
		double Risk;
		double LGD;

		bond_result();

		//void set_result(double price, double dv01, double risk, double LGD);
		void print();
		void tostr(char *buf, int length);
};

#endif

