/*
* CS3600, Spring 2013
* Project 4 Starter Code
* (c) 2013 Alan Mislove
*
*/

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

#include "3600sendrecv.h"

int main() {
 // first, open a UDP socket
 int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

 // next, construct the local port
 struct sockaddr_in out;
 out.sin_family = AF_INET;
 out.sin_port = htons(0);
 out.sin_addr.s_addr = htonl(INADDR_ANY);

 if (bind(sock, (struct sockaddr *) &out, sizeof(out))) {
   perror("bind");
   exit(1);
 }

 struct sockaddr_in tmp;
 int len = sizeof(tmp);
 if (getsockname(sock, (struct sockaddr *) &tmp, (socklen_t *) &len)) {
   perror("getsockname");
   exit(1);
 }

 mylog("[bound] %d\n", ntohs(tmp.sin_port));

 // wait for incoming packets
 struct sockaddr_in in;
 socklen_t in_len = sizeof(in);

 // construct the socket set
 fd_set socks;

 // construct the timeout
 struct timeval t;
 int recv_time = 7;
 t.tv_sec = recv_time;
 t.tv_usec = 0;

 // our receive buffer
 int buf_len = 1500;
 void *buf = malloc(buf_len);

 // stores out of order incoming packets
 char *recv_window[WINDOW_SIZE * 100] = {0};
 // stores length of packets in the recv_window
 unsigned int recv_len[WINDOW_SIZE *100] = {0};
 // the expected sequence number of the next packet to come in
 unsigned int curr_seq = 0;
 unsigned int loop_seq = 0;

 // wait to receive, or for a timeout
 while (1) {
   // reinitializes socket
   FD_ZERO(&socks);
   FD_SET(sock, &socks);

   if (select(sock + 1, &socks, NULL, NULL, &t)) {
     int received;
     if ((received = recvfrom(sock, buf, buf_len, 0, 
       (struct sockaddr *) &in, (socklen_t *) &in_len)) < 0) {
       perror("recvfrom");
       exit(1);
     }

     // sets timeout
     t.tv_sec = recv_time;

     // interprets received packet
     header *myheader = get_header(buf);
     char *data = get_data(buf);
     char *data_tmp = malloc(sizeof(char) * (strlen(data)));
     strcat(data_tmp, data);

     if (myheader->magic == MAGIC) {

       if (myheader->sequence == curr_seq) {
         // write out data
         write(1, data, myheader->length);
         curr_seq++;

         // while there is data at this index in the buffer array
         // some previous packets were stored
         while (recv_len[curr_seq] > 0) {
           // write out stored data
           write(1, recv_window[curr_seq], recv_len[curr_seq]);
           // clear position
           recv_len[curr_seq] = 0;
           memset(&recv_window[curr_seq], 0, sizeof(char *));
           curr_seq++;
         }

       } else if (myheader->sequence < curr_seq) {
         // ignore duplicate packets
         continue;

       } else {
         // if this packet is greater than the one expected
         // store the packet to be written out later
         unsigned int insert_seq = myheader->sequence;
         if (recv_window[insert_seq] == 0) {
           mylog("no data at this insert_seq %d\n", insert_seq);
           recv_len[insert_seq] = myheader->length;
           recv_window[insert_seq] = data_tmp;
         }

       }

       // update the response ACK's header
       loop_seq = curr_seq;
       mylog("[recv data] %d (%d) %s\n", myheader->sequence, myheader->length, "ACCEPTED (in-order)");
       mylog("[send ack] %d\n", loop_seq);

       // send ACK back
       header *responseheader = make_header(loop_seq, 0, myheader->eof, 1);
       if (sendto(sock, responseheader, sizeof(header), 0, (struct sockaddr *) &in, (socklen_t) sizeof(in)) < 0) {
         perror("sendto");
         exit(1);
       }

       // if this is the end of file, stop listening
       if (myheader->eof) {
         mylog("[recv eof]\n");
         mylog("[completed]\n");
         exit(0);
       }
     } else {
       mylog("[recv corrupted packet]\n");
     }
   } else {
     mylog("[error] timeout occurred\n");
     exit(1);
   }
 }

 return 0;
}
