
#ifndef BTIMER_H
#define BTIMER_H

#include "defines.h"

/**
 * Btimer is a class that lets us track both cpu time and wall clock time so that we can time and profile the code regardless of the number of threads being used.
 */

class btimer_t {
  public:

  clock_t cpu_start;
  clock_t cpu_end;

  struct timeval wall_start;
  struct timeval wall_end;

  void start();
  void end();

  double cpu_time();
  double wall_time();
};

#endif
