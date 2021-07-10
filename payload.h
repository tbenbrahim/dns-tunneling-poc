#ifndef _PAYLOAD
#define _PAYLOAD 1

#include <stdint.h>
#include <sys/types.h>
#include <uuid/uuid.h>

#define BLOCKSIZE 120

struct __attribute__((__packed__)) dns_payload {
  uuid_t uuid;
  uint32_t sequence;
  uint8_t length;
  char data[BLOCKSIZE];
};

#endif
