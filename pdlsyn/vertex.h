
#ifndef VERTEX_H
#define VERTEX_H

#include "defines.h"
#include "common.h"

#include "graph.h"
#include "transaction_sum.h"

/**
 * vertex types
 */

typedef enum vertex_type{
  FSM_ROOT,
  FSM_SINK,
  FSM_BODY
} vertex_type_t;

/**
 * vertices are the objects that connect the edges in our FSM. currently they have an extra name extension provided by transaction_sum_t, that should be replaced by parameter sum
 */

class vertex_t{
  public:
  vertex_type_t type;
  std::vector<vid_t> key;
  // so we need the state along the path
  // parameters wont work, i think we need transaction sum t
  // vector_t< std::pair<std::string, uint32_t> > parameters;
  std::pair<bool, transaction_sum_t> sum;

  // hack for paper.
  uint32_t id();

  vertex_t(vertex_type_t type, vid_t key);
  vertex_t(vertex_type_t type, std::vector<vid_t> key);
  vertex_t(vertex_type_t type, std::vector<vid_t> key, transaction_sum_t sum);
  vertex_t* copy();

  std::string to_string();
  int compare(vertex_t* other_vertex);
  vertex_type_t get_type();
};

// just make this vertex.equals(vertex)
struct vertex_comparator_t {
  bool operator() (vertex_t* a, vertex_t* b) const {

    // compare the key vid list
    if (a->key.size() > b->key.size()) return false;
    if (a->key.size() < b->key.size()) return true;
    int i;
    for(i=0; i<a->key.size(); i++) {
      if(a->key[i] < b->key[i]) return false;
      if(a->key[i] > b->key[i]) return true;
    }

    // compare state sum
    if ( !a->sum.first ) {
      return true;
    }
    else if ( a->sum.first && !b->sum.first ) {
      return false;
    }
    else {
      int compare_sum = a->sum.second.compare(b->sum.second);
      return (compare_sum < 0);
    }

  }
};

/**
 * the function used by graphviz to generate the dot files
 */

struct vertex_writer_t
{
  vertex_writer_t(graph_t& graph) : g(graph)
  {
    // this is our constructor
    // we dont need anything
  };
  void operator()(std::ostream& out, const vid_t& vid) const 
  {

#if (PRINT_VERTEX)
    out << "[label=\"";

    vertex_t* v = g[vid];
    out << v->to_string();

    out << "\"]";
#else
    out << "[width=.25, shape=point]";
#endif
  }

  private:
  graph_t& g;
  // can put stuff here
};

#endif
