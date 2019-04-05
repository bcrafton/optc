
#ifndef REF_H
#define REF_H

#include "defines.h"
#include "common.h"


/**
 * the ports direction ...
 */

typedef enum port_direction {
  PORT_DIR_NULL, // we are gonna use this for state_condition_t
  PORT_DIR_IN,
  PORT_DIR_OUT
} port_direction_t;


/**
 * the type of the port {data, control, packet}
 */

typedef enum port_type {
  PORT_TYPE_DATA,
  PORT_TYPE_CONTROL,
  PORT_TYPE_PACKET
} port_type_t;


/**
 * the port reference class. used to compare between connections, conditions, transactions, and ports. it is useful because we can just use the parent compare func and we dont have to repeat the same code.
 */

class port_ref_t {
  public:
  std::pair<bool, std::string> protocol_name;
  std::pair<bool, std::string> name;
  std::pair<bool, port_direction_t> dir;

  port_ref_t();
  port_ref_t(std::string protocol_name, std::string name, port_direction_t dir);

  void set_dir(port_direction_t dir);
  void set_protocol_name(std::string protocol_name);
  void set_name(std::string name);

  void reverse_dir();

  std::string get_protocol_name();
  port_direction_t get_dir();
  std::string get_name();

  int compare(port_ref_t* other_ref);

  std::string to_string();
  void print();
};


/**
 * one more layer of abstraction from port ref. has a heirarchy so we can compare between connections and transactions.
 */

class var_ref_t : public port_ref_t {
  public:
  std::pair<bool, vector_t<std::string> > heirarchy;

  var_ref_t();
  var_ref_t(std::string protocol_name, std::string name, port_direction_t dir);

  void set_name(std::string name);
  void set_heirarchy(vector_t<std::string> heirarchy);

  vector_t<std::string> get_heirarchy();

  std::string top();
  int compare(var_ref_t* other_ref);

  std::string to_string();
  void print();
};

/**
 * another layer of abstraction which contains the cut out of a port, so we can reference bit widths like data[31:0] we often need to compare chunks of a port.
 */

class cut_ref_t : public var_ref_t {
  public:
  bool has_cut;
  uint32_t msb;
  uint32_t lsb;

  // we are adding this here pretty much because we want it in state and transaction.
  // at the moment we do not need it in state, but want to put it there in case.
  // dosnt cost us anything becuase it needs to be specified anyways.
  std::pair<bool, uint32_t> width;

  cut_ref_t();
  cut_ref_t(std::string protocol_name, std::string name, port_direction_t dir, uint32_t width);
  cut_ref_t(std::string protocol_name, std::string name, port_direction_t dir, uint32_t width, uint32_t msb, uint32_t lsb);

  void set_cut(uint32_t msb, uint32_t lsb);
  void set_width(uint32_t width);

  uint32_t get_width();

  int compare(cut_ref_t* other_ref);

  std::string to_verilog();

  std::string to_string();
  void print();
};

/**
 * the comparator for port refs.
 */

struct port_ref_comparator_t {
  bool operator()(port_ref_t* a, port_ref_t* b) const {
    assert(a->name.first && b->name.first);
    // return !(a->compare(b) > 0);
    return (a->compare(b) > 0);
  }
};

#endif
