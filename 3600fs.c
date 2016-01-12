
/*
 * CS3600, Spring 2014
 * Project 2 Starter Code
 * (c) 2014 Alan Mislove
 *
 * This file contains all of the basic functions that you will need 
 * to implement for this project.  Please see the project handout
 * for more details on any particular function, and ask on Piazza if
 * you get stuck.
 */

#define FUSE_USE_VERSION 26

#ifdef linux
/* For pread()/pwrite() */
#define _XOPEN_SOURCE 500
#endif

#define _POSIX_C_SOURCE 199309
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <fuse.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <assert.h>
#include <sys/statfs.h>
#include <time.h>

#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

#include "3600fs.h"
#include "disk.h"

dirent getdirent(int i);
vcb getvcb();
fatent getfatent(int offset);
int geteof(fatent* fe);
void setdirent(int i, dirent de);
void setvcb(vcb vcb1);
int path_check(const char* path);
int changefat(fatent* fe);

/*
 * Initialize filesystem. Read in file system metadata and initialize
 * memory structures. If there are inconsistencies, now would also be
 * a good time to deal with that. 
 *
 * HINT: You don't need to deal with the 'conn' parameter AND you may
 * just return NULL.
 *
 */
static void* vfs_mount(struct fuse_conn_info *conn) {
  fprintf(stderr, "vfs_mount called\n");

  // Do not touch or move this code; connects the disk
  dconnect();

  /* 3600: YOU SHOULD ADD CODE HERE TO CHECK THE CONSISTENCY OF YOUR DISK
           AND LOAD ANY DATA STRUCTURES INTO MEMORY */

  vcb myvcb = getvcb();

  if(myvcb.magic != MAGICNUM){
    fprintf(stderr, "Wrong disk mounted.");
    dunconnect();
  }
  if(myvcb.mount != 0){
    fprintf(stderr, "Disk has not unmounted.");
    dunconnect();
  }
  else{
    myvcb.mount = 1;
    setvcb(myvcb);
 }
  return NULL;
}

/*
 * Called when your file system is unmount.
 *
 */
static void vfs_unmount (void *private_data) {
  fprintf(stderr, "vfs_unmount called\n");

  vcb myvcb = getvcb();
  
  myvcb.mount = 0;
  setvcb(myvcb);

  /* 3600: YOU SHOULD ADD CODE HERE TO MAKE SURE YOUR ON-DISK STRUCTURES
           ARE IN-SYNC BEFORE THE DISK IS UNmount (ONLY NECESSARY IF YOU
           KEEP DATA CACHED THAT'S NOT ON DISK */

  // Do not touch or move this code; unconnects the disk
  dunconnect();
}

/* 
 *
 * Given an absolute path to a file/directory (i.e., /foo ---all
 * paths will start with the root directory of the CS3600 file
 * system, "/"), you need to return the file attributes that is
 * similar stat system call.
 *
 * HINT: You must implement stbuf->stmode, stbuf->st_size, and
 * stbuf->st_blocks correctly.
 *
 */
static int vfs_getattr(const char *path, struct stat *stbuf) {
  fprintf(stderr, "vfs_getattr called\n");
  // Do not mess with this code 
  // The hard links
  stbuf->st_nlink = 1; 
  stbuf->st_rdev  = 0;
  stbuf->st_blksize = BLOCKSIZE;

  /* 3600: YOU MUST UNCOMMENT BELOW AND IMPLEMENT THIS CORRECTLY */
  
  if(strcmp(path, "/") == 0){
    vcb vcb1 = getvcb();
    
    struct tm * atime;
    struct tm * mtime;
    struct tm * ctime;
    atime = localtime(&((vcb1.access_time).tv_sec));
    mtime = localtime(&((vcb1.modify_time).tv_sec));
    ctime = localtime(&((vcb1.create_time).tv_sec));
    
    stbuf->st_mode = 0777 | S_IFDIR;
    
    stbuf->st_uid = vcb1.user;
    stbuf->st_gid = vcb1.group;
    stbuf->st_atime = mktime(atime);
    stbuf->st_mtime = mktime(mtime);
    stbuf->st_ctime = mktime(ctime);
    stbuf->st_size = BLOCKSIZE;
    stbuf->st_blocks = 1;
    return 0;
  }
  else{
    // Checks if the path is correct
    if(path_check(path) != 0) 
      return -1;
    path++;
    for(int i = 1; i < 101; i++){
      dirent de = getdirent(i);
      if(de.valid == 1){
	if(strcmp(de.name, path) == 0){
	  struct tm * atime;
	  struct tm * mtime;
	  struct tm * ctime;
	  atime = localtime(&((de.access_time).tv_sec));
	  mtime = localtime(&((de.modify_time).tv_sec));
	  ctime = localtime(&((de.create_time).tv_sec));
	  
	  stbuf->st_mode = de.mode | S_IFREG;
	  
	  stbuf->st_uid = de.user;
	  stbuf->st_gid = de.group;
	  stbuf->st_atime = mktime(atime);
	  stbuf->st_mtime = mktime(mtime);
	  stbuf->st_ctime = mktime(ctime);
	  stbuf->st_size = de.size;
	  stbuf->st_blocks = (de.size / BLOCKSIZE);
	  return 0;
	}
      }
    }
    return -ENOENT;
  }
}

/*
 * Given an absolute path to a directory (which may or may not end in
 * '/'), vfs_mkdir will create a new directory named dirname in that
 * directory, and will create it with the specified initial mode.
 *
 * HINT: Don't forget to create . and .. while creating a
 * directory.
 */
/*
 * NOTE: YOU CAN IGNORE THIS METHOD, UNLESS YOU ARE COMPLETING THE 
 *       EXTRA CREDIT PORTION OF THE PROJECT.  IF SO, YOU SHOULD
 *       UN-COMMENT THIS METHOD.
static int vfs_mkdir(const char *path, mode_t mode) {

  return -1;
  } */

/** Read directory
 *
 * Given an absolute path to a directory, vfs_readdir will return 
 * all the files and directories in that directory.
 *
 * HINT:
 * Use the filler parameter to fill in, look at fusexmp.c to see an example
 * Prototype below
 *
 * Function to add an entry in a readdir() operation
 *
 * @param buf the buffer passed to the readdir() operation
 * @param name the file name of the directory entry
 * @param stat file attributes, can be NULL
 * @param off offset of the next entry or zero
 * @return 1 if buffer is full, zero otherwise
 * typedef int (*fuse_fill_dir_t) (void *buf, const char *name,
 *                                 const struct stat *stbuf, off_t off);
 *			   
 * Your solution should not need to touch fi
 *
 */
static int vfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi)
{
  if(strcmp(path, "/") == 0){
    vcb vcb1 = getvcb();
    for(int i = vcb1.de_begin; i < vcb1.de_begin+vcb1.de_length; i++){
      dirent de = getdirent(i);
      if(filler(buf, de.name, NULL, 0) != 0){
	return -ENOMEM;
      }
    }
    return 0;
  }else{
    return -1;
  }
}

/*
 * Given an absolute path to a file (for example /a/b/myFile), vfs_create 
 * will create a new file named myFile in the /a/b directory.
 *
 */
static int vfs_create(const char *path, mode_t mode, struct fuse_file_info *fi) {

  // Gets VCB and uses it to locate the begin and end of the dirents
  vcb vcb1 = getvcb(); 

  // Checks if the path is correct
  if(path_check(path) != 0) 
    return -1;
  // To increment the path after "/"
  path++;

  // The index of the being of the dirent (first free-dirent)
  int freedir = -1; 

  // Reads the used dirents and looks for a duplicate to create a file.
  for(int i = vcb1.de_begin; i < vcb1.de_begin+vcb1.de_length && freedir < 0; i++){
    dirent de = getdirent(i);
 
    if(de.valid == 1){
      if(strcmp(de.name, path) == 0)
        return -EEXIST;
    } 
    else{
        freedir = i;
    }
  }

  // Case when the file does not exist. 
  // Looks for any free spaces 

  // Case when free dirents do exist 
  if(freedir >= 0){ 
    // Now creates a new dirent
    // Fields are assigned once created
    dirent newdirent; 
    newdirent.valid = 1;
    newdirent.first_block = -1; 
    newdirent.size = 0;
    newdirent.user = getuid();
    newdirent.group = getgid();
    newdirent.mode = mode;

    struct timespec newtime;
    clock_gettime(CLOCK_REALTIME, &newtime);

    newdirent.access_time = newtime;
    newdirent.modify_time = newtime;
    newdirent.create_time = newtime;

    memset(newdirent.name,0,sizeof(newdirent.name));

    // To construct a string with the name
    char filename[512 - (3*sizeof(struct timespec)) - 24]; 
    memset(filename,0,sizeof(filename));
    // Path will be saved to the filename.
    strcpy(filename,path); 
    strcpy(newdirent.name, filename);

    // Last step: Writing the new dirent to the disk where freedir is
    setdirent(freedir,newdirent); 
    return 0;
  }
  // Case when there are no free dirents
  return -1; 
}

/*
 * The function vfs_read provides the ability to read data from 
 * an absolute path 'path,' which should specify an existing file.
 * It will attempt to read 'size' bytes starting at the specified
 * offset (offset) from the specified file (path)
 * on your filesystem into the memory address 'buf'. The return 
 * value is the amount of bytes actually read; if the file is 
 * smaller than size, vfs_read will simply return the most amount
 * of bytes it could read. 
 *
 * HINT: You should be able to ignore 'fi'
 *
 */
static int vfs_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi)
{
  if(path_check(path) != 0)
    return -1;
  
  char block[BLOCKSIZE];
  memset(&block,0,BLOCKSIZE);
  // Represents the # of  bytes that are read
  int bread = 0;
  vcb vcb1 = getvcb();
  path++;
 
  // Inorder to parse the offset into the block
  int blockoffset = (int) (offset / 512);
  int offset_block = offset % 512;
  int offsetbuffer = 0;
  
  dread(blockoffset + vcb1.db_begin, block);
  while(size > 0 && offset_block < BLOCKSIZE){ 
    buf[offsetbuffer] = block[offset_block];
    size--;
    offset_block++;
    offsetbuffer++;
    bread++;
  }

  // The rest of the first block has been read
  while(size > 0){
    if(offset_block == BLOCKSIZE){
      blockoffset = getfatent(blockoffset).next;  // This gets the next fat entry or the data block offset
      memset(&block,0,BLOCKSIZE); // this gets the next fat entry or the data block offset
      dread(blockoffset + vcb1.db_begin, block);
    
      // Ressetting the pointers
      offset_block = 0;
    }
    else{
      buf[offsetbuffer] = block[offset_block]; // Reading of the byte into the buffer
      size--;
      offset_block++;
      offsetbuffer++;
      bread++;
    }
  }
  return bread;
}


/*
 * The function vfs_write will attempt to write 'size' bytes from 
 * memory address 'buf' into a file specified by an absolute 'path'.
 * It should do so starting at the specified offset 'offset'.  If
 * offset is beyond the current size of the file, you should pad the
 * file with 0s until you reach the appropriate length.
 *
 * You should return the number of bytes written.
 *
 * HINT: Ignore 'fi'
 */
static int vfs_write(const char *path, const char *buf, size_t size,
                     off_t offset, struct fuse_file_info *fi)
{
  // Checks if the path is correct
  if(path_check(path) != 0)
    return -1;
  
  vcb vcb1 = getvcb();
  // Increments the path 
  path++;   
  // # of bytes that have been written to the disk from the buffer 
  int bwrite = 0;
  // # of zeros that are required to pad b/w eof and the offset 
  int pad = 0;
  // Holds an offset for vfs_write     
  int offsetwrite = 0; 

  dirent de;
  int dirfound = 0;
  int dirindex = -1;
  
  for(int i = vcb1.de_begin; i < vcb1.de_begin + vcb1.de_length, dirfound == 0; i++){
    de = getdirent(i);
    if(de.valid == 1)
      if(strcmp(path,de.name)==0){
        dirfound = 1;
        dirindex = i;
      }
  }

  // Disk Writing
  if(dirfound){
    // Determines if padding needs to be done	
    if(offset > de.size){
      // The number of zeros that need to be added
      pad = (offset - de.size); 
    }
    if((size + offset) > de.size){
      // resets the file size
      de.size = (size + offset);   
    }
    
    // Resets time of the file
    struct timespec newtime;
    clock_gettime(CLOCK_REALTIME, &newtime);
    
    de.access_time = newtime;
    de.modify_time = newtime;
     
    // Determines if dirent has allocated fat.
    // Tries to allocate one if it did not
    if((int) de.first_block == -1){
      char block[BLOCKSIZE];
      int free_found = 0;
      
      for(int i = vcb1.fat_begin;(i < vcb1.fat_begin + ((int) (vcb1.fat_length/128))) && free_found == 0; i++){
	int bindex = (i-vcb1.fat_begin)*128;
	
        // resetting of the block
	memset(block,0,BLOCKSIZE);
        // reads the fat block 
	dread(i, block); 
	
	for(int j = 0; j < 128 && free_found == 0; j++){
	  fatent fe = getfatent( bindex + j);
	  if(fe.used == 0){
	    de.first_block = bindex + j;
	    fe.used = 1;
	    fe.eof = 1;
	    fe.next = 0;
	    free_found = 1;
	  }
	}
      }
      if(free_found != 1){
        return -ENOSPC;
      }
    }
      
    fatent fe = getfatent(de.first_block);
    
    char block[BLOCKSIZE];
    memset(block,0,BLOCKSIZE);
      
    // Determines if padding is necesary and done if so
    if(pad > 0){
      
      // to find the index of the eof
      int fatindex = geteof(&fe) + vcb1.db_begin;
      dread(fatindex,block);
	
      int dataindex;
	
      for(int i = 0; block[i] != EOF; i++)
        dataindex++;

      dataindex++; 
	
	while(pad > 0){
          if(dataindex < BLOCKSIZE){
	    memset(&block[dataindex],0,1);
	    dataindex++;
	    pad--;
	    if(pad == 0){
		dwrite(dataindex,block);
		memset(block,0,BLOCKSIZE);
	    }
          } 
	  else{
	    dwrite(fatindex,block);
	    memset(block,0,BLOCKSIZE);
	    if(changefat(&fe) != 0){
		return -ENOSPC;
	    }
	    fatindex = geteof(&fe) + vcb1.db_begin;
	    dataindex = 0;
	  }
	}
    }    
        
    //Wrting from buf into the file beginning at the offset
    
    // To find where the offset is located in the datablock
    int blockoffset = (int)(offset/512);
    int offset_block = offset % 512;
    int offsetbuffer = 0;
    
    // Reads the offset block and writes into the block beginning at the offset 
    memset(block,0,BLOCKSIZE);
    dread(blockoffset + vcb1.db_begin, block);
    
    // To memcpy the rest of the block 
    while(offset_block < BLOCKSIZE && size > 0){
      memcpy(&block[offset_block], buf, 1);
      size--;
      buf++;
      offsetbuffer++;
      offset_block++;
      bwrite++;
    }

    // For the rest of the block
    dwrite(blockoffset + vcb1.db_begin, block);
    
    // For the remaining bytes
    while(size > 0){
      if(offset_block == BLOCKSIZE){
        dwrite(blockoffset + vcb1.db_begin, block); // Block writing
	if(changefat(&fe) != 0){ // Allocation of new fat or data block
	  return -ENOSPC;
	}
	
	// resetting of the offset block 
	offset_block = 0;
	blockoffset = geteof(&fe);
        memset(block,0,BLOCKSIZE);
      }
      
      memcpy(&block[offset_block], &buf[offsetbuffer], 1);
      
      size--;
      offsetbuffer++;
      offset_block++;
      bwrite++;
    }

    // Writing of the rest of size bytes
    dwrite(blockoffset + vcb1.db_begin, block);
    setdirent(dirindex,de);
    return bwrite;
  }
  // case when there are no free dirents
  else{
    return -1;
  }
  
}
/**
 * This function deletes the last component of the path (e.g., /a/b/c you 
 * need to remove the file 'c' from the directory /a/b).
 */
static int vfs_delete(const char *path)
{
  // Gets a vcb and uses it to locate the begin and end of the dirents
  vcb vcb1 = getvcb(); 

  // Check if the path is correct
  if(path_check(path) != 0) 
    return -1;
  // Increments the path 
  path++; 

  for(int i = vcb1.de_begin; i < vcb1.de_begin+vcb1.de_length; i++){
    dirent de = getdirent(i); 
    if(strcmp(de.name, path) == 0){
      de.valid = 0;
      setdirent(i,de);
      return 0;
    }
  }
   return -EEXIST;
 
  /* 3600: NOTE THAT THE BLOCKS CORRESPONDING TO THE FILE SHOULD BE MARKED
           AS FREE, AND YOU SHOULD MAKE THEM AVAILABLE TO BE USED WITH OTHER FILES */
}

/*
 * The function rename will rename a file or directory named by the
 * string 'oldpath' and rename it to the file name specified by 'newpath'.
 *
 * HINT: Renaming could also be moving in disguise
 *
 */
static int vfs_rename(const char *from, const char *to)
{
  vcb vcb1 = getvcb();

  if(path_check(from) != 0)
    return -1;

  from++;
  for(int i = vcb1.de_begin; i < vcb1.de_begin+vcb1.de_length;i++){
    dirent de = getdirent(i);
    if(strcmp(de.name,from) == 0){
      strcpy(de.name, to);
      return 0;
    }
  }
  return -1;
}


/*
 * This function will change the permissions on the file
 * to be mode.  This should only update the file's mode.  
 * Only the permission bits of mode should be examined 
 * (basically, the last 16 bits).  You should do something like
 * 
 * fcb->mode = (mode & 0x0000ffff);
 *
 */
static int vfs_chmod(const char *file, mode_t mode)
{
  vcb vcb1 = getvcb();
  
  // Checks if the path is correct 
  if(path_check(file) != 0)
    return -1; 
  
  // Increments path
  file++;
  for(int i = vcb1.de_begin; i < vcb1.de_begin+vcb1.de_length; i++){
    dirent de = getdirent(i);
    if(strcmp(de.name,file)==0){
      de.mode = mode;
      setdirent(i,de);
      return 0; 
    }
  }
  // Case when the file is not found
  return -1; 
}

/*
 * This function will change the user and group of the file
 * to be uid and gid.  This should only update the file's owner
 * and group.
 */
static int vfs_chown(const char *file, uid_t uid, gid_t gid)
{
  vcb vcb1 = getvcb();
  
  // Checks if the path is correct
  if(path_check(file) != 0)
    return -1; 
  
  // Increments path
  file++;
  for(int i = vcb1.de_begin; i < vcb1.de_begin+vcb1.de_length; i++){
    dirent de = getdirent(i);
    if(strcmp(de.name,file)==0){
      de.user = uid;
      de.group = gid;
      setdirent(i,de);
      return 0; 
    }
  }
  // Case when file is not found
  return -1; 
}

/*
 * This function will update the file's last accessed time to
 * be ts[0] and will update the file's last modified time to be ts[1].
 */
static int vfs_utimens(const char *file, const struct timespec ts[2])
{
  vcb vcb1 = getvcb();
  
  // Checks if the path is correct
  if(path_check(file) != 0)
    return -1; 
  
  // Increments path
  file++;
  for(int i = vcb1.de_begin; i < vcb1.de_begin+vcb1.de_length; i++){
    dirent de = getdirent(i);
    if(strcmp(de.name,file)==0){
      de.access_time = ts[0];
      de.modify_time = ts[1];
      setdirent(i,de);
      return 0; 
    }
  }
  // Case when file is not found
  return -1; 
}

/*
 * This function will truncate the file at the given offset
 * (essentially, it should shorten the file to only be offset
 * bytes long).
 */
static int vfs_truncate(const char *file, off_t offset)
{
  // Checks if the path is correct
  if(path_check(file) != 0)
    return -1;

  // Incremenets path
  file++; 
  vcb vcb1 = getvcb();

  dirent de;
  int dirindex = -1;

  for(int i = vcb1.de_begin; i < vcb1.de_begin + vcb1.de_length && dirindex == -1; i++){ // Get matching dirent.
    de = getdirent(i);
    if(strcmp(de.name,file) == 0)
      dirindex = i;
  }
/*
   3600: NOTE THAT ANY BLOCKS FREED BY THIS OPERATION SHOULD
           BE AVAILABLE FOR OTHER FILES TO USE.
*/
  return 0;
}

/*
 * You shouldn't mess with this; it sets up FUSE
 *
 * NOTE: If you're supporting multiple directories for extra credit,
 * you should add 
 *
 *     .mkdir	 = vfs_mkdir,
 */
static struct fuse_operations vfs_oper = {
    .init    = vfs_mount,
    .destroy = vfs_unmount,
    .getattr = vfs_getattr,
    .readdir = vfs_readdir,
    .create	 = vfs_create,
    .read	 = vfs_read,
    .write	 = vfs_write,
    .unlink	 = vfs_delete,
    .rename	 = vfs_rename,
    .chmod	 = vfs_chmod,
    .chown	 = vfs_chown,
    .utimens	 = vfs_utimens,
    .truncate	 = vfs_truncate,
};

int main(int argc, char *argv[]) {
    /* Do not modify this function */
    umask(0);
    if ((argc < 4) || (strcmp("-s", argv[1])) || (strcmp("-d", argv[2]))) {
      printf("Usage: ./3600fs -s -d <dir>\n");
      exit(-1);
    }
    return fuse_main(argc, argv, &vfs_oper, NULL);
}

dirent getdirent(int i){
  dirent de;
  char tmp2[BLOCKSIZE];
  memset(tmp2, 0, BLOCKSIZE);
  dread(i, tmp2);
  memcpy(&de, tmp2, sizeof(de));
  return de;
}

vcb getvcb(){
  vcb vcb1;
  char tmp1[BLOCKSIZE];
  memset(tmp1, 0, BLOCKSIZE);
  dread(0, tmp1);
  memcpy(&vcb1, tmp1, sizeof(vcb));
  return vcb1;
}

fatent getfatent(int offset){
  vcb vcb1 = getvcb();
  char block[BLOCKSIZE];
  memset(block, 0, BLOCKSIZE);
  dread(((int)(offset/128) + vcb1.fat_begin),block);
  if(offset > 128){
    offset = offset % 128;
  }
  fatent fe;
  memcpy(&fe, &block[offset], sizeof(fe));
  return fe;
}

// Given any fe, gets the index of the eof of the fe
int geteof(fatent* fe){
  int end_dblock = 0;
  fatent tmp3;
  while(fe->eof != 1){
    end_dblock = fe->next;
    tmp3 = getfatent(fe->next);
    fe = &tmp3;
  }
  return end_dblock;
}

void setdirent(int i, dirent de){
  char tmp2[BLOCKSIZE];
  memset(tmp2,0,BLOCKSIZE);
  memcpy(tmp2,&de,sizeof(de));
  dwrite(i,tmp2);
}

void setvcb(vcb vcb1){
  char tmp1[BLOCKSIZE];
  memset(tmp1, 0, BLOCKSIZE);
  memcpy(tmp1, &vcb1, sizeof(vcb));
  dwrite(0, tmp1);
}

// Ensures the path is valid. (no more than one "/")
int path_check(const char* path){
  const char* temp = path;
  int acc = 0; //accumulates number of '/'

  while(*temp){
    if(*temp == '/')
      acc++;
    temp++;
  }
  // When more than 1 "/"
  if(acc != 1)
    return -1;
  //Case of valid path
  else 
    return 0;	
}

// Given a FAt entry, adds a new fat entry. Modifies fe to have new fat entry
int changefat(fatent* fe){
  vcb vcb1 = getvcb();
  fatent freefat[128];

  // To read the fat entries from the disk
  char block[BLOCKSIZE];

  // Locates free fat entry
  for(int countfat = 0; countfat < (vcb1.fat_length/128); countfat++){
    
    memset(block,0,BLOCKSIZE);
    dread(vcb1.fat_begin+countfat, block);
    
    for(int i = 0; i < 128; i++){
      memcpy(&freefat[i], &block[i*4], sizeof(fatent));
    }
    for(int j = 0; j < 128; j++){
      // Case when an unused fat entry has been located.
      // the eof is changed and is appended
      if(freefat[j].used == 0){ 
	fe->eof = 0;
	fe->next = j + (countfat * 128);

	freefat[j].eof = 1;
	freefat[j].next = 0;
	freefat[j].used = 1;
	
	memcpy(&block[j*4], &freefat[j], sizeof(fatent));
	dwrite(vcb1.fat_begin + countfat, block);
	return 0;
      }
    }
  }
  return -1;
}
