/**
 * Copyright (c) 2021 Tony BenBrahim
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "dns.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void extract_dns_query(unsigned char *dns_buffer,
                       struct dns_query *name_query) {
  unsigned char *query_ptr = dns_buffer + sizeof(struct dns_header);
  name_query->num_segments = 0;
  uint8_t segment_size;
  while ((segment_size = *((uint8_t *)query_ptr))) {
    strncpy(name_query->segment[name_query->num_segments],
            (char *)(query_ptr + 1), segment_size);
    name_query->segment[name_query->num_segments][segment_size] = '\0';
    ++name_query->num_segments;
    query_ptr += segment_size + 1;
  }
  uint16_t *qtype_ptr = (uint16_t *)(query_ptr + 1);
  name_query->type = ntohs(*qtype_ptr);
  uint16_t *qclass_ptr = (uint16_t *)(query_ptr + 3);
  name_query->qclass = ntohs(*qclass_ptr);
}

size_t prepare_response(struct dns_query *name_query, unsigned char *buffer,
                        size_t num_received, uint32_t ttl, char *ip) {
  struct dns_header *header = (struct dns_header *)buffer;
  header->qr = 1;
  header->aa = 0;
  header->tc = 0;
  header->ra = 0;
  switch (name_query->type) {
  case QTYPE_A:
    header->rcode = RESPONSE_SUCCESS;
    header->ancount = htons(1);
    break;
  case QTYPE_AAAA:
    header->rcode = RESPONSE_SUCCESS;
    header->ancount = 0;
    break;
  default:
    header->rcode = RESPONSE_REFUSED;
    header->ancount = 0;
    break;
  }
  header->nscount = 0;
  header->arcount = 0;
  size_t response_length =
      name_query->type == 1 ? num_received + 18 : num_received;
  if (name_query->type == 1) {
    struct dns_response_trailer *trailer =
        (struct dns_response_trailer *)(buffer + num_received);
    trailer->ans_type = 0xc0; // pointer
    trailer->name_offset = 0x0c;
    trailer->type = htons(QTYPE_A);
    trailer->qclass = htons(QCLASS_INET);
    trailer->ttl = htonl(ttl);
    trailer->rdlength = htons(4);
    inet_pton(AF_INET, ip, &trailer->rdata);
  }
  return response_length;
}

void debug_header(struct dns_header *header) {
  printf("id: %d\n", ntohs(header->id));
  printf("qr: %d\n", header->qr);
  printf("opcode: %d\n", header->opcode);
  printf("tc: %d\n", header->tc);
  printf("rd: %d\n", header->rd);
  printf("qdcount: %d\n", ntohs(header->qdcount));
}

void debug_name(struct dns_query *name_query) {
  printf("Name query: ");
  for (int i = 0; i < name_query->num_segments; ++i) {
    printf("%s%s", name_query->segment[i],
           i == name_query->num_segments - 1 ? " " : ".");
  }
  printf("type %02x class %02x\n", name_query->type, name_query->qclass);
}