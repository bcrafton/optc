
#include "packet.h"

using namespace std;
using namespace boost;

/////////////////////////////////////
// packet_contents_t
/////////////////////////////////////
// idky these need to be defined if they are virtual

packet_contents_t::~packet_contents_t()
{
}

void packet_contents_t::print()
{
}

string packet_contents_t::to_string()
{
}

uint32_t packet_contents_t::size()
{
}

pair<field_mapping_t*, uint32_t>* packet_contents_t::at(uint32_t bit)
{
}

pair<packet_edge_t*, vector_t<packet_edge_t*>*>* packet_contents_t::paths()
{
  fprintf(stderr, "Should never get here\n");
  assert(false);
}

name_space_t packet_contents_t::name_space()
{
  fprintf(stderr, "Should never get here\n");
  assert(false);
}

packet_contents_t* packet_contents_t::copy()
{
  fprintf(stderr, "Should never get here\n");
  assert(false);
}

void packet_contents_t::prepend_name(string name)
{
  fprintf(stderr, "Should never get here\n");
  assert(false);
}

/////////////////////////////////////
// field mapping
/////////////////////////////////////

field_mapping_t::~field_mapping_t()
{
}

field_mapping_t::field_mapping_t(packet_prim_type_t type, string field, uint32_t bit_size)
{
  this->type = type;
  this->field = field;
  this->bit_size = bit_size;
}

field_mapping_t::field_mapping_t(packet_prim_type_t type, string field, uint32_t bit_size, vector_t<parameter_t*> params)
{
  this->type = type;
  this->field = field;
  this->bit_size = bit_size;
  this->params = params;
}

void field_mapping_t::print() 
{
  printf("%s", this->to_string().c_str() );
}

string field_mapping_t::to_string() 
{
  string field_mapping_string;

  field_mapping_string += this->field;
  field_mapping_string += " " + std::to_string( (long long unsigned int) this->bit_size );
  field_mapping_string += " ";

  uint32_t i;
  for(i=0; i<this->params.size(); i++) {
    field_mapping_string += params.at(i)->to_string();
    field_mapping_string += " ";
  }

  return field_mapping_string;
}

uint32_t field_mapping_t::size()
{
  return this->bit_size;
}

pair<field_mapping_t*, uint32_t>* field_mapping_t::at(uint32_t bit)
{
  assert(bit <= this->size());
  return new pair<field_mapping_t*, uint32_t>(this, bit);
}

pair<packet_edge_t*, vector_t<packet_edge_t*>*>* field_mapping_t::paths()
{
  pair<packet_edge_t*, vector_t<packet_edge_t*>*>* ret = new pair<packet_edge_t*, vector_t<packet_edge_t*>*>();

  ret->first = new packet_edge_t(*this);

  ret->second = new vector_t<packet_edge_t*>();
  ret->second->push_back(ret->first);

  return ret;
}

name_space_t field_mapping_t::name_space()
{
  name_space_t space;

  // printf("%s\n", this->field.c_str());

  if (this->type == PACKET_PRIM_CONTROL) {
    pair<uint32_t, uint32_t> cut(0, this->bit_size-1);
    space.put(this->field, cut);
  }
  else {
    // do nothing because it is data.
  }

  return space;
}

packet_contents_t* field_mapping_t::copy()
{
  vector_t<parameter_t*> new_params;

  uint32_t i;
  for(i=0; i<params.size(); i++) {
    new_params.push_back( params.at(i)->copy() );
  }

  field_mapping_t* cpy = new field_mapping_t(type, field, bit_size, new_params);

  return (packet_contents_t*) cpy;
}

void field_mapping_t::prepend_name(string name)
{
  this->field = name + "." + this->field;
}

/////////////////////////////////////
// packet_inst 
/////////////////////////////////////

packet_inst_t::~packet_inst_t()
{
}

packet_inst_t::packet_inst_t(string name, packet_t* packet)
{
  this->name = name;
  this->packet = packet;
}

packet_inst_t::packet_inst_t(string name, packet_t* packet, vector_t<argument_t*> args)
{
  this->name = name;
  this->packet = packet;
  this->args = args;
}

void packet_inst_t::print() 
{
  printf("%s", this->to_string().c_str() );
}

string packet_inst_t::to_string() 
{
  string packet_inst_string;

  packet_inst_string += this->name + ":";
  packet_inst_string += this->packet->to_string();

  return packet_inst_string;
}

uint32_t packet_inst_t::size()
{
  return this->packet->size();
}

pair<field_mapping_t*, uint32_t>* packet_inst_t::at(uint32_t bit)
{
  assert(bit <= this->size());
  return this->packet->at(bit);
}

pair<packet_edge_t*, vector_t<packet_edge_t*>*>* packet_inst_t::paths()
{
  this->packet = this->packet->copy();

  pair<packet_edge_t*, vector_t<packet_edge_t*>*>* paths = this->packet->paths();
  paths->first->prepend_name(this->name);

  assert(paths != NULL);
  assert(paths->first != NULL);
  assert(paths->second != NULL);  

  paths->first->set(this->args);

  return paths;
}

name_space_t packet_inst_t::name_space()
{
  name_space_t space;
  space = this->packet->name_space();

  name_space_t new_space = space.prepend(this->name);

  return new_space;
}

packet_contents_t* packet_inst_t::copy()
{
  packet_inst_t* cpy = new packet_inst_t(this->name, this->packet->copy(), this->args);
  return (packet_contents_t*) cpy;
}

void packet_inst_t::prepend_name(string name)
{
  this->name = name + "." + this->name;
}

/////////////////////////////////////
// subpacket
/////////////////////////////////////

subpacket_t::~subpacket_t()
{
}

subpacket_t::subpacket_t()
{
}

subpacket_t::subpacket_t(vector_t<packet_contents_t*> packet_contents, vector_t<packet_branch_t*> branches)
{
  this->packet_contents = packet_contents;
  this->branches = branches;
}

void subpacket_t::print() 
{
  printf("%s", this->to_string().c_str() );
}

string subpacket_t::to_string() 
{
  string subpacket_string;

  int i, j;
  for(i=0; i<this->packet_contents.size(); i++) {
    subpacket_string += this->packet_contents.at(i)->to_string();
    subpacket_string += "\n";
  }

  for(i=0; i<this->branches.size(); i++) {
    packet_branch_t* branch = this->branches.at(i);
    subpacket_t* branch_packet = branch->packet;

    subpacket_string += branch->to_string();
  }

  return subpacket_string;
}

uint32_t subpacket_t::size()
{
  uint32_t sum = 0;

  int i;
  for(i=0; i<this->packet_contents.size(); i++) {
    sum += this->packet_contents.at(i)->size();
  }

  return sum;
}

pair<field_mapping_t*, uint32_t>* subpacket_t::at(uint32_t bit)
{
  //assert(bit < this->size());

  uint32_t bit_index = 0;

  int i; 
  for(i=0; i<this->packet_contents.size(); i++) {

    packet_contents_t* content = this->packet_contents.at(i);

    uint32_t starti = bit_index;
    uint32_t endi = bit_index + content->size() - 1;

    // printf("start %u end %u bit %u\n", starti, endi, bit);

    if (starti <= bit && endi >= bit) {
      return content->at(bit - starti);
    }

    bit_index += content->size();
  }

  fprintf(stderr, "Should never get here: %u %u\n", bit, this->size());
  assert(0);
}

pair<packet_edge_t*, vector_t<packet_edge_t*>*>* subpacket_t::paths()
{
  int i, j, k;
  packet_edge_t* head = NULL;
  vector_t<packet_edge_t*>* tail = NULL;

  for(i=0; i<this->packet_contents.size(); i++) {
    // for each content
    packet_contents_t* content = this->packet_contents.at(i);

    // turn it into packet edges.
    pair<packet_edge_t*, vector_t<packet_edge_t*>*>* content_edges = content->paths();
    packet_edge_t* ret_head = content_edges->first;

    vector_t<packet_edge_t*>* ret_tail = content_edges->second;

    // set our head and tail
    if (head == NULL) {
      head = ret_head;
      tail = ret_tail;
    }
    else {
      // add all the heads to our tails
      for(j=0; j<tail->size(); j++) {
        packet_edge_t* current_tail = tail->at(j);  
        current_tail->branches.push_back( ret_head );
      }
      // set our new tail
      tail = ret_tail;
    }
  }

  // printf("branches size: %u\n", this->branches.size());

  // assert(this->branches.size() > 0);
  if ( this->branches.size() > 0 ) {

    // fprintf(stderr, "Debugging and should not get here yet\n");
    // assert(false);

    vector_t<packet_edge_t*>* new_tail = new vector_t<packet_edge_t*>();
    for(i=0; i<this->branches.size(); i++) {
      // for each branch
      packet_branch_t* branch = this->branches.at(i);

      // get the branches paths
      pair<packet_edge_t*, vector_t<packet_edge_t*>*>* ret = branch->paths();
      packet_edge_t* ret_head = ret->first;
      vector_t<packet_edge_t*>* ret_tail = ret->second;

      // printf("tail size: %u\n", tail->size());

      // append them to our list of branch paths
      for(j=0; j<tail->size(); j++) {
        packet_edge_t* current_tail = tail->at(j);  
        current_tail->branches.push_back( ret_head );
      }

      // new tail = all the other branch tails
      for(j=0; j<ret_tail->size(); j++) {
        new_tail->push_back( ret_tail->at(j) );
      }
    }
    tail = new_tail;

    // printf("new tail size: %u\n", new_tail->size());

    return new pair<packet_edge_t*, vector_t<packet_edge_t*>*>(head, new_tail);
  }
  else {
    return new pair<packet_edge_t*, vector_t<packet_edge_t*>*>(head, tail);
  }
}

name_space_t subpacket_t::name_space()
{
  name_space_t space;

  uint32_t i;
  for(i=0; i<this->packet_contents.size(); i++) {
    packet_contents_t* content = this->packet_contents.at(i);
    name_space_t ret = content->name_space();
    space.put(ret);
  }

  for(i=0; i<this->branches.size(); i++) {
    packet_branch_t* branch = this->branches.at(i);
    name_space_t ret = branch->name_space();
    space.put(ret);
  }

  return space;
}

subpacket_t* subpacket_t::copy()
{
  vector_t<packet_contents_t*> new_packet_contents;
  vector_t<packet_branch_t*> new_branches;

  uint32_t i;
  for(i=0; i<packet_contents.size(); i++) {
    new_packet_contents.push_back( packet_contents.at(i)->copy() );
  } 

  for(i=0; i<branches.size(); i++) {
    new_branches.push_back( branches.at(i)->copy() );
  } 

  subpacket_t* cpy = new subpacket_t(new_packet_contents, new_branches);
  return cpy;
}

/////////////////////////////////////
// packet
/////////////////////////////////////

packet_t::~packet_t()
{
}

packet_t::packet_t(string type, vector_t<packet_contents_t*> packet_contents, vector_t<packet_branch_t*> branches)
{
  this->type = type;
  this->packet_contents = packet_contents;
  this->branches = branches;
}

packet_t::packet_t(string type, vector_t<string> args, vector_t<packet_contents_t*> packet_contents, vector_t<packet_branch_t*> branches)
{
  this->type = type;
  this->args = args;
  this->packet_contents = packet_contents;
  this->branches = branches;
}

void packet_t::print() 
{
  printf("%s", this->to_string().c_str() );
}

string packet_t::to_string() 
{
  string packet_string;

  packet_string += this->type;
  packet_string += "\n";

  int i, j;
  for(i=0; i<this->packet_contents.size(); i++) {
    packet_string += this->packet_contents.at(i)->to_string();
    packet_string += "\n";
  }

  for(i=0; i<this->branches.size(); i++) {
    packet_branch_t* branch = this->branches.at(i);
    subpacket_t* branch_packet = branch->packet;

    packet_string += branch_packet->to_string();
  }

  return packet_string;
}

packet_t* packet_t::copy()
{
  vector_t<packet_contents_t*> new_packet_contents;
  vector_t<packet_branch_t*> new_branches;

  uint32_t i;
  for(i=0; i<packet_contents.size(); i++) {
    new_packet_contents.push_back( packet_contents.at(i)->copy() );
  } 

  for(i=0; i<branches.size(); i++) {
    new_branches.push_back( branches.at(i)->copy() );
  } 

  packet_t* cpy = new packet_t(this->type, this->args, new_packet_contents, new_branches);
  return cpy;
}

/////////////////////////////////////
// packet branch
/////////////////////////////////////

/*
class packet_branch_t {
  public:
  std::pair<bool, state_list_t*> condition; 
  subpacket_t* packet;

  packet_branch_t(subpacket_t* packet);
  packet_branch_t(state_list_t* condition, subpacket_t* packet);

  std::string to_string();
  void print();
};
*/

packet_branch_t::packet_branch_t(subpacket_t* packet, string namespace_id)
{
  assert(false);
  fprintf(stderr, "This is never used\n");

  this->condition.first = false;
  this->packet = packet;
  this->namespace_id = namespace_id;
}

packet_branch_t::packet_branch_t(vector_t<packet_condition_t*>* condition, subpacket_t* packet, string namespace_id)
{
  assert(condition != NULL);
  assert(packet != NULL);

  this->condition.first = true;
  this->condition.second = condition;

  this->packet = packet;

  this->namespace_id = namespace_id;
}

pair<packet_edge_t*, vector_t<packet_edge_t*>*>* packet_branch_t::paths()
{
  pair<packet_edge_t*, vector_t<packet_edge_t*>*>* ret = this->packet->paths();
  ret->first->prepend_name(this->namespace_id);

  if (this->condition.first) {
    ret->first->condition.first = true;
    ret->first->condition.second = this->condition.second;
  }
  else {
    ret->first->condition.first = false;
    ret->first->condition.second = NULL;
  }

  return ret;
}

name_space_t packet_branch_t::name_space()
{
  name_space_t space;
  space = this->packet->name_space();

  name_space_t new_space = space.prepend(this->namespace_id);

  return new_space;
}

string packet_branch_t::to_string()
{
  assert(false);
  fprintf(stderr, "Not implemented yet\n");
}

void packet_branch_t::print()
{
  printf("%s", this->to_string().c_str() ); 
}

packet_branch_t* packet_branch_t::copy()
{
  uint32_t i;

  packet_branch_t* cpy;
  vector_t<packet_condition_t*>* new_condition = new vector_t<packet_condition_t*>();

  if (this->condition.first) {
    for(i=0; i<this->condition.second->size(); i++) {
      new_condition->push_back( this->condition.second->at(i)->copy() );
    }

    cpy = new packet_branch_t(new_condition, this->packet->copy(), this->namespace_id);
  }
  else {
    cpy = new packet_branch_t(this->packet->copy(), this->namespace_id);
  }

  return cpy;
}

/////////////////////////////////////
// packet edge
/////////////////////////////////////

/*

class packet_edge_t {
  public:
  std::pair<bool, state_list_t*> condition; 
  field_mapping_t field;
  vector_t<packet_edge_t*> branches;

  packet_edge_t(std::pair<bool, state_list_t*> condition, field_mapping_t field);
  packet_edge_t(field_mapping_t field);
};

*/

packet_edge_t::packet_edge_t(vector_t<packet_condition_t*>* condition, field_mapping_t field_mapping) : field(field_mapping)
{
  assert(false);
  fprintf(stderr, "This is never used\n");

  // for whatever reason condition is causing seg faults.
  assert(condition != NULL);

  this->condition.first = true;
  this->condition.second = condition;
}

packet_edge_t::packet_edge_t(field_mapping_t field_mapping) : field(field_mapping)
{
  this->condition.first = false;
  this->condition.second = NULL;
}

packet_edge_t* packet_edge_t::copy()
{
  fprintf(stderr, "This is never used\n");
  assert(false);

  uint32_t i;

  packet_edge_t* cpy = new packet_edge_t(this->field);
  vector_t<packet_condition_t*>* new_condition = new vector_t<packet_condition_t*>();
  vector_t<packet_edge_t*> new_branches;

  // new branches
  for(i=0; i<this->branches.size(); i++) {
    new_branches.push_back( this->branches.at(i)->copy() );
  }

  if (this->condition.first) {
    for(i=0; i<this->condition.second->size(); i++) {
      new_condition->push_back( this->condition.second->at(i)->copy() );
    }

    cpy->branches = new_branches;
    cpy->condition.first = true;
    cpy->condition.second = new_condition;
  }
  else {
    cpy->branches = new_branches;
    cpy->condition.first = false;
  }

  return cpy;
}

void packet_edge_t::set(vector_t<argument_t*> args)
{
  uint32_t i, j;
  for(i=0; i<this->field.params.size(); i++) {
    for(j=0; j<args.size(); j++) {
      parameter_t* param = this->field.params.at(i);
      argument_t* arg = args.at(j);
      param->set(arg);
    }
  }
  for(i=0; i<this->branches.size(); i++) {
    this->branches.at(i)->set(args);
  }
}


void packet_edge_t::prepend_name(string name)
{
  this->field.prepend_name(name);

  uint32_t i;
  for(i=0; i<branches.size(); i++) {
    packet_edge_t* branch = branches.at(i);
    branch->prepend_name(name);
  }
}

string packet_edge_t::to_string()
{
  string packet_edge_string;
  uint32_t i;

/*
  if (this->condition.first) {
    assert(this->condition.second != NULL);

    fprintf(stderr, "This part broken, must fix\n");
    assert(false);

    // packet_edge_string += this->condition.second->to_string();
    // packet_edge_string += ": ";
  }
*/

  packet_edge_string += this->field.to_string();
  packet_edge_string += "\n";

  for(i=0; i<branches.size(); i++) {
    // cant do this lol. it is like fibonacci
    // this->branches.at(i)->print();
    // printf("\n");

    // packet_edge_string += std::to_string( (unsigned long long int) this->branches.size() );  
    // packet_edge_string += " branch: ";
    packet_edge_string += this->branches.at(i)->to_string();
    // packet_edge_string += "\n";
  }
  // packet_edge_string += "\n";

  return packet_edge_string;
}

void packet_edge_t::print()
{
  printf("%s", this->to_string().c_str() ); 
}

/////////////////////////////////////
// packet state
/////////////////////////////////////

packet_condition_t::packet_condition_t(std::string name, value_t val)
{
  this->val = val;
  this->name = name;
}

packet_condition_t* packet_condition_t::copy()
{
  packet_condition_t* cpy = new packet_condition_t(this->name, this->val);
  return cpy;
}

/////////////////////////////////////
// name space
/////////////////////////////////////

/*
class name_space_t : public map_t<std::string, std::pair<uint32_t, uint32_t>> {
  std::string to_string();
  void print();
  
  void prepend(std::string name_space);
  void offset(uint32_t value);
};
*/

string name_space_t::to_string()
{
  string name_space_string;
  
  for(map<string, pair<uint32_t, uint32_t>>::iterator it = this->begin(); it != this->end(); ++it) {
    string name = it->first;
    pair<uint32_t, uint32_t> cut = it->second;

    name_space_string += name;
    name_space_string += " = [";
    name_space_string += std::to_string( (long long unsigned int) cut.first );
    name_space_string += ":";
    name_space_string += std::to_string( (long long unsigned int) cut.second );
    name_space_string += "]\n";
  }

  return name_space_string;
}

void name_space_t::print()
{
  printf("%s", this->to_string().c_str() );
}

name_space_t name_space_t::prepend(string name_space)
{
  name_space_t new_space;

  for(map<string, pair<uint32_t, uint32_t>>::iterator it = this->begin(); it != this->end(); ++it) {
    string name = it->first;
    pair<uint32_t, uint32_t> cut = it->second;

    name = name_space + "." + name;

    new_space.put(name, cut);
  }

  return new_space;
}

name_space_t name_space_t::offset(uint32_t value)
{
  name_space_t new_space;
  for(map<string, pair<uint32_t, uint32_t>>::iterator it = this->begin(); it != this->end(); ++it) {
    string name = it->first;
    pair<uint32_t, uint32_t> cut = it->second;

    cut.first = cut.first + value;
    cut.second = cut.second + value;

    new_space.put(name, cut);
  }

  return new_space;
}
























