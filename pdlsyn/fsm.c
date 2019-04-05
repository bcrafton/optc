
#include "fsm.h"

using namespace std;
using namespace boost;

extern config_t config;

// fsm

fsm_t::fsm_t()
{
}

static path_t make_path(path_t path, paths_t paths, connections_t* connections)
{
}

bool static needs(path_t receiver, path_t sender, edge_t* receive_edge, connections_t* connections)
{
}

bool static ready(path_t receiver, path_t sender, edge_t* receive_edge, connections_t* connections)
{
}

fsm_t::fsm_t(fsm_t* fsm1, fsm_t* fsm2, connections_t* connections)
{
}

void fsm_t::add_packet_edge(port_t* port, packet_edge_t* packet_edge, vid_t prev_vid, uint32_t current_edge_size, edge_t* new_edge, name_space_t name_space)
{
  uint32_t i;

  if (packet_edge->condition.first) {
    vector_t<packet_condition_t*>* packet_conds = packet_edge->condition.second;

    condition_list_t* conditions = new condition_list_t();
    for(i=0; i<packet_conds->size(); i++) {

      packet_condition_t* packet_cond = packet_conds->at(i);

      // get the state name
      string name =  packet_cond->name;

      // get the direction
      port_direction_t dir = port->get_dir();

      // get the value
      value_t val = packet_cond->val;
 
      // create the new state
      if ( !name_space.contains(name) ) {
        fprintf(stderr, "Dont contain : %s\n", name.c_str());
        assert(name_space.contains(name));
      }
      pair<uint32_t, uint32_t> cut = name_space.get(name);

      string port_name = port->get_name();
      condition_t* new_state = new control_condition_t(port_name, dir, val, cut.second, cut.first);

      // put it into our list
      conditions->push_back(new_state);
    }
    new_edge->conditions.push_back(conditions);
  }

  field_mapping_t field_mapping = packet_edge->field;
  // if its data, put it in data.
  if (field_mapping.type == PACKET_PRIM_DATA) {
    // we can easily insert the cut here in the parent.
    // yeah so [current edge size, current edge size + field size]
    uint32_t msb = current_edge_size+field_mapping.size()-1;
    uint32_t lsb = current_edge_size;
    //pair<uint32_t, uint32_t> packet_cut = pair<uint32_t, uint32_t>(lsb, current_edge_size+field_mapping.size());
    // i think packet cut here shud be strictly part of transaction.
    string transaction_name = port->get_name() + "." +  field_mapping.field;
    if (port->get_dir() == PORT_DIR_OUT) {
      transaction_t* send = new transaction_t(transaction_name, port->get_dir(), field_mapping.params, msb, lsb);
      new_edge->sends.push_back(send);
      // send->print();
    }
    else {
      assert(port->get_dir() == PORT_DIR_IN);
      transaction_t* rec = new transaction_t(transaction_name, port->get_dir(), field_mapping.params, msb, lsb);
      new_edge->receives.push_back(rec);
    }
  }

  // printf(" edge size = %u %u\n", current_edge_size, current_edge_size + field_mapping.size());
  current_edge_size += field_mapping.size();

  if (current_edge_size == port->width) {
    vid_t next_vid = add_vertex();
    this->add_edge(prev_vid, next_vid, new_edge);
    prev_vid = next_vid;

    // new_edge->print();

    current_edge_size = 0;
    vector_t<transaction_t*> new_sends = vector_t<transaction_t*>();
    vector_t<transaction_t*> new_receives = vector_t<transaction_t*>();
    condition_list_t new_conditions = condition_list_t();
    new_edge = new edge_t(new_conditions, new_sends, new_receives);
  }
  else if (current_edge_size > port->width) {
    fprintf(stderr, "Current edge size greater than bitwidth: %u %u\n", current_edge_size, port->width);
    assert( !(current_edge_size > port->width) );
  }
  else {
    // printf("current edge size = %u\n", current_edge_size);
  }

  for(i=0; i<packet_edge->branches.size(); i++) {
    edge_t* new_edge_copy = new_edge->copy();
    this->add_packet_edge(port, packet_edge->branches.at(i), prev_vid, current_edge_size, new_edge_copy, name_space);
  }
}

fsm_t::fsm_t(port_t* port, packet_t* packet)
{
  name_space_t space = packet->name_space();

  pair<packet_edge_t*, vector_t<packet_edge_t*>*>* ret = packet->paths();
  packet_edge_t* packet_edge = ret->first;

  // create our root.
  vid_t root = this->add_root();
  vid_t prev_vid = root;

  // starting at 0 obvi
  uint32_t current_edge_size = 0;  

  // create the new edge we will be adding
  vector_t<transaction_t*> new_sends = vector_t<transaction_t*>();
  vector_t<transaction_t*> new_receives = vector_t<transaction_t*>();
  condition_list_t new_conditions = condition_list_t();
  edge_t* new_edge = new edge_t(new_conditions, new_sends, new_receives);

  this->add_packet_edge(port, packet_edge, prev_vid, current_edge_size, new_edge, space);
}

// reg fsm
vid_t fsm_t::add_vertex(vertex_type_t type)
{
  // get new vid
  vid_t v = boost::add_vertex(this->g);

  // create the new vertex. can assume it is a body vertex
  vertex_t* new_vertex = new vertex_t(type, v);  

  // assign it
  this->g[v] = new_vertex;

  return v;
} 

// reg fsm
vid_t fsm_t::add_vertex()
{
  vid_t v = this->add_vertex(FSM_BODY);
  return v;
}

// reg fsm
vid_t fsm_t::add_root()
{
  vid_t v = this->add_vertex(FSM_ROOT);
  this->root = v;
  return v;
}

// reg fsm
void fsm_t::set_sink(vid_t v)
{
  vertex_t* sink = this->g[v];
  sink->type = FSM_SINK;
}

// merged fsm
vid_t fsm_t::add_vertex(vertex_type_t type, vector<vid_t> key)
{
  vid_t v;
  vertex_t* new_vertex = new vertex_t(type, key); // bug: shud not use body all the time

  // if we already have it, do not add again
  if (this->contains(new_vertex)) {
    v = this->get(new_vertex);
/*
    printf("already contain:" );
    int i;
    for(i=0; i<key.size(); i++) {
      printf("%lu ", key[i]);
    }
    printf("\n");
*/
  }
  else {
    // get a new vid
    v = boost::add_vertex(this->g);
    // create new vertex
    // vertex_t* new_vertex = new vertex_t(type, key); // bug: shud not use body all the time
    // assign it
    this->g[v] = new_vertex;
    // put it in our table
    this->put(new_vertex, v);
/*
    printf("putting:" );
    int i;
    for(i=0; i<key.size(); i++) {
      printf("%lu ", key[i]);
    }
    printf("\n");
*/
  }
  return v;
}

// merged fsm
vid_t fsm_t::add_vertex(vertex_type_t type, vector<vid_t> key, transaction_sum_t sum)
{
  vid_t v;
  vertex_t* new_vertex = new vertex_t(type, key, sum); // bug: shud not use body all the time

  // if we already have it, do not add again
  if (this->contains(new_vertex)) {
    v = this->get(new_vertex);
  }
  else {
    // get a new vid
    v = boost::add_vertex(this->g);
    // create new vertex
    // vertex_t* new_vertex = new vertex_t(type, key, sum); // bug: shud not use body all the time
    // assign it
    this->g[v] = new_vertex;
    // put it in our table
    this->put(new_vertex, v);
  }
  return v;
}

// merged fsm
vid_t fsm_t::add_vertex(vector<vid_t> key)
{
  vid_t v = this->add_vertex(FSM_BODY, key);
  return v;
}

// merged fsm
vid_t fsm_t::add_vertex(vector<vid_t> key, transaction_sum_t sum)
{
  vid_t v = this->add_vertex(FSM_BODY, key, sum);
  return v;
}

// merged fsm
vid_t fsm_t::add_root(vector<vid_t> key)
{
  vid_t v = this->add_vertex(FSM_ROOT, key);
  this->root = v;
  return v;
}

// merged fsm
vid_t fsm_t::add_root(vector<vid_t> key, transaction_sum_t sum)
{
  vid_t v = this->add_vertex(FSM_ROOT, key, sum);
  this->root = v;
  return v;
}

// merged fsm
vid_t fsm_t::add_sink(vector<vid_t> key)
{
  vid_t v = this->add_vertex(FSM_SINK, key);
  return v;
}

eid_t fsm_t::add_edge(vid_t v1, vid_t v2, edge_t* new_edge)
{
  pair<eid_t, bool> e = boost::add_edge(v1, v2, this->g);
  assert(e.second);
  this->g[e.first] = new_edge;

  return e.first;
}

/* 
so we can remove explicit sinks, and make sinks be the last vertex 

because we no longer need the idea of a explicit sink
which is great.

simplifying things is good.

or we can jsut use this method which is kinda hacky but w.e.

moving forward with multiple fsms, i am not sure what the correct asnwer is. 

lets make the change. it is much better.
*/

bool fsm_t::sink_exists(eid_t e)
{
/*
  vid_t src_vid = source(e, this->g);
  std::pair<graph_t::out_edge_iterator, graph_t::out_edge_iterator> range = out_edges(src_vid, this->g);
  
  for(graph_t::out_edge_iterator itr = range.first; itr != range.second; itr++) {
    vid_t vid = target(*itr, this->g);
    vertex_t* vtx = this->g[vid];
    if (vtx->get_type() == FSM_SINK) {
      return true;
    }
  }

  return false;
*/

/*
  vid_t dst_vid = target(e, this->g);
  vertex_t* dst_vertex = this->g[dst_vid];
  return (dst_vertex->type == FSM_SINK);
*/

  vid_t dst_vid = target(e, this->g);
  vertex_t* dst_vertex = this->g[dst_vid];
  if (dst_vertex->type == FSM_SINK) {
    return true;
  }

  std::pair<graph_t::out_edge_iterator, graph_t::out_edge_iterator> range = out_edges(dst_vid, this->g);
  if(range.first == range.second) {
    return true;
  }

  return false;

}

uint32_t fsm_t::size()
{
  return num_vertices(this->g);
}

paths_t* fsm_t::paths()
{
  pair<paths_t*, bool> explore(new paths_t(), false);

  std::pair<graph_t::out_edge_iterator, graph_t::out_edge_iterator> range = out_edges(this->root, this->g);
  for(graph_t::out_edge_iterator itr = range.first; itr != range.second; itr++) {

    edge_t* edge = this->g[*itr];
    pair<paths_t*, bool> explore_ret = paths_r(*itr);
    bool ret_success = explore_ret.second;
    paths_t* ret_paths = explore_ret.first->copy();

    if (ret_success) {
      explore.second = true;
      if (ret_paths->size() == 0) {
        // create a new path
        path_t* path = new path_t();
        path->add(*itr, edge);
        // add it to return paths
        explore.first->add(path);
      }
      else {
        // add this edge to all ret paths
        ret_paths->add(*itr, edge);
        // add these paths to paths
        explore.first->add(ret_paths);
      }
    }
  }
  assert(explore.second);
  return explore.first;
}

pair<paths_t*, bool> fsm_t::paths_r(eid_t e)
{
  pair<paths_t*, bool> explore(new paths_t(), false);

  vid_t dest_vid = target(e, this->g);
  vertex_t* dest_vertex = this->g[dest_vid];

  if (this->sink_exists(e)) {
    explore.second = true;
    return explore;
  }

  std::pair<graph_t::out_edge_iterator, graph_t::out_edge_iterator> range = out_edges(dest_vid, this->g);
  for(graph_t::out_edge_iterator itr = range.first; itr != range.second; itr++) {

    edge_t* edge = this->g[*itr];
    pair<paths_t*, bool> explore_ret = paths_r(*itr);
    bool ret_success = explore_ret.second;
    paths_t* ret_paths = explore_ret.first;

    if (ret_success) {
      explore.second = true;
      if (ret_paths->size() == 0) {
        // create a new path
        path_t* path = new path_t();
        path->add(*itr, edge);
        // add it to return paths
        explore.first->add(path);
      }
      else {
        // add this edge to all ret paths
        ret_paths->add(*itr, edge);
        // add these paths to paths
        explore.first->add(ret_paths);
      }
    }

  }
  return explore;
}

void fsm_t::to_visualize(std::string filename)
{
  std::ofstream outfile(filename.c_str());
  write_graphviz(outfile, this->g, vertex_writer_t(this->g), edge_writer_t(this->g));
}

bool fsm_t::contains(vertex_t* key)
{
  map<vertex_t*, vid_t>::iterator it = this->table.find(key);

/*
  if ( it != this->table.end() ) {
    int i;
    for(i=0; i<key.size(); i++) {
      assert( this->table.find(key)->first[i] == key[i] );
    }
  }
*/

  return (it != this->table.end());
}

vid_t fsm_t::get(vertex_t* key)
{
  assert( this->contains(key) );
  return this->table[key];
}

void fsm_t::put(vertex_t* key, vid_t value)
{
  assert( !this->contains(key) );
  pair<vertex_t*,vid_t> new_mapping(key, value);
  this->table.insert(new_mapping); 
}

// this shud be unique transactions, not unique strings
// so we are sticking the transaction in there and only comparing with name, packet_cut
vector_t<transaction_t*> fsm_t::unique_locations(port_ref_t* ref)
{
  int i;
  uint32_t num_locations = 0;

  // we are just using this as a set.
  // we cud probably add this in the common.h
  map_comp_t<transaction_t*, uint32_t, transaction_comparator_t> transaction_map;

  std::pair<graph_t::edge_iterator, graph_t::edge_iterator> range = edges(this->g);
  for(graph_t::edge_iterator itr = range.first; itr != range.second; itr++) {

    edge_t* edge = this->g[*itr];

    if (ref->get_dir() == PORT_DIR_OUT) {
      for(i=0; i<edge->num_sends(); i++) {
        transaction_t* send = edge->sends.at(i);
        if ( ref->compare(send)==0 ) {
          if ( !transaction_map.contains(send) ) {
            transaction_map.put(send, 1);
            num_locations++; 
          }
        }
      }     
    }

    else {
      assert(ref->get_dir() == PORT_DIR_IN);
      for(i=0; i<edge->num_receives(); i++) {
        transaction_t* rec = edge->receives.at(i);
        if ( ref->compare(rec)==0 ) {
          if ( !transaction_map.contains(rec) ) {
            transaction_map.put(rec, 1);
            num_locations++; 
          }
        }
      }  
    }

  }

  // its okay if we dont find anything ... we just dont create a mux for it.
  /*
  if (num_locations == 0) {
    fprintf(stderr, "Never found a transaction with this match %s\n", ref->to_string().c_str());
    assert( !(num_locations == 0) );
  }
  */

  vector_t<transaction_t*> unique;
  
  for (unique_transaction_map_type::iterator itr=transaction_map.begin(); itr!=transaction_map.end(); ++itr) {
    transaction_t* transaction = itr->first;
    unique.push_back(itr->first);

    // transaction->print();
    // printf("\n");
  }

  return unique;
}

vector_t<port_ref_t*> fsm_t::unique_matches(transaction_t* transaction)
{
  int i, j;

  // we are just using this as a set.
  // we cud probably add this in the common.h
  map_comp_t<port_ref_t*, uint32_t, port_ref_comparator_t> match_map;

  std::pair<graph_t::edge_iterator, graph_t::edge_iterator> range = edges(this->g);
  for(graph_t::edge_iterator itr = range.first; itr != range.second; itr++) {
    edge_t* edge = this->g[*itr];

    // make sure this is a receive.
    assert(transaction->get_dir() == PORT_DIR_IN);
    for(i=0; i<edge->num_receives(); i++) {
      transaction_t* rec = edge->receives.at(i);
      if ( transaction->compare(rec)==0 ) {
        vector_t<port_ref_t*> sources = rec->source();

        for(j=0; j<sources.size(); j++) {
          port_ref_t* next_source = sources.at(j);

          if ( !match_map.contains(next_source) ) {
            match_map.put(next_source, 1);
          }
        }
      }
    }
  }

  vector_t<port_ref_t*> matches;

  for (match_map_type::iterator itr=match_map.begin(); itr!=match_map.end(); ++itr) {
    port_ref_t* match = itr->first;
    matches.push_back(itr->first);

    // match->print();
    // printf("\n");
  }

  return matches;
}

uint32_t fsm_t::num_locations(port_ref_t* ref)
{
  /*
  This gets a little funky because when we merge the edges.
  We dont flip the direction of the sends and receives and we probably shouldnt.
  However we setup the code to do this.
  Instead of copying we could call edge->reverse().
  */

  int i;
  uint32_t num_locations = 0;

  // we are just using this as a set.
  // we cud probably add this in the common.h
  map_comp_t<transaction_t*, uint32_t, transaction_comparator_t> transaction_map;

  std::pair<graph_t::edge_iterator, graph_t::edge_iterator> range = edges(this->g);
  for(graph_t::edge_iterator itr = range.first; itr != range.second; itr++) {

    edge_t* edge = this->g[*itr];

    if (ref->get_dir() == PORT_DIR_OUT) {
      for(i=0; i<edge->num_sends(); i++) {
        transaction_t* send = edge->sends.at(i);
        if ( ref->compare(send)==0 ) {
          if ( !transaction_map.contains(send) ) {
            transaction_map.put(send, 1);
            num_locations++; 
          }
        }
      }     
    }

    else {
      assert(ref->get_dir() == PORT_DIR_IN);
      for(i=0; i<edge->num_receives(); i++) {
        transaction_t* rec = edge->receives.at(i);
        if ( ref->compare(rec)==0 ) {
          if ( !transaction_map.contains(rec) ) {
            transaction_map.put(rec, 1);
            num_locations++; 
          }
        }
      }  
    }

  }

  // should probably have a contains function for this.
  if (num_locations == 0) {
    fprintf(stderr, "Never found a transaction with this match %s\n", ref->to_string().c_str());
    assert( !(num_locations == 0) );
  }

  return num_locations;
}

uint32_t fsm_t::largest_width(port_ref_t* ref)
{
/*
  This gets a little funky because when we merge the edges.
  We dont flip the direction of the sends and receives and we probably shouldnt.
  However we setup the code to do this.
  Instead of copying we could call edge->reverse().
*/

  int i;
  int largest_width = -1;

  std::pair<graph_t::edge_iterator, graph_t::edge_iterator> range = edges(this->g);
  for(graph_t::edge_iterator itr = range.first; itr != range.second; itr++) {
    edge_t* edge = this->g[*itr];
    
    if (ref->get_dir() == PORT_DIR_OUT) {
      for(i=0; i<edge->num_sends(); i++) {
        transaction_t* send = edge->sends.at(i);
        if ( ref->compare(send)==0 && largest_width < send->weight() ) {
          largest_width = send->weight();
        }
      }     
    }
    else {
      assert(ref->get_dir() == PORT_DIR_IN);
      for(i=0; i<edge->num_receives(); i++) {
        transaction_t* rec = edge->receives.at(i);
        if ( ref->compare(rec)==0 && largest_width < rec->weight() ) {
          largest_width = rec->weight();
        }
      }  
    }

  }

  // should probably have a contains function for this.
  if (largest_width < 0) {
    fprintf(stderr, "Never found a transaction with this match %s\n", ref->to_string().c_str());
    assert( !(largest_width < 0) );
  }

  return largest_width;
}

// dont like how this fits of feels in fsm.
// but oh well.
uint32_t fsm_t::control_in_width()
{
  uint32_t sum=0;
  uint32_t i;
  for(i=0; i<this->control_in.size(); i++) {
    sum += this->control_in.at(i).len;
  }
  return sum;
}

// dont like how this fits of feels in fsm.
// but oh well.
uint32_t fsm_t::control_out_width()
{
  uint32_t sum=0;
  uint32_t i;
  for(i=0; i<this->control_out.size(); i++) {
    sum += this->control_out.at(i).len;
  }
  return sum;
}

std::string fsm_t::output_condition(edge_t* e)
{
  string output_string;

  // printf("control out size : %u\n", this->control_out.size());

  int i, j;
  for(i=0; i<this->control_in.size(); i++) {
    control_pos_t control_pos = this->control_in.at(i);  
  
    // control_pos.cond->print();
    // printf("\n");

    if(e->contains_condition(control_pos.cond)) {
      uint32_t value = e->get(control_pos.cond);
      // we prepend here because we want the order to be lsb->msb when indexing
      output_string = output_string + num2binary(value, control_pos.len);
    }
    else {
      // we prepend here because we want the order to be lsb->msb when indexing
      output_string = output_string + std::string(control_pos.len, '0');
    } 
  }

  return output_string;
}

std::string fsm_t::input_condition(edge_t* e)
{
  string input_string;

  // printf("control in size : %u\n", this->control_in.size());

  int i, j;
  for(i=0; i<this->control_out.size(); i++) {
    control_pos_t control_pos = this->control_out.at(i); 

    // control_pos.cond->print();
    // printf("\n");   

    if(e->contains_condition(control_pos.cond)) {
      uint32_t value = e->get(control_pos.cond);
      // we prepend here because we want the order to be lsb->msb when indexing
      input_string = input_string + num2binary(value, control_pos.len);
    }
    else {
      // we prepend here because we want the order to be lsb->msb when indexing
      input_string = input_string + std::string(control_pos.len, '?');
    }
  }

  return input_string;
}

string fsm_t::to_verilog()
{
  assert(0);
  string fsm_string;
  return fsm_string;
}

string fsm_t::to_string()
{
  string fsm_string;

  std::pair<graph_t::edge_iterator, graph_t::edge_iterator> range = edges(this->g);
  for(graph_t::edge_iterator itr = range.first; itr != range.second; itr++) {
    edge_t* edge = this->g[*itr];
    fsm_string += edge->to_string();
    fsm_string += "\n";
  }
  return fsm_string;
}

void fsm_t::print()
{
  printf("%s", this->to_string().c_str());
}




