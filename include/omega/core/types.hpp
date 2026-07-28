#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <ostream>
#include <span>
#include <type_traits>

namespace omega::core {

// Id class
template <class Tag, class Underlying = std::uint32_t> struct Id final {
  static_assert(std::is_unsigned_v<Underlying>,
                "Id Underlying type should be an unsigned integer");
  using underlying_type = Underlying;

  Underlying value{invalid_value()};

  constexpr Id() noexcept = default;
  constexpr explicit Id(Underlying v) noexcept : value(v) {}

  static constexpr Underlying invalid_value() noexcept {
    return std::numeric_limits<Underlying>::max();
  }

  // Return an invalid Id
  static constexpr Id invalid() noexcept { return Id{}; }

  constexpr bool valid() const noexcept { return value != invalid_value(); }

  constexpr explicit operator bool() const noexcept { return valid(); }

  // Auto generates <, <=, ==, etc (C++ 20 feature)
  constexpr auto operator<=>(const Id &) const noexcept = default;

  constexpr Underlying raw() const noexcept { return value; }
};

// Tags (distinct types)
struct StateTag {};
struct EdgeTag {};
struct APTag {}; // atomic propositions

using StateId = Id<StateTag>;
using EdgeId = Id<EdgeTag>;
using APId = Id<APTag>;

// Stream output
template <class Tag, class U>
inline std::ostream &operator<<(std::ostream &os, Id<Tag, U> id) {
  if (!id.valid())
    return os << "<invalid>";
  return os << id.raw();
}

} // namespace omega::core
// Hash support for using in unordered map / set
// (Has to be done outside namespace)

template <class Tag, class U> struct std::hash<omega::core::Id<Tag, U>> {
  std::size_t operator()(omega::core::Id<Tag, U> id) const noexcept {
    return std::hash<U>{}(id.raw());
  }
};

namespace omega::core {
// enums and helpers
enum class AcceptanceKind : std::uint8_t { Buchi, Rabin, Streett, Parity };

using Index = std::size_t;

using Priority = std::uint32_t;

template <class T> using View = std::span<const T>;

} // namespace omega::core
