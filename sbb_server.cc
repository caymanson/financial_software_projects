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
#include "portfolio.h"
#include "bond.h"
#include "vargenerator.h"
#include <vector>

using namespace std;

char * all_calculation(char * msg, YieldCurve &yc, Portfolio &opening_port, Portfolio &closing_port, VarGenerator & openVarGen, VarGenerator &closingVarGen)
{
	double shift,yc_s1,yc_s2,yc_s3,yc_s4;
	char * token;

	token = strtok(msg,",");
	if(NULL == token){
		fprintf(stderr,"cannot parse client msg: %s\n",msg);
		exit(1);
	}
	shift = atof(token);

	token = strtok(NULL,",");
	if(NULL == token){
		fprintf(stderr,"cannot parse client msg: %s\n",msg);
		exit(1);
	}	
	yc_s1 = atof(token);

	token = strtok(NULL,",");
	if(NULL == token){
		fprintf(stderr,"cannot parse client msg: %s\n",msg);
		exit(1);
	}		
	yc_s2 = atof(token);

	token = strtok(NULL,",");
	if(NULL == token){
		fprintf(stderr,"cannot parse client msg: %s\n",msg);
		exit(1);
	}
	yc_s3 = atof(token);

	token = strtok(NULL,",");
	if(NULL == token){
		fprintf(stderr,"cannot parse client msg: %s\n",msg);
		exit(1);
	}
	yc_s4 = atof(token);
	
	//initialize yc to 0, get original statistics
	yc.set_shift_arr(0,0,0,0);

	double dv01_2yr = yc.getDV01(2);
	double yc2,yc5,yc10,yc30;
	yc2 = yc.getYield(1);
	yc5 = yc.getYield(4);
	yc10 = yc.getYield(9);
	yc30 = yc.getYield(29);
	//before shift
	double R2,M2,H2,R5,M5,H5,R10,M10,H10,R30,M30,H30;
	H2 = closing_port.cal_2yr_hedge_amount(closing_port.bucket0_2,R2,dv01_2yr,0);
	M2 = closing_port.cal_bucket_mktval(closing_port.bucket0_2,0);
	H5 = closing_port.cal_2yr_hedge_amount(closing_port.bucket2_5,R5,dv01_2yr,0);
	M5 = closing_port.cal_bucket_mktval(closing_port.bucket2_5,0);
	H10 = closing_port.cal_2yr_hedge_amount(closing_port.bucket5_10,R10,dv01_2yr,0);
	M10 = closing_port.cal_bucket_mktval(closing_port.bucket5_10,0);
	H30 = closing_port.cal_2yr_hedge_amount(closing_port.bucket10_30,R30,dv01_2yr,0);
	M30 = closing_port.cal_bucket_mktval(closing_port.bucket10_30,0);
	
	//set yc
	yc.set_shift_arr(yc_s1,yc_s2,yc_s3,yc_s4);

	//after shift
	dv01_2yr = yc.getDV01(2);
	double _R2,_M2,_H2,_R5,_M5,_H5,_R10,_M10,_H10,_R30,_M30,_H30;
	_H2 = closing_port.cal_2yr_hedge_amount(closing_port.bucket0_2,_R2,dv01_2yr,shift);
	_M2 = closing_port.cal_bucket_mktval(closing_port.bucket0_2,shift);
	_H5 = closing_port.cal_2yr_hedge_amount(closing_port.bucket2_5,_R5,dv01_2yr,shift);
	_M5 = closing_port.cal_bucket_mktval(closing_port.bucket2_5,shift);
	_H10 = closing_port.cal_2yr_hedge_amount(closing_port.bucket5_10,_R10,dv01_2yr,shift);
	_M10 = closing_port.cal_bucket_mktval(closing_port.bucket5_10,shift);
	_H30 = closing_port.cal_2yr_hedge_amount(closing_port.bucket10_30,_R30,dv01_2yr,shift);
	_M30 = closing_port.cal_bucket_mktval(closing_port.bucket10_30,shift);

	char *buffer= new char[8192];
	int n = sprintf(buffer,"closing_position,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f;2yr_hedge,%.3f,%.3f,%.3f,%.3f;shifted_position,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f;shifted_2yr_hedge,%.3f,%.3f,%.3f,%.3f;Yield_Curve,%.3f,%.3f,%.3f,%.3f;",R2,M2,R5,M5,R10,M10,R30,M30,
			H2,H5,H10,H30,
			_R2,_M2,_R5,_M5,_R10,_M10,_R30,_M30,
			_H2,_H5,_H10,_H30,
			yc2,yc5,yc10,yc30	
		);
	char * dailyChgStr = Portfolio::DailyChangeByIssuer(&opening_port,&closing_port,shift);
	strcat(buffer,dailyChgStr);
	

	//printf("buffer before varstr:%s\n",buffer);
	
	char * varStr = VarGenerator::DailyChangeByIssuer(&openVarGen,&closingVarGen,shift);

	//printf("varStr:%s\n",varStr);

	strcat(buffer,varStr);
	delete [] varStr;
	delete [] dailyChgStr;


	return buffer;
}



int main(int argc, char ** argv)
{
	//timer
	SBB_util timer_obj;
	double real_time;
	double user_time;
	double system_time;
	
	//IO files and directory
	char tradingbook_opening[128];
	char tradingbook_closing[128];
	char yc_input_file[128];
	char historical_files_dir[128];
	strcpy(tradingbook_opening, argv[1]);
	strcpy(tradingbook_closing, argv[2]);
	strcpy(yc_input_file, argv[3]);
	strcpy(historical_files_dir, argv[4]); //directory for historical files
	
	//yield curve file
	YieldCurve yc(yc_input_file);

	//tradingbook_closing input 
	InstrumentInputFile closing_file(tradingbook_closing);
	int _bond_collection_size;
	InstrumentFields* records = closing_file.get_records(_bond_collection_size);
	BondCalculatorInterface** records_calc = new BondCalculatorInterface*[_bond_collection_size];
	for(int i=0;i<_bond_collection_size;i++){
		InstrumentFields* bond_record_ptr = &records[i];
		BondCalculatorInterface* bond_calc_ptr;
		if(0 == bond_record_ptr->CouponRate) 
			bond_calc_ptr = new ZeroCouponBondCalculator( bond_record_ptr, &yc);
		else
			bond_calc_ptr = new CouponBearingBondCalculator( bond_record_ptr, &yc);
		records_calc[i] = bond_calc_ptr;
		//!!!!!!!
		//records_calc[i]->show();
	}
	Portfolio closing_port(records, records_calc, _bond_collection_size);
	VarGenerator varCal_c(historical_files_dir,records,records_calc,_bond_collection_size);

	
	//tradingbook_opening input
	InstrumentInputFile opening_file(tradingbook_opening);
	int _bond_collection_size_o;
	InstrumentFields* records_o = opening_file.get_records(_bond_collection_size_o);
	BondCalculatorInterface** records_calc_o = new BondCalculatorInterface*[_bond_collection_size_o];
	for(int i=0;i<_bond_collection_size_o;i++){
		InstrumentFields* bond_record_ptr = &records_o[i];
		BondCalculatorInterface* bond_calc_ptr;
		if(0 == bond_record_ptr->CouponRate) 
			bond_calc_ptr = new ZeroCouponBondCalculator( bond_record_ptr, &yc);
		else
			bond_calc_ptr = new CouponBearingBondCalculator( bond_record_ptr, &yc);
		records_calc_o[i] = bond_calc_ptr;
	}
	Portfolio opening_port(records_o, records_calc_o, _bond_collection_size_o);
	VarGenerator varCal_o(historical_files_dir,records_o,records_calc_o,_bond_collection_size);
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

		char * result_str=all_calculation(msg, yc, opening_port, closing_port, varCal_o,varCal_c);
		//printf("%s\n",result_str);
		
		timer_obj.end_clock(real_time,user_time,system_time);
		
				

		//printf("%s\n",msg);	
		

				
				//strcpy(msg,result_str);
		memset(msg,'\0',sizeof(msg));
		sprintf(msg,"Server_Time,%.3f,%.3f,%.3f,%.3f;%s",real_time,user_time,system_time,user_time+system_time,
				result_str);

		//strcpy(msg," this is the server message response!");
		if (send(sd_current, msg, strlen(msg), 0) == -1) {
			fprintf(stderr,"SBB send(...) failed errno: %d exiting...\n", errno);
			exit(1);
		}

		//delete allocated buffer.
		delete [] result_str;

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

	// free records
	for (int i =0; i< _bond_collection_size; i++)
	{
		delete records_calc[i];
		delete records_calc_o[i];
	}
	delete [] records_calc;
	delete [] records_calc_o;
	opening_file.free_records();
	closing_file.free_records();
	
	


	return 0;
}
