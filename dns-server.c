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

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "debug.h"
#include "dns.h"

#define TTL 300

int main(int argc, char **argv) {
  printf("dns-server 0.0.1 😉\n");
  int sockfd;
  unsigned char buffer[MAX_BUFFER_SIZE];
  struct sockaddr_in servaddr, cliaddr;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  memset(&cliaddr, 0, sizeof(cliaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(DNS_PORT);

  if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  socklen_t len = sizeof(cliaddr);
  for (;;) {
    memset(buffer, 0, sizeof(buffer));
    int num_received = recvfrom(sockfd, (char *)buffer, MAX_BUFFER_SIZE,
                                MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);
    char client_addr_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(cliaddr.sin_addr), client_addr_str, INET_ADDRSTRLEN);
    printf("---------------------------\nReceived %d bytes from %s\n",
           num_received, client_addr_str);
    print_buffer(buffer, num_received);

    struct dns_header *header = (struct dns_header *)buffer;
    debug_header(header);

    struct dns_query name_query;
    extract_dns_query(buffer, &name_query);
    debug_name(&name_query);

    int response_length = prepare_response(&name_query, buffer, num_received,
                                           300, "16.32.64.128");

    if (sendto(sockfd, buffer, response_length, 0, (struct sockaddr *)&cliaddr,
               sizeof(cliaddr)) == -1) {
      perror("sendto failed");
    }
    printf("Response:\n");
    print_buffer(buffer, response_length);
  }
}