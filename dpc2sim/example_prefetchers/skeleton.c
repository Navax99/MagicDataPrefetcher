//
// Data Prefetching Championship Simulator 2
// Seth Pugsley, seth.h.pugsley@intel.com
//

/*

  This file does NOT implement any prefetcher, and is just an outline

 */

#include <stdio.h>
#include "../inc/prefetcher.h"
#define MAX_REFERENCES 10
#define MAX_NODES 10

struct reference {
	int score;
  	struct dir_node *pointer;
};

struct dir_node {
	unsigned long long int dir;
	struct reference ref_list[MAX_REFERENCES];
};	

int last_addr=-1;
struct dir_node Graph[MAX_NODES];

void init_structures(){
	int i,j;
	for(i=0;i<MAX_NODES;++i){
		for(j=0;j<MAX_REFERENCES;++j){
			struct reference tmp_ref;
			tmp_ref.score=0;
			Graph[i].ref_list[j]=tmp_ref;
		}
	        Graph[i].dir=-1;
	}
}

void l2_prefetcher_initialize(int cpu_num)
{
  printf("No Prefetching\n");
  init_structures();
  // you can inspect these knob values from your code to see which configuration you're runnig in
  printf("Knobs visible from prefetcher: %d %d %d\n", knob_scramble_loads, knob_small_llc, knob_low_bandwidth);
}

void l2_prefetcher_operate(int cpu_num, unsigned long long int addr, unsigned long long int ip, int cache_hit)
{
  // uncomment this line to see all the information available to make prefetch decisions
 // printf("(%d) ",(int) (addr-last_addr)); 
  //printf("(0x%llx 0x%llx %d %d %d)\n ", addr, ip, cache_hit, get_l2_read_queue_occupancy(0), get_l2_mshr_occupancy(0));
  last_addr=addr;
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
