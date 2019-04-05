
#ifndef RTL_BLOCK_H
#define RTL_BLOCK_H

#include "defines.h"
#include "common.h"

#include "edge.h"
#include "ref.h"
#include "transaction.h"

class mux_t;
class receive_mux_t;
class send_mux_t;
class send_muxes_t;
class receive_muxes_t;

class block_t;
class fifo_t;
class increment_t;

class reg_t;

/**
 * mux_t is the abstract mux class which receive and send mux inherit from
 */

class mux_t {
  public:
  std::string name;
  uint32_t fsm_id;
  uint32_t mux_id;

  virtual std::pair<bool, std::string> mux_sel(edge_t* edge);

  // verilog generation
  virtual std::string output_dec();
  virtual std::string output();

  virtual std::string sel();
  virtual std::string sel_dec();

  virtual std::string next_sel();
  virtual std::string next_sel_dec();

  virtual std::string to_verilog();

  virtual std::string to_string();
  void print();
};

/**
 * a receive mux. the mux that sits in front of all receive transaction ports.
 */

class receive_mux_t : public mux_t {
  public:
  transaction_t* receive;
  vector_t<port_ref_t*> ports;
  uint32_t num_ports;
  uint32_t num_sel;
  uint32_t width;

  // constructor
  receive_mux_t(transaction_t* receive, vector_t<port_ref_t*> ports, uint32_t fsm_id);

  // for not double adding muxes
  bool maps(transaction_t* other_receive);

  // this is to see if we already contain a certain send or not and then to add it.
  bool contains(port_ref_t* send);
  void add(port_ref_t* send);
  void add(vector_t<port_ref_t*> ports);

  // verilog generaton
  std::string mux_name();

  std::pair<bool, std::string> mux_sel(edge_t* edge);

  std::string output_dec();
  std::string output();
  std::string output_assign();

  std::string sel();
  std::string sel_dec();

  std::string next_sel();
  std::string next_sel_dec();

  std::string to_verilog();
};

/**
 * send mux. the mux that sits in front of all send blocks like fifo and increment. 
 */

class send_mux_t : public mux_t {
  public:
  transaction_t* send;
  vector_t<transaction_t*> ports;
  uint32_t num_ports;
  uint32_t num_sel;
  uint32_t width;

  // constructor
  send_mux_t(vector_t<transaction_t*> ports, uint32_t fsm_id);

  // for not double adding muxes
  bool maps(port_ref_t* send);

  bool contains(transaction_t* send);
  void add(transaction_t* send);
  void add(vector_t<transaction_t*> ports);

  // verilog generation
  std::pair<bool, std::string> mux_sel(edge_t* edge);

  std::string output_dec();
  std::string output();

  std::string sel();
  std::string sel_dec();

  std::string next_sel();
  std::string next_sel_dec();

  std::string to_verilog();
};

/**
 * a class for the list of all send muxes
 */

class send_muxes_t : public vector_t<send_mux_t*> {
  public:
  bool contains(port_ref_t* send);
  void add(port_ref_t* send, vector_t<transaction_t*> ports);
  vector_t<reg_t*> registers();
};

/**
 * a class for the list of all receive muxes
 */

class receive_muxes_t : public vector_t<receive_mux_t*> {
  public:
  bool contains(transaction_t* other_receive);
  void add(transaction_t* other_receive, vector_t<port_ref_t*> ports);
  vector_t<reg_t*> registers();
};

/**
 * the abstract class for all blocks 
 */

class block_t {
  public:
  std::string name;

  uint32_t fsm_id;
  uint32_t block_id;

  uint32_t in_width;
  uint32_t out_width;

  vector_t<transaction_t*> pops;
  
  bool push_state(transaction_t* transaction);
  bool pop_state(transaction_t* transaction);

  virtual std::string next_push_dec();
  virtual std::string next_pop_dec();
  virtual std::string next_push();
  virtual std::string next_pop();

  virtual std::string push_dec();
  virtual std::string pop_dec();
  virtual std::string push();
  virtual std::string pop();

  virtual std::string output_dec();

  virtual std::string to_verilog();

  virtual std::string to_string();
  void print();
};

/**
 * the class model for fifo. all range parameter defines classes will go through a fifo.
 */

class fifo_t : public block_t {
  public:
  // constructor
  fifo_t(std::string name, vector_t<transaction_t*> pops, uint32_t in_width, uint32_t out_width, uint32_t fsm_id);

  // bool push_state(transaction_t* transaction);
  // bool pop_state(transaction_t* transaction);

  std::string next_push_dec();
  std::string next_pop_dec();
  std::string next_push();
  std::string next_pop();

  std::string push_dec();
  std::string pop_dec();
  std::string push();
  std::string pop();

  std::string output_dec();

  std::string to_verilog();

  std::string to_string();
};

/**
 * the class model for an increment block. all offset parameter defines classes will go through a increment block.
 */

class increment_t : public block_t {
  public:
  // constructor
  increment_t(std::string name, vector_t<transaction_t*> pops, uint32_t in_width, uint32_t out_width, uint32_t fsm_id);

  // bool push_state(transaction_t* transaction);
  // bool pop_state(transaction_t* transaction);

  std::string next_push_dec();
  std::string next_pop_dec();
  std::string next_push();
  std::string next_pop();

  std::string push_dec();
  std::string pop_dec();
  std::string push();
  std::string pop();

  std::string output_dec();

  std::string to_verilog();

  std::string to_string();
};

/**
 * the class for a list of all the blocks
 */

class blocks_t : public vector_t<block_t*> {
  public:
  bool contains(std::string name);
  void add();
  vector_t<reg_t*> registers();
};

/**
 * each register is defined using this class and we keep a list of all registers in protocol
 */

class reg_t {
  public:
  std::string name;
  uint32_t msb;
  uint32_t lsb;

  reg_t(std::string name, uint32_t msb, uint32_t lsb);
  std::string to_verilog();
};


#endif
