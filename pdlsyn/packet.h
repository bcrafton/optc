
#ifndef PACKET_H
#define PACKET_H

#include "defines.h"
#include "common.h"

#include "value.h"
#include "parameter.h"

/*
So for this:

subpacket flitheader flitheader0

we need two names.
we cud return {string name, packet_t packet}
and have the packet keep 3 lists.
1. packet names:pointer map
2. field names:pointer map
3. a mixed list

or we just create a third class.

we are gonna create the third class.
*/

/////////////
// class declarations
/////////////

class packet_contents_t;
class packet_inst_t;
class subpacket_t;
class field_mapping_t;
class packet_t;
class packet_condition_t;
class packet_branch_t;
class packet_edge_t;
class packet_condition_t;
class name_space_t;
class packet_paths_t;

/**
 * the two different types of packet field values
 */

typedef enum packet_prim_type {
  PACKET_PRIM_DATA,
  PACKET_PRIM_CONTROL
} packet_prim_type_t;

/////////////
//class definitions.
/////////////

/**
 * packet contents is our abstract class which both field mapping and packet inst inherit. it allows us to keep a list of both types and manipulate them the same way.
 */

class packet_contents_t {
  public:
  // constructor
  // virtual subpacket_t();

  // destructor
  virtual ~packet_contents_t();

  // class functions
  virtual uint32_t size();
  virtual std::pair<field_mapping_t*, uint32_t>* at(uint32_t bit);
  virtual std::pair<packet_edge_t*, vector_t<packet_edge_t*>*>* paths();
  virtual name_space_t name_space();
  virtual packet_contents_t* copy();
  virtual void prepend_name(std::string name);

  // helper functions
  virtual void print();
  virtual std::string to_string();
};

/**
 * the instantiation of a packet inside another packet.
 */

class packet_inst_t : public packet_contents_t {
  public:
  std::string name;
  // dont think we need to copy this
  // just have a pointer.
  packet_t* packet;
  // args to replace the variables inside the packet.
  vector_t<argument_t*> args;

  // constructor
  packet_inst_t(std::string name, packet_t* packet);
  packet_inst_t(std::string name, packet_t* packet, vector_t<argument_t*> args);

  // destructor
  ~packet_inst_t();

  // class functions
  uint32_t size();
  std::pair<field_mapping_t*, uint32_t>* at(uint32_t bit);
  std::pair<packet_edge_t*, vector_t<packet_edge_t*>*>* paths();  
  name_space_t name_space();
  packet_contents_t* copy();
  void prepend_name(std::string name);

  // helper functions
  void print();
  std::string to_string();
};

/**
 * a single field inside of a packet
 */

class field_mapping_t : public packet_contents_t {
  public:
  packet_prim_type_t type;
  std::string field;
  uint32_t bit_size;
  // we are gonna role with this idea for flexibility.
  vector_t<parameter_t*> params;

  // constructor
  field_mapping_t(packet_prim_type_t type, std::string field, uint32_t bit_size);
  field_mapping_t(packet_prim_type_t type, std::string field, uint32_t bit_size, vector_t<parameter_t*> params);

  // destructor
  ~field_mapping_t();

  // class functions
  uint32_t size();
  std::pair<field_mapping_t*, uint32_t>* at(uint32_t bit);
  std::pair<packet_edge_t*, vector_t<packet_edge_t*>*>* paths();
  name_space_t name_space(); 
  packet_contents_t* copy(); 
  void prepend_name(std::string name);

  // helper functions
  void print();
  std::string to_string();
};

/**
 * subpacket is a packet minus the name. the abstraction is used for parsing purposes.
 */

class subpacket_t {
  public:

  /////////////
  // VARIABLEs
  /////////////

  vector_t<packet_contents_t*> packet_contents;

  // packets are trees of other packets.
  vector_t<packet_branch_t*> branches;

  /////////////
  // METHODS
  /////////////

  // constructor
  subpacket_t();
  subpacket_t(vector_t<packet_contents_t*> packet_contents, vector_t<packet_branch_t*> branches);

  // destructor
  ~subpacket_t();

  // class functions
  uint32_t size();
  std::pair<field_mapping_t*, uint32_t>* at(uint32_t bit);
  std::pair<packet_edge_t*, vector_t<packet_edge_t*>*>* paths();
  name_space_t name_space();
  subpacket_t* copy();

  // helper functions
  void print();
  std::string to_string();
};

/**
 * the packet defintion class
 */

class packet_t : public subpacket_t {
  public:

  /////////////
  // VARIABLEs
  /////////////

  // the type
  std::string type;
  // arguments
  vector_t<std::string> args;

  // the header/contents of our packet
  // vector_t<packet_contents_t*> packet_contents;

  // packets are trees of other packets.
  // vector_t<subpacket_t*> branches;

  /////////////
  // METHODS
  /////////////

  // constructor
  packet_t(std::string type, vector_t<packet_contents_t*> packet_contents, vector_t<packet_branch_t*> branches);
  packet_t(std::string type, vector_t<std::string> args, vector_t<packet_contents_t*> packet_contents, vector_t<packet_branch_t*> branches);

  // destructor
  ~packet_t();

  // class functions
  packet_t* copy();

  // helper functions
  void print();
  std::string to_string();
};

/**
 * when a packet branches it contains a condition and a sub packet. this is packet branch.
 */

class packet_branch_t {
  public:
  std::pair<bool, vector_t<packet_condition_t*>*> condition; 
  subpacket_t* packet;
  // this is for creating new namespaces for branches.
  std::string namespace_id;

  // packet_branch_t(subpacket_t* packet);
  // packet_branch_t(vector_t<packet_condition_t*>* conditon, subpacket_t* packet);

  packet_branch_t(subpacket_t* packet, std::string namespace_id);
  packet_branch_t(vector_t<packet_condition_t*>* conditon, subpacket_t* packet, std::string namespace_id);

  std::pair<packet_edge_t*, vector_t<packet_edge_t*>*>* paths();
  name_space_t name_space();

  packet_branch_t* copy();

  void prepend_name();

  std::string to_string();
  void print();
};

/**
 * packet edge is used for packet paths. we turn everything into packet edges
 */

class packet_edge_t {
  public:
  std::pair<bool, vector_t<packet_condition_t*>*> condition; 
  field_mapping_t field;
  vector_t<packet_edge_t*> branches;

  // constructor
  packet_edge_t(vector_t<packet_condition_t*>* conditon, field_mapping_t field);
  packet_edge_t(field_mapping_t field);

  // class functions
  packet_edge_t* copy();
  void set(vector_t<argument_t*> args);
  void prepend_name(std::string name);

  // helper functions
  void print();
  std::string to_string();
};

/**
 * packet condition is used in packet branches for the condition
 */

class packet_condition_t {
  public:
  value_t val;
  std::string name;

  // constructor
  packet_condition_t(std::string name, value_t val);

  // class functions
  packet_condition_t* copy();
};

/**
 * name space is the class that keeps track of the name space and references inside of the packet.
 */

class name_space_t : public map_t<std::string, std::pair<uint32_t, uint32_t>> {
  public:
  std::string to_string();
  void print();
  
  name_space_t prepend(std::string name_space);
  name_space_t offset(uint32_t value);
};

#endif


















