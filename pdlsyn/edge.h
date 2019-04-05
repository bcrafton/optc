
#ifndef EDGE_H
#define EDGE_H

#include "defines.h"
#include "common.h"

#include "transaction.h"
#include "condition.h"
#include "ref.h"
#include "graph.h"

/**
 * edge is the edges in our finite state machines. they contain the conditions and transactions on any state transition
 */

class edge_t {
  public:
  // we want to be able to modify these sends and receives hence the pointers.
  // it cost us a good chunk of time.
  uint32_t id;

  vector_t<transaction_t*> sends;
  vector_t<transaction_t*> receives;
  condition_list_t conditions; // would really prefer this just to be a vector of states.

  edge_t(condition_list_t conditions, vector_t<transaction_t*> sends, vector_t<transaction_t*> receives);
  edge_t(edge_t* e1, edge_t* e2);
  edge_t(vector_t<edge_t*> edges);

  ~edge_t();

  // make me!
  // edge_t(edge_t* other_edge);

  void add(edge_t* other_edge);

  uint32_t num_sends();
  uint32_t num_receives();
  bool has_send();
  bool has_receive();

  bool contains_receive(std::string receive);
  bool contains_send(std::string send);
  bool contains_transaction(std::string transaction);

  bool contains_receive(port_ref_t* ref);
  bool contains_send(port_ref_t* ref);
  bool contains_transaction(port_ref_t* ref);

  bool contains_condition(std::string condition);
  int get(std::string condition);

  bool contains_condition(condition_t* condition);
  int get(condition_t* condition);

  void print();
  std::string to_string();
  
  edge_t* copy();
  edge_t* reverse();
};

/**
 * edge writer is the function that is used when using graphviz to visualize our fsms.
 */

struct edge_writer_t
{
  edge_writer_t(graph_t& graph) : g(graph)
  {
    // this is our constructor
    // we dont need anything
  };
  void operator()(std::ostream& out, const eid_t& eid) const 
  {
#if (PRINT_EDGE)
    out << "[label=\"";

    edge_t* e = g[eid];
    out << e->to_string();

    out << "\"]";
#else
    // nothing goes here.
#endif
  }

  private:
  graph_t& g;
  // can put stuff here
};

#endif
