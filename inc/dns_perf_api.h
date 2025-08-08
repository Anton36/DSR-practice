#ifndef DNS_PERF_API_H
#define DNS_PERF_API_H



void* thread_body(void* arg);






struct thread_statistics
{
  struct timeval run_time;
  int target_qps;
  int estimated_qps;
  int estimated_rps;
  double pkt_sent;
  double pkt_recv;
  double ptk_nsent;
  double pkt_drop;
};

struct thread_arg
{
    int P; // number of requests per second for  thread
    int working_time; // working time in seconds
    char *ip; // ip address of the DNS server
    int port; // port of the DNS server
    
};


#endif