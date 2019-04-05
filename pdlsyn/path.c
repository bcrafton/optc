
#include "path.h"

using namespace std;
using namespace boost;

// PATHS

void paths_t::add(eid_t eid, edge_t* edge)
{
  int i;
  for(i=0; i<this->paths.size(); i++)
  {
    this->paths.at(i)->add(eid, edge);
  }
}

void paths_t::add(path_t* other_path)
{
  this->paths.push_back(other_path);
}

void paths_t::add(paths_t* other_paths)
{
  int i;
  for(i=0; i<other_paths->size(); i++) {
    this->add(other_paths->at(i));
  }
}

path_t* paths_t::at(uint32_t index)
{
  return this->paths.at(index);
}

uint32_t paths_t::size()
{
  return this->paths.size();
}

paths_t* paths_t::copy()
{
  paths_t* cpy = new paths_t();

  uint32_t i; 
  for(i=0; i<paths.size(); i++) {
    cpy->paths.push_back(paths.at(i)->copy());
  }

  return cpy;
}

void paths_t::print()
{
  int i;
  for(i=0; i<this->size(); i++) {
    this->at(i)->print_path();
    printf("\n");
  }
}

void paths_t::print_transactions()
{
  int i;
  for(i=0; i<this->size(); i++) {
    this->at(i)->print_transactions();
    printf("\n");
  }
}

// PATH

path_t::path_t()
{
}

path_t::path_t(path_t* path)
{
  int i;
  for(i=0; i<path->num_cycles(); i++) {
    pair<eid_t, edge_t*> next = path->edge_list.at(i);
    pair<eid_t, edge_t*> next_cpy = pair<eid_t, edge_t*>(next.first, next.second->copy());
    this->edge_list.push_back(next_cpy);
  }
}

path_t::~path_t()
{
  uint32_t i;
  for(i=0; i<edge_list.size(); i++) {
    pair<eid_t, edge_t*> next = edge_list.at(i);
    delete next.second;
  }
  for (id_set_type::iterator itr=this->id_set.begin(); itr!=this->id_set.end(); ++itr) {
    pair<std::string, uint32_t>* ptr = *itr;
    delete ptr;
  }
  for (dep_set_type::iterator itr=this->dep_set.begin(); itr!=this->dep_set.end(); ++itr) {
    pair<std::string, uint32_t>* ptr = *itr;
    delete ptr;
  }
}

void path_t::add(eid_t eid, edge_t* edge)
{
  pair<eid_t, edge_t*> trans(eid, edge);
  this->edge_list.insert(this->edge_list.begin(), trans);
}

void path_t::add(path_t* other_path)
{
  int i;
  for(i=0; i<other_path->num_cycles(); i++) {
    pair<eid_t, edge_t*> next = other_path->edge_list.at(i);
    pair<eid_t, edge_t*> next_cpy = pair<eid_t, edge_t*>(next.first, next.second->copy());
    this->edge_list.push_back(next_cpy);
  }
}

pair<eid_t, edge_t*> path_t::edge_at(uint32_t index)
{
  assert(index < this->num_cycles());
  return this->edge_list[index];
}

path_t* path_t::copy()
{
  path_t* cpy = new path_t();

  uint32_t i;
  for(i=0; i<edge_list.size(); i++) {
    pair<eid_t, edge_t*> next = edge_list.at(i);
    pair<eid_t, edge_t*> next_cpy = pair<eid_t, edge_t*>(next.first, next.second->copy());
    cpy->edge_list.push_back(next_cpy);
  }

  return cpy;
}

void path_t::set_id()
{
  uint32_t i, j, k;
  for(i=0; i<this->edge_list.size(); i++) {
    edge_t* next = this->edge_list.at(i).second;

    for(j=0; j<next->sends.size(); j++) {
      transaction_t* send = next->sends.at(j);

      string send_name;
      if (send->heirarchy.first) {
        for(k=0; k<send->heirarchy.second.size(); k++) {
          send_name += send->heirarchy.second.at(k) + ".";
        }
      }
      send_name += send->get_name();

      for(k=0; k<send->body.params.size(); k++) {
        parameter_t* param = send->body.params.at(k);
        if (param->type() == PARAM_ID) {
          parameter_id_t* param_id = (parameter_id_t*) param;

          pair<string, uint32_t>* new_id = new pair<string, uint32_t>(send_name, param_id->id);
          // assert(!id_set.contains(new_id));

          // BUG should not do this.
          if ( !id_set.contains(new_id) ) {
            this->id_set.put(new_id);
          } else {
            delete new_id;
          }
        }
      }
    }

    for(j=0; j<next->receives.size(); j++) {
      transaction_t* rec = next->receives.at(j);

      string rec_name;
      if (rec->heirarchy.first) {
        for(k=0; k<rec->heirarchy.second.size(); k++) {
          rec_name += rec->heirarchy.second.at(k) + ".";
        }
      }
      rec_name += rec->get_name();

      for(k=0; k<rec->body.params.size(); k++) {
        parameter_t* param = rec->body.params.at(k);
        if (param->type() == PARAM_ID) {
          parameter_id_t* param_id = (parameter_id_t*) param;

          pair<string, uint32_t>* new_id = new pair<string, uint32_t>(rec_name, param_id->id);
          // assert(!id_set.contains(new_id));

          // BUG should not do this.
          if ( !id_set.contains(new_id) ) {
            this->id_set.put(new_id);
          } else {
            delete new_id;
          }
        }
      }
    }
  }

/*
  string id_string;
  for (id_set_type::iterator itr=id_set.begin(); itr!=id_set.end(); ++itr) {
    string n = (*itr)->first;
    id_string += n;
    id_string += "\n";
  }
  id_string += "\n";
  cout << id_string << endl;
*/

}

void path_t::set_dep()
{
  uint32_t i, j, k;
  for(i=0; i<this->edge_list.size(); i++) {
    edge_t* next = this->edge_list.at(i).second;

    for(j=0; j<next->sends.size(); j++) {
      transaction_t* send = next->sends.at(j);

      for(k=0; k<send->body.params.size(); k++) {
        parameter_t* param = send->body.params.at(k);
        if (param->type() == PARAM_DEP) {
          parameter_dep_t* param_dep = (parameter_dep_t*) param;
          pair<string, uint32_t>* new_key = new pair<string, uint32_t>(param_dep->dep_name, param_dep->dep_id);
          if ( !this->dep_set.contains(new_key) ) {
            this->dep_set.put(new_key);
          } else {
            delete new_key;
          }
        }
      }
    }

    for(j=0; j<next->receives.size(); j++) {
      transaction_t* rec = next->receives.at(j);

      for(k=0; k<rec->body.params.size(); k++) {
        parameter_t* param = rec->body.params.at(k);
        if (param->type() == PARAM_DEP) {
          parameter_dep_t* param_dep = (parameter_dep_t*) param;
          pair<string, uint32_t>* new_key = new pair<string, uint32_t>(param_dep->dep_name, param_dep->dep_id);
          if ( !this->dep_set.contains(new_key) ) {
            this->dep_set.put(new_key);
          } else {
            delete new_key;
          }
        }
      }
    }
  }

/*
  uint32_t count = 0;
  string dep_string;
  for (dep_set_type::iterator itr=dep_set.begin(); itr!=dep_set.end(); ++itr) {
    dep_string += std::to_string( (unsigned long long int) count);
    dep_string += " ";
    dep_string += (*itr)->first;
    dep_string += " ";
    dep_string += std::to_string( (unsigned long long int) (*itr)->second);
    dep_string += "\n";
    count++;
  }
  dep_string += "--------\n";
  cout << dep_string;
*/

}

uint32_t path_t::num_sends()
{
  uint32_t sum = 0;
  uint32_t i;
  for(i=0; i<this->num_cycles(); i++) {
    edge_t* edge = this->edge_list[i].second;
    sum += edge->sends.size();
  }
  return sum;
}

uint32_t path_t::num_receives()
{
  uint32_t sum = 0;
  uint32_t i;
  for(i=0; i<this->num_cycles(); i++) {
    edge_t* edge = this->edge_list[i].second;
    sum += edge->receives.size();
  }
  return sum;
}

uint32_t path_t::num_cycles()
{
  return this->edge_list.size();
}

pair<uint32_t, float> path_t::value()
{
  uint32_t cycles = this->num_cycles();
  float data_sum = 0.0;

  uint32_t i, j;
  for(i=0; i<this->num_cycles(); i++) {
    edge_t* edge = this->edge_list[i].second;
    
    for(j=0; j<edge->sends.size(); j++) {
      transaction_t* send = edge->sends.at(j);
      data_sum += send->value();
    }
    for(j=0; j<edge->receives.size(); j++) {
      transaction_t* rec = edge->receives.at(j);
      data_sum += rec->value();
    }
  }
  
  // this->print_transactions();
  // printf("\n%d %f\n", cycles, data_sum);

  return pair<uint32_t, float>(cycles, data_sum);
}

string path_t::to_string_path() 
{
  string path_string;

  int i;
  for(i=0; i<this->num_cycles(); i++) {
    eid_t eid = this->edge_at(i).first;
    if(i==0) {
      path_string += std::to_string( (long long int) eid.m_source);
      path_string += "->";
      path_string += std::to_string( (long long int) eid.m_target);
      path_string += "->";
    }
    else if(i == this->num_cycles()-1) {
      path_string += std::to_string( (long long int) eid.m_target);
    }
    else {
      path_string += std::to_string( (long long int) eid.m_target);
      path_string += "->";
    }
  }

  return path_string;
}

void path_t::print_path()
{
  printf("%s", this->to_string_path().c_str());
}

string path_t::to_string_transactions()
{
  string path_transactions;

  int i, j;
  for(i=0; i<this->num_cycles(); i++) {
    path_transactions += this->edge_list[i].second->to_string();
    path_transactions += " ";
  }

  return path_transactions;
}

void path_t::print_transactions()
{
  printf("%s", this->to_string_transactions().c_str());
}

////////////////////////////////
/////////// path set ///////////
////////////////////////////////

/*
class path_set_t {
  public:
  uint32_t size;
  path_t* paths;

  path_set_t(uint32_t size);

  void set(uint32_t index, path_t path);
  path_t* get(uint32_t index);
};
*/

path_set_t::path_set_t(uint32_t size) 
{
  this->size = size;  
  this->paths = new path_t*[size];
  this->valid = new bool[size];

  uint32_t i;
  for(i=0; i<size; i++) {
    // let this be NULL
    // this->paths[i] = new path_t();
    this->paths[i] = NULL;
  }

  for(i=0; i<size; i++) {
    this->valid[i] = false;
  }
}

path_set_t::path_set_t(path_set_t* pset1, path_set_t* pset2)
{
  this->size = pset1->size + pset2->size;
  this->paths = new path_t*[this->size];
  this->valid = new bool[this->size];

  uint32_t i;

  for(i=0; i<pset1->size; i++) {
    if(pset1->has(i)) {
      this->paths[i] = pset1->at(i)->copy();
      this->valid[i] = true;
    }
    else {
      this->paths[i] = NULL;
      this->valid[i] = false;
    }
  }

  for(i=0; i<pset2->size; i++) {
    if(pset2->has(i)) {
      this->paths[pset1->size + i] = pset2->at(i)->copy();
      this->valid[pset1->size + i] = true;
    }
    else {
      this->paths[pset1->size + i] = NULL;
      this->valid[pset1->size + i] = false;
    }
  }
}

path_set_t::~path_set_t()
{
  uint32_t i;
  for(i=0; i<this->size; i++) {
    if (this->has(i)) {
      delete (this->paths[i]);
    }
  }
  delete[] this->paths;
  delete[] this->valid;
}

bool path_set_t::has(uint32_t index)
{
  if( !(index < this->size) )
  {
    fprintf(stderr, "Index !< size: %u %u\n", index, this->size);
    assert(index < this->size);
  }
  return this->valid[index];
}

void path_set_t::add(path_set_t* other_pset)
{
  if (other_pset->size != this->size) {
    fprintf(stderr, "%u %u\n", other_pset->size, this->size);
    assert(other_pset->size == this->size);
  }

  // we copy this in the path add func
  // other_pset = other_pset->copy();

  uint32_t i;
  for(i=0; i<other_pset->size; i++) {
    if ( other_pset->has(i) ) {
      if ( !this->has(i) ) {
        this->set(i, new path_t());
      }
      this->at(i)->add(other_pset->at(i));
    }
  }
}

void path_set_t::set(uint32_t index, path_t* path)
{
  this->paths[index] = path;
  this->valid[index] = true;
}

path_t* path_set_t::at(uint32_t index)
{
  if( !(index < this->size) )
  {
    fprintf(stderr, "Index !< size: %u %u\n", index, this->size);
    assert(index < this->size);
  }
  assert(this->valid[index]);

  return this->paths[index];
}

bool path_set_t::contains(path_set_t* other_pset, connections_t* connections)
{
  parameter_sum_t sum;
  
  uint32_t i, j;

  for(i=0; i<this->size; i++) {
    if (this->has(i)) {
      path_t* p = this->at(i);

      for(j=0; j<p->edge_list.size(); j++) {
        pair<eid_t, edge_t*> next = p->edge_list.at(j);
        bool ret = sum.add(next.second, connections);
        if (!ret) return false;
      }
    }
  }

  for(i=0; i<other_pset->size; i++) {
    if (other_pset->has(i)) {
      path_t* p = other_pset->at(i);

      for(j=0; j<p->edge_list.size(); j++) {
        pair<eid_t, edge_t*> next = p->edge_list.at(j);
        bool ret = sum.subtract(next.second, connections);
        if (!ret) return false;
        // else sum.print();
      }
    }
  }

  return sum.pos();
}

bool path_set_t::equals(path_set_t* other_pset, connections_t* connections)
{
  parameter_sum_t sum;
  
  uint32_t i, j;

  for(i=0; i<this->size; i++) {
    if (this->has(i)) {
      path_t* p = this->at(i);

      for(j=0; j<p->edge_list.size(); j++) {
        pair<eid_t, edge_t*> next = p->edge_list.at(j);
        bool ret = sum.add(next.second, connections);
        if (!ret) return false;
      }
    }
  }

  for(i=0; i<other_pset->size; i++) {
    if (other_pset->has(i)) {
      path_t* p = other_pset->at(i);

      for(j=0; j<p->edge_list.size(); j++) {
        pair<eid_t, edge_t*> next = p->edge_list.at(j);
        bool ret = sum.subtract(next.second, connections);
        if (!ret) return false;
        // else sum.print();
      }
    }
  }

  return sum.zero();
}

float path_set_t::value()
{
  uint32_t cycle_sum=0;
  float data_sum=0.0;

  uint32_t i;
  for(i=0; i<this->size; i++) {
    if (this->has(i)) {
      pair<uint32_t, float> curr_val = this->at(i)->value();
      cycle_sum += curr_val.first;
      data_sum += curr_val.second;
    }
  }

  // printf("%d %f\n", cycle_sum, data_sum);

  return (data_sum / cycle_sum);
}

path_set_t* path_set_t::accum()
{
  path_set_t* new_accum = new path_set_t(this->size);

  uint32_t i;
  for(i=0; i<this->size; i++) {
    new_accum->set(i, new path_t());
  }

  return new_accum;
}

path_set_t* path_set_t::copy()
{
  path_set_t* pset = new path_set_t(this->size);

  uint32_t i;
  for(i=0; i<this->size; i++) {
    if (this->has(i)) {
      pset->set(i, this->at(i)->copy());
    }
  }

  return pset;
}

string path_set_t::to_string_path()
{
  string path_set_string;

  uint32_t i;
  for(i=0; i<this->size; i++) {
    if (this->has(i)) {
      path_set_string += std::to_string( (long long int) i);
      path_set_string += ": ";
      path_set_string += this->at(i)->to_string_path();
      path_set_string += "\n";
    }
  }

  return path_set_string;
}

string path_set_t::to_string_transactions()
{
  string path_set_string;

  uint32_t i;
  for(i=0; i<this->size; i++) {
    if (this->has(i)) {
      path_set_string += std::to_string( (long long int) i);
      path_set_string += ": ";
      path_set_string += this->at(i)->to_string_transactions();
      path_set_string += "\n";
    }
  }

  return path_set_string;
}

void path_set_t::print_path()
{
  printf("%s", this->to_string_path().c_str());
}

void path_set_t::print_transactions()
{
  printf("%s", this->to_string_transactions().c_str());
}








