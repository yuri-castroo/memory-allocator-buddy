#ifndef _BuddyAllocator_h_                   // include file only once
#define _BuddyAllocator_h_
#include <iostream>
#include <vector>
using namespace std;
typedef unsigned int uint;

/* declare types as you need */

class BlockHeader{
public:
	int block_size;
	bool isFull;
	BlockHeader* next;
	BlockHeader(int size, bool isFull, BlockHeader* next);
};

class LinkedList{
	// this is a special linked list that is made out of BlockHeader s. 
public:
	// you need a head of the list
	BlockHeader* head;		
public:
	// adds a block to the list
	void insert (BlockHeader* b){			
			// cout << "[insert] Inserting block b: " << b << endl;
			// cout << "[insert] size: " << b->block_size << endl;
			// cout << "[insert] isFull: " << b->isFull << endl;
			b->isFull = false;
			if (head == NULL){
				head = b;
				b->next = NULL;
			}
			else{
				b->next = head;
				head = b;
			}
		}

		// removes a block from the list
		void remove (BlockHeader* b){  
			// cout << "[remove] Looking for b: " << b << endl;
			// cout << "[remove] size: " << b->block_size << endl;
			// cout << "[remove] isFull: " << b->isFull << endl;
			// cout << "[remove] next: " << b->next << endl;
			BlockHeader* iter = head;
			//no lisst
			if (iter == NULL){
				return;
			}
			//the one to be removed is the head
			if (iter == b){
					head = b->next;
					b->next = NULL;
					return;
			}
			//iterate through list to find block to be removed
			while (iter != NULL){
				if(iter->next == b){
					iter->next = b->next;
					b->next = NULL;
					return;
				}
				iter = iter->next;
			}
		}
		
		//search through linked list for a free block of smallest size
		BlockHeader* findFreeLL(){
			BlockHeader* iter = head;
			while (iter != NULL){
				if (iter->isFull == false){
					return iter;
				}
				else{
					iter = iter->next;
				}
			}
			return NULL;
		}
		
		//get size of linked list ~print out debug~
		int getSize(){
			BlockHeader* iter = head;
			int size = 0;
			while (iter != NULL){
				size += 1;
				iter = iter->next;
			}
			return size;
		}
};


class BuddyAllocator{
private:
	/* declare more member variables as necessary */
	vector<LinkedList> FreeList;
	int basic_block_size;
	int total_memory_size;
	char* start; // pointer to the start of the Free List

private:
	/* private function you are required to implement
	 this will allow you and us to do unit test */

	char* getbuddy (char * addr); 
	// given a block address, this function returns the address of its buddy 
	
	bool arebuddies (BlockHeader* block1, BlockHeader* block2);
	// checks whether the two blocks are buddies are not

	char* merge (char* block1, char* block2);
	// this function merges the two blocks returns the beginning address of the merged block
	// note that either block1 can be to the left of block2, or the other way around

	BlockHeader* split (BlockHeader* block);
	// splits the given block by putting a new header halfway through the block
	// also, the original header needs to be corrected
	
	bool check(int basic_block_size, int total_memory_size);
	//helper to check if inputs are valid
	
	int vecSize(int _basic_block_size, int _total_memory_length); 
	//calculates size of FreeList
	
	int getIndex(int allocSize);
	//finding the index in FreeList based on allocation size 
	
	BlockHeader* findFree(int length);
	//find the next index that is free - calls findFree in LinkedList too
	
	bool mergeAvailable(char* b);
	//before mergeing in order to Free the block, we need to check if we can merge them
	
public:
	BuddyAllocator (int _basic_block_size, int _total_memory_length); 
	/* This initializes the memory allocator and makes a portion of 
	   ’_total_memory_length’ bytes available. The allocator uses a ’_basic_block_size’ as 
	   its minimal unit of allocation. The function returns the amount of 
	   memory made available to the allocator. If an error occurred, 
	   it returns 0. 
	*/ 

	~BuddyAllocator();
	/* Destructor that returns any allocated memory back to the operating system. 
	   There should not be any memory leakage (i.e., memory staying allocated).
	*/ 

	char* alloc(int _length); 
	/* Allocate _length number of bytes of free memory and returns the 
		address of the allocated portion. Returns 0 when out of memory. */ 

	int free(char* _a); 
	/* Frees the section of physical memory previously allocated 
	   using ’my_malloc’. Returns 0 if everything ok. */ 

	void printlist ();
	/* Mainly used for debugging purposes and running short test cases */
	/* This function should print how many free blocks of each size belong to the allocator
	at that point. The output format should be the following (assuming basic block size = 128 bytes):

	[0] (128): 5
	[1] (256): 0
	[2] (512): 3
	[3] (1024): 0
	....
	....
	 which means that at this point, the allocator has 5 128 byte blocks, 3 512 byte blocks and so on.*/
};

#endif 
