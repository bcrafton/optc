
#include "edge.h"

using namespace std;
using namespace boost;

static uint32_t edge_id;
static uint32_t edges_destructed;

// edge

// want this constructor for tree_transaction
edge_t::edge_t(condition_list_t conditions, vector_t<transaction_t*> sends, vector_t<transaction_t*> receives)
{
  this->id = edge_id;
  edge_id++;

  this->sends = sends;
  this->receives = receives;
  this->conditions = conditions;
}

edge_t::edge_t(edge_t* e1, edge_t* e2)
{
  this->id = edge_id;
  edge_id++;

  this->sends.push_back(e1->sends);
  this->sends.push_back(e2->sends);

  this->receives.push_back(e1->receives);
  this->receives.push_back(e2->receives);

  // this is dumb
  int i;
  for(i=0; i<e1->conditions.size(); i++) {
    this->conditions.push_back(e1->conditions.at(i));
  }
  for(i=0; i<e2->conditions.size(); i++) {
    this->conditions.push_back(e2->conditions.at(i));
  }
}

edge_t::edge_t(vector_t<edge_t*> edges)
{
  this->id = edge_id;
  edge_id++;

  uint32_t i, j;
  for(i=0; i<edges.size(); i++) {
    edge_t* current_edge = edges.at(i);

    this->sends.push_back(current_edge->sends);

    this->receives.push_back(current_edge->receives);

    // this is dumb
    // we shud make this thing inherit from vector_t
    for(j=0; j<current_edge->conditions.size(); j++) {
      this->conditions.push_back(current_edge->conditions.at(j));
    }
  }
  
}

edge_t::~edge_t()
{
  int i;

  for(i=0; i<this->sends.size(); i++) {
    delete this->sends.at(i);
  }

  for(i=0; i<this->receives.size(); i++) {
    delete this->receives.at(i);
  }

  // we never copy these.
  for(i=0; i<this->conditions.size(); i++) {
    // delete this->conditions.at(i);
  }

  edges_destructed++;
  // printf("# edges: %u # edges destructed: %u\n", edge_id, edges_destructed);
}

void edge_t::print()
{
  printf("%s", this->to_string().c_str());
}

string edge_t::to_string() 
{
  string edge_string;

  edge_string += "Edge: ";
  edge_string += std::to_string( (long long unsigned int) this->id );
  edge_string += "\n";
  edge_string += "conditions: ";
  edge_string += this->conditions.to_string();
  edge_string += "\n";  

  int i;

  edge_string += "sends: ";
  for(i=0; i<this->sends.size(); i++) {
    edge_string += this->sends.at(i)->to_string() + " ";
  }
  edge_string += "\n";  

  edge_string += "receives: ";
  for(i=0; i<this->receives.size(); i++) {
    edge_string += this->receives.at(i)->to_string() + " ";
  }
  edge_string += "\n";  

  return edge_string;
}

//// MAY NEED TO CHANGE ////
// like if we want to ask if contains transaction not just the string name.

bool edge_t::contains_receive(string receive)
{
  uint32_t i;
  for(i=0; i<this->receives.size(); i++) {
    if(receive == this->receives.at(i)->get_name()) {
      return true;
    }
  }
  return false;
}

bool edge_t::contains_receive(port_ref_t* ref)
{
  uint32_t i;
  for(i=0; i<this->receives.size(); i++) {
    if(ref->compare( this->receives.at(i) ) == 0) {
      return true;
    }
  }
  return false;
}

bool edge_t::contains_send(string send)
{
  uint32_t i;
  for(i=0; i<this->sends.size(); i++) {
    if(send == this->sends.at(i)->get_name()) {
      return true;
    }
  }
  return false;
}

bool edge_t::contains_send(port_ref_t* ref)
{
  uint32_t i;
  for(i=0; i<this->sends.size(); i++) {
    if(ref->compare( this->sends.at(i) ) == 0) {
      return true;
    }
  }
  return false;
}

bool edge_t::contains_transaction(string transaction)
{
  uint32_t i;
  for(i=0; i<this->sends.size(); i++) {
    if(transaction == this->sends.at(i)->get_name()) {
      return true;
    }
  }
  for(i=0; i<this->receives.size(); i++) {
    if(transaction == this->receives.at(i)->get_name()) {
      return true;
    }
  }
  return false;
}

bool edge_t::contains_transaction(port_ref_t* ref)
{
  uint32_t i;
  for(i=0; i<this->sends.size(); i++) {
    if(ref->compare( this->sends.at(i) ) == 0) {
      return true;
    }
  }
  for(i=0; i<this->receives.size(); i++) {
    if(ref->compare( this->receives.at(i) ) == 0) {
      return true;
    }
  }
  return false;
}

bool edge_t::contains_condition(string condition)
{
  uint32_t i;
  for(i=0; i<this->conditions.size(); i++) {
    if(condition == this->conditions.at(i)->get_name()) {
      return true;
    }
  }
  return false;
}

int edge_t::get(string condition)
{
  assert( this->contains_condition(condition) );
  
  uint32_t i;
  for(i=0; i<this->conditions.size(); i++) {
    if(condition == this->conditions.at(i)->get_name()) {
      assert(this->conditions.at(i)->val.type == VALUE_TYPE_NUMBER);
      return this->conditions.at(i)->val.num;
    }
  }

  assert(0);
}

bool edge_t::contains_condition(condition_t* condition)
{
  uint32_t i;
  for(i=0; i<this->conditions.size(); i++) {
    condition_t* curr_cond = this->conditions.at(i);
    if(curr_cond->compare(condition) == 0) {
      return true;
    }
  }
  return false;
}

int edge_t::get(condition_t* condition)
{
  assert( this->contains_condition(condition) );
  
  uint32_t i;
  for(i=0; i<this->conditions.size(); i++) {
    condition_t* curr_cond = this->conditions.at(i);
    if(curr_cond->compare(condition) == 0) {
      return curr_cond->val.num;
    }
  }

  assert(0);
}

edge_t* edge_t::copy()
{
  vector_t<transaction_t*> new_sends;
  vector_t<transaction_t*> new_receives;

  int i;
  for(i=0; i<this->sends.size(); i++) {
    transaction_t* send = new transaction_t( this->sends.at(i) );
    new_sends.push_back(send);
  }
  for(i=0; i<this->receives.size(); i++) {
    transaction_t* receive = new transaction_t( this->receives.at(i) );
    new_receives.push_back(receive);
  }

  edge_t* new_edge = new edge_t(this->conditions, new_sends, new_receives);
  return new_edge;
}

edge_t* edge_t::reverse()
{
  edge_t* new_edge = this->copy();
  int i;
  for(i=0; i<this->sends.size(); i++) {
    transaction_t* send = new_edge->sends.at(i);
    send->reverse_dir();
  }
  for(i=0; i<this->receives.size(); i++) {
    transaction_t* rec = new_edge->receives.at(i);
    rec->reverse_dir();
  }
}

void edge_t::add(edge_t* other_edge)
{
  int i, j;
  for(i=0; i<this->sends.size(); i++) {
    transaction_t* send = this->sends.at(i);

    for(j=0; j<other_edge->sends.size(); j++) {
      transaction_t* other_send = other_edge->sends.at(j);

      if (send->get_name() == other_send->get_name()) {
        send->matches.push_back(other_send->matches);
      }
    }
  }

  for(i=0; i<this->receives.size(); i++) {
    transaction_t* rec = this->receives.at(i);
    
    for(j=0; j<other_edge->receives.size(); j++) {
      transaction_t* other_rec = other_edge->receives.at(j);

      if (rec->get_name() == other_rec->get_name()) {
        rec->matches.push_back(other_rec->matches);
      }
    }
  }

}

uint32_t edge_t::num_sends()
{
  return this->sends.size();
}

uint32_t edge_t::num_receives()
{
  return this->receives.size();
}

bool edge_t::has_send()
{
  return this->sends.size() > 0;
}

bool edge_t::has_receive()
{
  return this->receives.size() > 0;
}
