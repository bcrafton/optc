
#ifndef COMMON_STL_H
#define COMMON_STL_H

#include "defines.h"

////////////////////////////////
/////////////////////////////
/// MAP-COMP ///
/////////////////////////////
////////////////////////////////

/**
 * a generic map with a comparator
 */

template <typename KEY, typename VALUE, typename COMP>
class map_comp_t {
  public:
  std::map<KEY, VALUE, COMP> table;

  bool contains(KEY key);

  VALUE get(KEY key);

  void put(KEY key, VALUE value);
  void put(map_comp_t<KEY, VALUE, COMP> m);

  void set(KEY key, VALUE value);

  typename std::map<KEY, VALUE, COMP>::iterator begin();

  typename std::map<KEY, VALUE, COMP>::iterator end();
};

template <typename KEY, typename VALUE, typename COMP>
bool map_comp_t<KEY, VALUE, COMP>::contains(KEY key)
{
  typename std::map<KEY, VALUE, COMP>::iterator it = this->table.find(key);
  return (it != this->table.end());
}

template <typename KEY, typename VALUE, typename COMP>
VALUE map_comp_t<KEY, VALUE, COMP>::get(KEY key)
{
  assert( this->contains(key) );
  return this->table[key];
}

template <typename KEY, typename VALUE, typename COMP>
void map_comp_t<KEY, VALUE, COMP>::put(KEY key, VALUE value)
{
  assert( !this->contains(key) );
  std::pair<KEY, VALUE> new_mapping(key, value);
  this->table.insert(new_mapping); 
}

template <typename KEY, typename VALUE, typename COMP>
void map_comp_t<KEY, VALUE, COMP>::put( map_comp_t<KEY, VALUE, COMP> m )
{
  typename std::map<KEY, VALUE, COMP>::iterator itr;
  typename std::map<KEY, VALUE, COMP>::iterator itr_begin = m.begin();
  typename std::map<KEY, VALUE, COMP>::iterator itr_end = m.end();
  for (itr=itr_begin; itr!=itr_end; ++itr) {
    this->put(itr->first, itr->second);
  }
}

template <typename KEY, typename VALUE, typename COMP>
void map_comp_t<KEY, VALUE, COMP>::set(KEY key, VALUE value)
{
  assert( this->contains(key) );
  this->table.erase(key);
  std::pair<KEY, VALUE> new_mapping(key, value);
  this->table.insert(new_mapping); 
}

template <typename KEY, typename VALUE, typename COMP>
typename std::map<KEY, VALUE, COMP>::iterator map_comp_t<KEY, VALUE, COMP>::begin()
{
  return this->table.begin();
}

template <typename KEY, typename VALUE, typename COMP>
typename std::map<KEY, VALUE, COMP>::iterator map_comp_t<KEY, VALUE, COMP>::end()
{
  return this->table.end();
}

////////////////////////////////
/////////////////////////////
/// MAP ///
/////////////////////////////
////////////////////////////////

/**
 * a generic map without a comparator
 */

template <typename KEY, typename VALUE>
class map_t {
  public:
  std::map<KEY, VALUE> table;

  bool contains(KEY key);

  VALUE get(KEY key);

  void put(KEY key, VALUE value);
  void put(map_t<KEY, VALUE> m);

  void set(KEY key, VALUE value);

  typename std::map<KEY, VALUE>::iterator begin();

  typename std::map<KEY, VALUE>::iterator end();
};

template <typename KEY, typename VALUE>
bool map_t<KEY, VALUE>::contains(KEY key)
{
  typename std::map<KEY, VALUE>::iterator it = this->table.find(key);
  return (it != this->table.end());
}

template <typename KEY, typename VALUE>
VALUE map_t<KEY, VALUE>::get(KEY key)
{
  assert( this->contains(key) );
  return this->table[key];
}

template <typename KEY, typename VALUE>
void map_t<KEY, VALUE>::put(KEY key, VALUE value)
{
  assert( !this->contains(key) );
  std::pair<KEY, VALUE> new_mapping(key, value);
  this->table.insert(new_mapping); 
}

template <typename KEY, typename VALUE>
void map_t<KEY, VALUE>::put( map_t<KEY, VALUE> m )
{
  typename std::map<KEY, VALUE>::iterator itr;
  typename std::map<KEY, VALUE>::iterator itr_begin = m.begin();
  typename std::map<KEY, VALUE>::iterator itr_end = m.end();
  for (itr=itr_begin; itr!=itr_end; ++itr) {
    this->put(itr->first, itr->second);
  }
}

template <typename KEY, typename VALUE>
void map_t<KEY, VALUE>::set(KEY key, VALUE value)
{
  assert( this->contains(key) );
  this->table.erase(key);
  std::pair<KEY, VALUE> new_mapping(key, value);
  this->table.insert(new_mapping); 
}

template <typename KEY, typename VALUE>
typename std::map<KEY, VALUE>::iterator map_t<KEY, VALUE>::begin()
{
  return this->table.begin();
}

template <typename KEY, typename VALUE>
typename std::map<KEY, VALUE>::iterator map_t<KEY, VALUE>::end()
{
  return this->table.end();
}

////////////////////////////////
////////////////////////////////
/// VECTOR ///
////////////////////////////////
////////////////////////////////

/**
 * a generic vector
 */

template <typename VALUE>
class vector_t {
  private:
  std::vector<VALUE> v;

  public:
  vector_t();
  vector_t(uint32_t size);
  vector_t(uint32_t size, VALUE init_value);

  VALUE at(uint32_t index);
  uint32_t size();
  void push_back(VALUE val);
  void push_front(VALUE val);

  void push_back(vector_t<VALUE> val);
  void push_back(vector_t<VALUE>* val);

  typename std::vector<VALUE>::iterator begin();
  typename std::vector<VALUE>::iterator end();
};

template <typename VALUE>
vector_t<VALUE>::vector_t()
{
}

template <typename VALUE>
vector_t<VALUE>::vector_t(uint32_t size) : v(size)
{
}

template <typename VALUE>
vector_t<VALUE>::vector_t(uint32_t size, VALUE init_value) : v(size, init_value)
{
}

template <typename VALUE>
VALUE vector_t<VALUE>::at(uint32_t index)
{
  if ( index >= this->v.size() ) {
    fprintf(stderr, "Index out of range: %u %lu\n", index, this->v.size());
    assert( !(index >= this->v.size()) );
  }
  return this->v[index];
}

template <typename VALUE>
uint32_t vector_t<VALUE>::size()
{
  return this->v.size();
}

template <typename VALUE>
void vector_t<VALUE>::push_back(VALUE val)
{
  this->v.push_back(val);
}

template <typename VALUE>
void vector_t<VALUE>::push_front(VALUE val)
{
  this->v.insert(this->v.begin(), val);
}

template <typename VALUE>
typename std::vector<VALUE>::iterator vector_t<VALUE>::begin()
{
  return this->v.begin();
}

template <typename VALUE>
typename std::vector<VALUE>::iterator vector_t<VALUE>::end()
{
  return this->v.end();
}

template <typename VALUE>
void vector_t<VALUE>::push_back(vector_t<VALUE> val)
{
  this->v.insert(this->v.end(), val.begin(), val.end());
}

template <typename VALUE>
void vector_t<VALUE>::push_back(vector_t<VALUE>* val)
{
  this->v.insert(this->v.end(), val->begin(), val->end());
}

////////////////////////////////
////////////////////////////////
/// SET-COMP ///
////////////////////////////////
////////////////////////////////

/**
 * a generic set with a comparator
 */

template <typename KEY, typename COMP>
class set_comp_t {
  public:
  std::set<KEY, COMP> s;

  bool contains(KEY key);
  bool contains(set_comp_t<KEY, COMP> other_set);

  bool equals(set_comp_t<KEY, COMP> other_set);

  void put(KEY key);
  void put(set_comp_t<KEY, COMP> other_set);

  uint32_t size();

  typename std::set<KEY, COMP>::iterator begin();
  typename std::set<KEY, COMP>::iterator end();
};

template <typename KEY, typename COMP>
bool set_comp_t<KEY, COMP>::contains(KEY key)
{
  typename std::set<KEY, COMP>::iterator it = this->s.find(key);
  return (it != this->s.end());
}

template <typename KEY, typename COMP>
bool set_comp_t<KEY, COMP>::contains( set_comp_t<KEY, COMP> other_set )
{
  if ( (this->size() == 0) || (other_set.size() == 0) ) {
    return false;
  }

  typename std::set<KEY, COMP>::iterator itr;
  typename std::set<KEY, COMP>::iterator itr_begin = other_set.begin();
  typename std::set<KEY, COMP>::iterator itr_end = other_set.end();

  for (itr=itr_begin; itr!=itr_end; ++itr) {
    KEY other_key = *itr;
    if (!this->contains(other_key)) {
      return false;
    }
  }

  return true;
}

template <typename KEY, typename COMP>
bool set_comp_t<KEY, COMP>::equals( set_comp_t<KEY, COMP> other_set )
{
  return (this->contains(other_set) && other_set.contains(*this));
}

template <typename KEY, typename COMP>
void set_comp_t<KEY, COMP>::put(KEY key)
{
  assert( !this->contains(key) );
  this->s.insert(key); 
}

template <typename KEY, typename COMP>
void set_comp_t<KEY, COMP>::put( set_comp_t<KEY, COMP> other_set )
{
  typename std::set<KEY, COMP>::iterator itr;
  typename std::set<KEY, COMP>::iterator itr_begin = other_set.begin();
  typename std::set<KEY, COMP>::iterator itr_end = other_set.end();

  for (itr=itr_begin; itr!=itr_end; ++itr) {
    KEY other_key = *itr;
    this->put(other_key);
  }
}

template <typename KEY, typename COMP>
uint32_t set_comp_t<KEY, COMP>::size()
{
  return this->s.size();
}

template <typename KEY, typename COMP>
typename std::set<KEY, COMP>::iterator set_comp_t<KEY, COMP>::begin()
{
  return this->s.begin();
}

template <typename KEY, typename COMP>
typename std::set<KEY, COMP>::iterator set_comp_t<KEY, COMP>::end()
{
  return this->s.end();
}

////////////////////////////////
////////////////////////////////
/// SET ///
////////////////////////////////
////////////////////////////////

/**
 * a generic set without a comparator
 */

template <typename KEY>
class set_t {
  public:
  std::set<KEY> s;

  bool contains(KEY key);
  bool contains(set_t<KEY> other_set);

  bool equals(set_t<KEY> other_set);

  void put(KEY key);
  void put(set_t<KEY> other_set);

  uint32_t size();

  typename std::set<KEY>::iterator begin();
  typename std::set<KEY>::iterator end();
};

template <typename KEY>
bool set_t<KEY>::contains(KEY key)
{
  typename std::set<KEY>::iterator it = this->s.find(key);
  return (it != this->s.end());
}

template <typename KEY>
bool set_t<KEY>::contains( set_t<KEY> other_set )
{
  if ( (this->size() == 0) || (other_set.size() == 0) ) {
    return false;
  }

  typename std::set<KEY>::iterator itr;
  typename std::set<KEY>::iterator itr_begin = other_set.begin();
  typename std::set<KEY>::iterator itr_end = other_set.end();

  for (itr=itr_begin; itr!=itr_end; ++itr) {
    KEY other_key = *itr;
    if (!this->contains(other_key)) {
      return false;
    }
  }

  return true;
}

template <typename KEY>
bool set_t<KEY>::equals( set_t<KEY> other_set )
{
  return (this->contains(other_set) && other_set.contains(*this));
}

template <typename KEY>
void set_t<KEY>::put(KEY key)
{
  assert( !this->contains(key) );
  this->s.insert(key); 
}

template <typename KEY>
void set_t<KEY>::put( set_t<KEY> other_set )
{
  typename std::set<KEY>::iterator itr;
  typename std::set<KEY>::iterator itr_begin = other_set.begin();
  typename std::set<KEY>::iterator itr_end = other_set.end();

  for (itr=itr_begin; itr!=itr_end; ++itr) {
    KEY other_key = *itr;
    this->put(other_key);
  }
}

template <typename KEY>
uint32_t set_t<KEY>::size()
{
  return this->s.size();
}

template <typename KEY>
typename std::set<KEY>::iterator set_t<KEY>::begin()
{
  return this->s.begin();
}

template <typename KEY>
typename std::set<KEY>::iterator set_t<KEY>::end()
{
  return this->s.end();
}

////////////////////////////////
////////////////////////////////
/// MATRIX ///
////////////////////////////////
////////////////////////////////

/**
 * a matrix that is unused in the current code base
 */

template <typename VALUE>
class matrix_t {
  public:
  vector_t< vector_t<VALUE> > mat;
  uint32_t num_rows;
  uint32_t num_cols;

  // constructor 
  matrix_t(uint32_t num_rows = 1, uint32_t num_cols = 1);

  uint32_t rows() const;
  uint32_t cols() const;

  void add_row(vector_t<VALUE>);
  VALUE at(uint32_t i, uint32_t j);
};

template <typename VALUE>
matrix_t<VALUE>::matrix_t(uint32_t num_rows, uint32_t num_cols): num_cols(num_rows), num_cols(num_cols), mat(num_rows, vector_t<VALUE>(num_cols))
{
}

template <typename VALUE>
uint32_t matrix_t<VALUE>::rows() const
{
  return num_rows;
}

template <typename VALUE>
uint32_t matrix_t<VALUE>::cols() const
{
  return num_cols;
}

template <typename VALUE>
void matrix_t<VALUE>::add_row(vector_t<VALUE> row) 
{
  assert(row.size() == this->cols());
  this->mat.push_back(row);
}

template <typename VALUE>
VALUE matrix_t<VALUE>::at(uint32_t i, uint32_t j) 
{
  assert(i < this->rows());
  assert(j < this->cols());

  return this->mat.at(i).at(j);
}

#endif
































