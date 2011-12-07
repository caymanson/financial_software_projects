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
#include <vector>

using namespace std;

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

	InstrumentInputFile dif(data_input_file);
	int _bond_collection_size;
	InstrumentFields* records = dif.get_records(_bond_collection_size);

	YieldCurve yc(yc_input_file);
	double dv01_2yr = yc.getDV01(2);

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

	Portfolio portfolio(records, records_calc, _bond_collection_size);

	fclose(oFile);
	//fclose(nFile);

	/*if(remove(data_input_file) == -1)
		fprintf(stderr,"Could not delete %s\n",data_input_file);
	else 
		rename("temp.txt",data_input_file);	
*/

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
		double hedge_amount = portfolio.cal_amount10_30(dv01_2yr);
		double mktval_orginal = portfolio.cal_Market_Value(records_calc,_bond_collection_size,0);
		double mktval_100bp_up_change = portfolio.cal_Market_Value(records_calc,_bond_collection_size,100) - mktval_orginal;
		double mktval_100bp_dn_change= portfolio.cal_Market_Value(records_calc,_bond_collection_size,-100) - mktval_orginal;
		
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

	// free records
	for (int i =0; i< _bond_collection_size; i++)
	{
		delete records_calc[i];
	}
	delete [] records_calc;
	dif.free_records();

	return 0;
}
