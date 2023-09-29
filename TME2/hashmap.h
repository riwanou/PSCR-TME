#pragma once

#include <forward_list>
#include <vector>

using namespace std;

template <typename K, typename V> class HashMap {
  typedef pair<K, V> entry;

public:
  HashMap(size_t size) { 
    buckets = vector<forward_list<entry>>(size);
    for (auto& bucket : buckets) {
      bucket = forward_list<entry>();
    }
  }

  V* get(const K& key) {
    auto key_index = hash<K>(key) % buckets.size();
    auto bucket = buckets[key_index];
    for (const entry& value : bucket) {
      if (value.first == key) return &value.second;
    }
    return nullptr;
  }

  bool put(const K& key, const V& value) {
    auto key_index = hash<K>(key) % buckets.size();
    auto bucket = buckets[key_index];

  }

private:
  vector<forward_list<entry>> buckets;

};