
#include "rtl_block.h"

using namespace std;
using namespace boost;

static uint32_t next_mux_id = 0;
static uint32_t next_block_id = 0;

static string get_mux(uint32_t num_transactions, uint32_t largest_transaction)
{
  uint32_t num_ports;
  uint32_t num_sel_bits;

  uint32_t log2_num_transactions = log2number(num_transactions);
  if (log2_num_transactions == 0) {
    num_sel_bits = 1;
  }
  else {
    num_sel_bits = log2_num_transactions;
  }
  num_ports = 1 << num_sel_bits;

  string mux_name;
  mux_name += "mux";
  mux_name += std::to_string( (unsigned long long int) num_ports );
  mux_name += "x";
  mux_name += std::to_string( (unsigned long long int) num_sel_bits );
  mux_name += "#(";
  mux_name += std::to_string( (unsigned long long int) largest_transaction );
  mux_name += ")";
  return mux_name;
}

// mux

/*
class mux_t {
  public:
  std::string name;

  virtual std::string mux_sel(edge_t* edge);

  virtual std::string to_string();
  void print();
};
*/

string mux_t::output_dec()
{
  fprintf(stderr, "This function should never be called\n");
  assert(false);
}

string mux_t::output()
{
  fprintf(stderr, "This function should never be called\n");
  assert(false);
}

string mux_t::sel()
{
  fprintf(stderr, "This function should never be called\n");
  assert(false);
}

string mux_t::sel_dec()
{
  fprintf(stderr, "This function should never be called\n");
  assert(false);
}

string mux_t::next_sel()
{
  fprintf(stderr, "This function should never be called\n");
  assert(false);
}

string mux_t::next_sel_dec()
{
  fprintf(stderr, "This function should never be called\n");
  assert(false);
}

string mux_t::to_string()
{
  fprintf(stderr, "This function should never be called\n");
  assert(false);
}

void mux_t::print()
{
  printf("%s", this->to_string().c_str());
}

pair<bool, string> mux_t::mux_sel(edge_t* edge)
{
  fprintf(stderr, "This function should never be called\n");
  assert(false);
}

string mux_t::to_verilog()
{
  fprintf(stderr, "This function should never be called\n");
  assert(false);
}

// rec mux

/*
class receive_mux_t : public mux_t {
  public:
  transaction_t* receive;
  vector_t<port_ref_t*> ports;

  // constructor
  receive_mux_t(transaction_t* receive, vector_t<port_ref_t*> ports);

  // mux select
  virtual std::string mux_sel(edge_t* edge);

  std::string to_string();
};
*/

receive_mux_t::receive_mux_t(transaction_t* receive, vector_t<port_ref_t*> ports, uint32_t fsm_id)
{
  assert(ports.size() > 0);
  this->receive = receive;
  this->name = receive->get_name();
  this->ports = ports;
  this->mux_id = next_mux_id;
  next_mux_id++;
  this->fsm_id = fsm_id;
  this->width = receive->get_width();
  this->num_ports = ports.size();
  this->num_sel = (this->num_ports == 1) ? 1 : log2number(this->num_ports);
}

bool receive_mux_t::maps(transaction_t* other_receive)
{
  return (this->receive->compare(other_receive)==0);
}

bool receive_mux_t::contains(port_ref_t* send)
{
  uint32_t i;
  for(i=0; i<this->ports.size(); i++) {
    port_ref_t* port_ref = this->ports.at(i);
    if (port_ref->compare(send) == 0) {
      return true;
    }
  }
  return false;
}

void receive_mux_t::add(port_ref_t* send)
{
  assert(this->contains(send));
  this->ports.push_back(send);
}

void receive_mux_t::add(vector_t<port_ref_t*> ports)
{
  uint32_t i;
  for(i=0; i<ports.size(); i++) {
    port_ref_t* send = ports.at(i);
    if (!this->contains(send)) {
      this->add(send);
    }
  }
}

string receive_mux_t::mux_name()
{
  string mux_name_string;

  mux_name_string += this->receive->get_name();
  mux_name_string += "_";
  mux_name_string += std::to_string( (unsigned long long int) this->mux_id );

  return mux_name_string;
}

string receive_mux_t::output_dec()
{
  string output_string;
  
  output_string += "wire ";
  output_string += "[";
  output_string += std::to_string( (unsigned long long int) (this->width - 1) );
  output_string += ":0] ";
  output_string += this->mux_name();
  output_string += "_mux_out;";  

  return output_string;
}

string receive_mux_t::output()
{
  string output_string;
  
  output_string += this->mux_name();
  output_string += "_mux_out;";  

  return output_string;
}

string receive_mux_t::output_assign()
{
  string output_string;
  
  output_string += "assign ";
  output_string += this->receive->to_verilog();  
  output_string += " = ";
  output_string += this->output();

  return output_string;
}

string receive_mux_t::sel()
{
  string sel_string;

  sel_string += this->mux_name();
  sel_string += "_mux_sel";  

  return sel_string;
}

string receive_mux_t::sel_dec()
{
  string sel_string;

  sel_string += "reg ";
  sel_string += "[";
  sel_string += std::to_string( (unsigned long long int) (this->num_sel - 1) );
  sel_string += ":0] ";
  sel_string += this->mux_name();
  sel_string += "_mux_sel;";  

  return sel_string;
}

string receive_mux_t::next_sel()
{
  string next_sel_string;

  next_sel_string += "next_";
  next_sel_string += this->mux_name();
  next_sel_string += "_mux_sel";  

  return next_sel_string;
}

string receive_mux_t::next_sel_dec()
{
  string next_sel_string;

  next_sel_string += "reg ";
  next_sel_string += "[";
  next_sel_string += std::to_string( (unsigned long long int) (this->num_sel - 1) );
  next_sel_string += ":0] ";
  next_sel_string += "next_";
  next_sel_string += this->mux_name();
  next_sel_string += "_mux_sel;";  

  return next_sel_string;
}

/*
Whats going on here?

Each receive mux represents a different receive location
So cpi[63:0] - read_data, cpi[95:0] - read data get different muxes

so out receive mux has a transaction which will be matched against incoming transaction ... the name AND packet cut must match

and we have a list of different ports where we can receive data from ... each one of them being the output of a send fifo.

this is because each receive location can come from a number of different fifos

1. we iterate over all the receives on an edge
2. if the receive matches our this->receive with NAME AND PACKET CUT
3. we consider the source of the receive from edge AND our port. The sender of both of these must nmatch
4. once we have a match we return.
*/

/*
pair<bool, string> receive_mux_t::mux_sel(edge_t* edge)
{
  string mux_string;
  mux_string += this->next_sel();
  mux_string += " = ";

  uint32_t i, j;
  for(i=0; i<edge->num_receives(); i++) {
    transaction_t* other_receive = edge->receives.at(i);

    if (this->receive->compare(other_receive) == 0) {
      for(j=0; j<this->ports.size(); j++) {

        port_ref_t* port = this->ports.at(j);
        port_ref_t* other_source = other_receive->source();
        if ( port->compare(other_source) == 0 ) {
          mux_string += "2'b";
          mux_string += num2binary(j, 2);
          mux_string += ";";
          return pair<bool, string>(true, mux_string);
        }

      }
      fprintf(stderr, "Should never make it here\n");
      assert(false);
    }
  }

  return pair<bool, string>(false, "");
}
*/

pair<bool, string> receive_mux_t::mux_sel(edge_t* edge)
{
  string mux_string;
  mux_string += this->next_sel();
  mux_string += " = ";

  uint32_t i, j, k;
  for(i=0; i<edge->num_receives(); i++) {
    transaction_t* other_receive = edge->receives.at(i);
    vector_t<port_ref_t*> other_sources = other_receive->source();

    if (this->receive->compare(other_receive) == 0) {

      for(j=0; j<other_sources.size(); j++) {
        port_ref_t* other_source = other_sources.at(j);

        for(k=0; k<this->ports.size(); k++) {
          port_ref_t* port = this->ports.at(k);

          if ( port->compare(other_source) == 0 ) {
            mux_string += "2'b";
            mux_string += num2binary(k, 2);
            mux_string += ";";
            return pair<bool, string>(true, mux_string);
          }
        }
        fprintf(stderr, "Should never make it here\n");
        assert(false);
      }
    }
  }

  return pair<bool, string>(false, "");
}

string receive_mux_t::to_verilog()
{
  assert(this->ports.size() > 0);

  uint32_t i;

  string mux_name;
  mux_name += "\n";
  mux_name += get_mux(this->num_ports, this->width);
  mux_name += " ";

  mux_name += this->mux_name() + "_mux" + "(\n";

  for(i=0; i<this->ports.size(); i++) {    
    mux_name += ".in";
    mux_name += std::to_string( (unsigned long long int) i );
    mux_name += "(";
    mux_name += this->ports.at(i)->get_name();
    mux_name += "_block_out";
    mux_name += "),\n";
  }

  mux_name += ".sel(";
  mux_name += this->sel();
  mux_name += "),\n";

  mux_name += ".out(";
  mux_name += this->mux_name();
  mux_name += "_mux_out)\n";

  mux_name += ");";

  return mux_name;
}

// send mux

/*
class send_mux_t : public mux_t {
  public:
  vector_t<transaction_t*> ports;

  // constructor
  send_mux_t(vector_t<transaction_t*> ports);

  // mux select
  virtual std::string mux_sel(edge_t* edge);

  std::string to_string();
};
*/

send_mux_t::send_mux_t(vector_t<transaction_t*> ports, uint32_t fsm_id)
{
  uint32_t i;

  assert(ports.size() > 0);
  this->ports = ports;

  this->name = ports.at(0)->get_name();
  this->send = ports.at(0);

  uint32_t largest = 0;
  for(i=0; i<this->ports.size(); i++) {
    if (largest < this->ports.at(i)->get_width()) {
      largest = this->ports.at(i)->get_width();
    }
  }
  assert(largest > 0);
 
  this->width = largest;
  this->num_ports = ports.size();
  this->num_sel = (this->num_ports == 1) ? 1 : log2number(this->num_ports);

  this->mux_id = next_mux_id;
  next_mux_id++;
  this->fsm_id = fsm_id;
}

bool send_mux_t::maps(port_ref_t* send)
{
  return (send->compare(this->send)==0);
}

bool send_mux_t::contains(transaction_t* send)
{
  uint32_t i;
  for(i=0; i<this->ports.size(); i++) {
    transaction_t* next = this->ports.at(i);

    if (next->compare(send) == 0) {
      return true;
    }
  }
  return false;
}

void send_mux_t::add(transaction_t* send)
{
  assert(this->contains(send));
  this->ports.push_back(send);
}

void send_mux_t::add(vector_t<transaction_t*> ports)
{
  uint32_t i;
  for(i=0; i<ports.size(); i++) {
    transaction_t* send = ports.at(i);
    if ( !this->contains(send) ) {
      this->add(send);
    }
  }
}

string send_mux_t::output_dec()
{
  string output_string;

  output_string += "wire ";
  output_string += "[";
  output_string += std::to_string( (unsigned long long int) (this->width - 1) );
  output_string += ":0] ";
  output_string += this->ports.at(0)->get_name();
  output_string += "_mux_out;";  

  return output_string;
}

string send_mux_t::output()
{
  string output_string;

  output_string += this->ports.at(0)->get_name();
  output_string += "_mux_out;";  

  return output_string;
}

string send_mux_t::sel()
{
  string sel_string;

  sel_string += this->ports.at(0)->get_name();
  sel_string += "_mux_sel";  

  return sel_string;
}

string send_mux_t::sel_dec()
{
  string sel_string;

  sel_string += "reg ";
  sel_string += "[";
  sel_string += std::to_string( (unsigned long long int) (this->num_sel - 1) );
  sel_string += ":0] ";
  sel_string += this->ports.at(0)->get_name();
  sel_string += "_mux_sel;";  

  return sel_string;
}

string send_mux_t::next_sel()
{
  string next_sel_string;

  next_sel_string += "next_";
  next_sel_string += this->ports.at(0)->get_name();
  next_sel_string += "_mux_sel";  

  return next_sel_string;
}

string send_mux_t::next_sel_dec()
{
  string next_sel_string;

  next_sel_string += "reg ";
  next_sel_string += "[";
  next_sel_string += std::to_string( (unsigned long long int) (this->num_sel - 1) );
  next_sel_string += ":0] ";
  next_sel_string += "next_";
  next_sel_string += this->ports.at(0)->get_name();
  next_sel_string += "_mux_sel;";  

  return next_sel_string;
}

/*
Whats going on here?

Well for a send mux we have all the different locations that a transaction can come from

1. We iterate over all sends on the edge
2. Then we iterate over all ports in the send mux
3. Then we say: does this port match with this particular send?
4. If so then we have a match and set our select bits
*/

pair<bool, string> send_mux_t::mux_sel(edge_t* edge)
{
  string mux_string;
  mux_string += this->next_sel();
  mux_string += " = ";

  uint32_t i, j;
  for(i=0; i<edge->num_sends(); i++) {
    transaction_t* other_send = edge->sends.at(i);

    for(j=0; j<this->ports.size(); j++) {
      transaction_t* port = this->ports.at(j);

      if ( port->compare(other_send) == 0 ) {
        mux_string += "2'b";
        mux_string += num2binary(j, 2);
        mux_string += ";";
        return pair<bool, string>(true, mux_string);
      }

    }
  }

  return pair<bool, string>(false, "");
}

string send_mux_t::to_verilog()
{
  assert(this->ports.size() > 0);

  uint32_t i;

  string mux_name;
  mux_name += "\n";
  mux_name += get_mux(this->num_ports, this->width);
  mux_name += " ";

  // we assert at the top of the file that this exists
  mux_name += this->ports.at(0)->get_name() + "_mux" + "(\n";

  for(i=0; i<this->ports.size(); i++) {    
    mux_name += ".in";
    mux_name += std::to_string( (unsigned long long int) i );
    mux_name += "(";
    mux_name += this->ports.at(i)->to_verilog();
    mux_name += "),\n";
  }

  mux_name += ".sel(";
  mux_name += this->sel();
  mux_name += "),\n";

  mux_name += ".out(";
  mux_name += this->ports.at(0)->get_name();
  mux_name += "_mux_out)\n";

  mux_name += ");";

  return mux_name;
}

/*
class block_t {
};
*/

bool block_t::push_state(transaction_t* transaction)
{
  assert(transaction->get_dir() == PORT_DIR_OUT);
  return (transaction->get_name() == this->name);
}

/*
these are complex
u cannot just say if the edge contains the sender/receiver push/pop
because it dosnt necessary always go that way

well actually
for sender it does

but for receiver it is more complex.
need to make sure it is the correct receiver.
which wud be in our mappings.
*/

bool block_t::pop_state(transaction_t* transaction)
{
  assert(transaction->get_dir() == PORT_DIR_IN);

  uint32_t i;
  for(i=0; i<pops.size(); i++) {
    transaction_t* pop = this->pops.at(i);
    if (pop->compare(transaction)==0) {
      return true;
    }
  }

  return false;
}

string block_t::next_push_dec()
{
  fprintf(stderr, "This should never be called\n");
  assert(false); 
}

string block_t::next_pop_dec()
{
  fprintf(stderr, "This should never be called\n");
  assert(false); 
}

string block_t::next_push()
{
  fprintf(stderr, "This should never be called\n");
  assert(false); 
}

string block_t::next_pop()
{
  fprintf(stderr, "This should never be called\n");
  assert(false); 
}

string block_t::push_dec()
{
  fprintf(stderr, "This should never be called\n");
  assert(false); 
}

string block_t::pop_dec()
{
  fprintf(stderr, "This should never be called\n");
  assert(false); 
}

string block_t::push()
{
  fprintf(stderr, "This should never be called\n");
  assert(false); 
}

string block_t::pop()
{
  fprintf(stderr, "This should never be called\n");
  assert(false); 
}

string block_t::output_dec()
{
  fprintf(stderr, "This should never be called\n");
  assert(false); 
}

string block_t::to_verilog()
{
  fprintf(stderr, "This should never be called\n");
  assert(false); 
}

string block_t::to_string()
{
  fprintf(stderr, "This should never be called\n");
  assert(false); 
}

void block_t::print()
{
  printf("%s", this->to_string().c_str());
}

/*
class fifo_t {
};
*/

fifo_t::fifo_t(string name, vector_t<transaction_t*> pops, uint32_t in_width, uint32_t out_width, uint32_t fsm_id)
{
  this->name = name;

  this->pops = pops;

  this->in_width = in_width;
  this->out_width = out_width;

  this->block_id = next_block_id;
  this->fsm_id = fsm_id;
  next_block_id++;
}

/*
bool fifo_t::push_state(transaction_t* transaction)
{
  assert(transaction->get_dir() == PORT_DIR_OUT);
  return (transaction->get_name() == this->name);
}
*/

/*
bool fifo_t::pop_state(transaction_t* transaction)
{
  assert(false);
}
*/

string fifo_t::next_push_dec()
{
  string fifo_string;

  fifo_string += "reg next_";
  fifo_string += this->name;
  fifo_string += "_push;";

  return fifo_string;
}

string fifo_t::next_pop_dec()
{
  string fifo_string;

  fifo_string += "reg next_";
  fifo_string += this->name;
  fifo_string += "_pop;";

  return fifo_string;
}

string fifo_t::next_push()
{
  string fifo_string;

  fifo_string += "next_";
  fifo_string += this->name;
  fifo_string += "_push";

  return fifo_string;
}

string fifo_t::next_pop()
{
  string fifo_string;

  fifo_string += "next_";
  fifo_string += this->name;
  fifo_string += "_pop";

  return fifo_string;
}

string fifo_t::push_dec()
{
  string fifo_string;

  fifo_string += "reg ";
  fifo_string += this->name;
  fifo_string += "_push;";

  return fifo_string;
}

string fifo_t::pop_dec()
{
  string fifo_string;

  fifo_string += "reg ";
  fifo_string += this->name;
  fifo_string += "_pop;";

  return fifo_string;
}

string fifo_t::push()
{
  string fifo_string;

  fifo_string += this->name;
  fifo_string += "_push";

  return fifo_string;
}

string fifo_t::pop()
{
  string fifo_string;

  fifo_string += this->name;
  fifo_string += "_pop";

  return fifo_string;
}

string fifo_t::output_dec()
{
  string output_string;

  output_string += "wire [";
  output_string += std::to_string( (unsigned long long int) (this->out_width - 1) );
  output_string += ":0] ";
  output_string += this->name;
  output_string += "_block_out;";

  return output_string;
}

string fifo_t::to_verilog()
{
  string verilog_string;
  
  string fifo_name;
  if (this->in_width > this->out_width) {
    fifo_name = "down_fifo";
  }
  else {
    fifo_name = "up_fifo";
  }

  verilog_string += "\n" + fifo_name + "#(" + std::to_string( (unsigned long long int) this->in_width ) + ", " + std::to_string( (unsigned long long int) this->out_width ) + ") ";
  verilog_string += this->name + "_fifo" + "(\n";

  verilog_string += ".clk(clk),\n";
  verilog_string += ".reset(reset),\n";

  verilog_string += ".push(";
  verilog_string += this->push();
  verilog_string += "),\n";

  verilog_string += ".pop(";
  verilog_string += this->pop();
  verilog_string += "),\n";

  verilog_string += ".data_in(" + this->name + "_mux_out" + "),\n";
  verilog_string += ".data_out(" + this->name + "_block_out" + ")\n";
  verilog_string += ");\n";

  return verilog_string;
}

string fifo_t::to_string()
{
  fprintf(stderr, "Not implemented yet\n");
  assert(false);

  string fifo_string;
  return fifo_string;
}

/*
class increment_t : public block_t {
};
*/

increment_t::increment_t(string name, vector_t<transaction_t*> pops, uint32_t in_width, uint32_t out_width, uint32_t fsm_id)
{
  this->name = name;

  this->pops = pops;

  this->in_width = in_width;
  this->out_width = out_width;

  this->block_id = next_block_id;
  this->fsm_id = fsm_id;
  next_block_id++;
}

/*
bool increment_t::push_state(transaction_t* transaction)
{
  assert(transaction->get_dir() == PORT_DIR_OUT);
  return (transaction->get_name() == this->name);
}
*/

/*
bool increment_t::pop_state(transaction_t* transaction)
{
  assert(false);
}
*/

string increment_t::next_push_dec()
{
  string increment_string;

  increment_string += "reg next_";
  increment_string += this->name;
  increment_string += "_push;";

  return increment_string;
}

string increment_t::next_pop_dec()
{
  string increment_string;

  increment_string += "reg next_";
  increment_string += this->name;
  increment_string += "_pop;";

  return increment_string;
}

string increment_t::next_push()
{
  string increment_string;

  increment_string += "next_";
  increment_string += this->name;
  increment_string += "_push";

  return increment_string;
}

string increment_t::next_pop()
{
  string increment_string;

  increment_string += "next_";
  increment_string += this->name;
  increment_string += "_pop";

  return increment_string;
}

string increment_t::push_dec()
{
  string increment_string;

  increment_string += "reg ";
  increment_string += this->name;
  increment_string += "_push;";

  return increment_string;
}

string increment_t::pop_dec()
{
  string increment_string;

  increment_string += "reg ";
  increment_string += this->name;
  increment_string += "_pop;";

  return increment_string;
}

string increment_t::push()
{
  string increment_string;

  increment_string += this->name;
  increment_string += "_push";

  return increment_string;
}

string increment_t::pop()
{
  string increment_string;

  increment_string += this->name;
  increment_string += "_pop";

  return increment_string;
}

string increment_t::output_dec()
{
  string output_string;

  output_string += "wire [";
  output_string += std::to_string( (unsigned long long int) (this->out_width - 1) );
  output_string += ":0] ";
  output_string += this->name;
  output_string += "_block_out;";

  return output_string;
}

string increment_t::to_verilog()
{
  string verilog_string;
  
  string fifo_name;
  if (this->in_width > this->out_width) {
    fifo_name = "increment";
  }
  else {
    fifo_name = "increment";
  }

  verilog_string += "\n" + fifo_name + "#(" + std::to_string( (unsigned long long int) this->in_width ) + ", " + std::to_string( (unsigned long long int) this->out_width ) + ") ";
  verilog_string += this->name + "_increment" + "(\n";

  verilog_string += ".clk(clk),\n";
  verilog_string += ".reset(reset),\n";

  verilog_string += ".set(";
  verilog_string += this->push();
  verilog_string += "),\n";

  verilog_string += ".incr(";
  verilog_string += this->pop();
  verilog_string += "),\n";

  verilog_string += ".incr_amnt(4),\n";
  verilog_string += ".data_in(" + this->name + "_mux_out" + "),\n";
  verilog_string += ".data_out(" + this->name + "_block_out" + ")\n";
  verilog_string += ");\n";

  return verilog_string;
}

string increment_t::to_string()
{
  fprintf(stderr, "Not implemented yet\n");
  assert(false);

  string fifo_string;
  return fifo_string;
}

/*
class send_muxes_t : public vector_t<send_mux_t*> {
  bool contains(port_ref_t* send);
  void add(port_ref_t* send, vector_t<transaction_t*> ports);
};
*/

bool send_muxes_t::contains(port_ref_t* send)
{
  uint32_t i;
  for(i=0; i<this->size(); i++) {
    send_mux_t* next = this->at(i);
    if (next->maps(send)) {
      return true;
    }
  }
  return false;
}

void send_muxes_t::add(port_ref_t* send, vector_t<transaction_t*> ports)
{
  assert(this->contains(send));

  uint32_t i;
  for(i=0; i<this->size(); i++) {
    send_mux_t* next = this->at(i);
    if (next->maps(send)) {
      next->add(ports);
      return;
    }
  }

  fprintf(stderr, "Should never get here\n");
  assert(false);
}

vector_t<reg_t*> send_muxes_t::registers()
{
  vector_t<reg_t*> regs;

  uint32_t i;
  for(i=0; i<this->size(); i++) {
    send_mux_t* mux = this->at(i);
    regs.push_back(new reg_t(mux->sel(), mux->num_sel-1, 0));
    // regs.push_back(new reg_t(mux->next_sel(), mux->num_sel-1, 0));
  }

  return regs;
}

/*
class receive_muxes_t : public vector_t<receive_mux_t*> {
  bool contains(transaction_t* other_receive);
  void add(transaction_t* other_receive, vector_t<port_ref_t*> ports);
};
*/

bool receive_muxes_t::contains(transaction_t* other_receive)
{
  uint32_t i;
  for(i=0; i<this->size(); i++) {
    receive_mux_t* next = this->at(i);
    if (next->maps(other_receive)) {
      return true;
    }
  }
  return false;
}

void receive_muxes_t::add(transaction_t* other_receive, vector_t<port_ref_t*> ports)
{
  assert(this->contains(other_receive));

  uint32_t i;
  for(i=0; i<this->size(); i++) {
    receive_mux_t* next = this->at(i);
    if (next->maps(other_receive)) {
      next->add(ports);
      return;
    }
  }

  fprintf(stderr, "Should never get here\n");
  assert(false);
}

vector_t<reg_t*> receive_muxes_t::registers()
{
  vector_t<reg_t*> regs;

  uint32_t i;
  for(i=0; i<this->size(); i++) {
    receive_mux_t* mux = this->at(i);
    regs.push_back(new reg_t(mux->sel(), mux->num_sel-1, 0));
    // regs.push_back(new reg_t(mux->next_sel(), mux->num_sel-1, 0));
  }

  return regs;
}

/*
class blocks_t : public vector_t<block_t*> {
  public:
  bool contains(std::string name);
  void add();
};
*/

bool blocks_t::contains(string name)
{
  uint32_t i;
  for(i=0; i<this->size(); i++) {
    if(this->at(i)->name == name) {
      return true;
    }
  }
  return false;
}

void blocks_t::add()
{
  fprintf(stderr, "Not implemented yet\n");
  assert(false);
}

vector_t<reg_t*> blocks_t::registers()
{
  vector_t<reg_t*> regs;

  uint32_t i;
  for(i=0; i<this->size(); i++) {
    block_t* block = this->at(i);
    regs.push_back(new reg_t(block->push(), 0, 0));
    regs.push_back(new reg_t(block->pop(), 0, 0));
    // regs.push_back(new reg_t(block->next_push(), 0, 0));
    // regs.push_back(new reg_t(block->next_pop(), 0, 0));
  }

  return regs;
}

/*
class reg_t {
  public:
  std::string name;
  uint32_t msb;
  uint32_t lsb;

  reg_t(std::string name, uint32_t msb, uint32_t lsb);
  std::string to_verilog();
};
*/

reg_t::reg_t(std::string name, uint32_t msb, uint32_t lsb)
{
  this->name = name;
  this->msb = msb;
  this->lsb = lsb;
}

string reg_t::to_verilog()
{
  return this->name;
}










