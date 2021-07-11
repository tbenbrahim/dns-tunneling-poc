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
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

void print_buffer(unsigned char *buffer, size_t len) {
  unsigned char preview[17];
  preview[16] = '\0';
  memset(preview, ' ', 16);
  for (int i = 0; i < len; ++i) {
    if (i && i % 16 == 0) {
      printf(" %s\n", preview);
      memset(preview, ' ', 16);
    }
    unsigned char c = buffer[i];
    printf("%02x ", c);
    preview[i % 16] = (c == ' ' || (c >= '!' && c < '~')) ? c : '.';
  }
  for (int i = 0; i < 16 - len % 16; ++i) {
    printf("   ");
  }
  printf(" %s\n", preview);
}
