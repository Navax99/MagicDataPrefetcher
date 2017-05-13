//
// Data Prefetching Championship Simulator 2
// Seth Pugsley, seth.h.pugsley@intel.com
//

/*

  This file does NOT implement any prefetcher, and is just an outline

 */


//#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "../inc/prefetcher.h"
#include "instrumentation.h"

//index table size
#define ITS 512
typedef struct itf_struct{
  unsigned long long addr;
  unsigned int index; //index to the history
} index_table_field;

//history buffer size
#define HBS 512
typedef struct hf_struct{
  unsigned long long addr;
  unsigned int index; //index to the history
  bool valid_index;
} history_field;

static const unsigned int CACHE_EXP = log2(CACHE_LINE_SIZE);

index_table_field    index_table[ITS];
history_field     history_buffer[HBS];
unsigned int history_head = 0;


void l2_prefetcher_initialize(int cpu_num)
{
  L2_PREFETCH_INITIALIZE_INSTRUMENTED

  for(int i = 0 ; i < ITS ; ++i){
	index_table[i].index = 0;
  }

  for(int i = 0 ; i < HBS ; ++i){
	history_buffer[i].addr  = 0;
	history_buffer[i].index = 0;
  }
}

// This function is called once for each Mid Level Cache read, and is the entry point for participants' prefetching algorithms.
// addr - the byte address of the current cache read
// ip - the instruction pointer (program counter) of the instruction that caused the current cache read
// cache_hit - 1 for an L2 cache hit, 0 for an L2 cache miss
void l2_prefetcher_operate(int cpu_num, unsigned long long int addr, unsigned long long int ip, int cache_hit)
{
  L2_PREFETCH_OPERATE_INSTRUMENTED(cpu_num,addr,ip,cache_hit);

  if(!cache_hit){
	history_field new_hfield;
	new_hfield.valid_index = false;
	new_hfield.addr = addr;//>>(CACHE_EXP); //address are at level of line cache
	
	//indexing by line address
	unsigned int index = (addr>>(CACHE_EXP+1)) & (ITS-1);
	index_table_field ifield = index_table[index];
	index_table_field new_ifield;
	new_ifield.addr = addr;
	new_ifield.index = (history_head);
	index_table[index] = new_ifield;

	if(ifield.addr == new_hfield.addr){
	   new_hfield.index = (history_head);
	   new_hfield.valid_index = true;
	   //search prefetch 
	   unsigned int npa = (ifield.index+1)%HBS;
	   history_field next = history_buffer[npa];
	   printf("prefetch\n");
	   L2_PREFETCH_LINE(cpu_num,addr,next.addr,FILL_L2);
	} else { 
		//printf("Expecting %llu\n",ifield.addr);
		//printf("Found %llu\n",new_hfield.addr);
	}
	history_buffer[history_head] = new_hfield;
	history_head = (history_head+1)%HBS;

  }
 
}

// This function is called when a cache block is filled into the L2, and lets you konw which set and way of the cache the block occupies.
// You can use this function to know when prefetched lines arrive in the L2, and along with l2_get_set() and l2_get_way() you can
// reconstruct a view of the contents of the L2 cache.
// Using this function is optional.
void l2_cache_fill(int cpu_num, unsigned long long int addr, int set, int way, int prefetch, unsigned long long int evicted_addr)
{
  L2_PREFETCH_FILL_INSTRUMENTED(cpu_num,addr,set,way,prefetch,evicted_addr);
}

void l2_prefetcher_heartbeat_stats(int cpu_num)
{

}

void l2_prefetcher_warmup_stats(int cpu_num)
{

}

void l2_prefetcher_final_stats(int cpu_num)
{
  L2_PREFETCH_FINAL_STATS_INSTRUMENTED
}
