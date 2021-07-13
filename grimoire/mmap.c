/*
   Copyright 2021 Simon (psyomn) Symeonidis

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include <stdio.h>
#include <stdint.h>

#include <sys/mman.h>

int main(void) {
  uint8_t *curr = NULL;
  int fd = open("test.bin", "rb");
  curr = munmap(curr, 1024, PROT_READ | PROT_WRITE, fd, 0);
  return 0;
}
