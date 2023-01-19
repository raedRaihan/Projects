/*
	Name: Raed Ali, Luc Pham
	ID: 1001567598, 1001918323
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "mavalloc.h"


#define totalAllocations 100

enum TYPE  
{
  H = 0, // Hole, can be reused 
  P  // Process allocation, cannot be reused
}; 

struct Node
{
	size_t size;
	enum TYPE type;
	void * arena;
	int inUse;
	
};

static struct Node ledger[totalAllocations]={0}; // our linked list
static void *Arena=NULL; // Arena

static int lastUsed = -1;
#define ROOTNODE 0  
static int nextFitStart=0; // start node for nextfit
static int remainderSpace=INT_MAX; // holds the remainder from best fit
// any int in our scope should be less than INT_MAX
// so that's why i initialize it to that
static int remainderSpaceWF=0;// holds the remainder for worst fit
static int pickedBlockBF=0;// index of the picked best fit block
static int pickedBlockWF=0;// index of the picked worst fit block

 
static enum ALGORITHM choosenAlgo; // choosen algorithm by user

int findFreeNodeInternal()
{
	int i = 0;
	
	/**
	 *  Start searching the array beginning at the 0th element
	 *  and once we've found an element not in use we'll drop out 
	 *  and return the index that is free. 
	*/
	
	for (i = 0; i < totalAllocations; i++)
	{
		if (ledger[i].inUse == 0) 
		{
			return i;
		}
	}
	return -1;
	
	
}

void * insertNodeInternal(int previous, void *mem)
{
	int i = 0;  
	
	
	if ( previous < 0 || previous >= totalAllocations)
	{
		printf("ERROR: Tried to insert a node beyond our bounds %d\n", previous);
		return NULL;
	}


	/**
	 * Shift everything down and make room for the new node. But we only
	 * need to shift from lastUsed to previous + 1
	*/
	if (lastUsed == -1)
	{
		ledger[i].arena = mem;
		
		ledger[i].inUse = 1;
		lastUsed = i;
	}
	else
	{
	
		for (i = 10; i >= previous; i--) // shifitng 
		{
			
			ledger[i + 1].inUse = ledger[i].inUse;
			
		}
	
		ledger[previous].inUse = 1;

		lastUsed++;
	}
	
	return ledger[previous].arena;
}

void * insertNode(int previous, size_t size, void *mem)
{
	/*  Hold the index of the node we will insert behind */
	int index    =  findFreeNodeInternal(); 
	
	void *ret = NULL;

	if(index >= 0)
	{
		/** If we found a free node and we haven't run off the end of the list */
		if (previous >= -1)
		{
			ret = insertNodeInternal(previous, mem);
			
		}
		else if (previous >= totalAllocations || previous < 0)
		{
			printf("Error: Tried to insert beyond the bounds of the allocated list.\n");

		}
		// calculates the remainder remainder bellow 
		ledger[index].size=ledger[previous].size-size;
		ledger[previous].size = size;
		ledger[index].arena=mem+size;
		ledger[index].inUse = 1;	
		
	}
	
	return ret;
}


int mavalloc_init( size_t size, enum ALGORITHM algorithm )
{
	 
    lastUsed = -1;
	nextFitStart=0;
	pickedBlockBF=0;
	remainderSpace=INT_MAX;
	remainderSpaceWF=0;
	pickedBlockWF=0;
	// resets the global variables when init is called
	
	for(int i=0;i<totalAllocations;i++)
	{
		ledger[i].inUse=0;
		ledger[i].size=0;
		ledger[i].arena=NULL;
		ledger[i].type=H;
	}
	
	Arena= malloc(ALIGN4(size)); // initializes arena
	
	if(Arena==NULL) // if malloc failed
	{
		return -1;
	}
	
	ledger[0].arena=Arena;
	ledger[0].type=H;
	ledger[0].size = ALIGN4(size);
	ledger[0].inUse = 1;
	
	
	choosenAlgo=algorithm;
	
	
    return 0;
}

void mavalloc_destroy( )
{
	
	free(Arena);
	memset(ledger, 0, sizeof(ledger)); // empties out ledger 
	return;
}

void * mavalloc_alloc( size_t size )
{
	void *ret=NULL;	// what we return

	int foundblock=0;
	int foundblockWF=0;
	int notGoThrough=1; // checks to see if a loop was made of all the ledger entries
	
	switch(choosenAlgo)
	 {
		 case NEXT_FIT :
			
			for( int i=nextFitStart;i<totalAllocations;i++)
			{
				if(ledger[i].inUse && ledger[i].size >= ALIGN4(size) && ledger[i].type==H)
				{
					ledger[i].type=P;
					ret=ledger[i].arena;
					
					// split a node if it's bigger than requested 
					// Splitting means inserting a new node with the leftover space
					if(ledger[i].size>ALIGN4(size))
					{
						// 1. calculate remainder in insert node
						ret=insertNode(i,ALIGN4(size), ledger[i].arena);
						// 2. insert a new node, splitting happens here
						
					}
					
					nextFitStart=i; // keeps track of where we start
					
					
					ledger[i].size=ALIGN4(size);
					i=totalAllocations+1;
				}
				else if(i==totalAllocations-1 && notGoThrough)
				{
					i=-1; // goes back to begining
					// -1 since the for loop will 
					// increment it to 0
					notGoThrough=0;
				}
				else if(i==totalAllocations-1 && notGoThrough==0)
				{// if a full loop was made of the ledger it's time to get out of the loop
					i=totalAllocations+1;
					
				}
			}
			break;
		 case BEST_FIT :
			foundblock=0;
			for(int i=0;i<totalAllocations;i++)
			 {
				 // we are looking for a block that has less remainderSpace then the current
				 // selected block
				 if(ledger[i].inUse && ledger[i].type==H && ledger[i].size - ALIGN4(size) >= 0)
				 {
					 if(remainderSpace>=ledger[i].size - ALIGN4(size))
					 {
						remainderSpace=ledger[i].size - ALIGN4(size);
						pickedBlockBF=i;// keeps track of the pciked block
						foundblock=1;
						i+=totalAllocations+1;
					 }
				 }
				 
			 }
			if(foundblock)
			{
				// return null if the requested size is
				// larger the size of the picked block
				if(ledger[pickedBlockBF].size<ALIGN4(size))
				{
					return NULL;
				}
				ledger[pickedBlockBF].type=P;
				ret=ledger[pickedBlockBF].arena;
				
				// split a node if it's bigger than requested 
				// Splitting means inserting a new node with the leftover space
				if(ledger[pickedBlockBF].size>ALIGN4(size))
				{
					 // 1. calculate the remainder size
					ret=insertNode(pickedBlockBF,ALIGN4(size), ledger[pickedBlockBF].arena);
					// 2. insert a new node
				}		
				
				ledger[pickedBlockBF].size=ALIGN4(size);
			}
			break;
		case WORST_FIT :
			
			foundblockWF=0;
			for(int i=0;i<totalAllocations;i++)
			 {
				 // we are looking for a block that has more remainderSpace then 
				 // the current selected block
				 if(ledger[i].inUse && ledger[i].size - ALIGN4(size) >= 0 && ledger[i].type==H)
				 {
					 if(remainderSpaceWF<=ledger[i].size - ALIGN4(size))
					 {
						remainderSpaceWF=ledger[i].size - ALIGN4(size);
						pickedBlockWF=i;
						foundblockWF=1;
						i+=totalAllocations+1;
					 }
					 
				 }

			 }
			ret=ledger[pickedBlockWF].arena;
			if(foundblockWF)
			{
				// return null if the requested size is
				// larger the size of the picked block
				if(ledger[pickedBlockWF].size<ALIGN4(size))
				{
					return NULL;
				}
				ledger[pickedBlockWF].type=P;
				
				
				// split a node if it's bigger than requested 
				// Splitting means inserting a new node with the leftover space
				if(ledger[pickedBlockWF].size>ALIGN4(size))
				{
					// 1. calculate remainder in insert node
					ret=insertNode(pickedBlockWF,ALIGN4(size), ledger[pickedBlockWF].arena); 
					// 2. insert a new node
				}		
				ledger[pickedBlockWF].size=ALIGN4(size);
			}
			break;
		case FIRST_FIT :
			
			for(int i=0;i<totalAllocations;i++)
			{
				if(ledger[i].inUse && ledger[i].size >= ALIGN4(size) && ledger[i].type==H)
				{
					ledger[i].type=P;
					ret=ledger[i].arena;
					
					// split a node if it's bigger than requested 
					// Splitting means inserting a new node with the leftover space
					if(ledger[i].size>ALIGN4(size))
					{
						// 1. calculate remainder in insert node
						ret=insertNode(i,ALIGN4(size), ledger[i].arena);
						// 2. insert a new node, splitting happens here
					}
					
					
					
					ledger[i].size=ALIGN4(size);
					i=totalAllocations+1;
				}
			}
			break;
	 }
	 
	 

	 // only return NULL on failure
	 return ret;
}

void mavalloc_free( void * ptr )
{
	int i = 0;
	for( i = 0; i < totalAllocations; i++ )
	{

		if( ledger[i].arena == ptr )
	    {
			ledger[i].type = H;
			// make the index of the pointer
			// we are trying to free a hole
			// since we can reuse it
	
		}
		//combine nodes here
		// combine the past index and current
		// index if they are both holes
		if(ledger[i].type==H && i>0 ) 
		{
			// can't do the code bellow if i=0
			// that's why we check if i>0
			//check if previous node is a hole
			if(ledger[i-1].type == H)
			{
				
				ledger[i].inUse=0;
				if(ledger[i+1].type==H)
				{
					ledger[i+1].inUse=0;
				}
				
				ledger[i].arena=ledger[i-1].arena ;
				// move the previous arena up
				ledger[i].size= ledger[i-1].size+ledger[i].size;
				// combine the node sizes
				i=totalAllocations+1;
			}
			
		}
		
	}
	
	return;
}

int mavalloc_size( )
{
	int numNodes=0;
	int i=0;
	for(i=0;i < totalAllocations;i++)
	{
		if(ledger[i].inUse )
		{
			numNodes++;
		}
			
	}
	return numNodes;
}