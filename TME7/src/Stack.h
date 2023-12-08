#pragma once

#include <cstring> // size_t,memset
#include <iostream>
#include <semaphore.h>

namespace pr {

#define STACKSIZE 100

template <typename T> class Stack {
  T tab[STACKSIZE];
  size_t sz;
  sem_t s_mutex, s_read, s_write;

public:
  Stack() : sz(0) {
    memset(tab, 0, sizeof tab);
    sem_init(&s_mutex, 1, 1);
    sem_init(&s_write, 1, STACKSIZE);
    sem_init(&s_read, 1, 0);
  }

  ~Stack() {
    sem_destroy(&s_mutex);
    sem_destroy(&s_write);
    sem_destroy(&s_read);
  }

  T pop() {
    // bloquer si vide
    sem_wait(&s_read);

    sem_wait(&s_mutex);
    T toret = tab[--sz];
    sem_post(&s_mutex);

    sem_post(&s_write);
    return toret;
  }

  void push(T elt) {
    // bloquer si plein
    sem_wait(&s_write);

    sem_wait(&s_mutex);
    tab[sz++] = elt;
    sem_post(&s_mutex);

    sem_post(&s_read);
  }
};

} // namespace pr
