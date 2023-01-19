/*
	Name: Raed Ali
	ID: 1001567598
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 10     

#define PATH_MAX 4096


int main()
{

  char * command_string = (char*) malloc( MAX_COMMAND_SIZE );
  char history[15][MAX_COMMAND_SIZE+1]={}; // 2d array for our commmands
  long int pidList[20]={0};  // array for our pids
  int commandNum=0;
  int pidNum=0;
  // first i just put in a null terminator in each element of the array
  // since later i'll be checking the elements in this array are empty or not
  for(int i=0;i<15;i++)
  {
	  strcpy(history[i],"\0");
  }
  
  while( 1 )
  {
	char commandCopy[MAX_COMMAND_SIZE]={};
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (command_string, MAX_COMMAND_SIZE, stdin) );
	
	// the code bellow is for the user rerunning a previous command from the history
	char userSelectedHistory[MAX_COMMAND_SIZE]={};
	char userSelectedHistory2[MAX_COMMAND_SIZE]={};
	strcpy(userSelectedHistory,command_string);
	int l = strlen(userSelectedHistory);
	// here i'm just getting rid of the newline
    if (userSelectedHistory[l-1] == '\n')
	{
        userSelectedHistory[l-1] = 0;
	}
	strcpy(userSelectedHistory2,userSelectedHistory); 
	// we have a copy since we are tokenizing userSelectedHistory
	
	if(userSelectedHistory[0]=='!') // if the command starts with !
	{
		char *token2;
		token2=strtok(userSelectedHistory,"!"); // we tokenize the command to get the 
		//input after the !
		int userSelectedCommand=-1;
		
		if(token2!=NULL) 
		{
			userSelectedCommand=atoi(token2); // use atoi to get the input after the ! to be an int
		}
		if(commandNum>14)
		{
			userSelectedCommand=userSelectedCommand-1;
			// i noticed whenever the number of commands in history goes above 14
			// userSelectedCommand will be one off so for a simple fix I just
			// subtract it by one
			commandNum=14;
		}
		// make sure the command entered is within our range 
		// also make sure a command exists at the index entered by the user
		if(strcmp(history[userSelectedCommand],"\0")!=0)
		{
			if(userSelectedCommand<15&&userSelectedCommand>=0)
			{
				strcpy(command_string,history[userSelectedCommand]); 
			}
			
			// if it is valid, copy the element fromn that index of the history array
			// into command_string, so that command will be rerun
		}
		else
		{
			printf("Command not in history.\n");
		}
		strcpy(history[commandNum],userSelectedHistory2); 
		// adds userSelectedHistory2 to the history
		commandNum++;
		
	}
    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;                                         
                                                           
    char *working_string  = strdup( command_string );                

    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *head_ptr = working_string;

    // Tokenize the input strings with whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_string, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    // Now print the tokenized input as a debug check
	
	int isNotNull=0;
	// just test to see if the user entered anything in any of the indexes
	for(int i=0;i<MAX_NUM_ARGUMENTS;i++)
	{
		if(token[i]!=NULL)
		{
			isNotNull=1;
			// if something was entered isNotNull=1
		}
	}
	
	
	// im using the ints bellow to test the command was a built in
	// and if so what kind
	int cdCheck=1;
	int cdPosition;
	int historyCheck=1;
	int listpidsCheck=1;
	int builtIn=0;
	
	for(int i=0;i<MAX_NUM_ARGUMENTS;i++)
	{
		if(token[i] !=NULL)
		{
			
			if(strcmp(token[i],"cd")==0)
			{
				cdCheck=0;
				cdPosition=i;
			}
			else if(strcmp(token[i],"history")==0)
			{
				historyCheck=0;
			}
			else if(strcmp(token[i],"listpids")==0)
			{
				listpidsCheck=0;
			}
			
			
		}
		
	}
	// this block bellow executes the built in commands if a built in command was typed in
	if(isNotNull) 
	{
		if(token[0]!=NULL &&(strcmp(token[0],"exit")==0 ||strcmp(token[0],"quit")==0 ))
		{
			exit(0);
		}
		else if(!historyCheck) // if the user typed history execute the code bellow
		{
			builtIn=1;
			
			for(int i=0;i<15;i++)
			{
				if(strcmp(history[i],"\0")!=0)
				{
					printf("%d: %s\n",i,history[i]);
				}

			}
			
			strcpy(history[commandNum],"history");
			
		}
		else if(!cdCheck) // if the user typed cd 
		{
			char Dir[PATH_MAX];
			builtIn=1;
			strcat(commandCopy,token[cdPosition] ); 
			strcpy(history[commandNum],commandCopy);
			// if the argument after cd was ..
			if(token[cdPosition+1]!=NULL && strcmp(token[cdPosition+1],"..")==0) 
			{
				 chdir(".."); // this changes the directory to the root folder
				 getcwd(Dir, sizeof(Dir));
				 strcat(commandCopy," " ); 
				 strcat(commandCopy,token[cdPosition+1] ); 
				 strcpy(history[commandNum],commandCopy);
			}
			else if(token[cdPosition+1]!=NULL) // if the argument after cd is not null
			{    //this part changes the directory to the 2nd argument entered
				//by the user if the command was valid
				chdir(token[cdPosition+1]);
				getcwd(Dir, sizeof(Dir));
				strcat(commandCopy," " ); 
				strcat(commandCopy,token[cdPosition+1] ); 
				strcpy(history[commandNum],commandCopy);
				
			}		
		}
		else if(!listpidsCheck) // if the user typed in listpids
		{
			builtIn=1;
			for(int i=0;i<20;i++)
			{
				if(pidList[i]!=0)
				{
					printf("%d: %ld\n",i,pidList[i]);
				}
			}
			
			strcpy(history[commandNum],"listpids");
			
		}
		// after each command is executed i copy the command using a combination 
		//of strcpy and strcatr
	}
	
	// all built in commands happen before the fork
	// all the non-built ins require fork
	if(!builtIn) // if the call is not a built in
	{
		// bellow i just copied the user input to a temp string and i only did this for 
		//the lines that weren't NULL
		if(token[0]!=NULL)
		{
			strcat(commandCopy,token[0]);
		}
		
		for(int i=1;i<MAX_NUM_ARGUMENTS;i++)
		{
			if(token[i]!=NULL)
			{
				strcat(commandCopy," ");
				strcat(commandCopy,token[i]);
			}
			
		}
		if(isNotNull)
		{
			strcpy(history[commandNum],commandCopy);
			
		}
		else
		{
			strcpy(history[commandNum],"\0");
		}
		
		pid_t pid= fork(); // fork gets called
		
		if(pid==-1) // there was an error
		{
			perror("Error allocating a process\n");
			exit(0);
		}
		else if(pid==0) // we are in the child
		{
			
			if(!isNotNull) // nothing was typed in 
			{
				exit(0);
			}
			else
			{
				
				char *arguments[MAX_NUM_ARGUMENTS]={}; // all the non NULL arguments will go here
				
				arguments[0] = NULL;
				if(token[0] !=NULL)
				{
					arguments[0] = ( char * ) malloc( strlen( token[0] ) );
					
					strncpy( arguments[0], token[0], strlen( token[0] ) );
					 // copy all the non null arguments
					arguments[0][ strlen( token[0] )]='\0';
					// add the null terminator 
					
				}
				// we do the same thing above but for the other tokens
				for(int i=1;i<MAX_NUM_ARGUMENTS;i++)
				{
					arguments[i] = NULL;
					if(token[i] !=NULL)
					{
						arguments[i] = ( char * ) malloc( strlen( token[i] ) );
						
						// add the null terminator 
						strncpy( arguments[i], token[i], strlen( token[i] ) );
						 // copy all the non null arguments
						arguments[i][ strlen( token[i] )]='\0';
						
						
					}
					
				}
				
				
				for(int i=0;i<MAX_NUM_ARGUMENTS;i++)
				{
					int ret=0;
					
					if(arguments[i]!=NULL)
					{
						ret=execvp( arguments[i], &arguments[i] );
						  // we use execvp to execute our commands
					}
					
					if( ret == -1 )
					{
						i=MAX_NUM_ARGUMENTS;
						printf("%s: Command not Found.\n",commandCopy);
						
					}
					
				}
				exit(0);
				
			}
			
		}
		else // we are in the parent and the pid equals the childs pid
		{
			
			int status;
			char commandCopy[MAX_COMMAND_SIZE];
			wait( & status );
			pidList[pidNum]=pid; // copy the pid from the child into the pid array
			
		}
	}
	
	if(isNotNull)
	{
		commandNum++;
		// if commandNum is above 15, that means we went over our limit 
		// so i have this code just to save the last 15 of the commands and get rid 
		// of the code before the previous 15 commands from the history 
		if(commandNum>15)
		{
			for(int i=0;i<14;i++)
			{
				strcpy(history[i],history[i+1]);
			}
			commandNum=15;
		}
		// I did the same thing for the pid array, except none of the built ins have pids
		// so i check to make sure the command wasn't a built in
		if(!builtIn)
		{
			pidNum++;
		    if(pidNum>20)
			{
				for(int i=0;i<19;i++)
				{
					pidList[i]=pidList[i+1];
				}
				commandNum=19;
			}
		}
		
	}
    free( head_ptr );
  }
  return 0;
 
}
