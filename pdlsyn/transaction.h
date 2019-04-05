
#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "defines.h"
#include "common.h"

#include "port.h"
#include "ref.h"
#include "connection.h"
#include "parameter.h"

class transaction_t;
class transaction_body_t;
class transaction_slice_t;

/**
 * transaction body. this was once used for keeping a traction map and is the base of transaction. it can probably be integrated back in.
 */

class transaction_body_t {
  public:
  std::pair<bool, int> offset;
  std::pair<bool, int> range;

  vector_t<parameter_t*> params; 

  // constructor
  transaction_body_t();
  transaction_body_t(transaction_body_t* body);
  transaction_body_t(vector_t<parameter_t*> params);

  // class functions
  void add(transaction_body_t* body);

  void subtract(transaction_body_t* body);

  bool positive();
  bool negative();
  bool zero();

  int compare(transaction_body_t* other);

  int weight();

  std::string parameter();

  transaction_body_t* copy();

  void set(vector_t<argument_t*> args);

  std::string to_string();
  void print();
};

/**
 * our transaction class
 */

class transaction_t : public cut_ref_t {
  public:
  transaction_body_t body;

  vector_t<transaction_slice_t*> matches;
  // will probs need this to be wrapped with a pair<bool, x>
  // std::pair<uint32_t, uint32_t> packet_cut;

  transaction_t(std::string name, uint32_t width);
  transaction_t(std::string name, uint32_t width, vector_t<parameter_t*> params);
  transaction_t(std::string name, uint32_t width, port_direction_t dir, vector_t<parameter_t*> params);
  // this is getting a little wild
  transaction_t(std::string name, port_direction_t dir, vector_t<parameter_t*> params, uint32_t msb, uint32_t lsb);
  transaction_t(transaction_t* old_transaction);
  
  ~transaction_t();

  int compare(transaction_t* transaction);

  void set(vector_t<argument_t*> args);

  transaction_slice_t* slice();
  int weight();
  float value();

  vector_t<port_ref_t*> source();
  vector_t<port_ref_t*> dest();

  std::string parameter();
  std::pair<bool, parameter_id_t*> id();
  transaction_t* copy();

  std::string to_string();
  void print();
};

/**
 * the transaction comparator class.
 */

struct transaction_comparator_t {

  /*
  Wait! Before you change this ... it is being used in fsm.c:
  vector_t<transaction_t*> fsm_t::unique_matches(transaction_t* transaction);
  vector_t<transaction_t*> fsm_t::unique_locations(port_ref_t* ref);
  */

  // changed on 3-11-2018

  bool operator()(transaction_t* a, transaction_t* b) const {
    int comp = a->compare(b);

    if (comp != 0) {
      return (comp > 0);
    }
    return false;
  }
};

/**
 * transaction slice is used in scheduling to map each transaction to other transactions. if we have [63:0] and [31:0] [31:0] then each of the 31s will get a slice of the 64.
 */

class transaction_slice_t {
  public:
  transaction_t* transaction;
  uint32_t lsb;
  uint32_t msb;

  vector_t<transaction_slice_t*> matches;

  transaction_slice_t(transaction_t* transaction);
  transaction_slice_t(transaction_t* transaction, uint32_t lsb, uint32_t msb);

  std::pair<bool, transaction_slice_t*> match(transaction_slice_t* match);
  transaction_slice_t* slice(uint32_t num_bits);

  int weight();

  std::string to_string();
  void print();
};


#endif
