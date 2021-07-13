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
#pragma once

#include <cstdint>
#include <deque>
#include <string>

namespace psy::ds {
  class BigInt {
  public:
    explicit BigInt(const std::string& str);
    explicit BigInt(std::deque<std::int8_t>&& digits);
    const std::deque<std::int8_t>& GetDigits() const;
    std::string ToString() const;

    BigInt operator+(const BigInt& other) const;
    BigInt operator-(const BigInt& other) const;
  private:
    std::deque<std::int8_t> digits_;
    std::uint8_t sign_;
  };
}
