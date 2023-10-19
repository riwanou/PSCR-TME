#include "Banque.h"
#include <chrono>
#include <iostream>
#include <thread>

using namespace std;

const int NB_THREAD = 20;
const int NB_ACCOUNT = 100;
const int ACCOUNT_SOLDE = 234;

int main() {
  // random
  ::srand(::time(nullptr));

  vector<thread> threads;
  threads.reserve(NB_THREAD);

  pr::Banque bank(NB_ACCOUNT, ACCOUNT_SOLDE);

  for (int a = 0; a < NB_THREAD; a++) {
    threads.emplace_back([&bank, &a]() {
      // transfer
      int i = rand() % NB_ACCOUNT;
      int j = rand() % NB_ACCOUNT;
      int m = rand() % 100 + 1;

      printf("%d -> %d:%d[%d]\n", a, i, j, m);
      bank.transfert(i, j, m);

      bank.comptabiliser(ACCOUNT_SOLDE * NB_ACCOUNT);

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
