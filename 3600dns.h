/*
 * CS3600, Fall 2013
 * Project 2 Starter Code
 * (c) 2013 Alan Mislove
 *
 */

#ifndef __3600DNS_H__
#define __3600DNS_H__
typedef struct header_s{
 unsigned int ID:16;

 unsigned int RD:1;
 unsigned int TC:1;
 unsigned int AA:1;
 unsigned int OPCODE:4;
 unsigned int QR:1; 

 unsigned int RCODE:4;
 unsigned int Z:3;
 unsigned int RA:1;

 unsigned int QDCOUNT:16;
 unsigned int ANCOUNT:16;
 unsigned int NSCOUNT:16;
 unsigned int ARCOUNT:16;
} header;

typedef struct question_s{
  int QTYPE:16;
  int QCLASS:16;
} question;


typedef struct answer_s{
  int TYPE:16;
  int CLASS:16;
  int TTL:16;
  int RDLENGTH:16;
} answer; 

#endif

