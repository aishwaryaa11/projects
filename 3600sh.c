/**
 * Aishwarya Afzulpurkar (aishwary)
 *
 * CS3600, Spring 2014
 * Project 1 Starter Code
 * (c) 2014 Alan Mislove
 *
 * You should use this (very simple) starter code as a basis for
 * building your shell.  Please see the project handout for more
 * details.
 */

#include "3600sh.h"
#define USE(x) (x) = (x)
#define MAX_ARGS 2000
#include <stdio.h>
#include <stdlib.h>

/* Setting the initial background process state to none here */
pid_t end_bkgd = 0;

/* Setting the error state as no bugs */
int error = 0;

/* This function takes in a string and returns the word contained in it. 
 * Characters of the argument are iterated over to omit non-useful characters.
 * getword : String(arg_str), Pointer(word_end), String(word), Pointer(len) -> int
 * arg_str    : Is the given argument
 * word_end   : Tells when the new word ends
 * word   : The first word of the given argument will be allocated to this
 * len : Length of the new word.
 * This function returns 1 if there is a word and 0 incase of no next word
 */
int getword(char *arg_str, int *word_end, char *word, int *len) {
  
  /* Setting intial state of detected & as none */
    static int ampersand_state = 0;

    /* Case where there is no argument passed */
    if (arg_str[0] == '\0' ) {
         ampersand_state = 0;
         return 0;
    }

    /* Case when the argument isnt empty or null */
    /* Omit all the non-required characters at the beginning of the argument. 
       Namely the Whitespace, new line, tab characters */ 
    int i = 0;
    while( arg_str[i] != '\0')  {
      if (arg_str[i] == ' ' || arg_str[i] == '\n'  || arg_str[i] == '\t' ) {
           i++;
           continue;
        }
      /* Case if the argument was just spaces, in which case the next promt will appear */
      else if (arg_str[i] == '\0' ) {
           ampersand_state = 0;
            return 0;
        }
      else
            break;
    }

    
    /* Getting the word */
    /* Counter to fill up the word (char array) */
    int j = 0; 
    /* Start of the next word */
    while( arg_str[i]) {
        /* Check if the space character is ended with a backspace;
	 * which will then be taken as a part of the word */
        if (arg_str[i] == '\\') {
             if(arg_str[i+1] == ' ' || arg_str[i+1] == 't' || arg_str[i+1] ==  '&' ||  arg_str[i+1] == '\\' ) {
                if (arg_str[i+1] == 't')
		  word[j++]= '\t'; 
                else /* case when the character is part of the word */
		  word[j++]=arg_str[i+1];
                i = i + 2;
                continue; 
             }
             else {
                printf("%s\n", "Error: Unrecognized escape sequence.");
                ampersand_state = 0;
                return -1;
             }
        }

        /* Detecting if it is the end of the word */
        if (arg_str[i] == ' ' || arg_str[i] == '\n'  || arg_str[i] == '\t' )
            break;  
        else {
	  /* Since we should consider if & is the ending character of the argument; 
           * Then the parent process will not wait for its child to complete */
            if( arg_str [i] == '&')
	      ampersand_state = ampersand_state + 1; 
            word[j++] = arg_str[i++];
        }
     }

    /* Making sure ampersand is the last character, otherwise gives out an error */
    if(ampersand_state)  {
        if( word[j-1] != '&' || ampersand_state > 1) {
            printf("%s\n", "Error: Invalid syntax.");
            ampersand_state  = 0;
            return -1;
        }
	/* Case when detected ampersand is includes in the word; omit it */
        if (j > 1) {        
            ampersand_state = 0;
            j--; i--;
        }
    }

    /* Setting the end character of the word to null character inorder to save lives */
    word[j] = '\0';

    *len = j;
    if(*len == 0) {
        ampersand_state = 0;
        return 0;
    }
    *word_end = i;

    return 1;
}

/**
 * This function breaks argv and initializes the input, ouput and error files 
 * to fin, fout and ferr respectively. 
 * After processing the re-direction command, argv is reset to the remaining arguments.
 * Returns 0 for sucess and non-zero for invalid format errors.
 */
int getargs(char **argv, int *argn, char *fin, char *fout, char *ferr) {

    int i;
    int err = 0;
    int n = *argn;
    /* This is a flag to denote re-direction */ 
    int redir = 0; 
    /* These are the flags used to keep denote the input,output and error files in the command */
    int in_state = 0, out_state = 0, err_state = 0;

    /* The arguments are stored excluding the input-output redirection arguments */
    char **tmp = (char **)malloc(sizeof(char **) * MAX_ARGS);
    /* to iterate through tmp */
    int j = 0; 


    for( i = 0; i < n; i++) {
        if (!strcmp(argv[i], "<"))
        {
            /* Case : If there is only a one < and is not the last argument */
            if(in_state || (i == (n -1))) {
                err = 1;
                break;
            }
            else {
                if(!strcmp(argv[i+1], "<" ) || !strcmp(argv[i+1], ">" ) || !strcmp(argv[i+1], "2>" )) {
                    err = 1;
                    break;
                }

                strcpy( fin, argv[i + 1]);
                in_state = 1;
                free(argv[i]);
                free(argv[i+1]);
                i++;   
                redir = 1;
                continue;
            }
        }
        if (!strcmp(argv[i], ">"))
        {
            /* Case : If there is only a one > and is not the last argument */
            if(out_state || (i == (n -1))) {
                err = 1;
                break;
            }
            else {
                if(!strcmp(argv[i+1], "<" ) || !strcmp(argv[i+1], ">" ) || !strcmp(argv[i+1], "2>" )) {
                    err = 1;
                    break;
                }
                strcpy( fout, argv[i + 1]);
                out_state = 1;
                free(argv[i]);
                free(argv[i+1]);
                i++;    
                redir = 1;
                continue;
            }
        }
        if (!strcmp(argv[i], "2>"))
        {
	  /* Case : If there is only a one > and is not the last argument */
            if(err_state || (i == (n -1))) {
                err = 1;
                break;
            }
            else {
                if(!strcmp(argv[i+1], "<" ) || !strcmp(argv[i+1], ">" ) || !strcmp(argv[i+1], "2>" )) {
                    err = 1;
                    break;
                }
                strcpy( ferr, argv[i + 1]);
                err_state = 1;
                free(argv[i]);
                free(argv[i+1]);
                i++;   
                redir = 1;
                continue;
            }
        }


        /**
	 * No arguments after re-direction are allowed. An invalid synatax error is thrown if 
         * re-direction command was found among the arguments
         */
        if(redir) {
            err = 1;
            break;
        }

        tmp[j++] = argv[i];
    }

    /* Redirection has been completed. 
     * tmp contains the rest of the commands excluding the input-output re-direction 
     * tmp needs to be copied back to argv as it will allow the child process to the called with
     * its arguments 
     */

    /* Case when the argument list is empty
     * and generates an error 
     */
    if (j == 0) 
        return 1;

    if(err) {
      /* The rest of argv is freed since we no longer require the command  */
        for(;i<n;i++)
            free(argv[i]);
        return err;
    }

    /* The length of agrv is reset */
    *argn = j;     
    argv[j] = NULL;
    while(j--) {
        argv[j] = tmp[j];
    }
    free(tmp);
    return 0;
}

/* This function frees the memory for the in and out commands. */
void freespace(char **argv, int argn) {
    int x;
    for(x = 0; x < argn; x++)
        free( argv[x]);
    return;
}

/*
O_CREAT : create file if it doesn't exist already
O_RDWR : open the file for read and write both
O_TRUNC : if the file exists and is opened for write access, truncate it to zero length
S_IRUSR : set read rights to be true for user
S_IWUSR : set write rights to be true for user

*/


int openfiles(char *fin, char *fout, char *ferr) { /*open redirection error files*/
  int fdin = 0; /*file destination of input */
  int fdout= 0; /*output */
  int fderr = 0; /* and error files */

  if( strlen(fin)) { /*get string length of input file*/
    fdin = open(fin, O_CREAT | O_RDWR | O_TRUNC,  S_IRUSR | S_IWUSR); /*open file with permissions*/
    if(error) printf("Opened input file %s\n", fin); /*open in error mode*/
    if(fdin == 0) { /*if file cannot be opened*/
      printf("%s\n", "Error: Unable to open redirection file."); /*generate error*/
      return -1;
    }
    int file_state = dup2(fdin, 0); /*system call to duplicate file and delete prev one*/
    if (file_state == -1){ /*if operation cannot be performed*/
      printf("%s\n", "Error: Unable to open redirection file."); /*generate error*/
      return -1;
    }
  }

  if( strlen(fout)) {
    fdout = open(fout, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    if(error) printf("Opened output file %s\n", fout);
    if(fdout == 0) {
      printf("%s\n", "Error: Unable to open redirection file.");
      return -1;
    }
    int file_state = dup2(fdout, 1);
    if (file_state == -1){
      printf("%s\n", "Error: Unable to open redirection file.");
      return -1;
    }
  }
  
  if( strlen(ferr)) {
    fderr = open(ferr, O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    if(error) printf("Opened errput file %s\n", ferr);
    if(fderr == 0) {
      printf("%s\n", "Error: Unable to open redirection file.");
      return -1;
    }
    int file_state = dup2(fderr, 2);
    if (file_state == -1){
      printf("%s\n", "Error: Unable to open redirection file.");
      return -1;
    }
  }
  
  if(error) printf("In openfiles\n"); /*if no errors exist*/
  return 0; //exit
}



int finalchecks(char *argument){ /* change the argv, and populate in out and err files */
  if(error) printf("Processing line\n"); /*begin executing function if no errors found */
  
  char *arg = argument; /*set pointer to beginning of argument */
  char word[MAX_ARGS];
  int word_end = 0;
  int len = 0;

  char **argv = (char **)malloc(sizeof(char **) * MAX_ARGS);
  int argn = 0;
  
  int file_state, i;
  while ((file_state = getword(arg, &word_end, word, &len))) { /* clean up the 'word' */
    if (file_state == -1) { /* if error comes up */
      freespace(argv, argn); /*free memory used up by argv*/
      free(argv);
      return 0; /* there are no words, so go to next command */
    }
    char *tmp = (char *)malloc(sizeof(char)* MAX_ARGS);
    for(i= 0; i < len; i++) {
      tmp[i] = word[i]; /*copy word to tmp array*/
    }
    tmp[i] = '\0'; /*terminate tmp will null pointer*/
    
    arg = arg + word_end; /* for next iteration*/
    argv[argn++] = tmp; 
  }

  argv[argn] = NULL; /*remove last argument of argv */
  
  if(argn == 0) {
    free(argv);
    return 0;/*print array of arguments out (no spaces or tabs in between)*/
  }
  
  if( !strcmp(argv[0], "exit")) {   /* if the user has typed "exit", then we need to exit the shell*/
    freespace(argv, argn);
    free(argv);
    return 1;  /* non zero means exit */
  }
  
  if(error) { /*if no errors found till now */
    for (int x = 0; x < argn; x++)
      printf("%s,", argv[x]); /*print array of arguments out (no spaces or tabs in between) */
    printf("\n");
  }
  
  
  int background = 0; /*base case: no background processes running */
  if( argv[argn-1][0] == '&') { /*check if first char of the last arg is ampersand */
    background = 2; /*parent process does not wait for child process to complete = creation of background process */
    argn--; /*reduce # of arguments by 1*/
    argv[argn] = NULL; /*so that we can remove ampersand from argv*/
  }
  
  /*initialize in,out,err files to empty strings*/
  char fin[MAX_ARGS] = "";
  char fout[MAX_ARGS] = "";
  char ferr[MAX_ARGS] = "";
  
  if(getargs(argv, &argn, fin, fout, ferr)){ /*if errors exist in getargs*/
    free(argv); /*free resources occupied by argv*/
    printf("%s\n", "Error: Invalid syntax."); /*generate error*/
    return 0; /*exit*/
  }
  
  if(error) {
    printf("After calling getargs\n");
    for (int x = 0; x < argn; x++)
      printf("%s,", argv[x]); /*print array of arguments out (after processing inoutfiles)*/
    printf("\n");
    
    /*if files contain anything print it out*/
    if(strlen(fin))  printf("fin = %s\n", fin); 
    if(strlen(fout))  printf("fout = %s\n", fout);
    if(strlen(ferr))  printf("ferr = %s\n", ferr);
    printf("\n");
  }

  pid_t child_pid = fork(); /*create new duplicate process of parent*/
  if (child_pid == 0) { /*we are the child*/
    if (background) /*if the process should be a background process*/
      setpgid( 0,0); /*put the process in a new process group */
      /* man pages for setpgid: http://man7.org/linux/man-pages/man2/setpgid.2.html */
      
    if (openfiles(fin, fout, ferr)) /*check if errors exist in openfiles*/
      exit(1);

    /* ENOENT: The filename/script/ELF interpreter or shared library needed for file does not exist
       EACCES: Execute permission is denied for the file or a script or ELF interpreter */
    
    if (execvp(argv[0], argv) == -1) { /*executes command in argv[0] using the rest of argv as arguments*/
      if (errno == ENOENT )
	printf("Error: Command not found.\n");
      else if (errno == EACCES)
	printf("Error: Permission denied.\n");
      else
	printf("Error: %s\n", strerror(errno));
    }
    exit(1);
  } else {
    
    /* WNOHANG: returns immediately even if child is still running. 
       -1 helps catch SIGCHLD for older background processes */
    
    if (!background) /*if child is not a background process*/
      waitpid(child_pid, NULL, 0);  /* wait for it to exit*/
    else{
      waitpid(-1, NULL, WNOHANG);
      end_bkgd = child_pid; /*set last background process to child*/
    }
  }
  return 0; /*exit*/
}


int main(int argc, char *argv[]){
  // Code which sets stdout to be unbuffered
  // This is necessary for testing; do not change these lines
  USE(argc);
  USE(argv);
  setvbuf(stdout, NULL, _IONBF, 0);

  char* user = getenv("USER"); /*get username*/
  char* hostname = (char*)calloc(MAX_ARGS, sizeof(char)); /*dynamically allocate memory for hostname*/
  gethostname(hostname, MAX_ARGS); /*get hostname*/
  char* dir = getcwd(NULL, MAX_ARGS); /*get current directory*/
  char tmparray[MAX_ARGS];
  
  while (1) { /*start shell*/
    printf("%s@%s:%s> ", user, hostname, dir); /*print username, dir, host*/
    if( fgets( tmparray, MAX_ARGS, stdin)) { /*if there exists stored standard input in the tmporary array*/
      if(finalchecks(tmparray)) /*after argv is cleaned and in,out,err files are populated*/
	break;
    }if (feof(stdin)) /*if end of file is reached*/
      break;
  }
  
  printf("So long and thanks for all the fish!\n");
  
  if(end_bkgd) /*if last background process is running*/
    waitpid(end_bkgd, NULL, 0); /*wait for it to finish*/
  
  return 0; /*exit*/
}

void do_exit() {
  printf("So long and thanks for all the fish!\n");
  exit(0);
}
