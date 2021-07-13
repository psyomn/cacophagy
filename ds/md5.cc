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
#include "md5.h"

#include <iostream>

namespace psy::ds {
  // constants taken from https://en.wikipedia.org/wiki/MD5

  constexpr size_t kShiftSize = 64;
  const std::uint8_t kShift[kShiftSize] = {
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21,
  };

  constexpr size_t kSineSize = 64;
  const std::uint32_t kSine[kSineSize] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
  };

  const std::uint32_t kA0 = 0x67452301;
  const std::uint32_t kB0 = 0xefcdab89;
  const std::uint32_t kC0 = 0x98badcfe;
  const std::uint32_t kD0 = 0x10325476;

  std::array<std::uint32_t, 4> MD5::Digest(const std::vector<std::uint8_t>& message) {
    const auto padded_message = PadMessage(message);
    const size_t padded_size = padded_message.size();

    std::uint32_t A0 = kA0, B0 = kB0, C0 = kC0, D0 = kD0;

    std::uint32_t a = 0, b = 0, c = 0, d = 0;

    // for each chunk of 512 bits
    for (size_t i = 0; i < padded_size; i += 64) {
      a = A0; b = B0; c = C0; d = D0;

      std::array<uint32_t, 16> chunk = {0};
      ExtractChunk(padded_message, i, i+64, chunk);

      for (size_t j = 0; j < kShiftSize; ++j) {
        std::uint32_t F = 0, G = 0;

        if (j >= 48 && j <= 63) {
          // F := C xor (B or (not D))
          // g := (7×i) mod 16
          F = c ^ (b | (~d));
          G = (7 * i) % 16;
        } else if (j >= 32 && j <= 47) {
          // F := B xor C xor D
          // g := (3×i + 5) mod 16
          F = b ^ c ^ d;
          G = (3 * i + 5) % 16;
        } else if (j >= 16 && j <= 31) {
          // F := (D and B) or ((not D) and C)
          // g := (5×i + 1) mod 16
          F = (d & b) | ((!d) & c);
          G = (5 * i + 1) % 16;
        } else if (j <= 15) {
          // F := (B and C) or ((not B) and D)
          // g := i
          F = (b & c) | ((!b) & d);
          G = i;
        }

        // M[g] must be a 32-bits block
        F = F + a + kSine[j] + chunk[G];
        a = d;
        d = c;
        c = b;

        // B := B + leftrotate(F, s[i])
        const std::uint8_t shift = kShift[j];
        b = b + ((F << shift) | (F >> (32 - shift)));
      }

      A0 += a;
      B0 += b;
      C0 += c;
      D0 += d;
    }

    return std::array<std::uint32_t, 4>{A0, B0, C0, D0};
  }

  std::vector<std::uint8_t> MD5::PadMessage(const std::vector<std::uint8_t>& message) {
    // 64 byte chunks
    const size_t msg_size = message.size();
    const size_t remainder = msg_size % 64;
    const size_t surplus_bytes = 64 - remainder - 1 - 8;

    std::vector<std::uint8_t> padded(message);
    padded.push_back(0x80);

    for (size_t i = 0; i < surplus_bytes; ++i)
      padded.push_back(0);

    const std::uint64_t msg_size_64 = std::uint64_t(msg_size);

    padded.push_back(0xff & msg_size_64);
    padded.push_back(0xff & (msg_size_64 >> 8));
    padded.push_back(0xff & (msg_size_64 >> 16));
    padded.push_back(0xff & (msg_size_64 >> 24));

    padded.push_back(0xff & (msg_size_64 >> 32));
    padded.push_back(0xff & (msg_size_64 >> 40));
    padded.push_back(0xff & (msg_size_64 >> 48));
    padded.push_back(0xff & (msg_size_64 >> 56));

    return padded;
  }

  void MD5::ExtractChunk(const std::vector<std::uint8_t>& message,
                         size_t from, size_t to,
                         std::array<std::uint32_t, 16>& chunk) {
    for (size_t i = from; i < to; i += 4) {
      chunk[i/4] =
        std::uint32_t(message[i])         |
        std::uint32_t(message[i+1]) << 8  |
        std::uint32_t(message[i+2]) << 16 |
        std::uint32_t(message[i+3]) << 24 ;
    }
  }
}
