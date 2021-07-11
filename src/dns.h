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

#ifndef _DNS_H
#define _DNS_H 1

#include <stdint.h>
#include <sys/types.h>

#define MAX_BUFFER_SIZE 512
#define DNS_PORT 53

#define RESPONSE_SUCCESS 0
#define REPONSE_FORMAT_ERROR 1
#define RESPONSE_FAILURE 2
#define RESPONSE_NAME_ERROR 3
#define RESPONSE_REFUSED 5

#define QTYPE_A 0x01
#define QTYPE_AAAA 0x1C

#define QCLASS_INET 0x0001

// All uint16 are in network byte order!!!
struct __attribute__((__packed__)) dns_header {
  uint16_t id;

  unsigned int rd : 1;
  unsigned int tc : 1;
  unsigned int aa : 1;
  unsigned int opcode : 4;
  unsigned int qr : 1;

  unsigned int rcode : 4;
  unsigned int z : 3;
  unsigned int ra : 1;

  uint16_t qdcount;
  uint16_t ancount;
  uint16_t nscount;
  uint16_t arcount;
};

struct __attribute__((__packed__)) dns_response_trailer {
  uint8_t ans_type;
  uint8_t name_offset;
  uint16_t type;
  uint16_t qclass;
  uint32_t ttl;
  uint16_t rdlength;
  uint32_t rdata;
};

struct dns_query {
  size_t num_segments;
  char segment[10][64];
  uint16_t type;
  uint16_t qclass;
};

void extract_dns_query(unsigned char *dns_buffer, struct dns_query *name_query);
void debug_header(struct dns_header *header);
void debug_name(struct dns_query *name_query);
size_t prepare_response(struct dns_query *name_query, unsigned char *buffer,
                        size_t num_received, uint32_t ttl, char *ip);

#endif