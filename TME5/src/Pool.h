#pragma once

#include "Job.h"
#include "Queue.h"
#include <thread>
#include <vector>

namespace pr {

class Pool {
  Queue<Job> queue;
  std::vector<std::thread> threads;

public:
  Pool(int qsize) : queue(qsize) {}

  void start(int nbthread) {
    threads.reserve(nbthread);
    for (int i = 0; i < nbthread; i++) {
      threads.emplace_back(std::thread([&]() {
        while (true) {
          Job *job = queue.pop();
          if (!job)
            return;
          job->run();
          delete job;
        }
      }));
    }
  }

  void submit(Job *job) {
    queue.push(job);
  }

  void stop() {
    queue.set_blocking(false);
    for (auto &thread : threads) {
      thread.join();
    }
    threads.clear();
  }
};

} // namespace pr
