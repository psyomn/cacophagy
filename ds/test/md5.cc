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
#include <cstdint>

#include <iostream> // todo remove me

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "ds/md5.h"

TEST(md5, basic_pad) {
  using md5 = psy::ds::MD5;
  const std::vector<uint8_t> original = {0, 1, 2, 3};
  const auto padded_message = md5::PadMessage(original);
  EXPECT_EQ(padded_message.size(), 64);

  EXPECT_EQ(original[0], padded_message[0]);
  EXPECT_EQ(original[3], padded_message[3]);

  EXPECT_EQ(padded_message[4], 0b10000000);
  EXPECT_EQ(padded_message[5], 0);

  std::uint64_t msg_size = 0;
  auto it = padded_message.cend() - 8;

  for (size_t i = 0; i < 8; ++i) msg_size |= *it++;
  EXPECT_EQ(msg_size, 4);
}

TEST(md5, pad_n_plus_1) {
  using md5 = psy::ds::MD5;
  std::vector<uint8_t> original;

  for (size_t i = 0; i < 65; ++i) original.push_back(0xa);
  const auto padded_message = md5::PadMessage(original);

  EXPECT_EQ(padded_message.size(), 128);
  EXPECT_EQ(padded_message[65], 0b10000000);
}

TEST(md5, empty_message) {
  using md5 = psy::ds::MD5;
  const std::vector<std::uint8_t> message = {};
  const auto ret = md5::Digest(message);

  std::cout << std::hex
            << ret[0] << " "
            << ret[1] << " "
            << ret[2] << " "
            << ret[3] << " " << std::endl;
}

TEST(md5, extract_chunk) {
  using md5 = psy::ds::MD5;
  const std::vector<std::uint8_t> data = {
    0xaa, 0xaa, 0xaa, 0xaa,  0xbb, 0xbb, 0xbb, 0xbb,
    1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7,
  };

  std::array<std::uint32_t, 16> chunks = {0};
  md5::ExtractChunk(data, 0, 64, chunks);

  // for (size_t i = 0; i < 16; ++i)
  //   std::cout << "--" << chunks[i] << std::endl;

}
