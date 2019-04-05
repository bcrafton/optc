
#ifndef PARAMETER_H
#define PARAMETER_H

#include "defines.h"

// wish - 1
// originally I think we should have converted all strings in our program to #s and then had a map
// save on compute time and simplcity.
// but it feels to late for that.

// wish - 2
// we should have had everything inherit a class that has print and to_string.

// wish - 3
// there is definitely a smart way to compare parameters versus what we are summing against
// rather than pulling them out, leave them as parent pointer and call virtually overloaded compare.

class parameter_t;
class parameter_range_t;
class parameter_offset_t;
class parameter_id_t;
class parameter_dep_t;
class argument_t;
class argument_int_t;
class argument_pair_t;
struct dep_comparator_t;

typedef std::set<std::pair<std::string, uint32_t>*, dep_comparator_t> dep_set_type;
typedef std::set<std::pair<std::string, uint32_t>*, dep_comparator_t> id_set_type;

/**
 * the type of parameter. the 4 types of parameters that are seen in transactions
 */

typedef enum parameter_type {
  PARAM_RANGE,
  PARAM_OFFSET,
  PARAM_ID,
  PARAM_DEP
} parameter_type_t;

/**
 * the type of argument that is passed inside of packets.
 */

typedef enum argument_type {
  ARG_INT,
  ARG_PAIR
} argument_type_t;

/**
 * abstract class for parameter. all parameters inherit from this.
 */

class parameter_t {
  public:
  // constructor
  parameter_t();

  virtual parameter_type_t type();
  virtual parameter_t* copy();
  virtual void set(argument_t* arg);
  virtual float value();

  void print();
  virtual std::string to_string();
};

/**
 * parameter for range.
 */

class parameter_range_t : public parameter_t {
  public:
  std::pair<bool, std::string> range_var;
  int range;

  // constructor
  parameter_range_t(int range);
  parameter_range_t(std::string var);

  // return what type it is 
  parameter_type_t type();
  parameter_t* copy();
  void set(argument_t* arg);
  float value();

  std::string to_string();  
};

/**
 * parameter for offset.
 */

class parameter_offset_t : public parameter_t {
  public:
  std::pair<bool, std::string> offset_var;
  int offset;

  // constructor
  parameter_offset_t(int offset);
  parameter_offset_t(std::string var);

  // return what type it is 
  parameter_type_t type();
  parameter_t* copy();
  void set(argument_t* arg);
  float value();

  std::string to_string();  
};

/**
 * parameter for id
 */

class parameter_id_t : public parameter_t { 
  public:
  std::pair<bool, std::string> id_var;
  uint32_t id;

  // considering adding this just for ease.
  // wudnt have a var for it, because we wud set it later.
  // only thing that would change would be in transaction constructor we could set this.
  // std::pair<bool, std::string> id_name;

  // constructor 
  parameter_id_t(uint32_t id);
  parameter_id_t(std::string var);

  // return what type it is 
  parameter_type_t type();
  parameter_t* copy();
  void set(argument_t* arg);
  float value();

  std::string to_string();
};

/**
 * parameter for id
 */

class parameter_dep_t : public parameter_t { 
  public:
  std::pair<bool, std::string> dep_name_var;
  std::string dep_name;

  std::pair<bool, std::string> dep_id_var;
  uint32_t dep_id;

  // constructor 
  parameter_dep_t(std::string dep_name, uint32_t dep_id);
  parameter_dep_t(std::string dep_name, std::string dep_id_var);
  parameter_dep_t(std::string var);

  // return what type it is 
  parameter_type_t type();
  parameter_t* copy();
  void set(argument_t* arg);
  float value();

  std::string to_string();
};

/**
 * arguments are used in packets for reusing a packet with different parameters. you can use a variable and set it in the packet inst
 */

class argument_t {
  public:
  std::string name;

  argument_t();

  virtual argument_t* copy();
  virtual argument_type_t type();

  void print();
  virtual std::string to_string();
};

/**
 * integer argument
 */

class argument_int_t : public argument_t {
  public:
  uint32_t val;

  argument_int_t(std::string name, uint32_t val);

  argument_type_t type();
  argument_t* copy();

  std::string to_string();
};


/**
 * string, integer pair argument
 */

class argument_pair_t : public argument_t {
  public:
  std::string pair_name;
  uint32_t pair_val;

  argument_pair_t(std::string arg_name, std::string pair_name, uint32_t pair_val);

  argument_type_t type();
  argument_t* copy();

  std::string to_string();
};


/**
 * comparator for dependencies.
 */

struct dep_comparator_t {
  bool operator()(std::pair<std::string, uint32_t>* a, std::pair<std::string, uint32_t>* b) const {

    int comp1 = a->first.compare(b->first);
    int comp2 = a->second - b->second;

    if (comp1 != 0) {
      return (comp1 > 0);
    }
    if (comp2 != 0) {
      return (comp2 > 0);
    }

    return false;
  }
};

#endif


