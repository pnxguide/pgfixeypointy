#pragma once

#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

#include "Decimal.h"
#include "TypeId.h"
#include "defines.h"
#include "strong_typedef.h"

namespace DB {

/// Data types for representing elementary values from a database in RAM.

/// Hold a null value for untyped calculation. It can also store infinities to
/// handle nullable comparison which is used for nullable KeyCondition.
struct Null {
    enum class Value {
        Null,
        PositiveInfinity,
        NegativeInfinity,
    };

    Value value{Value::Null};

    bool isNull() const { return value == Value::Null; }
    bool isPositiveInfinity() const { return value == Value::PositiveInfinity; }
    bool isNegativeInfinity() const { return value == Value::NegativeInfinity; }

    bool operator==(const Null& other) const { return value == other.value; }

    bool operator!=(const Null& other) const { return !(*this == other); }
};

using UInt128 = ::UInt128;
// using UInt256 = ::UInt256;
using Int128 = ::Int128;
// using Int256 = ::Int256;

/// Not a data type in database, defined just for convenience.
using Strings = std::vector<String>;
using TypeIndexesSet = std::unordered_set<TypeIndex>;
}  // namespace DB