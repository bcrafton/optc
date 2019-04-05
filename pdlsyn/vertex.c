
#include "vertex.h"

using namespace std;
using namespace boost;

// vertex

vertex_t::vertex_t(vertex_type_t type, vid_t key)
{
  this->type = type;
  this->key.push_back(key);
  this->sum.first = false;
}

vertex_t::vertex_t(vertex_type_t type, vector<vid_t> key)
{
  this->type = type;
  this->key = key;
  this->sum.first = false;
}

vertex_t::vertex_t(vertex_type_t type, std::vector<vid_t> key, transaction_sum_t sum)
{
  this->type = type;
  this->key = key;
  this->sum = pair<bool, transaction_sum_t>(true, sum);
}

// hack for paper:
uint32_t vertex_t::id()
{
  assert(this->key.size() == 1);
  return this->key.at(0);
}

string vertex_t::to_string() 
{
  string vertex_string;
  if(this->type == FSM_ROOT)
  {
    vertex_string += "root";
  }
  else if(this->type == FSM_SINK)
  {
    vertex_string += "sink";
  }
  else
  {
    vertex_string += "body";
  }

  int i;
  for(i=0; i<this->key.size(); i++) {
    vertex_string += std::to_string( (long long unsigned int) this->key[i]);
    if ( (i < this->key.size()-1) ) {
      vertex_string += "x";
    }
  }

  if (this->sum.first) {
    vertex_string += this->sum.second.to_vertex_string();
  }

  return vertex_string;
}

vertex_t* vertex_t::copy()
{
  vertex_t* new_vertex = new vertex_t(this->type, this->key);
  return new_vertex;
}

vertex_type_t vertex_t::get_type()
{
  return this->type;
}
