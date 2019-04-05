
#include "condition.h"

using namespace std;
using namespace boost;

////////////////////////////
////// condition //////////
////////////////////////////

condition_t::condition_t() : cut_ref_t()
{
}

condition_type_t condition_t::type()
{
  fprintf(stderr, "Should never get here\n");
  assert(false);
}

string condition_t::to_string()
{
  fprintf(stderr, "Should never get here\n");
  assert(false);
}

void condition_t::print()
{
  printf("%s", this->to_string().c_str());
}

////////////////////////////
////// control condition ///
////////////////////////////

control_condition_t::control_condition_t(string portname, port_direction_t dir, uint32_t width, value_t val) : condition_t()
{
  this->set_name(portname);
  this->set_dir(dir);
  this->set_width(width);
  this->val = val;
}

control_condition_t::control_condition_t(string portname, port_direction_t dir, value_t val, uint32_t msb, uint32_t lsb) : condition_t()
{
  this->set_name(portname);
  this->set_dir(dir);
  this->set_width(msb-lsb+1);
  this->set_cut(msb, lsb);
  this->val = val;
}

condition_type_t control_condition_t::type()
{
  return COND_CONTROL;
}

string control_condition_t::to_string()
{
  string cond_string;

  if (this->heirarchy.first) {
    uint32_t i; 
    for(i=0; i<this->heirarchy.second.size(); i++) {
      cond_string += this->heirarchy.second.at(i);
      cond_string += ".";
    }
  }
  cond_string += this->get_name();

  cond_string += "(" + this->val.to_string() + ")";

  return cond_string;
}

////////////////////////////
////// state condition /////
////////////////////////////

state_condition_t::state_condition_t(string statename, uint32_t width, value_t val)
{
  this->set_dir(PORT_DIR_NULL);
  this->set_name(statename);
  this->set_width(width);
  this->val = val;
}

condition_type_t state_condition_t::type()
{
  return COND_STATE;
}

string state_condition_t::to_string()
{
  string cond_string;
  cond_string += this->get_name();
  cond_string += "(" + this->val.to_string() + ")";
  return cond_string;
}

////////////////////////////
////// condition list //////
////////////////////////////

string condition_list_t::to_string()
{
  string cond_list_string;
  int i;

  cond_list_string += "(";
  for (i=0; i<this->size(); i++) 
  { 
    cond_list_string += this->at(i)->to_string();

    if (i < (this->size()-1))
    {
      cond_list_string += ", ";
    }
  }
  cond_list_string += ")";
  return cond_list_string;
}

void condition_list_t::print()
{
  printf("%s", this->to_string().c_str());
}





