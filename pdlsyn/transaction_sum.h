
#ifndef TRANSACTION_SUM_H
#define TRANSACTION_SUM_H

#include "defines.h"
#include "common.h"

#include "transaction.h"
#include "edge.h"
#include "connection.h"

class transaction_sum_t;
class transaction_slice_list_t;
class transaction_slice_sum_t;

typedef std::map<port_ref_t*, parameter_t*, port_ref_comparator_t>              parameter_sum_type;
typedef std::map<port_ref_t*, transaction_body_t*, port_ref_comparator_t>       transaction_sum_type;
typedef std::map<port_ref_t*, transaction_slice_list_t*, port_ref_comparator_t> transaction_slice_sum_type;
typedef std::map<transaction_t*, uint32_t, transaction_comparator_t>            unique_transaction_map_type;
typedef std::map<port_ref_t*, uint32_t, port_ref_comparator_t>                  match_map_type;

/**
 * this class is used, but not really. it should be removed because it is not actually doing anything. essentially it was used to keep track of the transactions that had been send, but was replaced by parameter sum. currently it is still used inside of vertex for naming, however it does not print anything.
 */

class transaction_sum_t : public map_comp_t<port_ref_t*, transaction_body_t*, port_ref_comparator_t> {
  public:
  transaction_sum_t();  
  transaction_sum_t(transaction_sum_t* sum);

  void add(transaction_t* transaction, connections_t* connections);
  void subtract(transaction_sum_t sum, connections_t* connections);

  bool contains_transactions(vector_t<transaction_t*> transactions, connections_t* connections);
  bool positive();  
  bool negative();
  bool zero();
  int compare(transaction_sum_t other);
  std::string to_vertex_string();

  std::string to_string();
  void print();
};

/**
 * we keep a list of all transaction slices of the same type in the transaction slice sum
 */

class transaction_slice_list_t : public vector_t<transaction_slice_t*> {
  public:
  void add(transaction_slice_t* slice);
  std::pair<bool, transaction_slice_t*> match(transaction_slice_t* slice);
  uint32_t weight();

  std::string to_string();
  void print();
};


/**
 * this structure is used in scheduling to keep track of all transaction slice mappings
 */

class transaction_slice_sum_t : public map_comp_t<port_ref_t*, transaction_slice_list_t*, port_ref_comparator_t> {
  public:
  set_comp_t<std::pair<std::string, uint32_t>*, dep_comparator_t> id_set;

  transaction_slice_sum_t();  

  bool add(edge_t* edge, connections_t* connections);
  bool add(vector_t<transaction_t*> transactions, connections_t* connections);
  bool add(transaction_t* transaction, connections_t* connections);

  void add_dep(edge_t* edge);
  void add_dep(transaction_t* transaction);

  bool match(vector_t<transaction_t*> transactions, connections_t* connections);
  bool match(transaction_t* transaction, connections_t* connections);

  bool dep(edge_t* edge);
  bool needs(edge_t* sender, edge_t* receiver, connections_t* connections);
  // bool needs(edge_t* edge, connections_t* connections);
  bool ready(edge_t* edge, connections_t* connections);

  // may do something different
  void set_matches();

  std::string to_string();
  void print();
};

/**
 * replaces transaction sum, we keep track of all the parameters that have been sent rather than transactions
 */

class parameter_sum_t : public map_comp_t<port_ref_t*, parameter_t*, port_ref_comparator_t> {
  public:
  ~parameter_sum_t();

  bool add(edge_t*, connections_t*);
  bool subtract(edge_t*, connections_t*);
  bool add(transaction_t*, connections_t*);
  bool subtract(transaction_t*, connections_t*);
  bool zero();
  bool pos();
  std::string to_string();
  void print();
};

#endif
