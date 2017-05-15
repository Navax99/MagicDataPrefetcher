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
#define HBS (256)
#define DBS 128
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

unsigned int prefetch_degree = 1;


#define PENALIZER_MAX_RATE 3
unsigned long long degree_buffer[DBS];
unsigned int degree_head = 0;
unsigned int degree_tail = 0;
unsigned int penalizer_rate = 0;

history_field     history_buffer[HBS];
unsigned int history_head = 0;
unsigned int history_tail = 0;
//bool first_entry = true;

unsigned long long int longest_length =0;
long long int longest_stride =0;

void add_entry(history_field h){
	history_buffer[history_head] = h;
	if(history_tail == (history_head+1)%HBS ){
		history_tail = (history_tail+1)%HBS;
	}
	history_head = (history_head+1)%HBS;

}

void add_degree_entry(unsigned long long addr){
	degree_buffer[degree_head] = addr;
	if(degree_tail == (degree_head+1)%DBS){
		degree_tail = (degree_tail+1)%DBS;
	}
	degree_head = (degree_head+1)%DBS;

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

#define MAX_LENGTH 20
unsigned long long saturated_add(unsigned long long a, unsigned long long b, unsigned long long MAX){
	return a+b>MAX ? MAX : a+b;
}

long long int last_stride = 0;
unsigned int total_failed_misses = 0;
// This function is called once for each Mid Level Cache read, and is the entry point for participants' prefetching algorithms.
// addr - the byte address of the current cache read
// ip - the instruction pointer (program counter) of the instruction that caused the current cache read
// cache_hit - 1 for an L2 cache hit, 0 for an L2 cache miss
void l2_prefetcher_operate(int cpu_num, unsigned long long int addr, unsigned long long int ip, int cache_hit)
{
  L2_PREFETCH_OPERATE_INSTRUMENTED(cpu_num,addr,ip,cache_hit);
  
  //Degree control code. Check if the accesed line was prefeched
  bool found_degree  = false;
  for (unsigned int i = degree_head; i != degree_tail && !found_degree ; i = (i-1)%DBS) {
  		if(addr>>(CACHE_EXP)==degree_buffer[i]){
  			found_degree = true;
  		}
 
  }
  
  //Select a degree grade
  if(!found_degree && !cache_hit){
		penalizer_rate = 0;
  } else if(!found_degree && cache_hit) {
  	//penalizer_rate = 0;
  } else if(found_degree && !cache_hit){
  	++penalizer_rate;
  	if(penalizer_rate == PENALIZER_MAX_RATE){
  		if(prefetch_degree == 4) prefetch_degree=2;
  		else if(prefetch_degree == 2) prefetch_degree=1;
  		penalizer_rate = 0;
  	}
  } else if(found_degree && cache_hit)  {
    if(prefetch_degree == 1) prefetch_degree=2;
  	else if(prefetch_degree == 2) prefetch_degree=4;
  	penalizer_rate = 0;
  }
  //////////////////////////
  
  
  if(!cache_hit){
		history_field new_hfield;
		new_hfield.addr = addr>>(CACHE_EXP); //address are at level of line cache
		init_field(&new_hfield);

		//iterate from the newest to the oldest in the history buffer (Step 1)
		for(unsigned int i = history_head ; i != history_tail ; i = (i-1)%HBS ){

			history_field hbf = history_buffer[i];
			long long stride = ((long long)new_hfield.addr) - ((long long)hbf.addr);
			bool found = false;
			//check for all stride fields searching for a common stream
			for(unsigned int ii = 0 ; ii < SLSIZE && !found ; ++ii){
				if(hbf.sl[ii].stride == stride && hbf.sl[ii].length > 0 && hbf.sl[ii].stride > 0){
					
					new_hfield.sl[ii].stride = stride;
					new_hfield.sl[ii].length = saturated_add(hbf.sl[ii].length, 1, MAX_LENGTH);
					found = true;
				}
			}
		   
		   //If there's not a common stream, try to create a new one
		  if(!found) {
		  		//Step 2, seach a new stream
				for(unsigned int j = (i-1)%HBS  ; j != history_tail ; j = (j-1)%HBS ){
					history_field hbfj = history_buffer[j];
					long long stridej = hbf.addr - hbfj.addr;
					
					//if it finds a stride, we have a new stream
					if(stride == stridej && stridej > 0 && stride > 0){
						
						bool found2 = false;
						//search for a slot
						for(unsigned int jj = 0 ; jj < SLSIZE && !found2 ; ++jj){
							if(new_hfield.sl[jj].stride == 0){
								new_hfield.sl[jj].stride = stride;
								new_hfield.sl[jj].length = 1;
								found2 = true;
							}
						}
						break;
					}
				}
		  }
		}
  		
  		//add miss address to the buffer
		add_entry(new_hfield);
		//history_field h = new_hfield;
		
		long long stride = 0;
		unsigned long long length = 0;
		for(unsigned int i = 0 ; i < SLSIZE ; ++i){
			if(	new_hfield.sl[i].length > length ){
				stride = new_hfield.sl[i].stride;
				length = new_hfield.sl[i].length;
			}
		}

	  if(longest_length < length  || (longest_length == MAX_LENGTH && longest_length == length)){
			longest_length = length;
			longest_stride = stride;
		}
		
		//if there is a valid stream, take the longest stream of the history and prefetch it
		if(length > 0){
			if(get_l2_mshr_occupancy(0) > 8){
				for (unsigned int i = 1; i <= prefetch_degree; i++) {
					L2_PREFETCH_LINE(cpu_num,addr,addr+CACHE_LINE_SIZE*longest_stride*i,FILL_LLC);
				}
	  	} else {
	  			for (unsigned int i = 1; i <= prefetch_degree; i++) {
						L2_PREFETCH_LINE(cpu_num,addr,addr+CACHE_LINE_SIZE*longest_stride*i,FILL_L2);
						add_degree_entry(((unsigned long long int)(addr+CACHE_LINE_SIZE*longest_stride*i)) >> CACHE_EXP);
					}
	  	}

		}

	   
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
	/*for(int i = 0 ; i < HBS ; ++i){
		if(history_buffer[i].addr > 0){
			history_field h = history_buffer[i];
			
		//	printf("addr:%llu s0:%lli l0:%llu s1:%lli l1:%llu s2:%lli l2:%llu s3:%lli l3:%llu\n",history_buffer[i].addr,h.sl[0].stride,h.sl[0].length,h.sl[1].stride,h.sl[1].length,h.sl[2].stride,h.sl[2].length,h.sl[3].stride,h.sl[3].length);
		}
	}*/
	//printf("==========\n");
	//printf("============\n");
	//printf("==========\n");
}

void l2_prefetcher_warmup_stats(int cpu_num)
{

}

void l2_prefetcher_final_stats(int cpu_num)
{
  L2_PREFETCH_FINAL_STATS_INSTRUMENTED
 // printf("length %llu stride %lli\n",longest_length,longest_stride);
 // printf("faild misses %u",total_failed_misses);
}
