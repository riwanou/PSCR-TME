#pragma once

#include <condition_variable>
#include <mutex>

namespace pr {

class Barrier {
  std::mutex m;
  std::condition_variable cond;
  int counter;
  const int goal;

public:
  Barrier(int goal) : counter(0), goal(goal) {}

  void done() {
    std::unique_lock<std::mutex> l(m);
    counter++;
    if (counter == goal)
      cond.notify_all();
  }

  void wait_for() {
    std::unique_lock<std::mutex> lg(m);
    cond.wait(lg, [&]() { return counter == goal; });
  }
};

} // namespace pr
