#ifndef DNS_PERF_API_H
#define DNS_PERF_API_H



void* thread_body(void* arg);






struct thread_statistics
{
  double run_time;
  int target_qps;
  int estimated_qps;
  int estimated_rps;
  int pkt_sent;
  int pkt_recv;
  int ptk_nsent;
  int pkt_drop;
};

struct thread_arg
{
    int P; // number of requests per second for  thread
    int working_time; // working time in seconds
    char *ip; // ip address of the DNS server
    int port; // port of the DNS server
    
};


#endif