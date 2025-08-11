#ifndef DNS_PERF_API_H
#define DNS_PERF_API_H



void* thread_body(void* arg);






struct thread_statistics
{
  struct timeval run_time;
  int target_qps;
  int estimated_qps;
  int estimated_rps;
  uint64_t pkt_sent;
  uint64_t pkt_recv;
  uint64_t ptk_nsent;
  uint64_t pkt_drop;
};

struct thread_arg
{
    int P; // number of requests per second for  thread
    int working_time; // working time in seconds
    char *ip; // ip address of the DNS server
    int port; // port of the DNS server
    
};


#endif