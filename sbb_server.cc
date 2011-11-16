#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "SBB_util.h"
#include "io.h"
#include "sbb_socket.h"
#include "SBB_date.h"
#include "YieldCurve.h"
#include "bond.h"
#include <vector>

using namespace std;

class MSGHandler{
public:
	static double cal_amount_t2(vector<Expanded_instrument_fields> &bucket,double dv01_2yr )
	{
		double bucket_risk=0;
		for (unsigned int i=0; i<bucket.size(); i++)
		{
			bucket_risk+=bucket[i].Risk;
		}

		cout<<"dv01:"<<dv01_2yr<<endl;
		cout<<"bucket_risk:"<<bucket_risk<<endl;

		return bucket_risk/dv01_2yr*100;
	}

	static double cal_Market_Value(BondCalculatorInterface** records_calc,int item_count, int shift)
	{
		double market_value = 0;
		for (int i=0; i< item_count; i++)
		{
			market_value += records_calc[i]->getShiftMktVal(shift);
			//cout<<i<<": shift:"<<shift<<" shiftedMV:"<<records_calc[i]->getShiftMktVal(shift)<<endl;
		}
		return market_value;
	}
};




int main(int argc, char ** argv)
{
	//timer
	SBB_util timer_obj;
	double real_time;
	double user_time;
	double system_time;
	
	//IO
	char data_input_file[128];
	char yc_input_file[128];
	strcpy(data_input_file, argv[1]);
	strcpy(yc_input_file, argv[2]);
	
	YieldCurve yc(yc_input_file);
	double dv01_2yr = yc.getDV01(2);

	/////////////////////////create long trading book//////////////////////////
	FILE* oFile = fopen (data_input_file, "r");
	FILE* nFile = fopen ("temp.txt", "w");

	InstrumentInputFile dif(data_input_file);
	int _bond_collection_size;
	InstrumentFields* records = dif.get_records(_bond_collection_size);

	BondCalculatorInterface** records_calc1 = new BondCalculatorInterface*[_bond_collection_size];

	for(int i=0;i<_bond_collection_size;i++){

		InstrumentFields* bond_record_ptr = &records[i];
		BondCalculatorInterface* bond_calc_ptr;

		if(0 == bond_record_ptr->CouponRate) 
			bond_calc_ptr = new ZeroCouponBondCalculator( bond_record_ptr, &yc);
		else
			bond_calc_ptr = new CouponBearingBondCalculator( bond_record_ptr, &yc);

		records_calc1[i] = bond_calc_ptr;
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
			records_calc1[count]->getPrice(),
			records_calc1[count]->getDVO1(),
			records_calc1[count]->getRisk(),
			records_calc1[count]->getLGD());

		count++;
	}

	for (int i =0; i< _bond_collection_size; i++)
	{
		delete records_calc1[i];
	}
	delete [] records_calc1;
	dif.free_records();
	fclose(oFile);
	fclose(nFile);

	if(remove(data_input_file) == -1)
		fprintf(stderr,"Could not delete %s\n",data_input_file);
	else 
		rename("temp.txt",data_input_file);	

	/////////////////////////////////create long trading book ends//////////////////////


	Expanded_instrument_input_file tradingbook_input(data_input_file);

	int item_count=0;
	Expanded_instrument_fields * tradingbook_array;
	tradingbook_array = tradingbook_input.get_records(item_count);

	//define 4 buckets
	vector<Expanded_instrument_fields> bucket0_2;
	vector<Expanded_instrument_fields> bucket2_5;
	vector<Expanded_instrument_fields> bucket5_10;
	vector<Expanded_instrument_fields> bucket10_30;

	SBB_date from_dt;
	SBB_date to_dt;
	//trading book collection
	BondCalculatorInterface** records_calc = new BondCalculatorInterface*[item_count];

	for (int i=0;i<item_count;i++)
	{
		BondCalculatorInterface* bond_calc_ptr;
		if(0 == tradingbook_array[i].CouponRate) 
			bond_calc_ptr = new ZeroCouponBondCalculator( &tradingbook_array[i], &yc);
		else
			bond_calc_ptr = new CouponBearingBondCalculator( &tradingbook_array[i], &yc);
		//add to collection
		records_calc[i] = bond_calc_ptr;

		from_dt.set_from_yyyymmdd((long)tradingbook_array[i].SettlementDate);
		to_dt.set_from_yyyymmdd((long)tradingbook_array[i].MaturityDate);
		int T = Term::getNumberOfPeriods(from_dt,to_dt,1);	//here unit of T is year
		if (T>0 && T<=2)
		{
			bucket0_2.push_back(tradingbook_array[i]);
		}
		else if (T>2 && T<=5)
		{
			bucket2_5.push_back(tradingbook_array[i]);
		}
		else if (T>5 && T<=10)
		{
			bucket5_10.push_back(tradingbook_array[i]);
		}
		else if (T>10 && T<=30)
		{
			bucket10_30.push_back(tradingbook_array[i]);
		}
	}

	/* 
	 * get an internet domain socket 
	 */
	int sd;
	//TCP
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {	
		perror("socket");
		exit(1);
	}

	/* 
	 * set up the socket structure 
	 */
	struct sockaddr_in	sock_addr;	

	memset(&sock_addr, 0, sizeof(sock_addr));
	sock_addr.sin_family = AF_INET;

#ifdef SBB_ANY
	/* set to INADDR_ANY if want server to be open to any client on any machine */
	sock_addr.sin_addr.s_addr = INADDR_ANY;
#else
	char	hostname[128];
	/*
	 *  we'll default to this host and call a section 3 func to get this host
	 */
	if( gethostname(hostname,sizeof(hostname)) ){
		fprintf(stderr," SBB gethostname(...) failed errno: %d\n", errno);
		exit(1);
	}
	printf("SBB gethostname() local hostname: \"%s\"\n", hostname);

	/*
	 * set up socket structure for our host machine
	 */
	struct hostent *hp;
	if ((hp = gethostbyname(hostname)) == 0) {
		fprintf(stderr,"SBB gethostbyname(...) failed errno: %d exiting...\n", errno);
		exit(1);
	}
	sock_addr.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr))->s_addr;
#endif
	sock_addr.sin_port = htons(PORT);

	/* 
	 * bind the send socket to the port number 
	 */
	if (bind(sd, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) == -1) {
		perror("bind");
		exit(1);
	}

	/* 
	 * advertise we are available on this socket/port
	 */
	if (listen(sd, 5) == -1) {
		perror("listen");
		exit(1);
	}

	printf("wait for a client begin\n");

	/* 
	 * wait for a client to connect 
	 */
	struct sockaddr_in	sock_addr_from_client;
        socklen_t addrlen = sizeof(sock_addr_from_client); 
	int sd_current;	//returned new socket on which the actual connection is made
	if ((sd_current = accept(sd, (struct sockaddr *)  &sock_addr_from_client, &addrlen)) == -1) {
		fprintf(stderr,"SBB accept(...) failed errno: %d  exiting...\n", errno);
		exit(1);
	}
	printf("SBB client ip address: %s port: %x\n",
		inet_ntoa(sock_addr_from_client.sin_addr),
		PORT);

//		ntohs(sock_addr_from_client.sin_port));

	/*
	 * block on socket waiting for client message
	 */
	int ret = 0;	
        char	msg[MSGSIZE];
	fprintf(stderr," SBB: sizeof msg: %d\n", sizeof(msg));

	//ret contains the number of bytes received
	while (ret = recv(sd_current, msg, sizeof(msg), 0) > 0) {
        	printf("SBB server received msg: [%s] from client\n", msg);

		/* 
		 * ack back to the client 
		 */

		timer_obj.start_clock();
		
		//message handlers
		double hedge_amount = MSGHandler::cal_amount_t2(bucket10_30,dv01_2yr);
		double mktval_orginal = MSGHandler::cal_Market_Value(records_calc,item_count,0);
		double mktval_100bp_up_change = MSGHandler::cal_Market_Value(records_calc,item_count,100) - mktval_orginal;
		double mktval_100bp_dn_change= MSGHandler::cal_Market_Value(records_calc,item_count,-100) - mktval_orginal;
		
		timer_obj.end_clock();
		timer_obj.end_clock(real_time,user_time,system_time);
		
		sprintf(msg,"%.3f %.3f %.3f %.3f %.3f %.3f",
			hedge_amount,mktval_100bp_up_change,mktval_100bp_dn_change,
			real_time,user_time,system_time);

		//strcpy(msg," this is the server message response!");
		if (send(sd_current, msg, strlen(msg), 0) == -1) {
			fprintf(stderr,"SBB send(...) failed errno: %d exiting...\n", errno);
			exit(1);
		}
	}

	if( 0 == ret ) {
		printf("SBB client exited...\n");
		/* For TCP sockets	
		 * the return value 0 means the peer has closed its half side of the connection 
		 */
	}
	else if( -1 == ret ) {
		fprintf(stderr,"SBB recv(...) returned failed - errno: %d exiting...\n", errno);	
		exit(1);
	}
	close(sd_current); 
	close(sd);

	for (int i =0; i< item_count; i++)
	{
		delete records_calc[i];
	}
	delete [] records_calc;
	tradingbook_input.free_records();

	return 0;
}
