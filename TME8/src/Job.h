#ifndef SRC_JOB_H_
#define SRC_JOB_H_

#include "src/ConnectionHandler.h"

class Job {
public:
  virtual void run() = 0;
  virtual ~Job(){};
};

class HandleJob : public Job {
  pr::ConnectionHandler *handler;
  pr::Socket client;

public:
  HandleJob(pr::ConnectionHandler *handler, pr::Socket client)
      : handler(handler), client(client) {}

  void run() {
    handler->handleConnection(client);
    client.close();
  }
};

#endif /* SRC_JOB_H_ */
