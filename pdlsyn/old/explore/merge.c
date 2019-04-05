
#include "merge.h"

using namespace std;
using namespace boost;

// merged fsm

vid_t merged_fsm_t::add_root(merged_vertex_t vertex)
{
  vid_t v = this->add_vertex(vertex);
  this->root = v;
  return v;
}

vid_t merged_fsm_t::add_vertex(merged_vertex_t vertex)
{
  vid_t v = boost::add_vertex(this->g);
  this->g[v] = vertex;

  this->put(vertex, v);

  return v;
}

eid_t merged_fsm_t::add_edge(vid_t v1, vid_t v2, merged_edge_t edge)
{
  pair<eid_t, bool> e = boost::add_edge(v1, v2, this->g);
  assert(e.second);
  this->g[e.first] = edge;

  return e.first;
}

bool merged_fsm_t::contains(merged_vertex_t key)
{
  return (this->vertex_table.count(key) > 0);
}

vid_t merged_fsm_t::get(merged_vertex_t key)
{
  assert( this->contains(key) );
  return this->vertex_table[key];
}

void merged_fsm_t::put(merged_vertex_t key, vid_t value)
{
  assert( !this->contains(key) );
  pair<merged_vertex_t, vid_t> new_mapping(key, value);
  this->vertex_table.insert(new_mapping);
}



#if (DYNAMIC_ALGORITHM == 1)
eid_t merged_fsm_t::add_fsm_r(vid_t new_root, vid_t old_root, merged_fsm_t* fsm) // do not debug this function lightly ... it is a shit show.
{
  assert((this->contains(fsm->g[old_root])));

  std::pair<merged_graph_t::out_edge_iterator, merged_graph_t::out_edge_iterator> range = out_edges(old_root, fsm->g);
  for(merged_graph_t::out_edge_iterator itr = range.first; itr != range.second; itr++) {

    vid_t old_vid = target(*itr, fsm->g);
    merged_vertex_t old_vertex = fsm->g[old_vid];
    merged_edge_t old_edge = fsm->g[*itr];

    vid_t new_vid;
    if (this->contains(old_vertex)) {
      new_vid = this->get(old_vertex);
    }
    else {
      new_vid = this->add_vertex(old_vertex);
    }

    pair<eid_t, bool> e = boost::edge(new_root, new_vid, this->g);
    bool set_edge = !( e.second && (source(e.first, this->g) == new_root) && (target(e.first, this->g) == new_vid) );
    if ( set_edge ) {
      this->add_edge(new_root, new_vid, old_edge);
    }

    this->add_fsm_r(new_vid, old_vid, fsm);
  }
}

eid_t merged_fsm_t::add_fsm(vid_t v, merged_fsm_t* fsm, merged_edge_t edge) // do not debug this function lightly ... it is a shit show.
{

  // add root //

  vid_t new_root;
  merged_vertex_t old_root_vertex = fsm->g[fsm->root];

  if (this->contains(old_root_vertex)) {
    new_root = this->get(old_root_vertex);
  }
  else {
    new_root = this->add_vertex(old_root_vertex);
  }
  this->add_edge(v, new_root, edge);

  // add the next wave of them //

  std::pair<merged_graph_t::out_edge_iterator, merged_graph_t::out_edge_iterator> range = out_edges(fsm->root, fsm->g);
  for(merged_graph_t::out_edge_iterator itr = range.first; itr != range.second; itr++) {

    vid_t old_vid = target(*itr, fsm->g);
    merged_vertex_t old_vertex = fsm->g[old_vid];
    merged_edge_t old_edge = fsm->g[*itr];

    vid_t new_vid;
    if (this->contains(old_vertex)) {
      new_vid = this->get(old_vertex);
    }
    else {
      new_vid = this->add_vertex(old_vertex);
    }

    pair<eid_t, bool> e = boost::edge(new_root, new_vid, this->g);
    bool set_edge = !( e.second && (source(e.first, this->g) == new_root) && (target(e.first, this->g) == new_vid) );
    if ( set_edge ) {
      this->add_edge(new_root, new_vid, old_edge);
    }

    this->add_fsm_r(new_vid, old_vid, fsm);
  }
}
#endif

// need to say
// 1. not equal
// 2. replace
// 3. dont replace

/*
for compare
need to make sure the order is the same
and if u have divergence, then you need to make sure that all options are still avail.
*/
bool merged_fsm_t::compare(merged_fsm_t* fsm)
{
  std::pair<merged_graph_t::out_edge_iterator, merged_graph_t::out_edge_iterator> range1 = out_edges(this->root, this->g);
  std::pair<merged_graph_t::out_edge_iterator, merged_graph_t::out_edge_iterator> range2 = out_edges(fsm->root, fsm->g);

  for(merged_graph_t::out_edge_iterator itr1 = range1.first; itr1 != range1.second; itr1++) {

    bool result = false;
    for(merged_graph_t::out_edge_iterator itr2 = range2.first; itr2 != range2.second; itr2++) {
      merged_edge_t me1 = this->g[*itr1];
      merged_edge_t me2 = fsm->g[*itr2];

      vid_t target1 = target(*itr1, this->g);
      vid_t target2 = target(*itr2, fsm->g);

      merged_state_t next = {target1, target2};

      if (me1.e1->compare_data(me2.e1) && me1.e2->compare_data(me2.e2)) {
        result = this->compare_r(next, fsm);
      }
    }
    if (result == false) return false;
  }
  return true;
}

bool merged_fsm_t::compare_r(merged_state_t current, merged_fsm_t* fsm) {

  merged_vertex_t mv1 = this->g[current.s1];
  merged_vertex_t mv2 = fsm->g[current.s2];

  if( (mv1.v1->type == FSM_SINK) &&
      (mv1.v2->type == FSM_SINK) &&
      (mv1.v1->type == FSM_SINK) &&
      (mv2.v2->type == FSM_SINK) ) {
    return true;
  }

  std::pair<merged_graph_t::out_edge_iterator, merged_graph_t::out_edge_iterator> range1 = out_edges(current.s1, this->g);
  std::pair<merged_graph_t::out_edge_iterator, merged_graph_t::out_edge_iterator> range2 = out_edges(current.s2, fsm->g);

  for(merged_graph_t::out_edge_iterator itr1 = range1.first; itr1 != range1.second; itr1++) {

    bool result = false;
    for(merged_graph_t::out_edge_iterator itr2 = range2.first; itr2 != range2.second; itr2++) {
      merged_edge_t me1 = this->g[*itr1];
      merged_edge_t me2 = fsm->g[*itr2];

      vid_t target1 = target(*itr1, this->g);
      vid_t target2 = target(*itr2, fsm->g);

      merged_state_t next = {target1, target2};

      if (me1.e1->compare_data(me2.e1) && me1.e2->compare_data(me2.e2)) {
        result = this->compare_r(next, fsm);
      }
    }
    if (result == false) return false;
  }
  return true;

}

string merged_fsm_t::to_string()
{
  string fsm_string;
  pair<merged_graph_t::edge_iterator, merged_graph_t::edge_iterator> itr_range = edges(this->g);
  for(merged_graph_t::edge_iterator itr = itr_range.first; itr != itr_range.second; itr++)
  {
    vid_t src = source(*itr, this->g);
    vid_t dest = target(*itr, this->g);

    fsm_string += (this->g)[src].v1->to_string();
    fsm_string += " ";
    fsm_string += (this->g)[src].v2->to_string();
    fsm_string += "\n";
  }
  return fsm_string;
}

string merged_fsm_t::to_verilog()
{
}

void merged_fsm_t::print()
{
  pair<merged_graph_t::out_edge_iterator, merged_graph_t::out_edge_iterator> itr_range = out_edges(this->root, this->g);
  for(merged_graph_t::out_edge_iterator itr = itr_range.first; itr != itr_range.second; itr++)
  {
    vid_t src = source(*itr, this->g);
    vid_t dest = target(*itr, this->g);

    printf( "%s", (this->g)[src].v1->to_string().c_str() );
    printf( "%s\n", (this->g)[dest].v2->to_string().c_str() );

    print_r(dest);
  }
}

void merged_fsm_t::print_r(vid_t dest)
{
  pair<merged_graph_t::out_edge_iterator, merged_graph_t::out_edge_iterator> itr_range = out_edges(dest, this->g);
  for(merged_graph_t::out_edge_iterator itr = itr_range.first; itr != itr_range.second; itr++)
  {
    vid_t src = source(*itr, this->g);
    vid_t dest = target(*itr, this->g);

    printf( "%s", (this->g)[src].v1->to_string().c_str() );
    printf( "%s\n", (this->g)[dest].v2->to_string().c_str() );

    print_r(dest);
  }
}

void merged_fsm_t::to_visualize(std::string filename)
{
  std::ofstream outfile(filename.c_str());
  write_graphviz(outfile, this->g, merged_vertex_writer_t(this->g), merged_edge_writer_t(this->g));
}

uint32_t merged_fsm_t::size()
{
  return num_vertices(this->g);
}

#if (DYNAMIC_ALGORITHM == 0)
eid_t merged_fsm_t::add_fsm(vid_t v, merged_fsm_t* fsm, merged_edge_t edge)
{
  vector<vid_t> orig2copy_data(num_vertices(fsm->g));
  IsoMap mapV = make_iterator_property_map(orig2copy_data.begin(), boost::get(vertex_index, fsm->g));
  boost::copy_graph( fsm->g, this->g, boost::orig_to_copy(mapV) );
  vid_t old_root = mapV[fsm->root];
  this->add_edge(v, old_root, edge);
}
#endif

// merged protocol

merged_protocol_t::merged_protocol_t(protocol_t* p1, protocol_t* p2)//, std::vector< std::pair<std::string, std::string> > port_map)
{
  this->name += p1->name;
  this->name += "2";
  this->name += p2->name;

  //this->port_map = port_map;
  this->merge_ports(p1->ports, p2->ports);
  this->merge_init(p1->init, p2->init);
  this->merge_behavior(p1->behavior, p2->behavior);
}

void merged_protocol_t::merge_ports(port_list_t* ports1, port_list_t*  ports2)
{
  this->ports = new merged_ports_t();
  this->ports->ports1 = ports1;
  this->ports->ports2 = ports2;
}

void merged_protocol_t::merge_init(state_list_t* init1, state_list_t* init2)
{
  this->init = new merged_init_t();
  this->init->init1 = init1;
  this->init->init2 = init2;
}

void merged_protocol_t::merge_behavior(fsm_t* behavior1, fsm_t* behavior2)
{
  this->behavior = state_space(behavior1, behavior2);
}

string merged_protocol_t::to_string()
{
  string protocol_string;

  protocol_string += "PORTS\n";
  protocol_string += this->ports->ports1->to_string();
  protocol_string += this->ports->ports2->to_string();
  protocol_string += "\n";

  protocol_string += "INIT\n";
  protocol_string += this->init->init1->to_string();
  protocol_string += " ";
  protocol_string += this->init->init2->to_string();
  protocol_string += "\n";

  protocol_string += "BEHAVIOR\n";
  protocol_string += this->behavior->to_string();
  protocol_string += "\n";

  return protocol_string;
}

string merged_protocol_t::to_verilog()
{
  string protocol_verilog;
  protocol_verilog += "module ";
  protocol_verilog += this->name;
  protocol_verilog += "(\n";

  protocol_verilog += this->ports->ports1->to_verilog_interface();
  protocol_verilog += this->ports->ports2->to_verilog_interface();

  protocol_verilog += ");\n";

  protocol_verilog += this->ports->ports1->to_verilog_ports();
  protocol_verilog += this->ports->ports2->to_verilog_ports();

  protocol_verilog += "endmodule\n";
  return protocol_verilog;
}

void merged_protocol_t::print()
{
  printf("%s\n", this->to_string().c_str());
}

void merged_protocol_t::to_visualize(std::string filename)
{
  this->behavior->to_visualize(filename);
}

