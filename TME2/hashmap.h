#pragma once

#include <algorithm>
#include <forward_list>
#include <iostream>
#include <iterator>
#include <vector>

using namespace std;

template <typename K, typename V> class HashMap {
  typedef pair<K, V> entry;

public:
  class iterator {
  public:
    iterator(typename vector<forward_list<entry>>::iterator it,
             typename vector<forward_list<entry>>::iterator end)
        : vit(it), end(end) {
      go_next_nonempty();
    }

    iterator &operator++() {
      ++lit;
      if (lit == vit->end()) {
        ++vit;
        go_next_nonempty();
      }
      return *this;
    }

    bool operator!=(const iterator &o) {
      return (vit != o.vit && lit != o.lit);
    }

    entry &operator*() { return *lit; }

  private:
    typename vector<forward_list<entry>>::iterator vit;
    typename vector<forward_list<entry>>::iterator end;
    typename forward_list<entry>::iterator lit;

    void go_next_nonempty() {
      while (vit->empty() && vit != end) {
        ++vit;
      }
      lit = vit->begin();
    }
  };

  HashMap(size_t size) { buckets = vector<forward_list<entry>>(size); }

  iterator begin() { return iterator(buckets.begin(), buckets.end()); }
  iterator end() { return iterator(buckets.end(), buckets.end()); }

  V *get(const K &key) {
    int key_index = std::hash<K>()(key) % buckets.size();
    forward_list<entry> &bucket = buckets[key_index];
    for (entry &value : bucket) {
      if (value.first == key)
        return &value.second;
    }
    return nullptr;
  }

  bool put(const K &key, const V &value) {
    int key_index = hash<K>()(key) % buckets.size();
    forward_list<entry> &bucket = buckets[key_index];

    // check if already exist, in this case, modify in place
    for (entry &bucket_value : bucket) {
      if (bucket_value.first == key) {
        bucket_value.second = value;
        return true;
      }
    }

    // insert
    bucket.emplace_front(key, value);

    // resize if needed
    size_t nb_buckets = buckets.size();
    if (size() >= nb_buckets * 0.8)
      grow(nb_buckets * 2);

    return false;
  }

  size_t size() const {
    size_t size = 0;
    for (const forward_list<entry> &bucket : buckets) {
      size += distance(bucket.begin(), bucket.end());
    }
    return size;
  }

  const vector<forward_list<entry>> &entries() const { return buckets; }

private:
  vector<forward_list<entry>> buckets;

  // when size > 80%, resize the hashmap
  void grow(size_t size) {
    auto new_buckets = vector<forward_list<entry>>(size);
    // copy all elements into new hashmap
    for (const auto &bucket : buckets) {
      for (const auto &value : bucket) {
        int key_index = hash<K>()(value.first) % new_buckets.capacity();
        new_buckets[key_index].emplace_front(value.first, value.second);
      }
    }
    buckets = new_buckets;
  }
};