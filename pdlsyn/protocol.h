
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "defines.h"
#include "common.h"

#include "connection.h"
#include "rtl_block.h"
#include "fsm.h"
#include "value.h"
#include "port.h"
#include "config.h"
#include "body.h"

class sched_t;
class sched_thread_t;
class sched_thread_args_t;
class protocol_t;


/**
 * sched is a helper object used in the schedule function in protocol.c
 */

class sched_t {
  public:
  uint32_t size;

  bool* valid;
  bool* add;
  bool* needs;
  bool* ready;
  bool* none;
  bool* dep;
  edge_t** edge;
  eid_t* eid;

  sched_t(uint32_t size);
};


/**
 * pthreads only allows for one argument to be passed so we put all the arguments we need in this struct.
 */

class sched_thread_t {
  public:

  protocol_t* translator;
  fsm_t** scheduled;
  protocol_t* p1;
  protocol_t* p2;
  vector_t<path_set_t*> psets1;
  vector_t<path_set_t*> psets2;

  sched_thread_t(protocol_t* translator, fsm_t** scheduled, protocol_t* p1, protocol_t* p2, vector_t<path_set_t*> psets1, vector_t<path_set_t*> psets2);
};


/**
 * because we spawn many threads each one needs to know which data it is operating on. this tells each thread which data it is operating on.
 */

class sched_thread_args_t {
  public:

  uint32_t start_i;
  uint32_t end_i;
  sched_thread_t* sched_thread;

  sched_thread_args_t(uint32_t start_i, uint32_t end_i, sched_thread_t* sched_thread);
};


/**
 * the protocol class. it is the highest level class and contains all the info for a protocol
 */

class protocol_t {
  public:
  std::string         name;
  value_map_list_t*   maps;
  port_list_t*        ports;
  // probably need a states list right here.
  // fsm_t*           behavior;
  vector_t<fsm_t*>    behavior;
  connections_t*      connections;

  protocol_t* p1;
  protocol_t* p2;

  uint32_t num_fsms;
  uint32_t p1_offset;
  uint32_t* num_states;
  uint32_t* state_width;

  // this is used for tb
  // not sure about keeping this.
  vector_t<path_set_t*> psets; 

  // this is used for tb
  vector_t<edge_t**> sched;

  //vector_t<control_pos_t> control_in;
  //vector_t<control_pos_t> control_out;
  //map_comp_t<>        control_in;
  //map_comp_t<>        control_out;

  // vector_t<send_mux_t*> send_muxes;
  // vector_t<receive_mux_t*> receive_muxes;

  send_muxes_t send_muxes;
  receive_muxes_t receive_muxes;
  blocks_t blocks;
  vector_t<reg_t*> registers;

  bool* has_output_state;
  bool* has_input_state;

  map_t<std::string, packet_t*> packets;

  protocol_t();
  protocol_t(protocol_t* p1, protocol_t* p2, config_t* config);

  // vector_t<fsm_t*> schedule(vector_t<fsm_t*> behavior1, vector_t<fsm_t*> behavior2);
  vector_t<fsm_t*> schedule(protocol_t* p1, protocol_t* p2);

  vector_t<path_set_t*> path_sets();
  void to_file(std::string filename);
  void print();
  void to_visualize(std::string filename);
  void add_body(body_t* body);
  void set_name(std::string protocol_name);
  void set_control();
  void set_muxes();
  void set_blocks();
  void set_registers();
  uint32_t size();

  // translator generation
  std::string to_verilog();
  std::string to_verilog_states();
  std::string to_verilog_fsm();
  std::string to_verilog_fsm_help(uint32_t state_width, fsm_t* fsm, uint32_t fsm_id);
  std::string to_verilog_trans();
  std::string to_verilog_vars();
  std::string to_verilog_assign();
  // std::string to_verilog_reset();

  // test bench generation
  std::string test_bench();
  std::string test_bench_head();
  std::string test_bench_translator();

  std::string test_bench_body();
  std::string test_bench_body_init();
  std::string test_bench_body_always();
  std::string test_bench_body_always_next();
  std::string test_bench_body_always_set();
  std::string test_bench_body_always_test();

  // vector file
  std::string test_bench_vec();

  // makefile
  std::string makefile();
  
  std::string protocol_name();
  std::string clk_name();
  std::string reset_name();

  // std::string output_condition(edge_t* e);
  // std::string input_condition(edge_t* e);
  // std::string push_state(edge_t* edge, connections_t* connections);
  // std::string pop_state(edge_t* edge, connections_t* connections);

};

#endif
