
#ifndef CONFIG_H
#define CONFIG_H

#include "defines.h"
#include "ast.h"

class port_mapping_t {
  public:
  std::pair<std::string, std::string> mapping;
  std::pair<uint32_t, uint32_t> ratio;
  bool buffer;

  port_mapping_t(std::string master, std::string slave, uint32_t master_ratio, uint32_t slave_ratio);
  port_mapping_t(std::pair<std::string, std::string> mapping, std::pair<uint32_t, uint32_t> ratio);

  void print();
  std::string to_string();
};

/*
BUFFER VECTOR NOTE

plan wud really just be
create a new port map in config OR port map
that just handles bufferd ports
and we add do it when we say set buffer
then on lookups we go and see if its there.

we cud do that right now. but its really not worth it.

all it gives us is the size of the push state verilog vector
and we can iterate over it and see if we are to push into it.
instead of iterating over all the port mappings.

so all we are doing now is adding extra 0s and it works fine. 

instead of:
for all master mappings
  if buffer exists
    add 1
  else
    add 0

just do
for all buffer mappings
  if buffer
    add 1
  else 
    add 0

the whole iterating over master map is pretty funky...
we only check it for buffer as well...
thats probably gonna break damn it.
do we add buffer to both sides?

we can fix this, move on
*/

class port_map_t {
  public:
  std::vector< port_mapping_t > mappings;

  std::string get(std::string port);

  bool contains(std::string port1, std::string port2);
  bool contains(std::string port);
  bool contains(port_mapping_t port_mapping);

  void put(std::string port1, std::string port2, uint32_t port1_ratio, uint32_t port2_ratio);
  void put(port_mapping_t port_mapping);

  float ratio(std::string port1, std::string port2);

  bool buffer(std::string port1, std::string port2);

  void set_buffer(std::string port1, std::string port2);

  port_map_t* reverse();

  void print();
  std::string to_string();
};

class config_t {
  public:
  std::pair<std::string, std::string> protocol_file_names;
  std::pair<std::string, std::string> protocol_names;
  // using a vector because there can be multiple ports mapped to same one and look up time is tiny.
  port_map_t master_map;
  port_map_t slave_map;

  config_t();
  config_t(std::pair<std::string, std::string>, std::pair<std::string, std::string>, port_map_t);


  void print();
  std::string to_string();

/*
  std::string get(std::string entity);
  std::string get_master(std::string slave);
  std::string get_slave(std::string master);

  bool contains(std::string entity);
  bool contains_slave(std::string slave);
  bool contains_master(std::string master);

  bool contains(std::string master, std::string slave);
  void put(std::string master, std::string slave, uint32_t master_ratio, uint32_t slave_ratio);

  float ratio(std::string e1, std::string e2);
  uint32_t master_ratio(std::string master, std::string slave);
  uint32_t slave_ratio(std::string master, std::string slave);
  bool buffer(std::string master, std::string slave);
  // uint32_t size();
*/

};

class connection_t {
  public:
  std::string portname;
  port_direction_t dir;
  std::string protocol_name;
};

config_t get_config();

#endif



















