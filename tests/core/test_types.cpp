#include <omega/core/types.hpp>

#include <algorithm>
#include <cassert>
#include <sstream>
#include <unordered_set>
#include <vector>

using namespace omega::core;

static void test_invalid_by_default() {
  StateId s;
  assert(!s.valid());
  assert(!static_cast<bool>(s));
  assert(s == StateId::invalid());
}

static void test_explicit_construction_and_raw() {
  StateId s{42};
  assert(s.valid());
  assert(static_cast<bool>(s));
  assert(s.raw() == 42u);
}

static void test_comparisons() {
  StateId a{1};
  StateId b{2};
  StateId c{2};

  assert(a < b);
  assert(b > a);
  assert(b == c);
  assert(a != b);

  std::vector<StateId> v{b, a, c};
  std::sort(v.begin(), v.end());
  assert(v[0] == a);
  assert(v[1] == b);
  assert(v[2] == c);
}

static void test_hashing() {
  std::unordered_set<StateId> set;
  set.insert(StateId{1});
  set.insert(StateId{2});
  set.insert(StateId{2}); // duplicate

  assert(set.size() == 2);
  assert(set.contains(StateId{1}));
  assert(set.contains(StateId{2}));
  assert(!set.contains(StateId{3}));
}

static void test_stream_output() {
  {
    std::ostringstream oss;
    oss << StateId{7};
    assert(oss.str() == "7");
  }
  {
    std::ostringstream oss;
    oss << StateId{};
    assert(oss.str() == "<invalid>");
  }
}

static void test_span_view() {
  std::vector<int> xs{10, 20, 30};
  View<int> v(xs.data(), xs.size());
  assert(v.size() == 3);
  assert(v[0] == 10);
  assert(v[2] == 30);

  int sum = 0;
  for (int x : v)
    sum += x;
  assert(sum == 60);
}

int main() {
  test_invalid_by_default();
  test_explicit_construction_and_raw();
  test_comparisons();
  test_hashing();
  test_stream_output();
  test_span_view();
  return 0;
}
