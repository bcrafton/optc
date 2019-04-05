
#include "ref.h"

using namespace std;
using namespace boost;

// port ref

port_ref_t::port_ref_t()
{
  this->protocol_name.first = false;
  this->name.first = false;
  this->dir.first = false;
}

port_ref_t::port_ref_t(string protocol_name, string name, port_direction_t dir)
{
  this->protocol_name = pair<bool, string>(true, protocol_name);
  this->name = pair<bool, string>(true, name);
  this->dir = pair<bool, port_direction_t>(true, dir);
}

void port_ref_t::set_protocol_name(string protocol_name)
{
  this->protocol_name.first = true;
  this->protocol_name.second = protocol_name;
}

void port_ref_t::set_name(std::string name)
{
  this->name.first = true;
  this->name.second = name;
}

void port_ref_t::set_dir(port_direction_t dir)
{
  this->dir.first = true;
  this->dir.second = dir;
}

void port_ref_t::reverse_dir()
{
  assert(this->dir.first);
  if (this->dir.second == PORT_DIR_IN) {
    this->dir.second = PORT_DIR_OUT;
  }
  else {
    assert(this->dir.second == PORT_DIR_OUT);
    this->dir.second = PORT_DIR_IN;
  }
}

string port_ref_t::get_protocol_name()
{
  if ( !this->protocol_name.first ) {
    fprintf(stderr, "No protocol name for %s\n", this->get_name().c_str());
    assert(this->protocol_name.first);
  }

  return this->protocol_name.second;
}

string port_ref_t::get_name()
{
  if ( !this->name.first ) {
    assert(this->name.first);
  }

  return this->name.second;
}

port_direction_t port_ref_t::get_dir()
{
  if ( !this->dir.first ) {
    fprintf(stderr, "No dir for %s\n", this->get_name().c_str());
    assert(this->dir.first);
  }

  return this->dir.second;
}

string port_ref_t::to_string()
{
  string port_ref_string;

  assert(this->protocol_name.first);
  assert(this->name.first);
  assert(this->dir.first);

  port_ref_string += this->protocol_name.second;
  port_ref_string += " "; 
  port_ref_string += this->name.second;

  return port_ref_string;
}

void port_ref_t::print()
{
  printf("%s", this->to_string().c_str());
}

int port_ref_t::compare(port_ref_t* other_ref) 
{
  assert(this->name.first && other_ref->name.first);
  assert(this->dir.first && other_ref->dir.first);

  int comp1 = this->name.second.compare(other_ref->name.second);
  // subtracting enums
  int comp2 = this->dir.second - other_ref->dir.second;

  if (comp1 != 0) {
    return comp1;
  }
  if (comp2 != 0) {
    return comp2;
  }

  return 0;
}

// var_ref_t

/*
class var_ref_t : public port_ref_t {
  public:
  vector_t<std::string> heirarchy;

  var_ref_t();
  var_ref_t(std::string protocol_name, std::string name, port_direction_t dir);

  std::string top();
  int compare(var_ref_t* other_ref);
};
*/

var_ref_t::var_ref_t() : port_ref_t()
{
  // we just call the parent constructor here to set the stuff.
  this->heirarchy.first = false;
}

var_ref_t::var_ref_t(string protocol_name, string name, port_direction_t dir)
{
  this->protocol_name = pair<bool, string>(true, protocol_name);
  this->set_name(name);
  this->dir = pair<bool, port_direction_t>(true, dir);
}

void var_ref_t::set_name(string name)
{
  vector_t<string> names = split(name, ".");
  assert( names.size() > 0 );

  int i;
  for(i=0; i<names.size(); i++) {
    string new_name = names.at(i);

    // if we hit the last one ... == i+1 handles the .at(0) case as well.
    if (names.size() == i+1) {
      this->name = pair<bool, string>(true, new_name);
    }
    else {
      this->heirarchy.second.push_back(new_name);
    }
  }

  this->heirarchy.first = true;

  assert( this->name.first );
}

void var_ref_t::set_heirarchy(vector_t<std::string> heirarchy)
{
  this->heirarchy.second = heirarchy;
  this->heirarchy.first = true;
}

vector_t<std::string> var_ref_t::get_heirarchy()
{
  if ( !this->heirarchy.first ) {
    fprintf(stderr, "No protocol name for %s\n", this->get_name().c_str());
    assert(this->heirarchy.first);
  }

  return this->heirarchy.second;
}

string var_ref_t::top()
{
  if (this->heirarchy.first && (this->heirarchy.second.size() > 0)) {
    return this->heirarchy.second.at(0);
  }
  else {
    assert(this->name.first);
    return this->name.second;
  }
}

int var_ref_t::compare(var_ref_t* other_ref) 
{
  // just call parent compare for now.
  port_ref_t::compare(other_ref);
}

string var_ref_t::to_string()
{
  string port_heirarchy_string;

  int i;
  for(i=0; i<this->heirarchy.second.size(); i++) {
    port_heirarchy_string += this->heirarchy.second.at(i);
    port_heirarchy_string += " ";
  }

  port_heirarchy_string += port_ref_t::to_string();

  /*
  assert(this->protocol_name.first);
  assert(this->name.first);
  assert(this->dir.first);

  port_heirarchy_string += this->protocol_name.second;
  port_heirarchy_string += " "; 
  port_heirarchy_string += this->name.second;
  */

  return port_heirarchy_string;
}

void var_ref_t::print()
{
  printf("%s", this->to_string().c_str());
}

// cut ref t

/*
class cut_ref_t : public var_ref_t {
  public:
  std::pair<bool, std::pair<uint32_t, uint32_t>> packet_cut;

  cut_ref_t();
  cut_ref_t(std::string protocol_name, std::string name, port_direction_t dir);
  cut_ref_t(std::string protocol_name, std::string name, port_direction_t dir, std::pair<uint32_t, uint32_t> packet_cut);

  int compare(cut_ref_t* other_ref);

  std::string to_string();
  void print();
};
*/

cut_ref_t::cut_ref_t() : var_ref_t()
{
  this->has_cut = false;
  this->msb = 0;
  this->lsb = 0;
  this->width = pair<bool, uint32_t>(false, 0);
} 

cut_ref_t::cut_ref_t(string protocol_name, string name, port_direction_t dir, uint32_t width) : var_ref_t(protocol_name, name, dir)
{
  this->has_cut = false;
  this->msb = 0;
  this->lsb = 0;
  this->width = pair<bool, uint32_t>(true, width);
}

cut_ref_t::cut_ref_t(string protocol_name, string name, port_direction_t dir, uint32_t width, uint32_t msb, uint32_t lsb) : var_ref_t(protocol_name, name, dir)
{
  this->has_cut = true;
  this->msb = msb;
  this->lsb = lsb;
  this->width = pair<bool, uint32_t>(true, width);
}

string cut_ref_t::to_verilog()
{
  string verilog_string;

  if (this->has_cut) {
    verilog_string += this->top();
    verilog_string += "[";
    verilog_string += std::to_string( (unsigned long long int) this->msb );
    verilog_string += ":";
    verilog_string += std::to_string( (unsigned long long int) this->lsb );
    verilog_string += "]";
  }
  else {
    verilog_string += this->top();
  }

  return verilog_string;
}

void cut_ref_t::set_cut(uint32_t msb, uint32_t lsb)
{
  this->has_cut = true;
  this->msb = msb;
  this->lsb = lsb;
}

void cut_ref_t::set_width(uint32_t width)
{
  this->width = pair<bool, uint32_t>(true, width);
}

uint32_t cut_ref_t::get_width()
{
  assert(this->width.first);
  return this->width.second;
}

int cut_ref_t::compare(cut_ref_t* other_ref)
{
  int comp1 = var_ref_t::compare(other_ref);

  int comp2 = 0;
  int comp3 = 0;
  int comp4 = 0;

  // if one has cut but other dosnt
  if (this->has_cut ^ other_ref->has_cut) {
    if (this->has_cut) {
      comp2 = 1;
    }
    else {
      comp2 = -1;
    }
  }
  // if they both have the cut.
  else if (this->has_cut && other_ref->has_cut) {
    // first bit
    int comp3 = this->lsb - other_ref->lsb;
    // second bit
    int comp4 = this->msb - other_ref->msb;
  }

  if (comp1 != 0) {
    return comp1;
  }
  else if (comp2 != 0) {
    return comp2;
  }
  else if (comp3 != 0) {
    return comp3;
  }
  else if (comp4 != 0) {
    return comp4;
  }
  else {
    return 0;
  }
}

string cut_ref_t::to_string()
{
  string cut_ref_string;

  cut_ref_string += var_ref_t::to_string();

  if(has_cut) {
    cut_ref_string += "[";
    cut_ref_string += std::to_string( (unsigned long long int) msb );
    cut_ref_string += ":";
    cut_ref_string += std::to_string( (unsigned long long int) lsb );
    cut_ref_string += "]";
  } 

  return cut_ref_string;
}

void cut_ref_t::print()
{
  printf("%s", this->to_string().c_str());
}
