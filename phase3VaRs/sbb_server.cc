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
	}
	Portfolio closing_port(records, records_calc, _bond_collection_size);

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
		
		VarGenerator varCal(historical_files_dir,records,records_calc,_bond_collection_size);
		
		double total_var, cs_var, ir_var;
		varCal.getVaR(total_var,cs_var,ir_var);

		timer_obj.end_clock();
		timer_obj.end_clock(real_time,user_time,system_time);
		
		sprintf(msg,"%.3f %.3f %.3f %.3f %.3f %.3f",
			total_var,cs_var,ir_var,
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
