
#ifndef TRANSITION_H
#define TRANSITION_H

#include "defines.h"
#include "common.h"

#include "edge.h"

/**
 * used in parser
 */

class tree_transition_t {
  public:
  int                         head;
  edge_t*                     edge;
  std::vector< int >          tail;

  void set_tail(std::vector<int> tail);
  void add_tail(int v);
  void add_head(int v);
  void add_edge(edge_t* e);

  tree_transition_t* copy();
};

/**
 * used in parser
 */

class forest_transition_t {
  public:

  std::vector< int >          head;
  std::vector< edge_t* >      edge;
  std::vector< int >          tail;


  void add_tail(std::vector<int> tail);
  void set_tail(std::vector<int> tail);

  void add_tail(int v);
  void add_head(int v);
  void add_edge(edge_t* e);

  void add_head(std::vector<int> head);
  void add_edge(std::vector<edge_t*> e);


};

#endif


