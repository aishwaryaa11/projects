/*
 * CS3600, Fall 2013
 * Project 3 Starter Code
 * (c) 2013 Alan Mislove
 *
 */

#include <errno.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "3600dns.h"

/**
 * This function will print a hex dump of the provided packet to the screen
 * to help facilitate debugging.  In your milestone and final submission, you 
 * MUST call dump_packet() with your packet right before calling sendto().  
 * You're welcome to use it at other times to help debug, but please comment those
 * out in your submissions.
 *
 * DO NOT MODIFY THIS FUNCTION
 *
 * data - The pointer to your packet buffer
 * size - The length of your packet
 */
static void dump_packet(unsigned char *data, int size) {
	unsigned char *p = data;
	unsigned char c;
	int n;
	char bytestr[4] = {0};
	char addrstr[10] = {0};
	char hexstr[ 16*3 + 5] = {0};
	char charstr[16*1 + 5] = {0};
	for(n=1;n<=size;n++) {
		if (n%16 == 1) {
			/* store address for this line */
			snprintf(addrstr, sizeof(addrstr), "%.4x",
					((unsigned int)p-(unsigned int)data) );
		}

		c = *p;
		if (isprint(c) == 0) {
			c = '.';
		}

		/* store hex str (for left side) */
		snprintf(bytestr, sizeof(bytestr), "%02X ", *p);
		strncat(hexstr, bytestr, sizeof(hexstr)-strlen(hexstr)-1);

		/* store char str (for right side) */
		snprintf(bytestr, sizeof(bytestr), "%c", c);
		strncat(charstr, bytestr, sizeof(charstr)-strlen(charstr)-1);

		if(n%16 == 0) { 
			/* line completed */
			printf("[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
			hexstr[0] = 0;
			charstr[0] = 0;
		} else if(n%8 == 0) {
			/* half line: add whitespaces */
			strncat(hexstr, "  ", sizeof(hexstr)-strlen(hexstr)-1);
			strncat(charstr, " ", sizeof(charstr)-strlen(charstr)-1);
		}
		p++; /* next byte */
	}

	if (strlen(hexstr) > 0) {
		/* print rest of buffer if not empty */
		printf("[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
	}
}

unsigned char* parse_name(unsigned char* url){
	int len = strlen(url);
	int end=0;
	int start = 0;
	unsigned char *dup = url;
	unsigned char *temp;
	unsigned char *temp2 = malloc(sizeof(char) *(len + 1));
	char buff[16];
	for(int i = 0; i < len; i++){
		if(*url != '.'){
			end = i;
		}else{
			end++;
			snprintf(buff, 16,"%c",(end - start));
			strcat(temp2, buff);
			temp = malloc(sizeof(char)*(end-start));
			strncpy(temp, dup, (end-start));
			strcat(temp2, temp);
			dup = dup + (end - start)+1;
			start= i+1; 
			end = 0;
		}
		url++;
	}
	end++;
	snprintf(buff, 16,"%c",(end - start));
	temp = malloc(sizeof(char)*(end-start));
	strcat(temp2, buff);
	strncpy(temp, dup, (end-start));
	strcat(temp2, temp);
	return temp2;
}

int unpack_request(unsigned char *name, unsigned char *buffer, int start){

	unsigned char a = buffer[start];
	int posn = start + 1;
	int bwrite = 0; //number of bytes written
	int final = 0;
	while (a != 0) {
		if (a & 192) {
			a = buffer[posn];
			if (!final) {
				final = posn+1;
			}
			posn = a;
		} else {
			for (int i = 0; i < a; i++) {
				name[bwrite] = buffer[posn];
				posn++;
				bwrite++;
			}
			name[bwrite] = '.';
			bwrite++;
		}
		a = buffer[posn];
		posn++;
	}

	if (name[bwrite - 1] == '.') {
		bwrite--;
	}
	name[bwrite] = 0;
	if (!final) {
		final = posn;
	}
	return final-start;

}


int ip_format(unsigned char* pkt, unsigned char* rdata, int offset ) {
	offset= offset+2;
	unsigned char tmp = pkt[offset];
	int posn = offset + 1;
	unsigned char seg[4];
	for (int i = 0; i < 4; i++)
	{
		//fprintf(stderr, " value of a in ip_format %d\n", a);
		seg[i] = tmp;
		//fprintf(stderr, " value of segment %d\n" , segments[i]);
		tmp = pkt[posn];
		posn++;
	}
	sprintf((char*)rdata,"%d.%d.%d.%d",seg[0],seg[1],seg[2],seg[3]);
	return 4;
}

int main(int argc, char *argv[]) {
	/**
	 * I've included some basic code for opening a socket in C, sending
	 * a UDP packet, and then receiving a response (or timeout).  You'll 
	 * need to fill in many of the details, but this should be enough to
	 * get you started.
	 */
	short server_port =0;
	unsigned char *ip_add;
	unsigned char buf[65536];
	unsigned int packet_length = 0;
	unsigned char *server_name;
	unsigned char *server_tmp;
	unsigned char* server_ip;
	char serv_type = 1;
	// process the arguments
	if(*argv[1] == '@'){
		ip_add = argv[1];
		ip_add++;
		server_tmp = argv[2];
	}else if(argc == 4){
		if (!strcmp(argv[1],"-mx")){
			serv_type = 15;
		} else if (!strcmp(argv[1],"-ns")){
			serv_type = 2;
		} else {
			return -1;
		}
	if(*argv[2] == '@'){
		ip_add = argv[2];
		ip_add++;
		server_tmp = argv[3];
	}
	}
	if(strchr(ip_add, ':') != NULL){
		server_ip = strtok(ip_add, ":");
		unsigned char *server_port_tmp = strtok(NULL, " ");
		server_port = (short) strtol(server_port_tmp, NULL, 0);
	}else{
		server_ip = strtok(ip_add, " ");
		server_port = 53;
	}

	server_name = parse_name(server_tmp);

	// construct the DNS request
	header h;
	h.ID = htons(1337);
	h.RD = 1;
	h.TC = 0;
	h.AA = 0;
	h.OPCODE = 0;
	h.QR = 0;
	h.RCODE = 0;
	h.RA = 0;
	h.Z = 0;
	h.QDCOUNT = htons(1);
	h.ANCOUNT = htons(0);
	h.NSCOUNT = htons(0);
	h.ARCOUNT = htons(0);

	question q;
	switch (serv_type){
		case 1: 
			q.QTYPE = htons(0x0001);
			break;
		case 15:
			q.QTYPE = htons(15);
			break;
		case 2:
			q.QTYPE = htons(2);
			break;
	}
	//q.QTYPE = htons(1);
	q.QCLASS = htons(1);

	memcpy(&buf,&h,sizeof(header));

	memcpy(&buf[sizeof(header)],server_name,strlen(server_name));

	memcpy(&buf[sizeof(header)+strlen(server_name)+1],&q,sizeof(question));

	packet_length = sizeof(header)+strlen(server_name)+1+sizeof(question);

	// send the DNS request (and call dump_packet with your request)
	dump_packet(buf, packet_length);  

	//fprintf(stderr, " buf before sending!!!!!!!! %s\n", buf);
	// first, open a UDP socket  
	int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// next, construct the destination address
	struct sockaddr_in out;
	out.sin_family = AF_INET;
	out.sin_port = htons(server_port);
	out.sin_addr.s_addr = inet_addr(server_ip);

	if (sendto(sock, buf, packet_length, 0, &out, sizeof(out)) < 0) {
		// an error occurred
		fprintf(stderr,"Error occured while sending the packet %s\n", strerror(errno));
	}

	// wait for the DNS reply (timeout: 5 seconds)
	struct sockaddr_in in;
	socklen_t in_len;

	// construct the socket set
	fd_set socks;
	FD_ZERO(&socks);
	FD_SET(sock, &socks);

	// construct the timeout
	struct timeval t;
	t.tv_sec = 5;
	t.tv_usec = 0;

	memset(buf, 0, 65536);
	//fprintf(stderr, " print buf after 0s %s\n", buf);
	// wait to receive, or for a timeout
	if (select(sock + 1, &socks, NULL, NULL, &t)) {
		if (recvfrom(sock, buf, 65536, 0, &in, &in_len) < 0) {
			// an error occured
			fprintf(stderr, "Error occured while receiving the packet\n");
		}
	} else {
		// a timeout occurred
		//fprintf(stderr,"Timeout occured when receiving packet\n");
		printf("NORESPONSE");
		return -1;
	}
	int size = 0;

	// print out the result

	// create a header and grab header from the buffer
	// use nhtons() for this
	header resp_h;
	memcpy(&resp_h, &buf, sizeof(resp_h));
	int numAns = ntohs(resp_h.ANCOUNT);
	size = sizeof(resp_h);
	if ( ntohs(resp_h.ID) != 1337 || 
             resp_h.QR != 1 || 
             resp_h.RD != 1 || 
             resp_h.RA != 1 ) {
            printf("ERROR: Header does not match\n");
            return 1;
        }


	// have a helper function to put out the labels from buffer and store them in the qname
	unsigned char * resp_qname = (char *) malloc (strlen(server_name)+2);
	int added = unpack_request(resp_qname, buf, sizeof(resp_h));   
	//fprintf(stderr, "resp_qname %s\n", resp_qname);
	if (!strcmp(argv[2],resp_qname+1)) {
            printf("ERROR: qname mismatch '%s'\n",resp_qname);
            return -1;
        }
	size = size + added;

	// create a question and grab question fromt the buffer
	question resp_q;
	memcpy(&resp_q, &buf[size], sizeof(question));
	if ( (ntohs(resp_q.QTYPE) != 1 && 
             ntohs(resp_q.QTYPE) != 15 && 
             ntohs(resp_q.QTYPE) != 2) || 
             ntohs(resp_q.QCLASS) != (1) ) {
             printf("ERROR: question mismatch\n");
            return -1;
        }
	size = size + sizeof(resp_q);

	if(numAns == 0){
		printf("NOTFOUND\n");
		return -1;
	}

	while(numAns > 0){

		unsigned char * resp_aname = calloc(150, sizeof(char));
		added = unpack_request(resp_aname, buf, size);   
                if (!strcmp(argv[2],resp_aname+1)) {
                    printf("ERROR: answer qname mismatch '%s'\n",resp_aname);
                    return -1;
                }
		size = size + added;
		answer resp_a;
		memcpy(&resp_a, &buf[size], sizeof(resp_a));
                if ((ntohs(resp_a.TYPE) != 1 && 
                     ntohs(resp_a.TYPE) != 5 && 
                     ntohs(resp_a.TYPE) != 15 && 
                     ntohs(resp_a.TYPE) != 2) ||  
                    ntohs(resp_a.CLASS) != 1 ) {
                    printf("NOTFOUND\n");
                    return 1;
                }
		size = size + sizeof(resp_a);
		short offset = 0;
		unsigned char* rdata = calloc(150,sizeof(char));
		if ( ntohs(resp_a.TYPE) == 1 ) {
			ip_format(buf,rdata,size);
			printf("IP\t%s",rdata);
			size = size + ntohs(resp_a.RDLENGTH);
		} else if ( ntohs(resp_a.TYPE) == 5 ) {
			added = unpack_request(rdata, buf,size+2);
			printf("CNAME\t%s",rdata);
			size = size + added+2;
		} else if ( ntohs(resp_a.TYPE) == 2 ) {
			added = unpack_request(rdata, buf ,size+2);
			printf("NS\t%s",rdata);
			size =size+ added+2;
		} else if ( ntohs(resp_a.TYPE) == 15 ) {
			memcpy(&offset,&buf[size+2],sizeof(short));
			size =size + sizeof(offset);  
			offset = ntohs(offset); 
			added = unpack_request(rdata, buf,size+2);
			printf("MX\t%s\t%d",rdata,offset);
			size =size + added+2;
		}
		if ( resp_h.AA) {
			printf("\tauth\n");
		}else{
			printf("\tnonauth\n");
		}
		memset(&resp_a, 0, sizeof(resp_a));
		numAns--;
	}

	return 0;
}
