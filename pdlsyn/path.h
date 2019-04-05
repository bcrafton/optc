
#ifndef PATH_H
#define PATH_H

#include "defines.h"
#include "common.h"

#include "transaction_sum.h"
#include "edge.h"
#include "path.h"
#include "connection.h"

class path_t;
class paths_t;
class path_list_t;

// alright so i think path is gonna shrink
// dont need all this stuff and we dont want it either
// this and schedule is the last thing we are gonna be working on
// want to code the flow right now, we know the high level stuff
// and will figure out the optimizations as we go
// we make path combinations based on deps
// then feed those to the scheduler



/**
 * path is just a list of edges and eids. each path should be a unique list of edges from source to sink in the FSM graph.
 */

class path_t {
  public:
  std::vector< std::pair<eid_t, edge_t*> > edge_list;
  set_comp_t<std::pair<std::string, uint32_t>*, dep_comparator_t> dep_set;
  set_comp_t<std::pair<std::string, uint32_t>*, dep_comparator_t> id_set;

  path_t();
  path_t(path_t* path);

  ~path_t();

  void add(eid_t eid, edge_t* edge);
  void add(path_t *other_path);

  std::pair<eid_t, edge_t*> edge_at(uint32_t index);

  std::string to_string_path();
  void print_path();
  std::string to_string_transactions();
  void print_transactions();

  uint32_t num_cycles();
  uint32_t num_sends();
  uint32_t num_receives();

  std::pair<uint32_t, float> value();

  void set_id();
  void set_dep();

  path_t* copy();
};


/**
 * a list class for path that lets us do operations on all the paths making life easier.
 */

class paths_t {
  private:
  std::vector< path_t* > paths;

  public:
  void add(eid_t e, edge_t* edge);
  void add(path_t* p);
  void add(paths_t* paths);

  path_t* at(uint32_t index);
  uint32_t size();
  paths_t* copy();
  void print();
  void print_transactions();
};


/**
 * a path set is a set of paths that must be considered together because of dependencies. 
 */

class path_set_t {
  public:
  uint32_t size;
  path_t** paths;
  bool* valid;

  path_set_t(uint32_t size);
  path_set_t(path_set_t* pset1, path_set_t* pset2);

  ~path_set_t();

  // regarding "has":
  // with has we were trying to avoid using a no edges boolean vector in make_path_set / schedule
  // in either case we needed to keep track of what edges/path sets we were considering. 
  // it may have been easier to leave path set as is, and keep track of the no-edges.

  // we needed the "valid" array anyways because just because the path set has the index,
  // dosnt mean we are going to have an edge ... we get to the end of the path
  // if ( curr_path->num_cycles() > curr_accum->num_cycles() ) {

  bool has(uint32_t index);

  void add(path_set_t* pset);
  void set(uint32_t index, path_t* path);
  path_t* at(uint32_t index);

  path_set_t* accum();
  path_set_t* copy();

  bool contains(path_set_t*, connections_t*);
  bool equals(path_set_t*, connections_t*);

  float value();

  std::string to_string_path();
  std::string to_string_transactions();
  void print_path();
  void print_transactions();
};

#endif

























