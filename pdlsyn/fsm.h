
#ifndef FSM_H
#define FSM_H

#include "defines.h"
#include "common.h"

#include "vertex.h"
#include "edge.h"
#include "packet.h"
#include "transaction.h"
#include "ref.h"
#include "path.h"
#include "graph.h"
#include "config.h"
#include "parameter.h"

class fsm_t;
class control_pos_t;

/**
 * FSM is our FSM, it uses boost graph library, edges and vertices.
 */

class fsm_t {
  public:
  vid_t root;
  graph_t g;
  std::map<vertex_t*, vid_t, vertex_comparator_t> table;

  vector_t<control_pos_t> control_in;
  vector_t<control_pos_t> control_out;

  fsm_t();
  fsm_t(fsm_t* fsm1, fsm_t* fsm2, connections_t* connections);
  fsm_t(port_t* port, packet_t* packet);

  // used in fsm_t(port_t* port, packet_t* packet);
  void add_packet_edge(port_t* port, packet_edge_t* packet_edge, vid_t prev_vid, uint32_t current_edge_size, edge_t* new_edge, name_space_t name_space);

  vid_t add_root(); // reg fsm
  void set_sink(vid_t v);
  vid_t add_vertex(); // reg fsm
  vid_t add_vertex(vertex_type_t type); // reg fsm

  vid_t add_root(std::vector<vid_t> key); // merged fsm
  vid_t add_root(std::vector<vid_t> key, transaction_sum_t sum); // merged fsm

  vid_t add_sink(std::vector<vid_t> key);
  vid_t add_vertex(std::vector<vid_t> key); // merged fsm
  vid_t add_vertex(std::vector<vid_t> key, transaction_sum_t sum); // merged fsm
  vid_t add_vertex(vertex_type_t type, std::vector<vid_t> key); // merged fsm
  vid_t add_vertex(vertex_type_t type, std::vector<vid_t> key, transaction_sum_t sum); // merged fsm

  eid_t add_edge(vid_t v1, vid_t v2, edge_t* edge);

  eid_t add_fsm_r(vid_t new_root, vid_t old_root, fsm_t* fsm);
  eid_t add_fsm(vid_t v, fsm_t* fsm, edge_t* edge);

  // void print();
  // void print_r(vid_t dest);

  // std::string to_string();
  // std::string to_string_r();

  std::string output_condition(edge_t* e);
  std::string input_condition(edge_t* e);

  uint32_t control_in_width();
  uint32_t control_out_width();

  std::string to_verilog();

  // for multi fsm we can just dump to same file - no connections is all.
  void to_visualize(std::string filename);

  bool contains(vertex_t* key);
  vid_t get(vertex_t* key);
  void put(vertex_t* key, vid_t value);

  // can probably combine these two.
  uint32_t num_locations(port_ref_t* ref);
  uint32_t largest_width(port_ref_t* ref);
  // this shud be unique transactions, not unique strings
  // so we are sticking the transaction in there and only comparing with name, packet_cut
  vector_t<transaction_t*> unique_locations(port_ref_t* ref);
  vector_t<port_ref_t*> unique_matches(transaction_t* transaction);

  uint32_t size();

  bool sink_exists(eid_t e);

  paths_t* paths();
  std::pair<paths_t*, bool> paths_r(eid_t e);

  void print();
  std::string to_string();
};

/**
 * control position is a helper class used in generating the condition values for the verilog
 */

class control_pos_t {
  public:
  condition_t* cond;
  uint32_t start; // dont think this is necessary.
  uint32_t len;

  control_pos_t(condition_t* cond, uint32_t start, uint32_t len)
  {
    this->cond = cond;
    this->start = start;
    this->len = len;
  }
};


#endif


















