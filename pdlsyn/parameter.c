
#include "parameter.h"

using namespace std;
using namespace boost;

// parameter
/*
class parameter_t {
  public:
  std::pair<bool, std::string> var;  

  // constructor
  parameter_t(std::string var);

  virtual parameter_type_t type();

  void print();
  virtual std::string to_string();
};
*/

parameter_t::parameter_t()
{
}

parameter_type_t parameter_t::type()
{
  fprintf(stderr, "Should never get here\n");
  assert(false);
}

parameter_t* parameter_t::copy()
{
  fprintf(stderr, "Should never get here\n");
  assert(false);
}

void parameter_t::set(argument_t* arg)
{
  fprintf(stderr, "Should never get here\n");
  assert(false);
}

float parameter_t::value()
{
  fprintf(stderr, "Should never get here\n");
  assert(false);
}

void parameter_t::print()
{
  printf("%s", this->to_string().c_str());
}

string parameter_t::to_string()
{
  fprintf(stderr, "Should never get here\n");
  assert(false);
}

// parameter range
/*
class parameter_range_t : public parameter_t {
  public:
  int range;

  // constructor
  parameter_range_t(int range);
  parameter_range_t(std::string var);

  // return what type it is 
  parameter_type_t type();

  std::string to_string();  
};
*/

parameter_range_t::parameter_range_t(int range)
{
  this->range = range;

  this->range_var.first = false;
}

parameter_range_t::parameter_range_t(string var)
{
  this->range = 0;

  this->range_var.first = true;
  this->range_var.second = var;
}

parameter_type_t parameter_range_t::type()
{
  return PARAM_RANGE;
}

parameter_t* parameter_range_t::copy()
{
  parameter_range_t* new_param;
  if (this->range_var.first) {
    new_param = new parameter_range_t(this->range_var.second);
  }
  else {
    new_param = new parameter_range_t(this->range);
  }
  return new_param; 
}

void parameter_range_t::set(argument_t* arg)
{
  // assert( this->var.first && (arg->name == this->var.second) );
  // assert(arg->type() == ARG_INT);

  if (arg->type() == ARG_INT && this->range_var.first && arg->name == this->range_var.second) {
    argument_int_t* arg_int = (argument_int_t*) arg;
    this->range = arg_int->val;

    this->range_var.first = false;
  }
}

float parameter_range_t::value()
{
  return ((float)this->range) / RANGE_SCALE;
}

string parameter_range_t::to_string()
{
  string param_string;

  param_string += "range: ";
  if (this->range_var.first) {
    param_string += this->range_var.second;
  }
  else {
    param_string += std::to_string( (long long int) this->range);  
  }

  return param_string;
}

// parameter offset
/*
class parameter_offset_t : public parameter_t {
  public:
  int offset;

  // constructor
  parameter_offset_t(int offset);
  parameter_offset_t(std::string var);

  // return what type it is 
  parameter_type_t type();

  std::string to_string();  
};
*/

parameter_offset_t::parameter_offset_t(int offset)
{
  this->offset = offset;

  this->offset_var.first = false;
}

parameter_offset_t::parameter_offset_t(string var)
{
  this->offset = 0;

  this->offset_var.first = true;
  this->offset_var.second = var;
}

parameter_type_t parameter_offset_t::type()
{
  return PARAM_OFFSET;
}

parameter_t* parameter_offset_t::copy()
{
  parameter_offset_t* new_param;
  if (this->offset_var.first) {
    new_param = new parameter_offset_t(this->offset_var.second);
  }
  else {
    new_param = new parameter_offset_t(this->offset);
  }
  return new_param; 
}

void parameter_offset_t::set(argument_t* arg)
{
  // assert( this->var.first && (arg->name == this->var.second) );
  // assert(arg->type() == ARG_INT);

  if (arg->type() == ARG_INT && this->offset_var.first && arg->name == this->offset_var.second) {
    argument_int_t* arg_int = (argument_int_t*) arg;
    this->offset = arg_int->val;

    this->offset_var.first = false;
  }
}

float parameter_offset_t::value()
{
  return ((float)this->offset) / OFFSET_SCALE;
}

string parameter_offset_t::to_string()
{
  string param_string;

  param_string += "offset: ";
  if (this->offset_var.first) {
    param_string += this->offset_var.second;
  }
  else {
    param_string += std::to_string( (long long int) this->offset);  
  }

  return param_string;
}

// parameter id
/*
class parameter_id_t : public parameter_t { 
  public:
  uint32_t id;

  // constructor 
  parameter_id_t(uint32_t id);
  parameter_id_t(std::string var);

  // return what type it is 
  parameter_type_t type();

  std::string to_string();
};
*/

parameter_id_t::parameter_id_t(uint32_t id)
{
  this->id = id;

  this->id_var.first = false;
}

parameter_id_t::parameter_id_t(string var)
{
  this->id = 0;

  this->id_var.first = true;
  this->id_var.second = var;
}

parameter_type_t parameter_id_t::type()
{
  return PARAM_ID;
}

parameter_t* parameter_id_t::copy()
{
  parameter_id_t* new_param;
  if (this->id_var.first) {
    new_param = new parameter_id_t(this->id_var.second);
  }
  else {
    new_param = new parameter_id_t(this->id);
  }
  return new_param; 
}

void parameter_id_t::set(argument_t* arg)
{
  // assert( this->var.first && (arg->name == this->var.second) );
  // assert(arg->type() == ARG_INT);

  if (arg->type() == ARG_INT && this->id_var.first && arg->name == this->id_var.second) {
    argument_int_t* arg_int = (argument_int_t*) arg;
    this->id = arg_int->val;

    this->id_var.first = false;
  }
}

float parameter_id_t::value()
{
  fprintf(stderr, "Should never get here\n");
  assert(false);
}

string parameter_id_t::to_string()
{
  string param_string;

  param_string += "id: ";
  if (this->id_var.first) {
    param_string += this->id_var.second;
  }
  else {
    param_string += std::to_string( (long long int) this->id);  
  }

  return param_string;
}

// parameter dep
/*
class parameter_dep_t : public parameter_t { 
  public:
  std::string dep_name;
  uint32_t dep_id;

  // constructor 
  parameter_dep_t(std::string dep_name, uint32_t dep_id);
  parameter_dep_t(std::string var);

  // return what type it is 
  parameter_type_t type();

  std::string to_string();
};
*/

parameter_dep_t::parameter_dep_t(string dep_name, uint32_t dep_id)
{
  this->dep_name = dep_name;
  this->dep_name_var.first = false;

  this->dep_id = dep_id;
  this->dep_id_var.first = false;
}

parameter_dep_t::parameter_dep_t(string dep_name, string dep_id_var)
{
  this->dep_name = dep_name;
  this->dep_name_var.first = false;

  this->dep_id = 0;
  this->dep_id_var.first = true;
  this->dep_id_var.second = dep_id_var;
}

parameter_dep_t::parameter_dep_t(string var)
{
  this->dep_name_var.first = true;
  this->dep_name_var.second = var;
  
  this->dep_id = 0;
  this->dep_id_var.first = true;
  this->dep_id_var.second = var;
}

parameter_type_t parameter_dep_t::type()
{
  return PARAM_DEP;
}

parameter_t* parameter_dep_t::copy()
{
  parameter_dep_t* new_param;
  if (this->dep_name_var.first && this->dep_id_var.first) {
    new_param = new parameter_dep_t(this->dep_name_var.second);
  }
  else if (this->dep_id_var.first) {
    new_param = new parameter_dep_t(this->dep_name, this->dep_id_var.second);
  }
  else {
    new_param = new parameter_dep_t(this->dep_name, this->dep_id);
  }
  return new_param; 
}

void parameter_dep_t::set(argument_t* arg)
{
  // assert( this->var.first && (arg->name == this->var.second) );
  // assert(arg->type() == ARG_PAIR);

  /*
  problem with this setup is that
  if we pass garbage to all the params
  we cant assert saying it was garabge.
  */

  // printf("%u %u %u %s %s\n", arg->type(), this->dep_name_var.first, this->dep_id_var.first, this->dep_name_var.second.c_str(), arg->name.c_str());

  if ((arg->type() == ARG_PAIR) && 
     (this->dep_name_var.first && this->dep_id_var.first) &&
     (this->dep_name_var.second == arg->name)) 
  {
    argument_pair_t* arg_pair = (argument_pair_t*) arg;
    this->dep_name = arg_pair->pair_name;
    this->dep_id = arg_pair->pair_val;

    this->dep_name_var.first = false;
    this->dep_id_var.first = false;
  }
  else if ((arg->type() == ARG_INT) && 
          this->dep_id_var.first &&
          (this->dep_id_var.second == arg->name)) 
  {
    assert(!this->dep_name_var.first);
    argument_int_t* arg_int = (argument_int_t*) arg;
    this->dep_id = arg_int->val;

    this->dep_id_var.first = false;
  }
}

float parameter_dep_t::value()
{
  fprintf(stderr, "Should never get here\n");
  assert(false);
}

string parameter_dep_t::to_string()
{
  string param_string;

  param_string += "dep id: ";
  if (this->dep_name_var.first) {
    param_string += this->dep_name_var.second;
  }
  else {
    param_string += this->dep_name;  
  }

  param_string += " ";

  if (this->dep_id_var.first) {
    param_string += this->dep_id_var.second;
  }
  else {
    param_string += std::to_string( (long long int) this->dep_id);  
  }

  return param_string;
}

/*
class argument_t {
  public:
  std::string name;

  argument_t();

  virtual argument_t* copy();
  virtual argument_type_t type();

  void print();
  virtual std::string to_string();
};
*/

argument_t::argument_t()
{
}

argument_t* argument_t::copy()
{
  assert(false);
  fprintf(stderr, "Should never make it here\n");
}

argument_type_t argument_t::type()
{
  assert(false);
  fprintf(stderr, "Should never make it here\n");
}

void argument_t::print()
{
  printf("%s", this->to_string().c_str());
}

string argument_t::to_string()
{
  assert(false);
  fprintf(stderr, "Should never make it here\n");
}

/*
class argument_int_t : public argument_t {
  public:
  uint32_t val;

  argument_int_t(std::string name, uint32_t val);

  argument_type_t type();
  argument_t* copy();

  std::string to_string();
};
*/

argument_int_t::argument_int_t(string name, uint32_t val)
{
  this->name = name;
  this->val = val;
}

argument_t* argument_int_t::copy()
{
  return new argument_int_t(this->name, this->val);
}

argument_type_t argument_int_t::type()
{
  return ARG_INT;
}

string argument_int_t::to_string()
{
  string arg_string;

  arg_string += this->name;
  arg_string += " ";
  arg_string += std::to_string( (long long int) this->val);

  return arg_string;
}

/*
class argument_pair_t : public argument_t {
  public:
  std::string pair_name;
  uint32_t pair_val;

  argument_pair_t(std::string arg_name, std::string pair_name, uint32_t pair_val);

  argument_type_t type();
  argument_t* copy();

  std::string to_string();
};
*/

argument_pair_t::argument_pair_t(string arg_name, string pair_name, uint32_t pair_val)
{
  this->name = arg_name;
  this->pair_name = pair_name;
  this->pair_val = pair_val;
}

argument_t* argument_pair_t::copy()
{
  return new argument_pair_t(this->name, this->pair_name, this->pair_val);
}

argument_type_t argument_pair_t::type()
{
  return ARG_PAIR;
}

string argument_pair_t::to_string()
{
  string arg_string;

  arg_string += this->name;
  arg_string += " ";
  arg_string += this->pair_name;
  arg_string += " ";
  arg_string += std::to_string( (long long int) this->pair_val);

  return arg_string;
}
































