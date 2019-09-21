#include "Ackerman.h"
#include "BuddyAllocator.h"
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <cmath>

void easytest(BuddyAllocator* ba){
  // be creative here
  // know what to expect after every allocation/deallocation cycle

  // here are a few examples
  ba->printlist();
  // allocating a byte
  char * mem = ba->alloc (1);
  // now print again, how should the list look now
  ba->printlist ();

  ba->free (mem); // give back the memory you just allocated
  ba->printlist(); // shouldn't the list now look like as in the beginning

}

int main(int argc, char ** argv) {
  int c;
  int basic_block_size = 128;
  int memory_length = 1024*1024*128;
  c = getopt (argc, argv, "b:s:");
  while (c!= -1){
    switch (c){
      case 'b':
        basic_block_size = atoi(optarg);
        break;
      case 's':
        memory_length = atoi(optarg);
        break;
      default:
        basic_block_size = 128;
        memory_length = 1024*1024*128;
    }
  }

  if (basic_block_size > memory_length){
    cout << "basic block size bigger than total memory";
		return 0;
	}
	if (ceil(log2(basic_block_size)) != log2(basic_block_size)){
    cout << "basic block size not proper";
		return 0;
	}
	if (ceil(log2(memory_length)) != log2(memory_length)){
    cout << "total memory size not proper";
		return 0;
	}
  
  cout << "basic_block_size: " << basic_block_size << endl;
  cout << "memory_length: " << memory_length << endl;

  // create memory manager
  BuddyAllocator * allocator = new BuddyAllocator(basic_block_size, memory_length);

  // the following won't print anything until you start using FreeList and replace the "new" with your own implementation
  easytest (allocator);

  // test memory manager
  Ackerman* am = new Ackerman ();
  am->test(allocator); // this is the full-fledged test. 
  
  // destroy memory manager
  delete allocator;
}