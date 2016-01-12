/*
 * CS3600, Spring 2014
 * Project 2 Starter Code
 * (c) 2014 Alan Mislove
 *
 * This program is intended to format your disk file, and should be executed
 * BEFORE any attempt is made to mount your file system.  It will not, however
 * be called before every mount (you will call it manually when you format 
 * your disk file).
 */

#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include "3600fs.h"
#include "disk.h"

int get_num_dblocks(int size){
  int fat_plus_db = size - 101;
  return fat_plus_db * 0.992248062;
}

vcb mkvcb(int num_dblocks){
  vcb myvcb;
  myvcb.magic = MAGICNUM;
  myvcb.mount = 0;
  myvcb.blocksize = BLOCKSIZE;
  
  // Should de_start be in blocks or in bytes? Currently in blocks
  myvcb.de_begin = 1;
  // Should length be in blocks or in bytes? Currently in blocks
  myvcb.de_length = 100;
  
  // Should start be in blocks or in bytes? Currently in blocks
  myvcb.fat_begin = myvcb.de_length + myvcb.de_begin + 1;
  // Should length be in blocks or in bytes? Currently in blocks
  myvcb.fat_length = num_dblocks;


  myvcb.db_begin = ((int)myvcb.fat_length/128) + myvcb.fat_begin + 1;
  
  myvcb.user = getuid();
  myvcb.group = getgid();
  myvcb.mode = 0777;
  
  clock_gettime(CLOCK_REALTIME, &myvcb.access_time);
  clock_gettime(CLOCK_REALTIME, &myvcb.modify_time);
  clock_gettime(CLOCK_REALTIME, &myvcb.create_time);
  
  return myvcb;
}

dirent mkdirent(){
  dirent de;
  de.valid = 0;
  de.size = 0;
  return de;
}

fatent mkfatent(){
  fatent fe;
  fe.used = 0;
  return fe;
}

void myformat(int size) {
  // Do not touch or move this function
  dcreate_connect();

  int num_dblocks = get_num_dblocks(size);

  // Format vcb
  vcb myvcb = mkvcb(num_dblocks);
  // Do we need to malloc space(BLOCKSIZE) for vcbtemp?
  char vcbtemp[BLOCKSIZE];
  memset(vcbtemp, 0, BLOCKSIZE);
  memcpy(vcbtemp, &myvcb, BLOCKSIZE);
  dwrite(0, vcbtemp);

  // Format dirents
  dirent mydirent = mkdirent();
  // Do we need to malloc space(BLOCKSIZE) for dirtemp?
  char dirtemp[BLOCKSIZE];
  memset(dirtemp, 0, BLOCKSIZE);
  memcpy(dirtemp, &mydirent, BLOCKSIZE);
  
  for(int i = myvcb.de_begin; i < myvcb.de_begin+myvcb.de_length; i++){
    dwrite(i, dirtemp);
  }


  fatent fe = mkfatent();

  fatent fat_block[128];

  int remaining = myvcb.fat_length;
  int block = myvcb.fat_begin;

  while(remaining > 0){
    for(int i = 0; i<128; i++){
      fat_block[i] = fe;
      remaining--;
    }
    char fat_block_temp[BLOCKSIZE];
    memset(fat_block_temp,0,BLOCKSIZE);
    memcpy(fat_block_temp,&fat_block,sizeof(fat_block));
    dwrite(block,fat_block_temp);
    block++;
  }

  char empty_block[BLOCKSIZE];
  memset(empty_block,0,BLOCKSIZE);
  for(int i = 0; i < num_dblocks; i++){
     dwrite((myvcb.db_begin + i), empty_block);
  }

  // Do not touch or move this function
  dunconnect();
}

int main(int argc, char** argv) {
  // Do not touch this function
  if (argc != 2) {
    printf("Invalid number of arguments \n");
    printf("usage: %s diskSizeInBlockSize\n", argv[0]);
    return 1;
  }

  unsigned long size = atoi(argv[1]);
  printf("Formatting the disk with size %lu \n", size);
  myformat(size);
}
