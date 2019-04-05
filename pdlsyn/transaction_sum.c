
#include "transaction_sum.h"

using namespace std;
using namespace boost;

// transaction sum

transaction_sum_t::transaction_sum_t()
{
}

transaction_sum_t::transaction_sum_t(transaction_sum_t* sum)
{
  for (transaction_sum_type::iterator itr=sum->begin(); itr!=sum->end(); ++itr) {
    port_ref_t* key = itr->first;
    transaction_body_t* body = itr->second;
    transaction_body_t* new_body = new transaction_body_t(body);
    this->put(key, new_body);
  }
}

void transaction_sum_t::add(transaction_t* transaction, connections_t* connections)
{
  if ( this->contains( (port_ref_t*) transaction ) ) {
    //printf("going here\n");
    transaction_body_t* body = this->get( transaction );
    body->add( &transaction->body );
    this->set( transaction, body );
  }
  else {
    //printf("not going here\n");
    transaction_body_t* body = new transaction_body_t( &transaction->body );
    this->put( transaction, body );
  }
}

void transaction_sum_t::subtract(transaction_sum_t sum, connections_t* connections)
{ 

  for (transaction_sum_type::iterator rec_itr=sum.begin(); rec_itr!=sum.end(); ++rec_itr) {
    for (transaction_sum_type::iterator send_itr=this->begin(); send_itr!=this->end(); ++send_itr) {

      port_ref_t* key1 = send_itr->first;
      transaction_body_t* sum1 = send_itr->second;
      port_ref_t* key2 = rec_itr->first;
      transaction_body_t* sum2 = rec_itr->second;
      if ( connections->contains(key1, key2) ) {
/*
        key1->print();
        printf(" ");
        key2->print();
        printf("\n");
        printf("calling subtract\n");
*/
        sum1->subtract(sum2);
      }

    }
  }
}

bool transaction_sum_t::contains_transactions(vector_t<transaction_t*> transactions, connections_t* connections) {
  int i;
  for(i=0; i<transactions.size(); i++) {
    bool ret = false;
    for (transaction_sum_type::iterator itr=this->begin(); itr!=this->end(); ++itr) {

      port_ref_t* key1 = transactions.at(i);
      port_ref_t* key2 = itr->first;
      transaction_body_t* body = itr->second;

      if ( connections->contains(key1, key2) ) {
        if (body->weight() >= transactions.at(i)->weight()) {
          ret = true;
        } 
      }

    }
    if (!ret) {
      return false;
    }
  }
  return true;
}

bool transaction_sum_t::zero() {
  for (transaction_sum_type::iterator itr=this->begin(); itr!=this->end(); ++itr) {
    if ( ! itr->second->zero() ) {
      return false;
    }
  }
  return true;
}

bool transaction_sum_t::positive() {
  for (transaction_sum_type::iterator itr=this->begin(); itr!=this->end(); ++itr) {
    if ( ! itr->second->positive() ) {
      return false;
    }
  }
  return true;
}

bool transaction_sum_t::negative() {
  for (transaction_sum_type::iterator itr=this->begin(); itr!=this->end(); ++itr) {
    if ( ! itr->second->negative() ) {
      return false;
    }
  }
  return true;
}

// this is not actually correct
// think about what we need from compare.
// the body does work, but this can return same value for a.compare(b), b.compare(a)
int transaction_sum_t::compare(transaction_sum_t other) {
  for (transaction_sum_type::iterator itr=this->begin(); itr!=this->end(); ++itr) {
    port_ref_t* key = itr->first;
    transaction_body_t* body = itr->second;
    if(other.contains(itr->first)) {
      transaction_body_t* other_body = other.get(itr->first);
      int compare_result = body->compare(other_body);
      if ( !(compare_result == 0) ) {
        return compare_result;
      }
    }
  }
  return true;
}

string transaction_sum_t::to_string()
{
  string transaction_sum_string;

  for (transaction_sum_type::iterator itr=this->begin(); itr!=this->end(); ++itr) {
    transaction_sum_string += itr->first->to_string();
    transaction_sum_string += " ";
    transaction_sum_string += itr->second->to_string();
    transaction_sum_string += "\n";
  }
  transaction_sum_string += "\n";

  return transaction_sum_string;
}

string transaction_sum_t::to_vertex_string()
{
  string transaction_sum_vertex_string;

  for (transaction_sum_type::iterator itr=this->begin(); itr!=this->end(); ++itr) {
    transaction_sum_vertex_string += "v";
    // going to use absolute value here.
    // not sure it is right thing to do.
    transaction_sum_vertex_string += std::to_string( (long long int) abs(itr->second->weight()) );
  }

  return transaction_sum_vertex_string;
}

void transaction_sum_t::print()
{
  printf("%s", this->to_string().c_str());
}


// transaction_slice_list_t

/*
class transaction_slice_list_t : public vector_t<transaction_slice_t*> {
  public:
  void add(transaction_slice_t* slice);
  void match(transaction_slice_t* slice);
};
*/

void transaction_slice_list_t::add(transaction_slice_t* slice)
{
  this->push_back(slice);
}

pair<bool, transaction_slice_t*> transaction_slice_list_t::match(transaction_slice_t* slice)
{
  assert( this->weight() >= slice->weight() );

  pair<bool, transaction_slice_t*> result(false, (transaction_slice_t*) NULL);

  uint32_t i;
  for(i=0; i<this->size(); i++) {
    transaction_slice_t* this_slice = this->at(i);
    if (this_slice->weight() > 0) {

      result = this_slice->match(slice);

      if (result.first) {
        return result;
      }
      else {
        slice = result.second;
      }
    }
  }

  return result;

  // fprintf(stderr, "Could not find a match\n");
  // assert(false);
  
}

uint32_t transaction_slice_list_t::weight()
{
  uint32_t sum = 0;
  uint32_t i;
  for(i=0; i<this->size(); i++) {
    transaction_slice_t* slice = this->at(i);
    sum += slice->weight();
  }
  return sum;
}

string transaction_slice_list_t::to_string()
{
  string transaction_slice_string;

  int i;  
  for(i=0; i<this->size(); i++) {
    transaction_slice_t* slice = this->at(i);
    transaction_slice_string += slice->to_string();
    transaction_slice_string += "\n";
  }

  return transaction_slice_string;
}

void transaction_slice_list_t::print()
{
  printf("%s", this->to_string().c_str());
}

// transaction slice sum
/*
class transaction_slice_sum_t : public map_comp_t<port_ref_t*, transaction_slice_list_t*, port_ref_comparator_t> {
  public:
  transaction_slice_sum_t();  
  transaction_slice_sum_t(transaction_slice_sum_t* sum);

  void add(edge_t* edge1, edge_t* edge2, connections_t* connections);
  void add(edge_t* edge, connections_t* connections);

  void add(transaction_t* transaction, connections_t* connections);
  void match(transaction_t* transaction, connections_t* connections);

  bool needs(edge_t* edge, connections_t* connections);
  bool ready(edge_t* edge, connections_t* connections);

  // may do something different
  void set_matches();

  std::string to_string();
  void print();
};
*/

transaction_slice_sum_t::transaction_slice_sum_t()
{
}

bool transaction_slice_sum_t::add(edge_t* edge, connections_t* connections)
{
  int i;
  for (i=0; i<edge->num_sends(); i++) {
    transaction_t* send = edge->sends.at(i);
    this->add(send, connections);
  }
  for (i=0; i<edge->num_receives(); i++) {
    transaction_t* rec = edge->receives.at(i);
    bool ret = this->match(rec, connections);
    if (!ret) {
      return false;
    }
  }

  this->add_dep(edge);

  return true;
}

bool transaction_slice_sum_t::add(vector_t<transaction_t*> transactions, connections_t* connections)
{
  int i;
  for (i=0; i<transactions.size(); i++) {
    transaction_t* next = transactions.at(i);
    bool ret = this->add(next, connections);
    if (!ret) {
      return false;
    }
  }
  return true;
}

bool transaction_slice_sum_t::add(transaction_t* transaction, connections_t* connections)
{
  uint32_t i;

  if ( this->contains(transaction) ) {
    transaction_slice_list_t* new_slice_list = this->get(transaction);
    new_slice_list->add(transaction->slice());
    // dont need to set back which is nice.
    // wow this whole time we never needed set, just needed to use pointer.
  }
  else {
    transaction_slice_list_t* new_slice_list = new transaction_slice_list_t();
    new_slice_list->add(transaction->slice());
    this->put(transaction, new_slice_list); 
  }

  // put the id in our set.
  // we may have to look if there are deps in here I think.

/*
  string name;
  if (transaction->heirarchy.first) {
    for(i=0; i<transaction->heirarchy.second.size(); i++) {
      name += transaction->heirarchy.second.at(i) + ".";
    }
  }
  name += transaction->get_name();

  for(i=0; i<transaction->body.params.size(); i++) {
    parameter_t* param = transaction->body.params.at(i);
    if (param->type() == PARAM_ID) {
      parameter_id_t* param_id = (parameter_id_t*) param;
      this->id_set.put(new pair<string, uint32_t>(name, param_id->id));
    }
  }
*/

  return true;
}

/*
we shud make adding an edge part of the set class or w.e.
since we use it for paths and this.
*/

void transaction_slice_sum_t::add_dep(edge_t* edge)
{
  uint32_t i, j;

  for(i=0; i<edge->sends.size(); i++) {
    transaction_t* send = edge->sends.at(i);

    string send_name;
    if (send->heirarchy.first) {
      for(j=0; j<send->heirarchy.second.size(); j++) {
        send_name += send->heirarchy.second.at(j) + ".";
      }
    }
    send_name += send->get_name();

    for(j=0; j<send->body.params.size(); j++) {
      parameter_t* param = send->body.params.at(j);
      if (param->type() == PARAM_ID) {
        parameter_id_t* param_id = (parameter_id_t*) param;

        pair<string, uint32_t>* new_id = new pair<string, uint32_t>(send_name, param_id->id);
        // assert(!id_set.contains(new_id));

        // BUG should not do this.
        if ( !id_set.contains(new_id) ) {
          this->id_set.put(new_id);
        }
      }
    }
  }

  for(i=0; i<edge->receives.size(); i++) {
    transaction_t* rec = edge->receives.at(i);

    string rec_name;
    if (rec->heirarchy.first) {
      for(j=0; j<rec->heirarchy.second.size(); j++) {
        rec_name += rec->heirarchy.second.at(j) + ".";
      }
    }
    rec_name += rec->get_name();

    for(j=0; j<rec->body.params.size(); j++) {
      parameter_t* param = rec->body.params.at(j);
      if (param->type() == PARAM_ID) {
        parameter_id_t* param_id = (parameter_id_t*) param;

        pair<string, uint32_t>* new_id = new pair<string, uint32_t>(rec_name, param_id->id);
        // assert(!id_set.contains(new_id));

        // BUG should not do this.
        if ( !id_set.contains(new_id) ) {
          this->id_set.put(new_id);
        }
      }
    }
  }
}

void transaction_slice_sum_t::add_dep(transaction_t* transaction)
{
}

bool transaction_slice_sum_t::match(vector_t<transaction_t*> transactions, connections_t* connections)
{
  int i;
  for (i=0; i<transactions.size(); i++) {
    transaction_t* next = transactions.at(i);
    bool ret = this->match(next, connections);
    if (!ret) {
      return false;
    }
  }
  return true;
}

/*
different ways we can loop:
1. get connections with transaction, loop through these asking map if we contain them
2. loop through all transaction in our map and ask connections if {transaction, t e T}

it dosnt really matter.
i kinda like looping though our transaction rather than connections

nah lets loop through connects

okay another issue:
I think we want to make match return a pair<bool, slice> pair to know if we should 
die or not.
*/

bool transaction_slice_sum_t::match(transaction_t* transaction, connections_t* connections)
{
  // we had this before and it would not work:
  // vector_t<port_ref_t*> ret; 

  // changing this now to heirarchy, BUT weird part would remain the same ... wud prefer to return a list of pointers
  vector_t<var_ref_t> connects = connections->connections(transaction);
  
  int i;
  for(i=0; i<connects.size(); i++) {
    port_ref_t ref = connects.at(i);
    // ref.print();
    // printf("\n");
    if ( this->contains(&ref) ) {
      transaction_slice_list_t* new_slice_list = this->get(&ref);
      pair<bool, transaction_slice_t*> result = new_slice_list->match(transaction->slice());
      assert(result.first);
      return true;
    }
  }

  // fprintf(stderr, "Could not find a match!\n");
  // assert(false);

  return false;
}

/*
we shud make adding an edge part of the set class or w.e.
since we use it for paths and this.
*/
bool transaction_slice_sum_t::dep(edge_t* edge)
{
  uint32_t i, j;

  set_comp_t<pair<string, uint32_t>*, dep_comparator_t> dep_set;

  for(i=0; i<edge->sends.size(); i++) {
    transaction_t* send = edge->sends.at(i);

    for(j=0; j<send->body.params.size(); j++) {
      parameter_t* param = send->body.params.at(j);

      if (param->type() == PARAM_DEP) {
        parameter_dep_t* param_dep = (parameter_dep_t*) param;

        pair<string, uint32_t>* new_key = new pair<string, uint32_t>(param_dep->dep_name, param_dep->dep_id);
        if ( !dep_set.contains(new_key) ) {
          dep_set.put(new_key);
        }
      }
    }
  }

  for(i=0; i<edge->receives.size(); i++) {
    transaction_t* rec = edge->receives.at(i);

    for(j=0; j<rec->body.params.size(); j++) {
      parameter_t* param = rec->body.params.at(j);

      if (param->type() == PARAM_DEP) {
        parameter_dep_t* param_dep = (parameter_dep_t*) param;

        pair<string, uint32_t>* new_key = new pair<string, uint32_t>(param_dep->dep_name, param_dep->dep_id);
        if ( !dep_set.contains(new_key) ) {
          dep_set.put(new_key);
        }
      }
    }
  }

  return ( this->id_set.contains(dep_set) || (dep_set.size()==0) );
}

bool transaction_slice_sum_t::needs(edge_t* sender, edge_t* receiver, connections_t* connections)
{
  uint32_t i, j;

  for(i=0; i<receiver->num_receives(); i++) {
    transaction_t* rec = receiver->receives.at(i);
    
    for(j=0; j<sender->num_sends(); j++) {
      transaction_t* send = sender->sends.at(j);

      if (connections->contains((port_ref_t*)send, (port_ref_t*)rec)) {
        if (this->contains(send)) {
          transaction_slice_list_t* slice_list = this->get(send);
          bool contains = slice_list->weight() >= rec->weight();
          if (!contains) {
            return true;
          }
        }
        else {
          return true;
        }
      }
    }
  }
  return false;
}

/*
bool transaction_slice_sum_t::needs(edge_t* edge, connections_t* connections)
{
  uint32_t i, j;

  for(i=0; i<edge->num_receives(); i++) {
    transaction_t* rec = edge->receives.at(i);
    vector_t<var_ref_t> connects = connections->connections(rec);
    
    for(j=0; j<connects.size(); j++) {
      port_ref_t ref = connects.at(j);
      if ( this->contains(&ref) ) {
        transaction_slice_list_t* slice_list = this->get(&ref);
        bool contains = slice_list->weight() >= rec->weight();
        if (!contains) {
          return true;
        }
      }
      else {
        return true;
      }
    }
  }
  return false;
}
*/

bool transaction_slice_sum_t::ready(edge_t* edge, connections_t* connections)
{
  uint32_t i, j;

  for(i=0; i<edge->num_receives(); i++) {
    transaction_t* rec = edge->receives.at(i);
    vector_t<var_ref_t> connects = connections->connections(rec);
    
    for(j=0; j<connects.size(); j++) {
      port_ref_t ref = connects.at(j);
      if ( this->contains(&ref) ) {
        transaction_slice_list_t* slice_list = this->get(&ref);
        bool contains = slice_list->weight() >= rec->weight();
        if (contains) {
          return true;
        }
      }
    }
  }
  return false;
}

string transaction_slice_sum_t::to_string()
{
  string transaction_slice_sum_string;

  for (transaction_slice_sum_type::iterator itr=this->begin(); itr!=this->end(); ++itr) {

    transaction_slice_sum_string += itr->first->to_string();
    transaction_slice_sum_string += " ";
    transaction_slice_sum_string += itr->second->to_string();
    transaction_slice_sum_string += "\n";

  }
  transaction_slice_sum_string += "\n";

  return transaction_slice_sum_string;
}

void transaction_slice_sum_t::print()
{
  printf("%s", this->to_string().c_str());
}

/*
class parameter_sum_t : public map_comp_t<port_ref_t*, parameter_t*, port_ref_comparator_t> {
  void add(transaction_t*, connections_t*);
  void subtract(transaction_t*, connections_t*);
  bool equals();
  bool contains();
};
*/

parameter_sum_t::~parameter_sum_t()
{
  for (parameter_sum_type::iterator itr=this->begin(); itr!=this->end(); ++itr) {
    delete itr->second;
  }
}

bool parameter_sum_t::add(edge_t* e, connections_t* connections)
{
  uint32_t i;
  for(i=0; i<e->sends.size(); i++) {
    bool ret = this->add(e->sends.at(i), connections);
    if (!ret) return false;
  }
  for(i=0; i<e->receives.size(); i++) {
    bool ret = this->add(e->receives.at(i), connections);
    if (!ret) return false;
  }
  return true;
}

bool parameter_sum_t::subtract(edge_t* e, connections_t* connections)
{
  uint32_t i;
  for(i=0; i<e->sends.size(); i++) {
    bool ret = this->subtract(e->sends.at(i), connections);
    if (!ret) return false;
  }
  for(i=0; i<e->receives.size(); i++) {
    bool ret = this->subtract(e->receives.at(i), connections);
    if (!ret) return false;
  }
  return true;
}

bool parameter_sum_t::add(transaction_t* t, connections_t* connections)
{
  uint32_t i;

  parameter_t* new_param = NULL;
  for(i=0; i<t->body.params.size(); i++) {
    if (t->body.params.at(i)->type() == PARAM_RANGE) {
      new_param = t->body.params.at(i);
    }
    else if (t->body.params.at(i)->type() == PARAM_OFFSET) {
      new_param = t->body.params.at(i);
    }
  }
  assert(new_param);
  new_param = new_param->copy();

  if ( this->contains( (port_ref_t*) t ) ) {
    parameter_t* current_param = this->get( t );

    assert(current_param->type() == new_param->type());

    if (current_param->type() == PARAM_RANGE) {
      ((parameter_range_t*)current_param)->range += ((parameter_range_t*)new_param)->range;
    }
    else if (current_param->type() == PARAM_OFFSET) {
      ((parameter_offset_t*)current_param)->offset += ((parameter_offset_t*)new_param)->offset;
    }
    
    delete new_param;
  }
  else {
    this->put( t, new_param );
  }
  return true;
}

bool parameter_sum_t::subtract(transaction_t* t, connections_t* connections)
{
  uint32_t i;

  parameter_t* new_param = NULL;
  for(i=0; i<t->body.params.size(); i++) {
    if (t->body.params.at(i)->type() == PARAM_RANGE) {
      new_param = t->body.params.at(i);
    }
    else if (t->body.params.at(i)->type() == PARAM_OFFSET) {
      new_param = t->body.params.at(i);
    }
  }
  assert(new_param);
  new_param = new_param->copy();

  vector_t<var_ref_t> connects = connections->connections(t);
  
  for(i=0; i<connects.size(); i++) {
    port_ref_t ref = connects.at(i);

    if ( this->contains(&ref) ) {
      parameter_t* current_param = this->get( &ref );

      assert(current_param->type() == new_param->type());

      if (current_param->type() == PARAM_RANGE) {
        ((parameter_range_t*)current_param)->range -= ((parameter_range_t*)new_param)->range;
      }
      else if (current_param->type() == PARAM_OFFSET) {
        ((parameter_offset_t*)current_param)->offset -= ((parameter_offset_t*)new_param)->offset;
      }

      delete new_param;

      return true;
    }
  }

  // printf("connects size: %u\n", connects.size());

  return false;
}

bool parameter_sum_t::zero()
{
  for (parameter_sum_type::iterator itr=this->begin(); itr!=this->end(); ++itr) {
    parameter_t* param = NULL;
    if (itr->second->type() == PARAM_RANGE) {
      param = itr->second;
      if (((parameter_range_t*)param)->range != 0) {
        return false;
      }
    }
    else if (itr->second->type() == PARAM_OFFSET) {
      param = itr->second;
      if (((parameter_offset_t*)param)->offset != 0) {
        return false;
      }
    }
  }
  return true;
}

bool parameter_sum_t::pos()
{
  for (parameter_sum_type::iterator itr=this->begin(); itr!=this->end(); ++itr) {
    parameter_t* param = NULL;
    if (itr->second->type() == PARAM_RANGE) {
      param = itr->second;
      if (((parameter_range_t*)param)->range < 0) {
        return false;
      }
    }
    else if (itr->second->type() == PARAM_OFFSET) {
      param = itr->second;
      if (((parameter_offset_t*)param)->offset < 0) {
        return false;
      }
    }
  }
  return true;
}

string parameter_sum_t::to_string()
{
  string param_sum_string;

  for (parameter_sum_type::iterator itr=this->begin(); itr!=this->end(); ++itr) {
    param_sum_string += itr->first->to_string();
    param_sum_string += " ";
    param_sum_string += itr->second->to_string();
    param_sum_string += "\n";
  }
  param_sum_string += "\n";

  return param_sum_string;
}

void parameter_sum_t::print()
{
  printf("%s", this->to_string().c_str());
}






















