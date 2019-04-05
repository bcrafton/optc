
#include "transition.h"

using namespace std;
using namespace boost;

// tree transition

void tree_transition_t::set_tail(vector<int> tail)
{
  this->tail = tail;
}

void tree_transition_t::add_tail(int v)
{
  this->tail.push_back(v);
}

void tree_transition_t::add_head(int v)
{
  this->head = v;
}

void tree_transition_t::add_edge(edge_t* e)
{
  this->edge = e;
}

tree_transition_t* tree_transition_t::copy()
{
  tree_transition_t* t = new tree_transition_t();

  t->tail = this->tail;
  t->edge = this->edge;
  t->head = this->head;
}

// forest transition

void forest_transition_t::add_tail(vector<int> tail)
{
  int i;
  for(i=0; i<tail.size(); i++) {
    this->tail.push_back(tail[i]);    
  }
}

void forest_transition_t::set_tail(vector<int> tail)
{
  this->tail = tail;
}

void forest_transition_t::add_tail(int v)
{
  this->tail.push_back(v);
}

void forest_transition_t::add_head(int v)
{
  this->head.push_back(v);
}

void forest_transition_t::add_edge(edge_t* e)
{
  this->edge.push_back(e);
}

void forest_transition_t::add_head(vector<int> head)
{
  int i;
  for(i=0; i<head.size(); i++) {
    this->head.push_back(head[i]);    
  }
}

void forest_transition_t::add_edge(vector<edge_t*> edge)
{
  int i;
  for(i=0; i<edge.size(); i++) {
    this->edge.push_back(edge[i]);    
  }
}








