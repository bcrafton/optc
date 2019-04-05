
#ifndef CONDITION_T
#define CONDITION_T

#include "defines.h"
#include "common.h"

#include "value.h"
#include "port.h"

/**
 * condition type is an enumeration for the type of condition. condition is what goes into our FSM block for deciding next state. the condition can be based on input values OR other state machine's current state
 */

typedef enum condition_type {
  COND_CONTROL,
  COND_STATE
} condition_type_t;

/**
 * the object that defines a conditon required to advance in our state machine
 */

class condition_t : public cut_ref_t {
  public:
  value_t val;

  condition_t();
  virtual condition_type_t type();
  virtual std::string to_string();
  void print();
};

/**
 * a conditon that has to do with input values
 */

class control_condition_t : public condition_t { 
  public:
  control_condition_t(std::string portname, port_direction_t dir, uint32_t width, value_t val);
  control_condition_t(std::string portname, port_direction_t dir, value_t val, uint32_t msb, uint32_t lsb);

  condition_type_t type();
  std::string to_string();
};

/**
 * a condition that is based on the states of other state machines
 */

class state_condition_t : public condition_t {
  public:
  state_condition_t(std::string state_name, uint32_t width, value_t val);

  condition_type_t type();
  std::string to_string();
};

/**
 * a list of conditions, this is only used for print and to_string.
 */

class condition_list_t : public vector_t<condition_t*> {
  public:
  std::string to_string();
  void print();
};

/**
 * the comparator that is used in maps and sets for conditions.
 */

struct state_comparator_t {
  bool operator()(condition_t* a, condition_t* b) const {
    int comp = a->compare(b);

    if (comp != 0) {
      return (comp > 0);
    }

    return false;
  }
};

#endif
