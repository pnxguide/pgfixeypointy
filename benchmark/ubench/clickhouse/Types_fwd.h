#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

#include "Decimal_fwd.h"
#include "defines.h"
#include "strong_typedef.h"
#include "types.h"

namespace wide {

template <size_t Bits, typename Signed>
class integer;

}

using Int128 = wide::integer<128, signed>;
using UInt128 = wide::integer<128, unsigned>;
using Int256 = wide::integer<256, signed>;
using UInt256 = wide::integer<256, unsigned>;

namespace DB {

using UUID = StrongTypedef<UInt128, struct UUIDTag>;

struct IPv4;

struct IPv6;

struct Null;

using UInt128 = ::UInt128;
using UInt256 = ::UInt256;
using Int128 = ::Int128;
using Int256 = ::Int256;

enum class TypeIndex;

/// Not a data type in database, defined just for convenience.
using Strings = std::vector<String>;
using TypeIndexesSet = std::unordered_set<TypeIndex>;

}  // namespace DB