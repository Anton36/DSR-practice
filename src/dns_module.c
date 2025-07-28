#include "dns_module_api.h"
#include "dns_module_inc.h"
#include "main_inc.h"

void make_dns_request(char *line)
{
    int qname_len = 0;
    struct dns_header header; //dns packet header structure
    memset(&header,0,sizeof(header));
    header.qr =QR_QUERY;
    header.opcode = OPCODE_STANDARD;
    header.tc = TC_NOT_TRUNCATED;
    header.rd = RD_RECURSION_ALLOWED;
    header.qdcount = 1;

    char *pos = strpbrk(line, "\r\n"); //clearing a string of service characters
    if (pos != NULL)
    {
        *pos = '\0';
    }
    //splitting a string into tokens (request class and address)
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
    //TODO: serialization of the header and final formation of the packet into a common buffer

}

char *get_qname_from_line(char *domain, int *qname_len)
{
    char *qname_buffer = malloc(DNS_MAXNAME + 1); //creating a buffer with the maximum size allowed by the specifications
    if (qname_buffer == NULL)
    {

        fprintf(stderr, "ERROR: buffer not initialzed.\n");
        return NULL;
    }
    char *buffer_offset = qname_buffer; //offset for iterating over buffer
    char *domain_copy = strdup(domain); //copy of string since strdup modifies original string

    char *token = strtok(domain_copy, "."); //split into tokens with a dot separator
    while (token != NULL)
    {
        int token_length = strlen(token);//token length calculation
        if (token_length > DNS_MAXLABEL) //check for maximum allowed substring length according to specification
        {
            fprintf(stderr, "ERROR: token length to much.\n");
            free(qname_buffer);
            free(domain_copy);
            return NULL;
        }

        *buffer_offset++ = (char)token_length; //write token length to buffer

        memcpy(buffer_offset, token, token_length);//writing token to buffer
        buffer_offset += token_length; //iteration over token length
        token = strtok(NULL, ".");
    }

    *buffer_offset++ = 0; // qname finalization
    *qname_len = buffer_offset - qname_buffer; //calculate the length of the qname
    free(domain_copy);
    return qname_buffer;
    //TODO buffer overflow check
}
