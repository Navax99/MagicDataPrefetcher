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



//history buffer size
#define HBS 256
#define SLSIZE 4

typedef struct sl_struct{
	long long stride;
  unsigned long long length;
} sl_field;

typedef struct hf_struct{
  unsigned long long addr;
	sl_field sl[SLSIZE];
} history_field;

static const unsigned int CACHE_EXP = log2(CACHE_LINE_SIZE);

history_field     history_buffer[HBS];
unsigned int history_head = 1;
unsigned int history_tail = 0;

void add_entry(history_field h){
	history_buffer[history_head] = h;
	if(history_head == history_head){
		history_tail = (history_tail+1)%HBS;
	}
	history_head = (history_head+1)%HBS;

}

void init_field(history_field* hf){
	for(unsigned int i = 0 ; i < SLSIZE ; ++i){
		hf->sl[i].length = 0;
		hf->sl[i].stride = 0;
	}
}

void l2_prefetcher_initialize(int cpu_num)
{
  L2_PREFETCH_INITIALIZE_INSTRUMENTED

  for(int i = 0 ; i < HBS ; ++i){
		history_buffer[i].addr  = 0;
		for(int j = 0 ; j < SLSIZE ; ++j){
			history_buffer[i].sl[j].stride = 0;
			history_buffer[i].sl[j].length = 0;
  	}
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
		new_hfield.addr = addr>>(CACHE_EXP); //address are at level of line cache
		init_field(&new_hfield);
		
		for(unsigned int i = history_tail ; i != history_head ; i = (i+1)%HBS ){
			history_field hbf = history_buffer[i];
			long long stride = new_hfield.addr - hbf.addr;
			
			bool found = false;
			//check for all stride fields
			for(unsigned int ii = 0 ; ii < SLSIZE && !found ; ++ii){
				if(hbf.sl[ii].stride == stride && hbf.sl[ii].length > 0){
					new_hfield.sl[ii].stride = stride;
					new_hfield.sl[ii].length = hbf.sl[ii].length + 1;
					found = true;
				}
			}
				
		  if(!found) {
				for(unsigned int j = i ; j != history_head ; j = (j+1)%HBS ){
					history_field hbfj = history_buffer[j];
					long long stridej = hbf.addr - hbfj.addr;
					if(stride == stridej){
						bool found2 = false;
						for(unsigned int jj = 0 ; jj < SLSIZE && !found2 ; ++jj){
							if(new_hfield.sl[jj].stride == 0){
								new_hfield.sl[jj].stride = stride;
								new_hfield.sl[jj].length = 1;
								found2 = true;
							}
						}
						/*if(!found2){
								new_hfield.sl[0].stride = stride;
								new_hfield.sl[0].length = 1;
						}*/
						break;
					}
				}
		  }
  	}
  			
	
		add_entry(new_hfield);
		
		long long stride = 0;
		unsigned long long length = 0;
		for(unsigned int i = 0 ; i < SLSIZE ; ++i){
			if(	new_hfield.sl[i].length > length ){
				stride = new_hfield.sl[i].stride;
				length = new_hfield.sl[i].length;
			}
		}
		L2_PREFETCH_LINE(cpu_num,addr,addr+stride,FILL_L2);
		L2_PREFETCH_LINE(cpu_num,addr,addr+2*stride,FILL_L2);
	  L2_PREFETCH_LINE(cpu_num,addr,addr+3*stride,FILL_L2);
	  L2_PREFETCH_LINE(cpu_num,addr,addr+4*stride,FILL_L2);
	
		//for(unsigned int i = 0 ; i < length ; ++i ){

			//saddr += stride;
		//}
	   
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
	for(int i = 0 ; i < HBS ; ++i){
		if(history_buffer[i].addr > 0){
			history_field h = history_buffer[i];
			//printf("addr: %llu s0: %ll l0: %llu s1: %ll l1: %llu s2: %ll l2: %llu s3: %ll l3: %llu \n",history_buffer[i].addr,h.sl[0].stride,h.sl[0].length,h.sl[1].stride,h.sl[1].length,h.sl[2].stride,h.sl[2].length,h.sl[3].stride,h.sl[3].length);
			printf("addr:%llu-s0:%ll-l0:%llu-s1:%ll-l1:%llu-s2:%ll-l2:%llu-s3:%ll-l3:%llu\n",history_buffer[i].addr,h.sl[0].stride,h.sl[0].length,h.sl[1].stride,h.sl[1].length,h.sl[2].stride,h.sl[2].length,h.sl[3].stride,h.sl[3].length);
		}
	}
	printf("==========");
	printf("============");
	printf("==========");
}

void l2_prefetcher_warmup_stats(int cpu_num)
{

}

void l2_prefetcher_final_stats(int cpu_num)
{
  L2_PREFETCH_FINAL_STATS_INSTRUMENTED
}
