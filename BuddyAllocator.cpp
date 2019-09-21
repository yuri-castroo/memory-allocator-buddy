#include "BuddyAllocator.h"
#include <iostream>
#include <cmath>
#include <vector>
using namespace std;

// ~~~~~~~~~~ BLOCK HEADER CLASS IMPLEMENTATION ~~~~~~~~~~ //
BlockHeader::BlockHeader(int size, bool isFull = false, BlockHeader* next = NULL){
  this->isFull = isFull;
  this->block_size = size;
  this->next = next;
}

// ~~~~~~~~~~ BUDDY ALLOCATOR CLASS IMPLEMENTATION ~~~~~~~~~~ //
BuddyAllocator::BuddyAllocator(int _basic_block_size, int _total_memory_length){
	//Test Inputs
	// while (check(_basic_block_size, _total_memory_length) == false){
	// 	cin >> _basic_block_size;
	// 	cin >> _total_memory_length;
	// }
	
	//Initialize Allocator Members
	basic_block_size = _basic_block_size;
	total_memory_size = _total_memory_length;
	
	start = new char[_total_memory_length];
	//set size of FreeList 
	int vectorSize = log2(_total_memory_length) - log2(_basic_block_size) + 1; 
  	FreeList.resize(vectorSize); 
	//place an empy linked list at each index
	for (int i = 0; i < FreeList.size(); i++){
		FreeList.at(i) = LinkedList();
	}
	
	BlockHeader* first = (BlockHeader*) start;
	first->block_size = total_memory_size;
	first->isFull = false;
	first->next = NULL;
	FreeList[getIndex(first->block_size)].insert(first);

	printlist();
}

BuddyAllocator::~BuddyAllocator (){
	delete[] start;
}

char* BuddyAllocator::alloc(int _length) {
   	/* This preliminary implementation simply hands the call over the 
		the C standard library! 
    	Of course this needs to be replaced by your implementation.
  	*/
	cout << "~~~~~ ALLOC ~~~~~~" << endl;
  	int realLength = _length + sizeof(BlockHeader);
	//requested is longer than allocated
	if (realLength > total_memory_size){
		return NULL;
	}
	//requested is smaller than basic block size
	if (realLength < basic_block_size){
		realLength = basic_block_size;
	}
	//after checking, get index to allocate in FreeList
	else{
		realLength = pow(2,ceil(log2(realLength)));
	}

	BlockHeader* free = findFree(realLength);
	if (free == NULL){
		return NULL;
	}
	
	while(free->block_size > realLength){
		free = split(free);
	}
	
	FreeList[getIndex(realLength)].remove(free);
	free->isFull = true;
	printlist();
	return (char*) free + sizeof(BlockHeader);
	
}

int BuddyAllocator::free(char* _a) {
	cout << "~~~~~ FREE ~~~~~~" << endl;
	//remove the blockheader portion
	_a = _a - sizeof(BlockHeader);
	//block is already empty
	if (((BlockHeader*) _a)->isFull == false){
		return 0;
	}
	//find blocks to merge and then merge
	while (((BlockHeader*) _a)->block_size < total_memory_size && mergeAvailable(_a)){
		_a = merge(_a, getbuddy(_a));
	}
	//indicate that it is free now
	((BlockHeader*) _a)->isFull = false;
	printlist();
	return 0;
}

void BuddyAllocator::printlist (){
  //cout << "Printing the Freelist in the format \"[index] (block size) : # of blocks\"" << endl;
  for (int i=0; i<FreeList.size(); i++){
    cout << "[" << i <<"] (" << ((1<<i) * basic_block_size) << ") : ";  // block size at index should always be 2^i * basic_block_sizes
    int count = 0;
    BlockHeader* b = FreeList [i].head;
    // go through the list from head to tail and count
    while (b){
      count ++;
      // block size at index should always be 2^i * basic_block_sizes
      // checking to make sure that the block is not out of place
      if (b->block_size != (1<<i) * basic_block_size){
        cerr << "ERROR:: Block is in a wrong list" << endl;
        exit (-1);
      }
      b = b->next;
    }
    cout << count << endl;  
  }
}

char* BuddyAllocator::getbuddy(char* addr){
    BlockHeader* curr = (BlockHeader*) addr;
	//calculating the address of the buddy for a given address
    unsigned long offset = (unsigned long)addr - (unsigned long)start;
	uint blockSize = curr->block_size;
    return (char*) ((offset ^ (blockSize)) + (unsigned long)start);
}

bool BuddyAllocator::arebuddies(BlockHeader* block1, BlockHeader* block2){
	BlockHeader* block1Buddy = (BlockHeader*)getbuddy((char*) block1);
  	//get buddy of block1 and check if it is block2 
  	if (block1Buddy != block2){
    	return false;
  	}
  	//compare sizes.
  	int block1size = block1->block_size;
  	int block2size = block2->block_size;
  	if(block1size != block2size){
   	 return false;
  	}
  	return true;
}	

char* BuddyAllocator::merge(char* block1, char* block2){
	cout << "~~~~~ MERGE ~~~~~~" << endl;
	BlockHeader* blockHead1 = (BlockHeader*) block1;
	BlockHeader* blockHead2 = (BlockHeader*) block2;

	//switch it around if needed
	if (blockHead1 > blockHead2){
		blockHead1 = (BlockHeader*) block2;
		blockHead2 = (BlockHeader*) block1;
	}
	
	blockHead1->block_size = blockHead1->block_size + blockHead2->block_size;

	int index1 = getIndex(blockHead1->block_size);
  	int index2 = getIndex(blockHead2->block_size);
	FreeList[index2].remove(blockHead2);
	FreeList[index2].remove(blockHead1);
	FreeList[index1].insert(blockHead1);
	blockHead1->isFull = false;
	
	printlist();
	return (char*) blockHead1;
}

BlockHeader* BuddyAllocator::split(BlockHeader* block){
	cout << "~~~~~ SPLIT ~~~~~~" << endl;
	int size = block->block_size;
	//split it and indicate it is free
  	block->block_size = size/2;
	block->isFull = false; 

	//the other hald of the block we just split 
	BlockHeader* newBlock = (BlockHeader*) ((char*) block + (size /2));
	newBlock->block_size = size/2;
	newBlock->isFull = false;
	newBlock->next = NULL; 

	int index = getIndex(size/2); 
	FreeList[index+1].remove(block);
	block->next = NULL; 

	FreeList[index].insert(newBlock);
	FreeList[index].insert(block);
	return block;
}

int BuddyAllocator::getIndex(int allocSize){
	return ceil(log2(allocSize)) - log2(basic_block_size);
}

BlockHeader* BuddyAllocator::findFree(int length){
	  int i = getIndex(length); 
	BlockHeader* free = NULL; 
	while(i <= getIndex(total_memory_size)){
		free = FreeList[i].findFreeLL();
		if (free != NULL){
		return free;
		}
		i++;
	}
	return free;
}

bool BuddyAllocator::mergeAvailable(char* _a){
	char* budAddress = getbuddy(_a);
	BlockHeader* a = (BlockHeader*) _a;
	if (!arebuddies((BlockHeader*)_a, (BlockHeader*) budAddress)){
		return false;
	}
	if (((BlockHeader*) budAddress)->isFull == true){
		return false;
	}
	return true;
}

bool BuddyAllocator::check(int basic_block_size, int total_memory_size){
	if (basic_block_size > total_memory_size){
		return false;
	}
	if (ceil(log2(basic_block_size)) != log2(basic_block_size)){
		return false;
	}
	if (ceil(log2(total_memory_size)) != log2(total_memory_size)){
		return false;
	}
	return true;
}

int BuddyAllocator::vecSize(int _basic_block_size, int _total_memory_length){
	return log2(_total_memory_length) - log2(_basic_block_size) + 1;
}