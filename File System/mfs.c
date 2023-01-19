/*
	Name: Raed Ali, Luc Pham
	ID: 1001567598, 1001918323
*/

// The MIT License (MIT)
// 
// Copyright (c) 2016, 2017 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.


#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports five arguments


#define MAXFILESIZE 10240000

#define MAXFILENAMESIZE 32
#define MAXFILES 125

#define NUM_BLOCKS 4226
#define BLOCK_SIZE 8192
#define TOTALSPACE 34603008
// each file system gets 33MB 

struct FileNode // represents a file
{
	char fileName[MAXFILENAMESIZE];
	int hidden;
	int readOnly;
	int size;
	char date[MAXFILENAMESIZE];
	int inUse; // if not inUse then the file has been deleted 
	int start; //start block in fileData array
	int end; //end block in fileData array
	int writtenToDisk;
	struct FileNode *nextPtr; // points to next file node
}
FileNode;

// each system image is a file system
struct FILESYSTEMLIST // represents a system image
{
	char fileSystemName[MAXFILENAMESIZE];
	struct FileNode *fs; // points to first file node in file system
	unsigned char fileData[NUM_BLOCKS][BLOCK_SIZE];
	int currentIndex;
	int totalFiles;
	int spaceLeft;
};

#define FILELISTSIZE 40
struct FILESYSTEMLIST fileSystemList[FILELISTSIZE]={}; // array of system images
char selectedImage[MAXFILENAMESIZE]={};
/*setMonth is the name since it sets the month. The first parameter is a int 
representing what month it is, the 2nd parameter a string buffer. Where the
month is set. Nothing is returned from the function. This function is used
in the Setdate function, since tm_mon in the dm struct returns the month in
numerical form and we need it to be in alphabetical form*/
void setMonth(int numMonth, char outputMonth[])
{
	switch(numMonth)
	{
		case 1:
			strcpy(outputMonth,"Jan ");
			break;
		case 2:
			strcpy(outputMonth,"Feb ");
			break;
		case 3:
			strcpy(outputMonth,"Mar ");
			break;
		case 4:
			strcpy(outputMonth,"April ");
			break;
		case 5:
			strcpy(outputMonth,"May ");
			break;
		case 6:
			strcpy(outputMonth,"Jun ");
			break;
		case 7:
			strcpy(outputMonth,"Jul ");
			break;
		case 8:
			strcpy(outputMonth,"Aug ");
			break;
		case 9:
			strcpy(outputMonth,"Sep ");
			break;
		case 10:
			strcpy(outputMonth,"Oct ");
			break;
		case 11:
			strcpy(outputMonth,"Nov ");
			break;
		case 12:
			strcpy(outputMonth,"Dec ");
			break;
	}
	
}
/*setDate is the name since it sets the date. The only parameter is a string
buffer, the date gets copied to that. This function returns nothing. This function
gets the current date and time and sets a string to that value.
*/
void setDate(char outputMonth[])
{
      time_t t = time(NULL);
	  struct tm timeNow = *localtime(&t);
	  
	  char date[MAXFILENAMESIZE]={};
	  char month[5]={};
	  setMonth(timeNow.tm_mon,month);
	  strcat(date,month);
	  char day[5]={};
	  sprintf(day,"%d",timeNow.tm_mday);
	  strcat(day," ");
	  strcat(date,day);
	  char hour[5]={};
	  sprintf(hour,"%d",timeNow.tm_hour);
	  strcat(hour,":");
	  strcat(date,hour);
	  char min[5]={};
	  sprintf(min,"%d",timeNow.tm_min);
	  strcat(date,min);
	  strcpy(outputMonth,date);
	
}
/*setLinkList is called that since it sets the link list for any file systems made before
the program was run. The first parameter is an int of the choosenIndex of the file system and
the 2nd parameter is a string for the directory of the choosen file system. Nothing is returned.
The point of this function is to add the files from the previous folders to a the link list
associated with it's file system*/
void setLinkList(int choosenIndex, char target[])
{
	// the code in this function is very similar to our code in set command part of our code
	// in main
	DIR *dir=opendir(target); // all files and folder in our current directory
	
    struct dirent *entity;
    entity=readdir(dir);
	
	while(entity !=NULL) 
	{
		if(strcmp(".", entity->d_name)!=0 && strcmp("..", entity->d_name)!=0)
		{
			struct stat filestat;
			char fileName[MAXFILENAMESIZE];
			strcpy(fileName,entity->d_name);
			
			char nameTemp[4096 ]={};
			strcpy(nameTemp,target); 
			strcat(nameTemp,"/");  
			strcat(nameTemp,entity->d_name); 
			FILE *fp=fopen(nameTemp,"r"); 
			fseek(fp, 0L, SEEK_END);
			// use fseek to get file size
			int size = ftell(fp);
			struct FileNode *temp=malloc(sizeof(struct FileNode));
			temp->start=fileSystemList[choosenIndex].currentIndex;;
			// in unix all hidden files start with .
			if(fileName[0]=='.')
			{
				temp->hidden=1;
				strcpy(fileName,fileName+1);
				// add 1 to the fileName to get rid of the .
				// in the name
				strcpy(temp->fileName,fileName);
			}
			else
			{
				temp->hidden=0;
				
			}
			strcpy(temp->fileName,fileName);
			
			FILE *fpTest=fopen(nameTemp,"a");
			// we open our file in append mode
			if(fpTest)
			{
				// if we can open it in append mode
				// it's not read only
				temp->readOnly=0;
			}
			else
			{
				// if we cannot open it in append mode
				// it's a read only file
				temp->readOnly=1;
			}
			
			fclose(fpTest);
			temp->nextPtr=NULL;
			temp->inUse=1;
			temp->size=size;
			temp->writtenToDisk=1;
			
			stat(nameTemp,&filestat);
			char dateTemp[MAXFILENAMESIZE];
			char dateTemp2[MAXFILENAMESIZE]={};
			strcpy(dateTemp,ctime(&filestat.st_ctime));
			strcpy(dateTemp2,dateTemp);
			int spaceIndex=0;
			
			for(int i=0;i<strlen(dateTemp);i++)
			{
				if(dateTemp[i]==' ')
				{
					spaceIndex=i+1;
					i=strlen(dateTemp)+1;
				}

			}
			
			for(int i=0;i<strlen(dateTemp);i++)
			{
				if(dateTemp2[spaceIndex+i]=='\0')
				{
					dateTemp[i]='\0';
					i=strlen(dateTemp)+1;
				}
				else
				{
					dateTemp[i]=dateTemp2[spaceIndex+i];
				}

			}
			// in the date we don't want the day of
			// the week, so the code above gets rid
			// of it
			int counter=0;

			for(int i=0;i<strlen(dateTemp);i++)
			{
				if(dateTemp[i]==':')
				{
					counter++;
				}
				if(counter==2)
				{
					counter=i;
					i=MAXFILENAMESIZE+1;
				}
			}
			dateTemp[counter]='\0';
			// getting rid of the unnecessary info 
			// from st_ctime
			
			strcpy(temp->date, dateTemp);
			fileSystemList[choosenIndex].totalFiles+=1;
			fileSystemList[choosenIndex].spaceLeft-=size;
			
			int copy_size   = size;
			int offset      = 0; 
			int block_index = temp->start;
			
			while( copy_size > 0 )
			{
				  fseek( fp, offset, SEEK_SET );
				  
				  int bytes;
				  bytes=fread(fileSystemList[choosenIndex].fileData[block_index],BLOCK_SIZE,1,fp);
				  
				  if( bytes == 0 && !feof( fp ) )
				  {
					printf("An error occured reading from the input file.\n");
					copy_size=-1;
				  }
				  clearerr( fp );
				  copy_size -= BLOCK_SIZE;
				  offset    += BLOCK_SIZE;
				  block_index ++;
			}
			temp->end=block_index;
			fileSystemList[choosenIndex].currentIndex=block_index+1; 
			// next blocks will start to be allocated one block over
			// from the current block index
			fclose( fp );
			  
			struct FileNode *prev=NULL;
			struct FileNode *temp2=fileSystemList[choosenIndex].fs;
			// head of link list
			  
			while(temp2 !=NULL)
			{
				  prev=temp2;
				  temp2=temp2->nextPtr;
			}
			  
			if(prev!=NULL)
			{
				  prev->nextPtr=temp;
				  // we are adding an file to the file link list
			}
			else
			{
				  fileSystemList[choosenIndex].fs=temp;
				  // we are the begining of the file link list
			} 
		}
		
		entity=readdir(dir); // picks the next element in our directory 
	}
}

static int CIndex=1; // index of which file system we are in
/*setSystem is the name since it looks through the current directory and looks for any folders
and adds them to the file system array and sets their values. No parameter or return valuee.
When you are making and saving any file systems with my program, you need the data about those 
previous file systems to still be in the system and that's what this function does.
*/
void setSystem()
{
	DIR *dir=opendir("./"); // all files and folder in our current directory
	
    struct dirent *entity;
    entity=readdir(dir);// first element in the our current directory
	// keep looping until there is nothing left in our current directory
	while(entity!=NULL)
	{
		// we use opendir() to check the directory exists
		if((opendir(entity->d_name)!=NULL && strcmp(".", entity->d_name)!=0) &&
		(strcmp("..", entity->d_name)!=0) && strcmp(".git", entity->d_name)!=0)
        { 
			strcpy(fileSystemList[CIndex].fileSystemName,entity->d_name);
			fileSystemList[CIndex].fs = NULL;
			// initialize the first file index in linked list to null
			fileSystemList[CIndex].totalFiles=0;
			fileSystemList[CIndex].spaceLeft=TOTALSPACE;
			fileSystemList[CIndex].currentIndex=4;
			for(int i=0;i<NUM_BLOCKS;i++)
			{
				strcpy(fileSystemList[CIndex].fileData[i],"\0");
			}
			char target[4096];
			getcwd(target, sizeof(target));
			strcat(target,"/");
			strcat(target,entity->d_name);
			setLinkList(CIndex,target);
			
            CIndex++;
        }
		entity=readdir(dir); // picks the next element in our directory 
	}
	
	
}



int main()
{

  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
  setSystem();
   // add any previosly created file systems to our array
  // of file systems
  
  
  while( 1 )
  {
	
    // Print out the mfs prompt
    printf ("mfs> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );
	int l = strlen(cmd_str);
	// here i'm just getting rid of the newline
	
    if (cmd_str[l-1] == '\n')
	{
        cmd_str[l-1] = 0;
	}

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;                                         
                                                           
    char *working_str  = strdup( cmd_str );                

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    int token_index  = 0;
	
	char Dir[4096];
    getcwd(Dir, sizeof(Dir));
	
	int selection=4;
				  
    for(int i=0;i<FILELISTSIZE;i++)
    {
		  if(strcmp(fileSystemList[i].fileSystemName,selectedImage)==0)
		  {
				selection=i;
				i=FILELISTSIZE+1;
		  }
    }
	// get index of our selected file system
	
	
	if(token[0]!=NULL && strcmp(token[0],"quit")==0)
	{
		exit(0);
	}
	else if(token[0]!=NULL && strcmp(token[0],"put")==0 && token[1]!=NULL)
	{
		int    status;                   // Hold the status of all return values.
		struct stat buf;                 // stat struct to hold the returns from the stat call
		
		// Call stat with out input filename to verify that the file exists.  It will also 
		// allow us to get the file size. We also get interesting file system info about the
		// file such as inode number, block size, and number of blocks.  For now, we don't 
		// care about anything but the filesize.
		status =  stat( token[1], &buf ); 
		
		// Save off the size of the input file since we'll use it in a couple of places
		int copy_size   = buf . st_size;
		int offset      = 0; 
		int block_index = 0; 
		block_index=fileSystemList[selection].currentIndex;
		// has the index of which fileData block we last left off on
	
		if(strlen(token[1])>MAXFILENAMESIZE)
		{
			printf("put error: File name too long.\n");
		}
		else if(strlen(selectedImage)<1 && status != -1)
		{
			
			printf("put error: No System Image Open\n");
			
		}
		else if(fileSystemList[selection].spaceLeft-copy_size<0)
		{
			printf("put error: Not enough disk space.\n");
		}
		else if(fileSystemList[selection].totalFiles>124)
		{
			
			printf("put error: Max file count reached.\n");
		}
		else if(copy_size>10240000)
		{
			printf("put error: No inserting files beyond 10,240,000 bytes in size.\n");
		}
		else
		{
			  // if an image is selected we will put it in the fileData array block associated
			  //with that name
			  // If stat did not return -1 then we know the input file exists and we can use it.
			  if( status != -1 )
			  {

				  // Open the input file read-only 
				  FILE *ifp = fopen ( token[1], "r" );
				  
				  struct FileNode *temp=malloc(sizeof(struct FileNode));
				  char date[MAXFILENAMESIZE]={};
				  strcpy(temp->fileName,token[1] );
				  temp->start=block_index;
				  temp->hidden=0;
				  temp->readOnly=0;
				  temp->nextPtr=NULL;
				  temp->inUse=1;
				  temp->writtenToDisk=0;
				  temp->size=buf.st_size;
				  setDate(date);
				  strcpy(temp->date, date);
				  // initialize a new file node 
				  fileSystemList[selection].totalFiles+=1;
				  fileSystemList[selection].spaceLeft-=copy_size;
				  // decrease total space by the newly added file
				  
				  //copy_size is initialized to the size of the input file so each loop iteration
				  //we will copy BLOCK_SIZE bytes from the file then reduce our copy_size counter
				  //by BLOCK_SIZE number of bytes. When copy_size is less than or equal to zero we
				  //have copied all the data from the input file.
				  while( copy_size > 0 )
				  {
					  // Index into the input file by offset number of bytes.  Initially offset
					  //is set to
					  // zero so we copy BLOCK_SIZE number of bytes from the front of the file.  We 
					  // then increase the offset by BLOCK_SIZE and continue the process.  This will
					  // make us copy from offsets 0, BLOCK_SIZE, 2*BLOCK_SIZE, 3*BLOCK_SIZE, etc.
					  fseek( ifp, offset, SEEK_SET );
					  
					  int bytes=fread(fileSystemList[selection].fileData[block_index],BLOCK_SIZE,
					  1,ifp);
					  // Read BLOCK_SIZE number of bytes from the input file and store them in our
					  // data array. 
					  
					  if( bytes == 0 && !feof( ifp ) )
					  {
						printf("An error occured reading from the input file.\n");
						copy_size=-1;
					  }
					  // Clear the EOF file flag.
					  clearerr( ifp );
					  // Reduce copy_size by the BLOCK_SIZE bytes.
					  copy_size -= BLOCK_SIZE;
					  // Increase the offset into our input file by BLOCK_SIZE.  This will allow
					  // the fseek at the top of the loop to position us to the correct spot.
					  offset    += BLOCK_SIZE;
					  block_index ++;
				  }
				  temp->end=block_index;
				  // sets end block of the file node to the last block index
				  fileSystemList[selection].currentIndex=block_index+1;
				  // this will make it so the next start value for the next file node 
				  // will be one over from the end of the previosly added file node's end
				  fclose( ifp );
				  
				  int begining=0;
				  for(int i=0;i<FILELISTSIZE;i++)
				  {
					  if(strcmp(fileSystemList[i].fileSystemName,selectedImage)==0)
					  {
						  begining=i;
						  i=FILELISTSIZE+1;
					  }
				  }
				  
				  struct FileNode *prev=NULL;
				  struct FileNode *temp2=fileSystemList[begining].fs;// head of link list
				  
				  while(temp2 !=NULL)
				  {
					  prev=temp2;
					  temp2=temp2->nextPtr;
				  }
				  
				  if(prev!=NULL)
				  {
					  // we are adding an file to the file link list
					  prev->nextPtr=temp;
				  }
				  else
				  {
					  // we are the begining of the file link list
					  fileSystemList[begining].fs=temp;
				  }    
				 
			  }
		}
				
	}
	else if(token[0]!=NULL && strcmp(token[0],"savefs")==0)
	{
		  struct FileNode *temp= fileSystemList[selection].fs;
		  
		  char Dir[4096];
		  getcwd(Dir, sizeof(Dir));
		  
		  int copy_size   = 0;
		  int offset      = 0; 
		  int block_index = 0;
	
		  while(temp !=NULL)
		  {
			  char nameTemp[4096]={};
			  char nameTemp3[4096]={};
			 
			  strcpy(nameTemp,Dir); 
			  strcat(nameTemp,"/");  
			  strcat(nameTemp,selectedImage);
			  strcat(nameTemp,"/");
			  
			  char tempFileName[MAXFILENAMESIZE]={};
			  strcpy(tempFileName,temp->fileName);
			  
			  strcpy(nameTemp3,nameTemp);
			  strcat(nameTemp3,tempFileName);
			  remove(nameTemp3);
			  // we don't want to add another . if it's already there
			  if(temp->hidden && tempFileName[0]!='.')
			  {
				  // in unix all hidden files start with .
				  char nameTemp2[MAXFILENAMESIZE]=".";
				  strcat(nameTemp2,tempFileName);
				  strcpy(tempFileName,nameTemp2);
			  }
			  
			  strcat(nameTemp,tempFileName);
			  remove(nameTemp);
			  // remove the file if it was already in that directory
			  if(temp->inUse)
			  {
				  FILE *ofp;
			  
				  ofp = fopen(nameTemp, "w");
				  temp->writtenToDisk=1;
				  block_index = 0;
				  block_index=temp->start;
				  copy_size   = temp->size;
				  offset      = 0;
				  
				  while( copy_size > 0 &&  block_index<=temp->end)
				  {
					  int num_bytes;
					  // If the remaining number of bytes we need to copy is less than BLOCK_SIZE then
					  // only copy the amount that remains. If we copied BLOCK_SIZE number of bytes we'd
					  // end up with garbage at the end of the file.
					  if( copy_size < BLOCK_SIZE )
					  {
						num_bytes = copy_size;
					  }
					  else 
					  {
						num_bytes = BLOCK_SIZE;
					  }
					  
					  fwrite( fileSystemList[selection].fileData[block_index], num_bytes, 1, ofp ); 
					  // copies file data to fileData array in our file system struct
					  copy_size -= BLOCK_SIZE;
					  offset    += BLOCK_SIZE;
					  block_index ++;
					  // Since we've copied from the point pointed to by our current file pointer, increment
					  // offset number of bytes so we will be ready to copy to the next area of our output file.
					  fseek( ofp, offset, SEEK_SET );	
				  }
				  
				  if(temp->readOnly)
				  {
					  int ret=chmod(nameTemp,S_IRUSR|S_IRGRP|S_IROTH); // makes a file read only
				  }
				  
				  fclose( ofp );
			  }
			  
			  temp=temp->nextPtr;
	  }
		
		
	}
	else if(token[0]!=NULL && strcmp(token[0],"get")==0 && token[1]!=NULL )
	{
		struct FileNode *temp= fileSystemList[selection].fs;
		int count=0;
		
		while(temp !=NULL)
		{	
			if(strcmp(temp->fileName,token[1])==0)
			{
				remove(token[1]);// need to remove previous file if
				// it's already there, if not we will run into problems
				// making it read only
				count++;
				FILE *ofp;
				char nameTemp[MAXFILENAMESIZE]={};
				
				
				if(token[2]!=NULL)
				{
					strcpy(nameTemp,token[2]);
					if(temp->hidden)
					{
						// hidden files start with .
						char nameTemp2[MAXFILENAMESIZE]=".";
						strcat(nameTemp2,nameTemp);
						ofp = fopen(nameTemp2, "w");
					}
					else
					{
						ofp = fopen(nameTemp, "w");
					}
					
				}
				else
				{
					
					strcpy(nameTemp,token[1]);
					if(temp->hidden)
					{
						// hidden files start with .
						char nameTemp2[MAXFILENAMESIZE]=".";
						strcat(nameTemp2,nameTemp);
						ofp = fopen(nameTemp2, "w");
					}
					else
					{
						ofp = fopen(nameTemp, "w");
					}
					
				}
			
				
				
				int block_index = 0;
				block_index=temp->start;
			    int copy_size   = temp->size;
			    int offset      = 0;
			    // Using copy_size as a count to determine when we've copied enough bytes to the output file.
				// Each time through the loop, except the last time, we will copy BLOCK_SIZE number of bytes from
				// our stored data to the file fp, then we will increment the offset into the file we are writing to.
				// On the last iteration of the loop, instead of copying BLOCK_SIZE number of bytes we just copy
				// how ever much is remaining ( copy_size % BLOCK_SIZE ).  If we just copied BLOCK_SIZE on the
				// last iteration we'd end up with gibberish at the end of our file. 
				
				// we also loop through the fileData array at the start block and end at end block
			    while( copy_size > 0 && block_index<=temp->end)
			    {
					  int num_bytes;
					  
					  // If the remaining number of bytes we need to copy is less than BLOCK_SIZE then
					  // only copy the amount that remains. If we copied BLOCK_SIZE number of bytes we'd
					  // end up with garbage at the end of the file.
					  
					  if( copy_size < BLOCK_SIZE )
					  {
						num_bytes = copy_size;
					  }
					  else 
					  {
						num_bytes = BLOCK_SIZE;
					  }
					  // Write num_bytes number of bytes from our data array into our output file.
					  fwrite( fileSystemList[selection].fileData[block_index], num_bytes, 1, ofp ); 
					  
					  copy_size -= BLOCK_SIZE;
					  offset    += BLOCK_SIZE;
					  block_index ++;
					  //Reduce the amount of bytes remaining to copy, increase the offset into
					  //the file and increment the block_index to move us to the next data block.
					  
					  fseek( ofp, offset, SEEK_SET );
					  // Since we've copied from the point pointed to by our current file pointer,
					  //increment offset number of bytes so we will be ready
					  // to copy to the next area of our output file.
					  
			    }
					
				if(temp->readOnly)
			    {
				    int ret=chmod(nameTemp,S_IRUSR|S_IRGRP|S_IROTH);// makes a file read only   
			    }
				// Close the output file, we're done.
			    fclose( ofp );	
				break;
			}
			temp=temp->nextPtr;
		}
		
		if(count<1)
		{
			printf("get error: File not found.\n");
		}

		
	}
	else if(token[0]!=NULL && strcmp(token[0],"createfs")==0 && token[1]!=NULL)
	{
		int result = mkdir(token[1], 0777);
		int currentIndex=CIndex;
		strcpy(fileSystemList[currentIndex].fileSystemName,token[1]);
		fileSystemList[currentIndex].fs = NULL;
		// initialize the first file index in linked list to null
		fileSystemList[currentIndex].totalFiles=0;
		fileSystemList[currentIndex].spaceLeft=TOTALSPACE;
		fileSystemList[currentIndex].currentIndex=4;
		for(int i=0;i<NUM_BLOCKS;i++)
		{
			strcpy(fileSystemList[currentIndex].fileData[i],"\0");
		}
		CIndex++; 
		
	}
	else if(token[0]!=NULL && strcmp(token[0],"del")==0 && token[1]!=NULL)
	{
	
		
		struct FileNode *temp= fileSystemList[selection].fs;
		int count=0;
		
		while(temp !=NULL )
		{
			 if(strcmp(temp->fileName,token[1])==0)
			 {
				 if(temp->inUse)
				 {
					 // we check to see if the file hasn't already been deleted 
					 // if it's already deleted this code won't execute 
					 fileSystemList[selection].spaceLeft += temp->size;
				 }
				 count++;
				 temp->inUse=0;
			 }
			 temp=temp->nextPtr;
		}
		
		if(count<1)
		{
			printf("del error: File not found.\n");
		}
		
	}
	else if(token[0]!=NULL && strcmp(token[0],"undel")==0 && token[1]!=NULL)
	{
	
		
		struct FileNode *temp= fileSystemList[selection].fs;
		int found=0;
		
		while(temp !=NULL)
		{
			 if(strcmp(temp->fileName,token[1])==0)
			 {
				 if(temp->inUse==0)
				 {
					 // we check to see if the file is deleted 
					 // if it hasn't been deleted, the code in this block won't execute
					 fileSystemList[selection].spaceLeft -= temp->size;
				 }
				 temp->inUse=1;
				 
				 found++;
				 break;
			 }
			 temp=temp->nextPtr;
		}
		
		if(found<1)
		{
			printf("undel: Can not find the file.\n");
		}
	
		
	}
	else if(token[0]!=NULL && strcmp(token[0],"attrib")==0 && token[1]!=NULL && token[2]!=NULL)
	{
		struct FileNode *temp= fileSystemList[selection].fs;
		int count=0;
		
		while(temp !=NULL)
		{
			if(strcmp(temp->fileName,token[2])==0)
			{
				if(strcmp(token[1],"+h")==0)
				{
					temp->hidden=1;
				}
				else if(strcmp(token[1],"+r")==0)
				{
					temp->readOnly=1;
				}
				else if(strcmp(token[1],"-h")==0)
				{
					temp->hidden=0;
				}
				else if(strcmp(token[1],"-r")==0)
				{
					temp->readOnly=0;
				}
				count++;				
				break;
			}
			 temp=temp->nextPtr;
		}
		
		if(count<1)
		{
			printf("attrib: File not found\n");
		}
		
	}
	else if(token[0]!=NULL && strcmp(token[0],"open")==0 && token[1]!=NULL)
	{
		int found=0;		  
		for(int i=0;i<FILELISTSIZE;i++)
		{
			  if(strcmp(fileSystemList[i].fileSystemName,token[1])==0)
			  {
					found=1;
					selection=i;
					i=FILELISTSIZE+1;
			  }
		}
		
		if(found)
		{
			strcpy(selectedImage,token[1]);

		}
		else
		{
			printf("open: File not found\n");
		}
		
	}
	else if(token[0]!=NULL && strcmp(token[0],"df")==0)
	{
		printf("%d bytes free\n",fileSystemList[selection].spaceLeft);
	}
	else if(token[0]!=NULL && strcmp(token[0],"close")==0)
	{
		memset(selectedImage,'\0',strlen(selectedImage));
		// clears the selectedImage string
	}
	else if(token[0]!=NULL && strcmp(token[0],"list")==0)
	{
		int lookingForHidden=0;
		if(token[1]!=NULL && strcmp(token[1],"-h")==0)
		{
			lookingForHidden=1;
		}
		
		struct FileNode *temp= fileSystemList[selection].fs;
		
		
		int count=0;
		
		while(temp !=NULL )
		{
			if(temp->inUse && temp->hidden==0)
			{
				printf("%d %s %s\n",temp->size,temp->date,temp->fileName);
				count++;
			}
			else if(temp->inUse && lookingForHidden)
			{
				if(temp->writtenToDisk)
				{
					// we don't want the . to be displayed as the file name
					printf("%d %s %s\n",temp->size,temp->date,temp->fileName);
				}
				else
				{
					printf("%d %s %s\n",temp->size,temp->date,temp->fileName);
				}
				count++;
			}
			 temp=temp->nextPtr;
		}
		if(count<1)
		{
			printf("No files found\n");
		}
		
	}
	
	for(int i=0;i<MAX_NUM_ARGUMENTS;i++)
	{
		token[i]=NULL;
	}
	// clear out our tokens
	
	strcpy(fileSystemList[selection].fileData[3],"");
	// free block map
	for(int i=4;i<NUM_BLOCKS;i++)
	{
		// check if anything was written to that block
		if(strlen(fileSystemList[selection].fileData[i])<1)
		{
			strcat(fileSystemList[selection].fileData[3],"1");
		}
		else
		{
			strcat(fileSystemList[selection].fileData[3],"0");
		}
	}
	
	strcpy(fileSystemList[selection].fileData[2],"");
	struct FileNode *temp= fileSystemList[selection].fs;
	int linkedListCount=0;
	// free iNode Map bellow
	while(temp!=NULL)
	{
		// if not in use it's free
		if(temp->inUse==0)
		{
			strcat(fileSystemList[selection].fileData[2],"1");
		}
		else
		{
			strcat(fileSystemList[selection].fileData[2],"0");
		}
		linkedListCount++;
		temp=temp->nextPtr;
	}
	temp = fileSystemList[selection].fs;
	// we are using a linked list for our files
	// so we need to do this to show the inodes as free
	// for the remaining nodes
	for(int i=linkedListCount;i<30;i++)
	{
		strcat(fileSystemList[selection].fileData[2],"1");
		
	}
	
    free( working_root );

  }
  return 0;
}