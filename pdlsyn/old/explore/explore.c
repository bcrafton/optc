
#include "defines.h"
#include "merge.h"

using namespace std;
using namespace boost;

long unsigned rec_count = 0;

explore_t::explore_t()
{
  this->fsm = new merged_fsm_t();
}

bool explore_table_t::contains(merged_state_t key)
{
  return (this->vertex_fsm_table.count(key) > 0);
}

merged_fsm_t* explore_table_t::get(merged_state_t key)
{
  assert( this->contains(key) );
  return this->vertex_fsm_table[key];
}

void explore_table_t::put(merged_state_t key, merged_fsm_t* value)
{
  if (this->contains(key)) {
    if(value->size() < this->vertex_fsm_table[key]->size())
    {
      this->vertex_fsm_table[key] = value;
    }
  }
  else {
    pair<merged_state_t, merged_fsm_t*> new_mapping(key, value);
    this->vertex_fsm_table.insert(new_mapping);
  }
}

bool sink_path_exists(fsm_t* f1, fsm_t* f2, merged_state_t prev)
{
  std::pair<graph_t::out_edge_iterator, graph_t::out_edge_iterator> range1 = out_edges(prev.s1, f1->g);
  std::pair<graph_t::out_edge_iterator, graph_t::out_edge_iterator> range2 = out_edges(prev.s2, f2->g);

  for(graph_t::out_edge_iterator itr1 = range1.first; itr1 != range1.second; itr1++) {
    for(graph_t::out_edge_iterator itr2 = range2.first; itr2 != range2.second; itr2++) {
      vid_t next1 = target(*itr1, f1->g);
      vid_t next2 = target(*itr2, f2->g);

      std::pair<eid_t, bool> e1 = edge(prev.s1, next1, f1->g);
      std::pair<eid_t, bool> e2 = edge(prev.s2, next2, f2->g);

      if (!e1.second) {
        fprintf(stderr, "Edge does not exist %lu %lu ", prev.s1, next1);
        fprintf(stderr, "%s", f1->g[prev.s1]->to_string().c_str() );
        fprintf(stderr, " ");
        fprintf(stderr, "%s", f1->g[next1]->to_string().c_str() );
        fprintf(stderr, "\n");
        assert(e1.second);
      }
      if (!e2.second) {
        fprintf(stderr, "Edge does not exist %lu %lu ", prev.s2, next1);
        fprintf(stderr, "%s", f2->g[prev.s2]->to_string().c_str() );
        fprintf(stderr, " ");
        fprintf(stderr, "%s", f2->g[next1]->to_string().c_str() );
        fprintf(stderr, "\n");
        assert(e2.second);
      }
      edge_t* edge1 = f1->g[e1.first];
      edge_t* edge2 = f2->g[e2.first];

      if(edge1->type == FSM_EDGE_SINK) {
        return true;
      }
    }
  }
  return false;
}

bool check_consistency(fsm_t* f1, fsm_t* f2, merged_state_t prev, merged_state_t current, merged_transition_t trans)
{

  edge_t* e1 = f1->g[trans.t1];
  edge_t* e2 = f2->g[trans.t2];

  if((e1->type == FSM_EDGE_SINK) && (e2->type == FSM_EDGE_SINK))
  {
    return true;
  }
  // this is no longer legal.
  // if((trans.t1->type == FSM_EDGE_IMMEDIATE) && (trans.t1->type == FSM_EDGE_SINK))      return true;

  // this case is illegal
  if( !((e1->type == FSM_EDGE_BODY) && (e2->type == FSM_EDGE_BODY)) )
  {
    return false;
  }

  // if there is a path to the sink and we did not take it, return false
  if( sink_path_exists(f1, f2, prev) )
  {
    return false;
  }

  if( ! (e1->condition->transactions->size() == e2->condition->transactions->size()) )
  {
    return false;
  }

  int i;
  for(i=0; i<e1->condition->transactions->size(); i++)
  {
    int ctr=0;
    std::string transaction1 = e1->condition->transactions->at(i);
    std::string transaction2 = e2->condition->transactions->at(i);
    for(int j=0; j<port_map.size();++j)
    {

      if ((transaction1 == port_map[j].first) && (transaction2 == port_map[j].second))
      {
       ++ctr;
     }
    }
    if( ctr==0 )
    {
      return false;
    }
  }

  return true;
}

merged_fsm_t* state_space(fsm_t* f1, fsm_t* f2)
{
  explore_t* explore = new explore_t();
  explore_table_t* t = new explore_table_t();

  vertex_t* v1 = f1->g[ f1->root ]->copy();
  vertex_t* v2 = f2->g[ f2->root ]->copy();
  merged_vertex_t merged_vertex = {v1, v2};

  vid_t root = explore->fsm->add_root( merged_vertex );

  explore->result = EXPLORE_FAIL;

  vector<merged_fsm_t*> rets;

  /////////////////////////////////////////////////////////////////////////////////

  std::pair<graph_t::out_edge_iterator, graph_t::out_edge_iterator> range1 = out_edges(f1->root, f1->g);
  std::pair<graph_t::out_edge_iterator, graph_t::out_edge_iterator> range2 = out_edges(f2->root, f2->g);

  for(graph_t::out_edge_iterator itr1 = range1.first; itr1 != range1.second; itr1++) {
    for(graph_t::out_edge_iterator itr2 = range2.first; itr2 != range2.second; itr2++) {

      vid_t next1 = target(*itr1, f1->g);
      vid_t next2 = target(*itr2, f2->g);

      merged_state_t current = {f1->root, f2->root};
      merged_state_t next = {next1, next2};
      merged_transition_t trans = {*itr1, *itr2};

      if (check_consistency(f1, f2, current, next, trans)) {

        explore_t* ret;
#if (DYNAMIC_ALGORITHM == 1)
        if (t->contains(next)) {
          ret = new explore_t();
          ret->fsm = t->get(next);
          ret->result = EXPLORE_SUCCESS;
        }
        else {
          ret = state_space_r(f1, f2, current, next, t);
        }
#else
        ret = state_space_r(f1, f2, current, next, t);
#endif
        if (ret->result == EXPLORE_SUCCESS) {
          explore->result = EXPLORE_SUCCESS;

          edge_t* e1 = f1->g[*itr1]->copy();
          edge_t* e2 = f2->g[*itr2]->copy();
          merged_edge_t merged_edge = {e1, e2};

          // clean this code up!
#if (DYNAMIC_ALGORITHM == 1)
          bool has = false;
          int i;
          for(i=0; i<rets.size(); i++) {
            if( ret->fsm->compare(rets[i]) ) {
              has = true;
            }
          }
          if( !has ) {
            rets.push_back(ret->fsm);
            explore->fsm->add_fsm(root, ret->fsm, merged_edge);
          }
          // clean this code up!
#else
          explore->fsm->add_fsm(root, ret->fsm, merged_edge);
#endif
        }
      }

    }
  }
  // explore->fsm->print();
  return explore->fsm;

}

explore_t* state_space_r(fsm_t* f1, fsm_t* f2, merged_state_t prev, merged_state_t current, explore_table_t* t)
{
  rec_count++;

  explore_t* explore = new explore_t();

  vertex_t* v1 = f1->g[ current.s1 ]->copy();
  vertex_t* v2 = f2->g[ current.s2 ]->copy();
  merged_vertex_t merged_vertex = {v1, v2};

  vid_t root = explore->fsm->add_root( merged_vertex );

  if(v1->type == FSM_SINK && v2->type == FSM_SINK)
  {
    explore->result = EXPLORE_SUCCESS;
    return explore;
  }

  explore->result = EXPLORE_FAIL;

  vector<merged_fsm_t*> rets;

  /////////////////////////////////////////////////////////////////////////////////

  std::pair<graph_t::out_edge_iterator, graph_t::out_edge_iterator> range1 = out_edges(current.s1, f1->g);
  std::pair<graph_t::out_edge_iterator, graph_t::out_edge_iterator> range2 = out_edges(current.s2, f2->g);

  for(graph_t::out_edge_iterator itr1 = range1.first; itr1 != range1.second; itr1++) {
    for(graph_t::out_edge_iterator itr2 = range2.first; itr2 != range2.second; itr2++) {

      vid_t next1 = target(*itr1, f1->g);
      vid_t next2 = target(*itr2, f2->g);
      merged_state_t next = {next1, next2};
      merged_transition_t trans = {*itr1, *itr2};

      if (check_consistency(f1, f2, current, next, trans))
      {

        explore_t* ret;
        if (t->contains(next)) {
          ret = new explore_t();
          ret->fsm = t->get(next);
          ret->result = EXPLORE_SUCCESS;
        }
        else {
          ret = state_space_r(f1, f2, current, next, t);
        }

        if (ret->result == EXPLORE_SUCCESS) {
          explore->result = EXPLORE_SUCCESS;

          edge_t* e1 = f1->g[*itr1]->copy();
          edge_t* e2 = f2->g[*itr2]->copy();
          merged_edge_t merged_edge = {e1, e2};

#if (DYNAMIC_ALGORITHM == 1)
          // clean this code up!
          bool has = false;
          int i;
          for(i=0; i<rets.size(); i++) {
            if( ret->fsm->compare(rets[i]) ) {
              has = true;
            }
          }
          if( !has ) {
            rets.push_back(ret->fsm);
            explore->fsm->add_fsm(root, ret->fsm, merged_edge);
          }
          // clean this code up!
#else
          explore->fsm->add_fsm(root, ret->fsm, merged_edge);
#endif
        }
      }
    }
  }
  if (explore->result == EXPLORE_SUCCESS) {
    t->put(current, explore->fsm);
  }
  return explore;
}
