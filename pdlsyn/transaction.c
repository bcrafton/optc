
#include "transaction.h"

using namespace std;
using namespace boost;

// transaction body

transaction_body_t::transaction_body_t() 
{
  fprintf(stderr, "Should never actually use this\n");
  assert(false);
}

transaction_body_t::transaction_body_t(transaction_body_t* body) 
{
  assert( body->offset.first ^ body->range.first );

  this->range = body->range;
  this->offset = body->offset;

  // create the param list
  uint32_t i;
  for(i=0; i<body->params.size(); i++) {
    this->params.push_back( body->params.at(i)->copy() );
  }  

  assert( this->offset.first ^ this->range.first );
}

transaction_body_t::transaction_body_t(vector_t<parameter_t*> params) 
{
  assert(params.size() > 0);

  this->params = params;

  int i;
  for(i=0; i<params.size(); i++) {
    // current bind
    parameter_t* param = params.at(i);

    if (param->type() == PARAM_RANGE) {
      parameter_range_t* param_range = (parameter_range_t*) param;
      assert(!param_range->range_var.first);
      this->range = pair<bool, int>(true, param_range->range);
    }
    else if (param->type() == PARAM_OFFSET) {
      parameter_offset_t* param_offset = (parameter_offset_t*) param;
      assert(!param_offset->offset_var.first);
      this->offset = pair<bool, int>(true, param_offset->offset);
    }
    else if (param->type() == PARAM_ID) {
    }
    else if (param->type() == PARAM_DEP) {
    }
    else {
      fprintf(stderr, "Param not mapped %u\n", param->type());
      assert(false);
    }
  }

  assert( this->offset.first ^ this->range.first );
}

bool transaction_body_t::zero()
{
  if (offset.first) {
    return offset.second == 0;
  }
  if (range.first) {
    return range.second == 0;
  }
  fprintf(stderr, "Invalid transaction body\n");
  assert(offset.first ^ range.first);
}

bool transaction_body_t::positive()
{
  if (offset.first) {
    return offset.second > 0;
  }
  if (range.first) {
    return range.second > 0;
  }
  fprintf(stderr, "Invalid transaction body\n");
  assert(offset.first ^ range.first);
}

bool transaction_body_t::negative()
{
  if (offset.first) {
    return offset.second < 0;
  }
  if (range.first) {
    return range.second < 0;
  }
  fprintf(stderr, "Invalid transaction body\n");
  assert(offset.first ^ range.first);
}

int transaction_body_t::weight()
{
  if (offset.first) {
    return offset.second;
  }
  if (range.first) {
    return range.second;
  }
  fprintf(stderr, "Invalid transaction body\n");
  assert(offset.first ^ range.first);
}

string transaction_body_t::parameter()
{
  if (offset.first) {
    return "offset";
  }
  else if (range.first) {
    return "range";
  }
  fprintf(stderr, "Invalid transaction body\n");
  assert(offset.first ^ range.first);
}

transaction_body_t* transaction_body_t::copy()
{
  vector_t<parameter_t*> new_params;

  uint32_t i;
  for(i=0; i<this->params.size(); i++) {
    new_params.push_back( this->params.at(i)->copy() );
  }  

  return new transaction_body_t(new_params);
}

void transaction_body_t::set(vector_t<argument_t*> args)
{
  uint32_t i, j;
  for(i=0; i<this->params.size(); i++) {
    for(j=0; j<args.size(); j++) {
      parameter_t* param = this->params.at(i);
      argument_t* arg = args.at(j);
      param->set(arg);
    }
  }
}

int transaction_body_t::compare(transaction_body_t* other) {
  assert( other->offset.first ^ other->range.first );
  assert( this->offset.first ^ this->range.first );

  if (other->offset.first) {
    assert(this->offset.first);
    return (this->offset.second - other->offset.second);
  }

  else if (other->range.first) {
    assert(this->range.first);
    return (this->range.second - other->range.second);
  }

  else {
    fprintf(stderr, "Should never get here : %d %d %d\n", 
      other->offset.first, 
      other->range.first, 
      (other->offset.first ^ other->range.first)
    );
    assert(false);
  }
}

void transaction_body_t::add(transaction_body_t* body)
{
  assert( body->offset.first ^ body->range.first );
  assert( this->offset.first ^ this->range.first );

  if (body->offset.first) {
    assert(this->offset.first);
    this->offset.second += body->offset.second;
  }

  else if (body->range.first) {
    assert(this->range.first);
    this->range.second += body->range.second;
  }

  else {
    fprintf(stderr, "Should never get here : %d %d %d\n", 
      body->offset.first, 
      body->range.first, 
      (body->offset.first ^ body->range.first)
    );
    assert(false);
  }
}

void transaction_body_t::subtract(transaction_body_t* body)
{
  // printf("subtract called\n");

  assert( body->offset.first ^ body->range.first );
  assert( this->offset.first ^ this->range.first );

  if (body->offset.first) {
    assert(this->offset.first);
    this->offset.second -= body->offset.second;
  }

  else if (body->range.first) {
    if (!this->range.first) {
      fprintf(stderr, "Mismatch : %d %d | %d %d\n", 
        body->range.first, 
        body->offset.first, 
        this->range.first, 
        this->offset.first
      ); 
      assert(this->range.first);
    }
    this->range.second -= body->range.second;
  }

  else {
    fprintf(stderr, "Should never get here : %d %d %d\n", 
      body->offset.first, 
      body->range.first, 
      (body->offset.first ^ body->range.first)
    );
    assert(false);
  }
}

string transaction_body_t::to_string()
{
  assert( this->offset.first ^ this->range.first );
  string transaction_body_string;

  if (offset.first) {
    transaction_body_string += "offset = ";
    transaction_body_string += std::to_string( (long long int) this->offset.second );
    transaction_body_string += " ";
  }

  if (range.first) {
    transaction_body_string += "range = ";
    transaction_body_string += std::to_string( (long long int) this->range.second );
    transaction_body_string += " ";
  }

  uint32_t i;
  for(i=0; i<this->params.size(); i++) {
    parameter_t* param = this->params.at(i);
    transaction_body_string += param->to_string();
    transaction_body_string += " ";
  }

  return transaction_body_string;
}

void transaction_body_t::print()
{
  printf("%s", this->to_string().c_str());
}

// transaction
// we are calling var_ref_t constructor because it sets all stuff to false for us.

/*
This stuff gets weird because we have the heirarchy + name and then just the name
So how do we pass stuff???

We are gonna change :
set_name
set_heirarchy
*/

transaction_t::transaction_t(string name, uint32_t width) : cut_ref_t(), body()
{
  this->set_name(name);
  this->set_width(width);
}

transaction_t::transaction_t(string name, uint32_t width, vector_t<parameter_t*> params) : cut_ref_t(), body(params)
{
  this->set_name(name);
  this->set_width(width);

}

transaction_t::transaction_t(string name, uint32_t width, port_direction_t dir, vector_t<parameter_t*> params) : cut_ref_t(), body(params)
{
  this->set_name(name);
  this->set_width(width);
  this->set_dir(dir);
}

/*
would like to change body and all the copy stuff surrounding this
it just gets messy with copy constructors
before it never actually copied, because we didnt pass the pointer.

could implement the copy functions we put in here.
*/

transaction_t::transaction_t(transaction_t* old_transaction) : cut_ref_t(), body( &(old_transaction->body) )
{
  this->set_name(old_transaction->get_name());
  this->set_dir(old_transaction->get_dir());
  this->set_heirarchy(old_transaction->get_heirarchy());
  this->set_width(old_transaction->get_width());

  if (old_transaction->has_cut) {
    this->set_cut(old_transaction->msb, old_transaction->lsb);
  }

  // this is probably really dumb to do ...
  // probably could be source of bugs later on.
  // but for now its fine.

  if (old_transaction->protocol_name.first) {
    this->set_protocol_name(old_transaction->get_protocol_name());
  }

  // this->body.offset = old_transaction->body.offset;
  // this->body.range = old_transaction->body.range;
}

transaction_t::transaction_t(string name, port_direction_t dir, vector_t<parameter_t*> params, uint32_t msb, uint32_t lsb) : cut_ref_t(), body(params)
{
  this->set_name(name);

/*
  uint32_t i;
  for(i=0; i<this->heirarchy.second.size(); i++) {
    cout << this->heirarchy.second.at(i) << " ";
  }
  cout << this->name.second;
  cout << endl;
*/

  this->set_width(msb-lsb+1);
  this->set_dir(dir);
  this->set_cut(msb, lsb);
}

transaction_t::~transaction_t()
{
  uint32_t i;
  for(i=0; i<this->body.params.size(); i++) {
    delete this->body.params.at(i);
  }
}

int transaction_t::compare(transaction_t* transaction)
{
  int comp = cut_ref_t::compare(transaction);
  return comp;
}

transaction_slice_t* transaction_t::slice()
{
  return new transaction_slice_t(this);
}

int transaction_t::weight()
{
  return this->body.weight();
}

float transaction_t::value()
{
  float value_sum=0;

  uint32_t i;
  for(i=0; i<this->body.params.size(); i++) {
    if(this->body.params.at(i)->type() == PARAM_RANGE) {
      value_sum = this->body.params.at(i)->value();
    }
    else if(this->body.params.at(i)->type() == PARAM_OFFSET) {
      value_sum = this->body.params.at(i)->value();
    }
  }

  return value_sum;
}

vector_t<port_ref_t*> transaction_t::source()
{
  assert(this->get_dir() == PORT_DIR_IN);
  assert(this->matches.size() > 0);

  vector_t<port_ref_t*> sources;
  set_t<string> sources_set;

  uint32_t i;
  for(i=0; i<this->matches.size(); i++) {
    transaction_t* n = this->matches.at(i)->transaction;

    string name = n->get_name();

    if (!sources_set.contains(name)) {
      sources.push_back(n);
      sources_set.put(name);
    }
  }

  return sources;
}

vector_t<port_ref_t*> transaction_t::dest()
{
  assert(this->get_dir() == PORT_DIR_OUT);
  assert(this->matches.size() > 0);
 
  vector_t<port_ref_t*> dests;
  set_t<string> dests_set;

  uint32_t i;
  for(i=0; i<this->matches.size(); i++) {
    transaction_t* n = this->matches.at(i)->transaction;

    string name = n->get_name();

    if (!dests_set.contains(name)) {
      dests.push_back(n);
      dests_set.put(name);
    }
  }

  return dests;
}

string transaction_t::parameter()
{
  return this->body.parameter();
}

pair<bool, parameter_id_t*> transaction_t::id()
{
  uint32_t i;

  for(i=0; i<this->body.params.size(); i++) {
    parameter_t* param = this->body.params.at(i);

    if (this->body.params.at(i)->type() == PARAM_ID) {
      return pair<bool, parameter_id_t*>(true, (parameter_id_t*) param);
    }
  }

  return pair<bool, parameter_id_t*>(false, (parameter_id_t*) NULL);
}

transaction_t* transaction_t::copy()
{
  fprintf(stderr, "Not implemented yet\n");
  assert(false);
}

void transaction_t::set(vector_t<argument_t*> args)
{
  this->body.set(args);
}

string transaction_t::to_string()
{
  string transaction_string;

  transaction_string += this->get_name();
  transaction_string += " ";
  
  transaction_string += this->body.to_string();

  int i;
  for(i=0; i<this->matches.size(); i++) {
    transaction_slice_t* slice = this->matches.at(i);
    transaction_string += slice->to_string();
    transaction_string += " ";
  }

  return transaction_string;
}

void transaction_t::print()
{
  printf("%s", this->to_string().c_str());
}


// transaction slice
/*
class transaction_slice_t {
  public:
  transaction_t* transaction;
  uint32_t lsb;
  uint32_t msb;

  vector_t<transaction_slice_t*> matches;

  transaction_slice_t(transaction_t* transaction);
  transaction_slice_t(transaction_t* transaction, uint32_t lsb, uint32_t msb);

  void match(transaction_slice_t* match);
  transaction_slice_t* slice(uint32_t num_bits);

  int weight();

  std::string to_string();
  void print();
};
*/

transaction_slice_t::transaction_slice_t(transaction_t* transaction)
{
  this->transaction = transaction;

  transaction_body_t* body = &transaction->body;

  assert( body->offset.first ^ body->range.first );

  if (body->offset.first) {
    this->lsb = 0;
    this->msb = body->offset.second - 1;
  }

  else if (body->range.first) {
    this->lsb = 0;
    this->msb = body->range.second - 1;
  }

  else {
    fprintf(stderr, "Should never get here : %d %d %d\n", 
      body->offset.first, 
      body->range.first, 
      (body->offset.first ^ body->range.first)
    );
    assert(false);
  }

  // printf("Creating new transaction slice: %u %u\n", this->lsb, this->msb);

}

transaction_slice_t::transaction_slice_t(transaction_t* transaction, uint32_t lsb, uint32_t msb)
{
  this->transaction = transaction;
  this->lsb = lsb;
  this->msb = msb;

  // printf("Creating new transaction slice: %u %u\n", this->lsb, this->msb);
}

/*
so here we are taking a slice of this and match in the else block
and i am not sure if this is the right thing to do 
but realize that we only need to track the sender size and make that equal to 0.
dont worry about receiver side, only need worry about 1.
*/
pair<bool, transaction_slice_t*> transaction_slice_t::match(transaction_slice_t* match)
{
  pair<bool, transaction_slice_t*> result;

  if (this->weight() >= match->weight()) {
    // printf("%u %u\n", this->weight(), match->weight());
    uint32_t slice_size = match->weight();

    transaction_slice_t* this_slice = this->slice(slice_size);

    this->transaction->matches.push_back(match);
    this->matches.push_back(match);

    match->matches.push_back(this_slice);
    match->transaction->matches.push_back(this_slice);

    result = pair<bool, transaction_slice_t*>(true, (transaction_slice_t*) NULL);
  }
  else {
    // printf("%u %u\n", this->weight(), match->weight());
    uint32_t slice_size = this->weight();

    transaction_slice_t* this_slice = this->slice(slice_size);
    transaction_slice_t* match_slice = match->slice(slice_size);

    this->transaction->matches.push_back(match_slice);
    this->matches.push_back(match_slice);

    match->matches.push_back(this_slice);
    match->transaction->matches.push_back(this_slice);

    result = pair<bool, transaction_slice_t*>(false, match);
  }

  return result;

}

transaction_slice_t* transaction_slice_t::slice(uint32_t num_bits)
{
  // this is just the weight of the matches we have 
  // we are using this to calculate lsb
  // we cud probably just update the lsb here...
  // this is the only place that we would change it right??
  // this (slice()) or in match().
  // yeah i def think we can set lsb/msb here.
  // and then weight just becomes msb - lsb

  uint32_t old_lsb = this->lsb;
  uint32_t new_lsb = this->lsb + num_bits;

  transaction_slice_t* new_slice = new transaction_slice_t(transaction, old_lsb, new_lsb - 1);

  this->lsb = new_lsb;

  // printf("Setting lsb to: %u\n", this->lsb);

  return new_slice;
}

/*
int transaction_slice_t::weight()
{
  uint32_t weight = this->transaction->weight();

  uint32_t matches_weight_sum = 0;
  uint32_t i;
  for(i=0; i<this->matches.size(); i++) {
    matches_weight_sum += this->matches.at(i)->weight();
  }

  if( !(weight >= matches_weight_sum) ) {
    fprintf(stderr, "Weight less than matches weight sum: %u %u\n", weight, matches_weight_sum);
    assert(weight >= matches_weight_sum);
  }

  return (weight - matches_weight_sum);
}
*/

int transaction_slice_t::weight()
{
  uint32_t matches_weight_sum = 0;
  uint32_t i;
  for(i=0; i<this->matches.size(); i++) {
    matches_weight_sum += this->matches.at(i)->weight();
    // printf("%u %u %u\n", this->transaction->weight(), this->matches.at(i)->weight(), matches_weight_sum);
  }

  if( !(this->transaction->weight() >= matches_weight_sum) ) {
    fprintf(stderr, "Weight less than matches weight sum: %u %u\n", this->transaction->weight(), matches_weight_sum);
    assert(this->transaction->weight() >= matches_weight_sum);
  }

  return (this->msb - this->lsb + 1);
}

string transaction_slice_t::to_string()
{
  string transaction_slice_string;

  transaction_slice_string += this->transaction->get_name();
  transaction_slice_string += "[";
  transaction_slice_string += std::to_string( (long long int) this->msb );
  transaction_slice_string += ":";
  transaction_slice_string += std::to_string( (long long int) this->lsb );
  transaction_slice_string += "]";

  return transaction_slice_string;
}

void transaction_slice_t::print()
{
  printf("%s", this->to_string().c_str());
}


