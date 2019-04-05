
#ifndef PORT_H
#define PORT_H

#include "defines.h"
#include "common.h"

#include "ref.h"


/**
 * port is self explanatory. it is a port in the protocol.
 */

class port_t : public port_ref_t {
  public:
  port_type_t type;
  uint32_t width;
  std::string packet_type;

  port_t(port_type_t type, port_direction_t dir, uint32_t width, std::string name);
  port_t(port_type_t type, port_direction_t dir, uint32_t width, std::string name, std::string packet_type);

  std::string to_string();
  void print();

  std::string to_verilog_ports();
  std::string test_bench_head();
  std::string test_bench_translator();

  port_t* copy();
};


/**
 * a list of ports, we have a list so we can do operations on all the ports at once.
 */

class port_list_t {
  private:
  std::vector<port_t*> ports;

  public:
  // std::vector<port_t*> control_in;
  // std::vector<port_t*> control_out;

  port_list_t();
  port_list_t(port_list_t* ports1, port_list_t* ports2);

  port_t* at(uint32_t index);
  uint32_t size();
  void push_back(port_t* port);

  std::string to_string();

  std::string to_verilog_ports();
  std::string test_bench_head();
  std::string test_bench_translator();

  port_list_t* copy();
  void print();
  port_t* find(std::string portname);
  port_t* find(std::pair<std::string, std::string> portname);
  port_direction_t direction(std::string portname);

  // uint32_t control_in_width();
  // uint32_t control_out_width();
};

#endif
