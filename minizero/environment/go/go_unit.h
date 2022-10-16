#pragma once

#include <bitset>
#include <sstream>
#include <string>

namespace minizero::env::go {

const std::string kGoName = "go";
const int kGoNumPlayer = 2;
const int kMaxGoBoardSize = 19;

typedef unsigned long long GoHashKey;
typedef std::bitset<kMaxGoBoardSize * kMaxGoBoardSize> GoBitboard;

} // namespace minizero::env::go