
#ifndef VALUE_H
#define VALUE_H

#include "defines.h"
#include "common.h"

/**
 * two types of values althought only one is used: number
 */

typedef enum value_type {
  VALUE_TYPE_NUMBER,
  VALUE_TYPE_VARIABLE
} value_type_t;

/**
 * value is used in parser inside of conditions and transactions 
 */

class value_t {
  public:
  value_type_t type;
  std::string var;
  uint32_t num;

  value_t();
  value_t(value_type_t type, uint32_t num);
  value_t(value_type_t type, std::string var);
  std::string to_string();
};

/**
 * in PDL we can define a map which maps bit enumerations to actual values, this is that abstract class
 */

class value_map_t {
  private:
  // std::vector<mapping> mappings; // this should probable be a tree/map. oh well for now.
  std::map<uint32_t, uint32_t> mappings;
  // lol these should also be mapped in our "map_list_t"
  std::string*                 map_name;
 
  public:
  uint32_t get(uint32_t key);

  bool contains(uint32_t key);

  void put(uint32_t key, uint32_t value);
  void put(std::pair<uint32_t, uint32_t> new_mapping);

  void set_name(std::string* map_name);
  std::string get_name();

  uint32_t get_max();
  bool value_exists(uint32_t value);
  uint32_t get_key(uint32_t value);

  std::string to_string();
  void print();
};

/**
 * a list of value maps that is used inside of protocol. this is needed to do functions on the set of all maps.
 */

class value_map_list_t {
  private:
  std::vector<value_map_t*> maps;

  public:
  value_map_t* at(uint32_t index);
  uint32_t size();
  void push_back(value_map_t* m);

  std::string to_string();
  void print();

  bool map_exists(std::string map_name);
  value_map_t* get_map(std::string map_name);
};

#endif

