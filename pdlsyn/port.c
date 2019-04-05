
#include "port.h"

using namespace std;
using namespace boost;

// port
port_t::port_t(port_type_t type, port_direction_t dir, uint32_t width, std::string name)
{
  // ideally these would all be sets i think...
  // and use the pair<bool, type>
  this->type = type;
  this->set_dir(dir);
  this->width = width;
  this->set_name(name);
}

port_t::port_t(port_type_t type, port_direction_t dir, uint32_t width, std::string name, std::string packet_type)
{
  // ideally these would all be sets i think...
  // and use the pair<bool, type>
  this->type = type;
  this->set_dir(dir);
  this->width = width;
  this->set_name(name);
  this->packet_type = packet_type;
}

string port_t::to_string()
{
  std::string port_string;

  port_string += std::to_string( (unsigned long long int) this->type);  
  port_string += " ";

  port_string += std::to_string( (unsigned long long int) this->get_dir());  
  port_string += " ";

  port_string += std::to_string( (unsigned long long int) this->width);  
  port_string += " ";

  // printf( "%s\n", (*(this->name)).c_str() );
  port_string += this->get_name();  
  port_string += " ";

  return port_string;
}

void port_t::print()
{
  printf("%s", this->to_string().c_str());
}

// TO VERILOG
string port_t::to_verilog_ports()
{
  std::string port_verilog;

  if (this->get_dir() == PORT_DIR_IN) {
    port_verilog += "input wire ";
  } 
  else if (this->get_dir() == PORT_DIR_OUT) {
    port_verilog += "output wire ";
  }
  else {
    fprintf(stderr, "Invalid Port Type");
    assert(0);
  }

  if (this->width > 1) {
    port_verilog += "[";
    port_verilog += std::to_string( (long long unsigned int) this->width-1 );
    port_verilog += ":0] ";
  }

  port_verilog += this->get_name();

  return port_verilog;
}

// TEST BENCH HEADER
string port_t::test_bench_head()
{
  std::string test_bench_string;

  if (this->get_dir() == PORT_DIR_IN) {
    test_bench_string += "reg ";
  } 
  else if (this->get_dir() == PORT_DIR_OUT) {
    test_bench_string += "wire ";
  }
  else {
    fprintf(stderr, "Invalid Port Type");
    assert(0);
  }

  if (this->width > 1) {
    test_bench_string += "[";
    test_bench_string += std::to_string( (long long unsigned int) this->width-1 );
    test_bench_string += ":0] ";
  }

  test_bench_string += this->get_name();

  return test_bench_string;
}

// TEST BENCH INST
string port_t::test_bench_translator()
{
  std::string test_bench_string;

  test_bench_string += ".";
  test_bench_string += this->get_name(); 
  test_bench_string += "(";
  test_bench_string += this->get_name();
  test_bench_string += ")";

  return test_bench_string;
}

port_t* port_t::copy()
{
  // assert(0);
  // dont need this yet

  /*
  MISTAKE OF THE YEAR. 2 HOURS APPROX

  string name_copy = string( *(this->name) );
  port_t* new_port = new port_t(this->type, this->dir, this->width, &name_copy);

  BASICALLY WE WERE COPYING A STRING AND IT WAS ON STACK.
  THEN WHEN WE POPPED FORM FUNC IT WAS CORRUPTED.
  */
  
  port_t* new_port = new port_t(this->type, this->get_dir(), this->width, this->get_name());
  // AHHH this cost us time as well
  // constructor does not include this did not realize was not copying the protocol name
  new_port->set_protocol_name(this->get_protocol_name());

  return new_port;

}

//port_list_t

port_list_t::port_list_t()
{
}

port_list_t::port_list_t(port_list_t* ports1, port_list_t* ports2)
{
  int i;
  for(i=0; i<ports1->size(); i++) {
    port_t* new_port = ports1->at(i)->copy();

    if (new_port->get_dir() == PORT_DIR_IN) {
      new_port->set_dir(PORT_DIR_OUT);
      this->push_back(new_port);
    }
    else if (new_port->get_dir() == PORT_DIR_OUT) {
      new_port->set_dir(PORT_DIR_IN);
      this->push_back(new_port);
    }
    else {
      fprintf(stderr, "Invalid Port Type");
      assert(0);
    }
  }

  for(i=0; i<ports2->size(); i++) {
    port_t* new_port = ports2->at(i)->copy();

    if (new_port->get_dir() == PORT_DIR_IN) {
      new_port->set_dir(PORT_DIR_OUT);
      this->push_back(new_port);
    }
    else if (new_port->get_dir() == PORT_DIR_OUT) {
      new_port->set_dir(PORT_DIR_IN);
      this->push_back(new_port);
    }
    else {
      fprintf(stderr, "Invalid Port Type");
      assert(0);
    }
  }
}

port_t* port_list_t::at(uint32_t index)
{
  return this->ports[index];
}

uint32_t port_list_t::size()
{
  return this->ports.size();
}

void port_list_t::push_back(port_t* port)
{
/*
  if (port->type == PORT_TYPE_CONTROL && port->get_dir() == PORT_DIR_IN) {
    this->control_in.push_back(port);
  }
  if (port->type == PORT_TYPE_CONTROL && port->get_dir() == PORT_DIR_OUT) {
    this->control_out.push_back(port);
  }
*/
  this->ports.push_back(port);
}

string port_list_t::to_string()
{
  string port_list_string;
  int i;

  for (i=0; i<this->size(); i++) 
  { 
    port_list_string += this->at(i)->to_string();
    port_list_string += "\n";
  }

  return port_list_string;
}

port_list_t* port_list_t::copy()
{
  assert(0);

  port_list_t* new_port_list = new port_list_t();

  int i;
  for(i=0; i<this->size(); i++) {
    port_t* new_port = this->at(i)->copy();
    new_port_list->push_back(new_port);
  }

  return new_port_list;
}

void port_list_t::print()
{
  printf("%s", this->to_string().c_str());
}

// should this be legal without giving protocol name?
port_t* port_list_t::find(string portname)
{
  int i;
  for (i=0; i<this->size(); i++) 
  {
    if (portname == this->ports[i]->get_name())
    {
      return this->ports[i];
    }
  }

  fprintf(stderr, "Could not find port: %s\n", portname.c_str());
  assert(0);

  return NULL;
}

port_t* port_list_t::find(std::pair<std::string, std::string> portname)
{
  int i;
  for (i=0; i<this->size(); i++) 
  {
    // FIXME 
    // lol we cannot return this port for literally everything lol
    // as soon as our loop gets here we return this port.
    // that dosnt explain why this is returning nothing however... or atleast no name
    // this was returning nothing so not sure why.
    if (this->ports[i]->type == PORT_TYPE_PACKET) {
      // return this->ports[i];
      // printf( "%s\n", this->ports[i]->name.c_str() );
    }
    if (portname.first == this->ports[i]->get_protocol_name() && portname.second == this->ports[i]->get_name())
    {
      return this->ports[i];
    }
    // printf("%s %s | %s %s\n", portname.first.c_str(), portname.second.c_str(), this->ports[i]->protocol_name.c_str(), this->ports[i]->name.c_str());
  }

  fprintf(stderr, "Could not find port: %s:%s\n", portname.first.c_str(), portname.second.c_str());
  assert(0);

  return NULL;
}

port_direction_t port_list_t::direction(string portname)
{
  return this->find(portname)->get_dir();
}

// TO VERILOG
string port_list_t::to_verilog_ports()
{
  string port_list_verilog;
  int i;
  for (i=0; i<this->size(); i++) 
  { 
    // printf("%d\n", i);
    port_list_verilog += this->at(i)->to_verilog_ports();
    if (i == this->size()-1) {
      port_list_verilog += "\n";
    }
    else {
      port_list_verilog += ",\n";
    }
  }

  return port_list_verilog;
}

// TEST BENCH HEAD
string port_list_t::test_bench_head()
{
  string test_bench_string;
  int i;
  for (i=0; i<this->size(); i++) 
  { 
    test_bench_string += "  ";
    test_bench_string += this->at(i)->test_bench_head();
    test_bench_string += ";\n";
  }

  return test_bench_string;
}

// TEST BENCH INST 
string port_list_t::test_bench_translator()
{
  string test_bench_string;
  int i;
  for (i=0; i<this->size(); i++) 
  { 
    test_bench_string += "  ";
    test_bench_string += this->at(i)->test_bench_translator();
    if (i == this->size()-1) {
      test_bench_string += "\n";
    }
    else {
      test_bench_string += ",\n";
    }
  }

  return test_bench_string;
}

/*
uint32_t port_list_t::control_in_width()
{
  uint32_t sum = 0;

  int i;
  for(i=0; i<this->size(); i++) {
    if (this->at(i)->type == PORT_TYPE_CONTROL && 
        this->at(i)->get_dir() == PORT_DIR_IN) {
      sum += this->at(i)->width;
    }
  }

  return sum;
}

uint32_t port_list_t::control_out_width()
{
  uint32_t sum = 0;

  int i;
  for(i=0; i<this->size(); i++) {
    if (this->at(i)->type == PORT_TYPE_CONTROL && 
        this->at(i)->get_dir() == PORT_DIR_OUT) {
      sum += this->at(i)->width;
    }
  }

  return sum;
}
*/
