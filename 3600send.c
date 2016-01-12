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

static int DATA_SIZE = 1460;

// determines the sequence of created packets
unsigned int logicsequence = 0; 
// sequence of received packet
unsigned int sequence = 0;

void usage() {
 printf("Usage: 3600send host:port\n");
 exit(1);
}

/**
* Reads the next block of data from stdin - as given
*/
int get_next_data(char *data, int size) {
 return read(0, data, size);
}

/**
* Builds and returns the next packet, or NULL
* if no more data is available. - as given
*/
void *get_next_packet(int sequence, int *len) {
 char *data = malloc(DATA_SIZE);
 int data_len = get_next_data(data, DATA_SIZE);

 if (data_len == 0) {
   mylog("Data len is 0.\n");
   free(data);
   return NULL;
 }

 header *myheader = make_header(logicsequence, data_len, 0, 0);
 void *packet = malloc(sizeof(header) + data_len);
 memcpy(packet, myheader, sizeof(header));
 memcpy(((char *) packet) + sizeof(header), data, data_len);

 free(data);
 free(myheader);

 *len = sizeof(header) + data_len;

 fprintf(stderr, "Getting next packet %d.\n", sequence);
 return packet;
}

/*
* Sends a packet to the receiver - as given
*/
int send_next_packet(int sock, struct sockaddr_in out) {
 mylog("Send next.\n");
 int packet_len = 0;
 void *packet = get_next_packet(logicsequence, &packet_len);

 if (packet == NULL)
   return 0;

 mylog("[send data] %d (%d)\n", logicsequence, packet_len - sizeof(header));

 if (sendto(sock, packet, packet_len, 0, (struct sockaddr *) &out, (socklen_t) sizeof(out)) < 0) {
   perror("sendto\n");
   exit(1);
 }

 fprintf(stderr, "Sending next packet %d.\n", logicsequence);
 return 1;
}

/*
* Sends a packet indicating the end of a file -- as given
*/
void send_final_packet(int sock, struct sockaddr_in out) {
 header *myheader = make_header(logicsequence, 0, 1, 0);
 mylog("[send eof]\n");

 if (sendto(sock, myheader, sizeof(header), 0, (struct sockaddr *) &out, (socklen_t) sizeof(out)) < 0) {
   perror("sendto");
   exit(1);
 }

 fprintf(stderr, "Sent last packet %d.\n", logicsequence);
}

int main(int argc, char *argv[]) {
 // extract the host IP and port
 if ((argc != 2) || (strstr(argv[1], ":") == NULL)) {
   usage();
 }

 char *tmp = (char *) malloc(strlen(argv[1])+1);
 strcpy(tmp, argv[1]);

 char *ip_s = strtok(tmp, ":");
 char *port_s = strtok(NULL, ":");

 // first, open a UDP socket
 int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

 // next, construct the local port
 struct sockaddr_in out;
 out.sin_family = AF_INET;
 out.sin_port = htons(atoi(port_s));
 out.sin_addr.s_addr = inet_addr(ip_s);

 // socket for received packets
 struct sockaddr_in in;
 socklen_t in_len = sizeof(in);

 // construct the socket set
 fd_set socks;

 // free_space controlls the sending rate
 int free_space = WINDOW_SIZE;

 // stores all the packets created
 char * window[WINDOW_SIZE * 100]={0};
 // stores the length of each packet created
 int pack_len[WINDOW_SIZE *100]={0};

 // current value of timeout
 int new_time = 3;
 struct timeval t;
 t.tv_usec = 0;

 // FLAGS
 // how many packets were created
 unsigned int pack_created = 0;
 // if a timeout occurred
 unsigned int TIMEOUT = 0;
 // whether you should keep sending
 int keep_send = 1;
 // is this ack late?
 int late_ack = 1;
 // whether a packet was able to be created
 int packet_exist = 1;

 // the last ACK sequence number
 unsigned int last_ack = 0;
 // the number of times a sequence number has repeated
 int repeat = 0;
 // the packet sequence to start sending
 unsigned int start = 0;
 // how many ACKs are late?
 int count_late = 0;

 while ((packet_exist || TIMEOUT || keep_send) && new_time > 0) {

   // reinitializes timeout value
   t.tv_sec = new_time;

   // initializes the socket
   FD_ZERO(&socks);
   FD_SET(sock, &socks);


   if (!TIMEOUT && packet_exist) {
     start = pack_created;
     // create the correct amount of packets
     while (free_space > 0) {
       window[pack_created%(WINDOW_SIZE*100)] = 
         get_next_packet(logicsequence, &(pack_len[pack_created%(WINDOW_SIZE*100)]));

       // if no more data
       if (window[pack_created%(WINDOW_SIZE*100)] == NULL) {
         mylog("Packet is null.\n");
         packet_exist = 0;
         break;
       }

       // change counters
       pack_created++;
       free_space--;
       logicsequence++; 
     }

     // send until last packet created
     for(unsigned int i = start; i < pack_created; i++){
       if (sendto(sock, window[i%(WINDOW_SIZE*100)], 
         pack_len[i%(WINDOW_SIZE*100)], 0, 
         (struct sockaddr *) &out, (socklen_t) sizeof(out)) < 0) {
         perror("sendto");
         exit(1);
       }
     }
   }

   // wait to receive, or for a timeout
   if (select(sock + 1, &socks, NULL, NULL, &t)) {
     late_ack = 0;
     unsigned char buf[10000];
     int buf_len = sizeof(buf);
     int received;
     if ((received = recvfrom(sock, &buf, buf_len, 0, 
       (struct sockaddr *) &in, (socklen_t *) &in_len)) < 0) {
       perror("recvfrom");
       exit(1);
     }

     // interpret received packet
     header *myheader = get_header(buf);

     // if an ACK
     if ((myheader->magic == MAGIC) 
       && (myheader->sequence >= sequence) 
       && (myheader->ack == 1)) {
       sequence = myheader->sequence;

       // this is not a repeated ack
       if (sequence > last_ack) {
         repeat = 0;
         new_time = new_time + new_time * 0.2; // allow more time for data to come in
         t.tv_usec = new_time * 1000;
         free_space = sequence - last_ack; // send more packets
       } else {
         // ack is repeated
         free_space++;
         repeat++;
       }

       last_ack = sequence; // update last_ack

       /* if a packet was dropped, retransmit the dropped packet
       * received ack = sent ack + 1
       * therefore last_ack is the dropped packet
       */
       if (repeat >= 2) {
         if (sendto(sock, window[(last_ack) %(WINDOW_SIZE*100)], 
           pack_len[(last_ack)%(WINDOW_SIZE*100)], 0, 
           (struct sockaddr *) &out, (socklen_t) sizeof(out)) < 0) {
           perror("sendto");
           exit(1);
         }
       } else {
         keep_send = 1;
       }

       // if you are done sending
       if (last_ack == pack_created) {
         TIMEOUT = 0;
         keep_send = 0;
         packet_exist = 0;
       }
     } else {
       mylog("[recv corrupted ack] %x %d\n", MAGIC, sequence);
     }

   } else {
     // if < 2 acks are late, try giving them a little more time
     if (late_ack && count_late < 2) {
       new_time = new_time * 1.5;
       t.tv_usec = new_time * 1000;
       count_late++;
     } else {

       // a timeout occurred and the last packet needs retransmitting
       TIMEOUT = 1;
       if (new_time < 7) {
         if (sendto(sock, window[(last_ack) %(WINDOW_SIZE*100)], 
           pack_len[(last_ack)%(WINDOW_SIZE*100)], 0, 
           (struct sockaddr *) &out, (socklen_t) sizeof(out)) < 0) {
           perror("sendto");
           exit(1);
         }

       }

       new_time = new_time * 0.75; // reduce timeout

       // retransmit all the packets that weren't received but we sent
       for (unsigned int i = last_ack; i < pack_created; i++) {
         if (sendto(sock, window[i %(WINDOW_SIZE*100)], 
           pack_len[i%(WINDOW_SIZE*100)], 0, 
           (struct sockaddr *) &out, (socklen_t) sizeof(out)) < 0) {
           perror("sendto");
           exit(1);
         }
       }
     }
   }
 }

 send_final_packet(sock, out);
 mylog("[completed]\n");

 return 0;
}
