#ifndef SRC_QUEUE_H_
#define SRC_QUEUE_H_

#include <condition_variable>
#include <cstdlib>
#include <mutex>

namespace pr {

// MT safe version of the Queue, non blocking.
template <typename T> class Queue {
  T **tab;
  const size_t allocsize;
  size_t begin;
  size_t sz;
  mutable std::mutex m;
  std::condition_variable cond;
  bool is_blocking;

  // fonctions private, sans protection mutex
  bool empty() const { return sz == 0; }
  bool full() const { return sz == allocsize; }

public:
  Queue(size_t size) : allocsize(size), begin(0), sz(0), is_blocking(true) {
    tab = new T *[size];
    memset(tab, 0, size * sizeof(T *));
  }

  ~Queue() {
    // ?? lock a priori inutile, ne pas detruire si on travaille encore avec
    for (size_t i = 0; i < sz; i++) {
      auto ind = (begin + i) % allocsize;
      delete tab[ind];
    }
    delete[] tab;
  }

  size_t size() const {
    std::unique_lock<std::mutex> lg(m);
    return sz;
  }

  void set_blocking(bool blocking) {
    std::unique_lock<std::mutex> lg(m);
    is_blocking = blocking;
    cond.notify_all();
  }

  T *pop() {
    std::unique_lock<std::mutex> lg(m);
    cond.wait(lg, [&]() { return !empty() || !is_blocking; });
    if (!is_blocking && empty())
      return nullptr;
    if (full())
      cond.notify_all();

    auto ret = tab[begin];
    tab[begin] = nullptr;
    sz--;
    begin = (begin + 1) % allocsize;
    return ret;
  }

  bool push(T *elt) {
    std::unique_lock<std::mutex> lg(m);
    cond.wait(lg, [&]() { return !full() || !is_blocking; });
    if (!is_blocking && full())
      return false;
    if (empty())
      cond.notify_all();

    tab[(begin + sz) % allocsize] = elt;
    sz++;
    return true;
  }
};

} // namespace pr

#endif /* SRC_QUEUE_H_ */
