#include "Banque.h"
#include <chrono>
#include <iostream>
#include <thread>

using namespace std;

const int NB_THREAD = 10;
const int NB_ACCOUNT = 50;

int main() {
  // random
  ::srand(::time(nullptr));

  vector<thread> threads;
  threads.reserve(NB_THREAD);

  pr::Banque bank(NB_ACCOUNT, 10000);

  for (int a = 0; a < NB_THREAD; a++) {
    threads.emplace_back([&bank, &a]() {
      // transfer
      int i = rand() % NB_ACCOUNT;
      int j = rand() % NB_ACCOUNT;
      int m = rand() % 100 + 1;

      printf("%d -> %d:%d[%d]\n", a, i, j, m);
      bank.transfert(i, j, m);

      // sleep
      int time = rand() % 20;
      this_thread::sleep_for(chrono::milliseconds(time));
    });
  }

  for (auto &t : threads) {
    t.join();
  }

  // TODO : tester solde = NB_THREAD * JP
  return 0;
}
