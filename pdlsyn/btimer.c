
#include "btimer.h"

void btimer_t::start()
{
  // cpu start
  this->cpu_start = clock();
  // wall start
  gettimeofday(&(this->wall_start), NULL);
}

void btimer_t::end()
{
  // cpu end
  this->cpu_end = clock();
  // wall end
  gettimeofday(&(this->wall_end), NULL);
}

double btimer_t::cpu_time()
{
  double time_spent = (double)(this->cpu_end - this->cpu_start) / CLOCKS_PER_SEC;
  return time_spent;
}

double btimer_t::wall_time()
{
  uint64_t end_us = this->wall_end.tv_sec * 1000000 + this->wall_end.tv_usec;
  uint64_t start_us = this->wall_start.tv_sec * 1000000 + this->wall_start.tv_usec; 
  double time_spent = ((double) end_us) - start_us;
  return (time_spent / 1000000);
}
