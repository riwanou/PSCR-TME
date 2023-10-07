#include <forward_list>
#include <gtest/gtest.h>

#include "hashmap.h"
#include "iterator.h"

using namespace std;

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

TEST(Iterator, Count) {
  forward_list<string> values = {"bloup", "blap",  "blip", "blup",
                                 "blop",  "bluap", "blop"};
  EXPECT_EQ(count(values.begin(), values.end()), 7);
}

TEST(Iterator, CountIf) {
  forward_list<string> values = {"bloup", "blap",  "blip", "blup",
                                 "blop",  "bluap", "blop"};
  EXPECT_EQ(count_if_equal(values.begin(), values.end(), "blop"), 2);
  EXPECT_EQ(count_if_equal(values.begin(), values.end(), "hello"), 0);
}

TEST(Hashmap, Iterator) {
  HashMap<string, int> map(1);
  map.put("blup", 1);
  map.put("blop", 5);
  map.put("blap", 10);
  map.put("bloup", 42);

  auto it = map.begin();
  EXPECT_EQ((*it).first, "blap");
  EXPECT_EQ((*it).second, 10);
  ++it;
  EXPECT_EQ((*it).first, "blop");
  EXPECT_EQ((*it).second, 5);
  ++it;
  EXPECT_EQ((*it).first, "bloup");
  EXPECT_EQ((*it).second, 42);
  ++it;
  EXPECT_EQ((*it).first, "blup");
  EXPECT_EQ((*it).second, 1);
  ++it;
  EXPECT_FALSE(it != map.end());
}

TEST(Hashmap, CountAndCountIf) {
  HashMap<string, int> map(1);
  map.put("blup", 1);
  map.put("blop", 5);
  map.put("blap", 10);
  map.put("bloup", 42);

  EXPECT_EQ(count(map.begin(), map.end()), map.size());
  pair<string, int> val("blup", 1);
  EXPECT_EQ(count_if_equal(map.begin(), map.end(), val), 1);
}
