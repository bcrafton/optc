
#include "config.h"

using namespace std;
using namespace boost;

// port mapping

port_mapping_t::port_mapping_t(string master, string slave, uint32_t master_ratio, uint32_t slave_ratio)
{
  this->mapping = pair<string, string>(master, slave);
  this->ratio = pair<uint32_t, uint32_t>(master_ratio, slave_ratio);

  if (this->ratio.first == this->ratio.second) {
    this->buffer = false;
  }
  else {
    this->buffer = true;
  }
}

port_mapping_t::port_mapping_t(pair<string, string> mapping, pair<uint32_t, uint32_t> ratio)
{
  this->mapping = mapping;
  this->ratio = ratio;

  if (this->ratio.first == this->ratio.second) {
    this->buffer = false;
  }
  else {
    this->buffer = true;
  }
}

void port_mapping_t::print()
{
  printf("%s\n", this->to_string().c_str());
}

string port_mapping_t::to_string()
{
  string port_mapping_string;
  
  port_mapping_string += this->mapping.first;
  port_mapping_string += " ";
  port_mapping_string += this->mapping.second;

  port_mapping_string += " ";

  port_mapping_string += std::to_string( (unsigned long long int) this->ratio.first); 
  port_mapping_string += " ";
  port_mapping_string += std::to_string( (unsigned long long int) this->ratio.first); 

  return port_mapping_string;
}

// port map

bool port_map_t::contains(string port1, string port2)
{
  int i;
  for(i=0; i<mappings.size(); i++) {
    pair<string, string> mapping = this->mappings[i].mapping;
    if (mapping.first == port1 && mapping.second == port2) {
      return true;
    }
  }
  return false;
}

bool port_map_t::contains(string port)
{
  int i;
  for(i=0; i<mappings.size(); i++) {
    pair<string, string> mapping = this->mappings[i].mapping;
    if (mapping.first == port) {
      return true;
    }
  }
  return false;
}

bool port_map_t::contains(port_mapping_t port_mapping)
{
  int i;
  for(i=0; i<mappings.size(); i++) {
    pair<string, string> mapping = this->mappings[i].mapping;
    if (mapping.first == port_mapping.mapping.first && mapping.second == port_mapping.mapping.second) {
      return true;
    }
  }
  return false;
}

string port_map_t::get(string port)
{
  if( !this->contains(port) ) {
    fprintf(stderr, "Does not contain port: %s\n", port.c_str());
    assert(this->contains(port));
  }

  int i;
  for(i=0; i<mappings.size(); i++) {
    pair<string, string> mapping = this->mappings[i].mapping;
    if (mapping.first == port) {
      return mapping.second;
    }
  }

  assert(0);
}

void port_map_t::put(string port1, string port2, uint32_t port1_ratio, uint32_t port2_ratio)
{
  if (this->contains(port1, port2)) {
    fprintf(stderr, "Already contain pair: %s %s\n", port1.c_str(), port2.c_str());
    assert( !this->contains(port1, port2) );
  }

  this->mappings.push_back( port_mapping_t(port1, port2, port1_ratio, port2_ratio) );
}

void port_map_t::put(port_mapping_t port_mapping)
{
  if (this->contains(port_mapping)) {
    fprintf(stderr, "Already contain pair: %s %s\n", 
      port_mapping.mapping.first.c_str(), 
      port_mapping.mapping.second.c_str()
    );
    assert( !this->contains(port_mapping) );
  }

  this->mappings.push_back(port_mapping);
}

float port_map_t::ratio(string port1, string port2)
{
  if( !this->contains(port1, port2) ) {
    fprintf(stderr, "port map does not contain: %s %s\n", port1.c_str(), port2.c_str());
    assert( this->contains(port1, port2) );
  }

  int i;
  for(i=0; i<mappings.size(); i++) {
    pair<string, string> mapping = this->mappings[i].mapping;
    if (mapping.first == port1 && mapping.second == port2) {
      return float(this->mappings[i].ratio.second) / this->mappings[i].ratio.first;
    }
  }

  assert(0);
}

bool port_map_t::buffer(string port1, string port2)
{
  if (!this->contains(port1, port2)) {
    fprintf(stderr, "No pair: %s %s\n", port1.c_str(), port2.c_str());
    assert( this->contains(port1, port2) );
  }

  int i;
  for(i=0; i<mappings.size(); i++) {
    pair<string, string> mapping = this->mappings[i].mapping;
    if (mapping.first == port1 && mapping.second == port2) {
      return this->mappings[i].buffer;
    }
  }

  assert(0);
}

void port_map_t::set_buffer(string port1, string port2)
{
  int i;
  for(i=0; i<mappings.size(); i++) {
    pair<string, string> mapping = this->mappings[i].mapping;
    if (mapping.first == port1 && mapping.second == port2) {
      this->mappings[i].buffer = true;
    }
  }
}

port_map_t* port_map_t::reverse()
{
  port_map_t* port_map = new port_map_t();

  int i;
  for(i=0; i<this->mappings.size(); i++) {
    port_mapping_t port_mapping = this->mappings.at(i);

    string port1 = port_mapping.mapping.second;
    string port2 = port_mapping.mapping.first;

    uint32_t ratio1 = port_mapping.ratio.second;
    uint32_t ratio2 = port_mapping.ratio.first;

    port_map->put(port1, port2, ratio1, ratio2);
  }
  return port_map;
}

void port_map_t::print()
{
  printf("%s\n", this->to_string().c_str());
}

string port_map_t::to_string()
{
  string port_map_string;
  
  int i;
  for(i=0; i<this->mappings.size(); i++) {
    port_map_string += this->mappings.at(i).to_string();
    port_map_string += "\n";
  }

  return port_map_string;
}

// get the config.

config_t::config_t()
{
}

config_t::config_t(pair<string, string> protocol_file_names, pair<string, string> protocol_names, port_map_t port_map)
{
  this->protocol_file_names = protocol_file_names;
  this->protocol_names = protocol_names;
  this->master_map = port_map;

  port_map_t* slave_map = port_map.reverse();
  this->slave_map = *( slave_map );
}

void config_t::print()
{
  printf("%s\n", this->to_string().c_str());
}

string config_t::to_string()
{
  string config_string;
  
  config_string += this->protocol_file_names.first;
  config_string += " ";
  config_string += this->protocol_file_names.second;

  config_string += "\n";

  config_string += this->protocol_names.first;
  config_string += " ";
  config_string += this->protocol_names.second;

  config_string += "\n";

  config_string += this->master_map.to_string();

  config_string += "\n";

  config_string += this->slave_map.to_string();

  return config_string;
}

config_t get_config() {

  config_t config;
  char buffer1[100];
  char buffer2[100];
  uint32_t master_ratio;
  uint32_t slave_ratio;
  FILE* f;

  // we will just assume the config file is in current directory
  f = fopen("config", "rb");
  if (f==NULL) {
    fprintf(stderr, "File not found\n");
  }

  fscanf(f, "%s %s", buffer1, buffer2);
  config.protocol_file_names.first = buffer1;
  config.protocol_names.first = buffer2;

  fscanf(f, "%s %s", buffer1, buffer2);
  config.protocol_file_names.second = buffer1;
  config.protocol_names.second = buffer2;

  port_map_t master_map;
  port_map_t slave_map;

  // THERE MUST BE SPACES INBETWEEN THE %s AND %u otherwise it thinks its part of the string.
  while(fscanf(f, "%s : %u %s : %u", buffer1, &master_ratio, buffer2, &slave_ratio) != EOF)
  {
    // printf("got: %s %u %s %u\n", buffer1, master_ratio, buffer2, slave_ratio);
    // should add the reverse, just because it makes look ups easier.
    master_map.put( string(buffer1), string(buffer2), master_ratio, slave_ratio);
    slave_map.put( string(buffer2), string(buffer1), slave_ratio, master_ratio);
  }

  config.master_map = master_map;
  config.slave_map = slave_map;

  fclose(f);

  return config;
}
