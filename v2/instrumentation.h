
#include <stdio.h>
#include <time.h>


#ifdef INSTRUMENTATION
  #pragma message "Compiling with instrumentation"
#endif

#ifndef INSTRUMENTATION
  #define L2_PREFETCH_LINE(cpu_num,base_addr,pf_addr,fill_level) \
    l2_prefetch_line(cpu_num, base_addr,  pf_addr, fill_level)
  #define L2_PREFETCH_OPERATE_INSTRUMENTED
  #define L2_PREFETCH_FILL_INSTRUMENTED
  #define L2_PREFETCH_INITIALIZE_INSTRUMENTED
  #define L2_PREFETCH_FINAL_STATS_INSTRUMENTED
#else
  #define L2_PREFETCH_LINE(cpu_num,base_addr,pf_addr,fill_level)	\
    l2_prefetch_line_instrumented(get_current_cycle(0),cpu_num, base_addr,  pf_addr, fill_level)
  #define L2_PREFETCH_OPERATE_INSTRUMENTED(cpu_num, addr,ip,cache_hit) \
    l2_prefetcher_operate_instrumented(get_current_cycle(0), cpu_num,  addr,  ip,  cache_hit)
  #define L2_PREFETCH_FILL_INSTRUMENTED(cpu_num, add, set, way, prefetch, evicted_addr) \
    l2_cache_fill_instrumented(get_current_cycle(0),  cpu_num,  addr,  set,  way,  prefetch,  evicted_addr)
  #define L2_PREFETCH_INITIALIZE_INSTRUMENTED \
    l2_prefetcher_initialize_instrumented();
  #define L2_PREFETCH_FINAL_STATS_INSTRUMENTED \
    l2_prefetcher_final_stats_instrumented();
#endif





/*
 * Implementation
 */

#define INITIALIZE_CODE 0
#define PREFETCH_LINE_CODE 1
#define PREFETCH_OPERATE_CODE 2
#define CACHE_FILL_CODE 3
#define FINAL_STATS_CODE 4
    
    
FILE *traceFile;

void l2_prefetcher_initialize_instrumented(){
  char tBuf[256];
  sprintf(tBuf, "tracefile-%li.trace", time(NULL));
  traceFile = fopen(tBuf,"w");
  if(!traceFile){
     fprintf(stderr, "Can't open trace file!\n");
     exit(1);
  }
}

void l2_prefetch_line_instrumented(unsigned long long int cycles, int cpu_num, unsigned long long int base_addr, unsigned long long int pf_addr, int fill_level){
  fprintf(traceFile,"%llu:%i:%i:%llu:%llu:%i\n",cycles,PREFETCH_LINE_CODE,cpu_num,base_addr,pf_addr,fill_level);
}
void l2_prefetcher_operate_instrumented(unsigned long long int cycles,int cpu_num, unsigned long long int addr, unsigned long long int ip, int cache_hit){
  fprintf(traceFile,"%llu:%i:%i:%llu:%llu:%i\n",cycles,PREFETCH_OPERATE_CODE,cpu_num, addr, ip, cache_hit);
}

void l2_cache_fill_instrumented(unsigned long long int cycles, int cpu_num, unsigned long long int addr, int set, int way, int prefetch, unsigned long long int evicted_addr)
{
  fprintf(traceFile,"%llu:%i:%i:%llu:%i:%i:%i:%llu\n",cycles,CACHE_FILL_CODE,cpu_num,addr,set,way,prefetch,evicted_addr);
}

void l2_prefetcher_final_stats_instrumented(){
  if(traceFile)
    fclose(traceFile);
}
