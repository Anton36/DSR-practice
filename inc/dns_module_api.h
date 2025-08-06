#ifndef DNS_MODULE_API_H
#define DNS_MODULE_API_H
#include "dns_module_inc.h"
void make_dns_request(char *line);
char *get_qname_from_line(char *domain, int *qname_len);
void* thread_body(void* arg);


#pragma pack(push, 1)
struct dns_header
{
  uint16_t id;
  struct dns_flags
  {
    uint8_t rd : 1;
    uint8_t tc : 1;
    uint8_t aa : 1;
    uint8_t opcode : 4;
    uint8_t qr : 1;
    uint8_t rcode : 4;
    uint8_t z : 3;
    uint8_t ra : 1;
  } flags;
  uint16_t qdcount;
  uint16_t ancount;
  uint16_t nscount;
  uint16_t arcount;
};


#pragma pack(pop)
struct dns_question
{
  unsigned short qtype;
  unsigned short qclass;
};







#endif