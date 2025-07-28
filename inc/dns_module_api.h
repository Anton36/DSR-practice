#ifndef DNS_MODULE_API_H
#define DNS_MODULE_API_H





void make_dns_request(char * line);
char *get_qname_from_line(char *domain,int * qname_len);


struct dns_header {
		unsigned qid:16; //id for packet
		unsigned qr:1; // 1 bit for specified is a query(0) or response(1)
		unsigned opcode:4; // specifies what kind of query is this 
		unsigned aa:1; //is the responding server have authority for the domain name
		unsigned tc:1; //os the message truncated ,1 for yes
		unsigned rd:1; // recursive query support
		unsigned ra:1;//set by server ,is the recursion available
		unsigned reserved:3;// reserver for future features
		unsigned rcode:4;// response code for server
        unsigned qdcount:16; //number of entries in question section
		unsigned ancount:16; // number of resource records in the answer section
		unsigned nscount:16; //  number of name server resource records in the authority records section.
		unsigned arcount:16; // number of resource records in the additional records section

}; 

#endif