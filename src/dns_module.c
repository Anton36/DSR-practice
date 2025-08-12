#include "dns_module_api.h"
#include "linked_list_api.h"
#include "dns_module_inc.h"
#include "main_inc.h"

void make_dns_request(char *line)
{
    if (line == NULL)
    {
        fprintf(stderr, "ERROR: line is NULL.\n");
        return;
    }
    unsigned char buf[MAX_UDP_MESSAGE_SIZE]; // buffer for package
    memset(&buf, 0, sizeof(buf));
    int qname_len = 0;
    struct dns_header *header = (struct dns_header *)&buf; // writing header to packet buffer
    unsigned char *qname_section = NULL;                   // pointer for writing qname
    unsigned char *question_section = NULL;                // pointer to write question_section
    struct dns_question *question = NULL;                  // structure for question

    header->flags.qr = QR_QUERY;
    header->flags.opcode = OPCODE_STANDARD;
    header->flags.tc = TC_NOT_TRUNCATED;
    header->flags.rd = RD_RECURSION_ALLOWED;
    header->qdcount = htons(1); // write results to header keeping network order
    header->ancount = htons(0);
    header->nscount = htons(0);
    header->arcount = htons(0);

    char *pos = strpbrk(line, "\r\n"); // clearing a string of service characters
    if (pos != NULL)
    {
        *pos = '\0';
    }
    // splitting a string into tokens (request class and address)
    char *type_str = strtok(line, " ");
    char *domain = strtok(NULL, " ");

    if (!type_str || !domain)
    {
        fprintf(stderr, "ERROR: Invalid  format.\n");
        return;
    }

    printf("type from function %s \n", type_str);
    printf("domain from function %s \n", domain);

    char *qname = get_qname_from_line(domain, &qname_len);
    if (qname == NULL)
    {
        fprintf(stderr, "ERROR: qname not retrieve.\n");
        return;
    }
    
    if (qname_len > (MAX_UDP_MESSAGE_SIZE - sizeof(struct dns_header))) 
    {
    fprintf(stderr, "ERROR: qname is too large for the buffer.\n");
    return;
    }
    qname_section = buf + sizeof(struct dns_header); // calculating the position for the qname entry
    memcpy(qname_section, qname, qname_len); // insert qname into packet buffer

    question_section = qname_section + qname_len;       // calculating the position for the question section
    question = (struct dns_question *)question_section; // writing question section to packet buffer
    memset(question, 0, sizeof(struct dns_question));

    question->qtype = htons(1);  // 1 for A writing
    question->qclass = htons(1); // 1 for internet

    int packet_size = sizeof(struct dns_header) + qname_len + sizeof(struct dns_question); // Calculating packet size for storage purposes
    list_push(buf, packet_size);                                                           // moving a package to a request list
    free(qname);
}

char *get_qname_from_line(char *domain, int *qname_len)
{
    if(domain == NULL)
    {
        fprintf(stderr, "ERROR: domain can be NULL.\n");
        return NULL;
    }
    char *qname_buffer = malloc(DNS_MAXNAME + 1); // creating a buffer with the maximum size allowed by the specifications
    if (qname_buffer == NULL)
    {

        fprintf(stderr, "ERROR: buffer not initialzed.\n");
        return NULL;
    }
    char *buffer_offset = qname_buffer; // offset for iterating over buffer
    char *domain_copy = strdup(domain); // copy of string since strdup modifies original string
    int all_length = 0;

    char *token = strtok(domain_copy, "."); // split into tokens with a dot separator
    while (token != NULL)
    {
        int token_length = strlen(token); // token length calculation
        if (token_length > DNS_MAXLABEL)  // check for maximum allowed substring length according to specification
        {
            fprintf(stderr, "ERROR: token length to much.\n");
            free(qname_buffer);
            free(domain_copy);
            return NULL;
        }
        if ((all_length + token_length + 1) > DNS_MAXNAME)
        {
            fprintf(stderr, "ERROR: qname is out of max size.\n");
            free(qname_buffer);
            free(domain_copy);
            return NULL;
        }

        *buffer_offset++ = (char)token_length; // write token length to buffer

        memcpy(buffer_offset, token, token_length); // writing token to buffer
        buffer_offset += token_length;              // iteration over token length
        all_length += token_length + 1;
        token = strtok(NULL, ".");
    }

    *buffer_offset++ = 0;                      // qname finalization
    *qname_len = buffer_offset - qname_buffer; // calculate the length of the qname
    free(domain_copy);
    return qname_buffer;
}
