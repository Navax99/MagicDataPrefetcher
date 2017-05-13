//
// Data Prefetching Championship Simulator 2
// Seth Pugsley, seth.h.pugsley@intel.com
//

/*

  This file does NOT implement any prefetcher, and is just an outline

 */

#include <stdio.h>
#include <stdlib.h>
#include "../inc/prefetcher.h"

long long int stride=0;
long long last_addr=-1;
void l2_prefetcher_initialize(int cpu_num)
{
  printf("No Prefetching\n");
  // you can inspect these knob values from your code to see which configuration you're runnig in
  printf("Knobs visible from prefetcher: %d %d %d\n", knob_scramble_loads, knob_small_llc, knob_low_bandwidth);
}

void l2_prefetcher_operate(int cpu_num, unsigned long long int addr, unsigned long long int ip, int cache_hit)
{
  // uncomment this line to see all the information available to make prefetch decisions
 // printf("(%d) ",(int) (addr-last_addr)); 
  //printf("(0x%llx 0x%llx %d %d %d)\n ", addr, ip, cache_hit, get_l2_read_queue_occupancy(0), get_l2_mshr_occupancy(0));
  if(last_addr==-1) last_addr=addr;
  else {
	stride = llabs(addr-((long long)last_addr));
	if(stride < PAGE_SIZE && !cache_hit && get_l2_read_queue_occupancy(cpu_num) < 32 && get_l2_mshr_occupancy(cpu_num)<16 ){
		 printf("(%lli) \n ",stride); 
		 int succ = l2_prefetch_line(cpu_num,addr,addr+stride,FILL_L2);
		 if(!succ)
			printf("Prefetch have fail %i\n",succ);
	}
 	last_addr = addr;
  }
}

void l2_cache_fill(int cpu_num, unsigned long long int addr, int set, int way, int prefetch, unsigned long long int evicted_addr)
{
  // uncomment this line to see the information available to you when there is a cache fill event
  //printf("0x%llx %d %d %d 0x%llx\n", addr, set, way, prefetch, evicted_addr);
}

void l2_prefetcher_heartbeat_stats(int cpu_num)
{
  printf("Prefetcher heartbeat stats\n");
}

void l2_prefetcher_warmup_stats(int cpu_num)
{
  printf("Prefetcher warmup complete stats\n\n");
}

void l2_prefetcher_final_stats(int cpu_num)
{
  printf("Prefetcher final stats\n");
}
