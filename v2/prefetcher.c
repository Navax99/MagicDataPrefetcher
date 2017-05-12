//
// Data Prefetching Championship Simulator 2
// Seth Pugsley, seth.h.pugsley@intel.com
//

/*

  This file does NOT implement any prefetcher, and is just an outline

 */


//#include <stdio.h>
#include <stdlib.h>
#include "../inc/prefetcher.h"
#include "instrumentation.h"

void l2_prefetcher_initialize(int cpu_num)
{
  L2_PREFETCH_INITIALIZE_INSTRUMENTED
}

// This function is called once for each Mid Level Cache read, and is the entry point for participants' prefetching algorithms.
// addr - the byte address of the current cache read
// ip - the instruction pointer (program counter) of the instruction that caused the current cache read
// cache_hit - 1 for an L2 cache hit, 0 for an L2 cache miss
void l2_prefetcher_operate(int cpu_num, unsigned long long int addr, unsigned long long int ip, int cache_hit)
{
  L2_PREFETCH_OPERATE_INSTRUMENTED(cpu_num,addr,ip,cache_hit);
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
