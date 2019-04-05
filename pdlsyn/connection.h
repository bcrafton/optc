
#ifndef CONNECTION_H
#define CONNECTION_H

#include "defines.h"
#include "common.h"

#include "ref.h"
#include "port.h"


/**
 * connection just gives us a 1-set of ports that are connected to one another in our translator design
 */

class connection_t {
  public:
  var_ref_t send_ref;
  var_ref_t rec_ref;

  connection_t(std::string prot_name1, std::string name1, port_direction dir1, std::string prot_name2, std::string name2, port_direction dir2);

  void print();
  std::string to_string();
};

/**
 * the list of connections in our config file
 */

class connections_t : public vector_t<connection_t> {
  public:
  connections_t();

  // class functions
  // bool contains(transaction_t t1, transaction_t t2);
  // bool contains(var_ref_t* ref1, var_ref_t* ref2);

  // really not sure if we shud do this:
  bool contains(port_ref_t* ref1, port_ref_t* ref2);

  vector_t<var_ref_t> connections(var_ref_t* ref);
  bool buffer(var_ref_t* t1, var_ref_t* t2);

  void print();
  std::string to_string();
};

#endif
