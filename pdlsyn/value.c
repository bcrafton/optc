

#include "value.h"

using namespace std;
using namespace boost;

// value
value_t::value_t()
{
}

value_t::value_t(value_type_t type, std::string var)
{
  this->type = type;
  this->var = var;
  this->num = 0;
}

value_t::value_t(value_type_t type, uint32_t num)
{
  this->type = type;
  this->num = num;
}

string value_t::to_string()
{
  std::string value_string;

  if (this->type == VALUE_TYPE_NUMBER) {
    value_string += std::to_string( (long long unsigned int) this->num );
  }
  else {
    value_string += this->var;
  }

  return value_string;
}

// value_map_t

uint32_t value_map_t::get(uint32_t key)
{
  assert( this->contains(key) );
  return this->mappings[key];
}

bool value_map_t::contains(uint32_t key)
{
  map<uint32_t,uint32_t>::iterator it = this->mappings.find(key);
  return (it != this->mappings.end());
}

void value_map_t::put(uint32_t key, uint32_t value)
{
  assert( !this->contains(key) );
  pair<uint32_t, uint32_t> new_mapping(key, value);
  this->mappings.insert(new_mapping); 
}

void value_map_t::put(pair<uint32_t, uint32_t> new_mapping)
{
  assert( !this->contains(new_mapping.first) );
  this->mappings.insert(new_mapping); 
}

void value_map_t::set_name(std::string* map_name)
{
  this->map_name = map_name;
}

std::string value_map_t::get_name()
{
  return *(this->map_name);
}

uint32_t value_map_t::get_max()
{
  map<uint32_t,uint32_t>::iterator it = this->mappings.begin();

  int max = it->second;

  for(; it != this->mappings.end(); ++it) {
    if(it->second > max) {
      max = it->second;
    }
  }

  return max;
}

bool value_map_t::value_exists(uint32_t value)
{
  map<uint32_t,uint32_t>::iterator it = this->mappings.begin();
  for(; it != this->mappings.end(); ++it) {
    if(it->second == value) {
      return true;
    }
  }
  return false;
}

uint32_t value_map_t::get_key(uint32_t value)
{
  // there is a better way to do this
  assert(this->value_exists(value));

  map<uint32_t,uint32_t>::iterator it = this->mappings.begin();
  for(; it != this->mappings.end(); ++it) {
    if(it->second == value) {
      return it->first;
    }
  }
}

string value_map_t::to_string()
{
  string map_string;
  map_string += *(this->map_name);
  map_string += "\n";
  int i;
  for(map<uint32_t,uint32_t>::iterator it = this->mappings.begin(); it != this->mappings.end(); ++it) {
    map_string += std::to_string( (long long unsigned int) it->first );
    map_string += " : ";
    map_string += std::to_string( (long long unsigned int) it->second );
    map_string += "\n";
  }
  return map_string;
}

void value_map_t::print()
{
  printf("%s", this->to_string().c_str());
}

// map_list_t

value_map_t* value_map_list_t::at(uint32_t index)
{
  return this->maps[index];
}

uint32_t value_map_list_t::size()
{
  return this->maps.size();
}

void value_map_list_t::push_back(value_map_t* m)
{
  this->maps.push_back(m);
}

string value_map_list_t::to_string()
{
  string map_list_string;
  int i;
  for(i=0; i<this->size(); i++)
  {
    map_list_string += this->at(i)->to_string();
  }
  return map_list_string;
}

void value_map_list_t::print()
{
  printf("%s", this->to_string().c_str());
}

bool value_map_list_t::map_exists(string map_name)
{
  int i;
  for(i=0; i<this->size(); i++) {
    if( this->at(i)->get_name() == map_name ) {
      return true;
    }
  }
  return false;
}

value_map_t* value_map_list_t::get_map(string map_name)
{

  assert( this-map_exists(map_name) );

  int i;
  for(i=0; i<this->size(); i++) {
    if( this->at(i)->get_name() == map_name ) {
      return this->at(i);
    }
  }

  assert(0);
}
