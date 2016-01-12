/*
 * CS3600, Spring 2013
 * Project 2 Starter Code
 * (c) 2013 Alan Mislove
 *
 */

#ifndef __3600FS_H__
#define __3600FS_H__

#define MAGICNUM 7

typedef struct vcb_s{
  int magic; // Unique disk identifier
  int mount; // Set to 1 when disk is mounted, else 0
  int blocksize; // Size of a block in the disk
  int de_begin; // Starting location of directory entry blocks.
  int de_length; // Length of the directory entry block section.
  int fat_begin;
  int fat_length;
  int db_begin;

  uid_t user;
  gid_t group;
  mode_t mode;

  struct timespec access_time;
  struct timespec modify_time;
  struct timespec create_time;
}vcb;

typedef struct dirent_s{
  unsigned int valid;
  unsigned int first_block; // An offset, in fatents, from the beginning of FAT.
  unsigned int size;  // Size of file in bytes.
  
  uid_t user;
  gid_t group;
  mode_t mode;
  
  struct timespec access_time;
  struct timespec modify_time;
  struct timespec create_time;
  
  char name[512 - (3 * (sizeof(struct timespec))) - 24];
}dirent;

typedef struct fatent_s{
  unsigned int used:1;
  unsigned int eof:1;
  unsigned int next:30;
}fatent;
#endif
