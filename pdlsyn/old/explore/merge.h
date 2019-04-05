
#ifndef MERGE_H
#define MERGE_H

#include "defines.h"
#include "fsm.h"


// no need for this. it is same as old ports list.
// class merged_port_list_t {};
extern std::vector<std::pair<std::string,std::string>>port_map;
struct merge_config_t {
  public:
  bool viz;
  bool verilog;
  std::pair<std::string, std::string> p_file_names;
  std::pair<std::string, std::string> p_names;
  //std::vector< std::pair<std::string, std::string> > map;
};

typedef struct merged_vertex {
  public:
  vertex_t* v1;
  vertex_t* v2;

  std::string getName() {
      std::string name_string;
      name_string = v1->to_string();
      name_string += v2->to_string();
      return name_string;
  }

} merged_vertex_t;

typedef struct merged_edge {
  public:
  edge_t* e1;
  edge_t* e2;

} merged_edge_t;

typedef struct merged_state {
  vid_t s1;
  vid_t s2;
} merged_state_t;

typedef struct merged_transition {
  eid_t t1;
  eid_t t2;
} merged_transition_t;

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, merged_vertex_t, merged_edge_t, boost::no_property> merged_graph_t;

#if (DYNAMIC_ALGORITHM == 0)
typedef boost::property_map<merged_graph_t, boost::vertex_index_t>::type index_map_t;
typedef boost::iterator_property_map<typename std::vector<vid_t>::iterator, index_map_t, vid_t, vid_t&> IsoMap;
#endif

class merged_ports_t {
  public:
  port_list_t* ports1;
  port_list_t* ports2;

  int getNumberOfInputs() {
      return (ports1->getNumberOfInputs() + ports2->getNumberOfInputs());
  }

  int getNumberOfOutputs() {
      return (ports1->getNumberOfOutputs() + ports2->getNumberOfOutputs());
  }

  std::string getInputName(int index) {
      if(index < ports1->getNumberOfInputs()) {
          return ports1->getInputName(index);
      } else {
          return ports2->getInputName(index-(ports1->getNumberOfInputs()));
      }
  }

  std::string getOutputName(int index) {
      if(index < ports1->getNumberOfOutputs()) {
          return ports1->getOutputName(index);
      } else {
          return ports2->getOutputName(index-(ports1->getNumberOfOutputs()));
      }
  }

  int getOutputSize(int index) {
      if(index < ports1->getNumberOfOutputs()) {
          return ports1->getOutputSize(index);
      } else {
          return ports2->getOutputSize(index-(ports1->getNumberOfOutputs()));
      }
  }

  int getInputSize(int index) {
      if(index < ports1->getNumberOfInputs()) {
          return ports1->getInputSize(index);
      } else {
          return ports2->getInputSize(index-(ports1->getNumberOfInputs()));
      }
  }
};

class merged_init_t {
  public:
  state_list_t* init1;
  state_list_t* init2;
};

struct merged_vertex_comparator_t {
    bool operator()(const merged_vertex_t& a, const merged_vertex_t& b) const {
        unsigned long long a_val = (a.v1->vid << 32) | a.v2->vid;
        unsigned long long b_val = (b.v1->vid << 32) | b.v2->vid;
        return a_val < b_val;
    }
};

typedef enum compare_result {
  NOT_EQUAL,
  REPLACE,
  NO_REPLACE
} compare_result_t;

class merged_fsm_t {
  public:
  vid_t root;
  vid_t sink;
  merged_graph_t g;
  std::map<merged_vertex_t, vid_t, merged_vertex_comparator_t> vertex_table;

  vid_t add_root(merged_vertex_t vertex);
  vid_t add_vertex(merged_vertex_t vertex);
  eid_t add_edge(vid_t v1, vid_t v2, merged_edge_t edge);

  eid_t add_fsm_r(vid_t new_root, vid_t old_root, merged_fsm_t* fsm);
  eid_t add_fsm(vid_t v, merged_fsm_t* fsm, merged_edge_t edge);

  bool compare(merged_fsm_t* fsm);
  bool compare_r(merged_state_t current, merged_fsm_t* fsm);

  void print();
  void print_r(vid_t dest);

  std::string to_string();
  std::string to_verilog();
  void to_visualize(std::string filename);

  bool contains(merged_vertex_t key);
  vid_t get(merged_vertex_t key);
  void put(merged_vertex_t key, vid_t value);

  uint32_t size();

};

// we are using just vid_t and eid_t
 typedef merged_graph_t::vertex_descriptor merged_vid_t;
 typedef merged_graph_t::edge_descriptor   merged_eid_t;

class merged_protocol_t {

  private:
  // maybe these need to be structs and not pointers.
  std::string     name;
  merged_ports_t* ports;
  merged_init_t*  init;
  merged_fsm_t*   behavior;
  //std::vector< std::pair<std::string, std::string> > port_map;

  public:
  merged_protocol_t(protocol_t* p1, protocol_t* p2);//, std::vector< std::pair<std::string, std::string> > port_map);

  void merge_ports(port_list_t* ports1, port_list_t*  ports2);
  void merge_init(state_list_t* init1, state_list_t* init2);
  void merge_behavior(fsm_t* behavior1, fsm_t* behavior2);

  void to_visualize(std::string filename);
  std::string to_verilog();
  std::string to_string();
  void print();

  std::string getClockName() {
      return "clk";
  }

  std::string getResetName() {
      return "resetn";
  }

  std::string getName() {
      return name;
  }

  int getNumberOfInputs() {
      return ports->getNumberOfInputs();
  }

  int getNumberOfOutputs() {
      return ports->getNumberOfOutputs();
  }

  std::string getInputName(int index) {
      return ports->getInputName(index);
  }

  std::string getOutputName(int index) {
      return ports->getOutputName(index);
  }

  int getInputSize(int index) {
      return ports->getInputSize(index);
  }

  int getOutputSize(int index) {
      return ports->getOutputSize(index);
  }

  merged_fsm_t* getBehavior(){
      return behavior;
  }

  merged_graph_t getGraph() {
      return behavior->g;
  }

  std::string num2binary(int count, int width) {
    int n = count;
    std::string output;
    for (int i = 0; i < width; i++) {
       if (n) {
           int bit = n%2;
           output += boost::lexical_cast<std::string>(bit);
       } else {
           output += "0";
       }
       n = n/2;
    }
    std::reverse(output.begin(), output.end());
    return output;
  }

  std::string packInputPort(std::vector<std::string> ports, int* val, int* portId, int num) {
      int* size = (int *) malloc(num*sizeof(int));

      int inPorts = getNumberOfInputs();
      int total_edges = num_edges(behavior->g);

      int num_ctrl_ins =  getNumControlInputs();

      for (int p = 0; p < num; p++) {
          size[p] = 0;
          for (int i = 0; i < num_ctrl_ins; i++) {
              if(i == portId[p]) {
                  break;
              }
              int inSize = getControlInputWidth(i);
              size[p] += inSize;
          }
      }

      int width = getWidthControlInputs();
      char *binaryVector = (char*)malloc(width*sizeof(char));

      for (int p = 0; p < width; p++) {
          binaryVector[p] = '?';
      }

      for (int p = 0; p < num; p++) {
          strncpy(&binaryVector[size[p]],num2binary(val[p], getControlInputWidth(portId[p])).c_str(), getControlInputWidth(portId[p]));
      }
      std::string retVector(binaryVector, width);
      free(binaryVector);
      free(size);
      return retVector;
  }

  std::string packOutputPort(std::vector<std::string> ports, int* val, int *portId, int num) {
      int* size = (int *) malloc(num*sizeof(int));

      int outPorts = getNumberOfOutputs();
      int total_edges = num_edges(behavior->g);


      int num_ctrl_outs =  getNumControlOutputs();

      for (int p = 0; p < num; p++) {
          size[p] = 0;
          for (int i = 0; i < num_ctrl_outs; i++) {
              if(i == portId[p]) {
                  break;
              }
              int outSize = getControlOutputWidth(i);
              size[p] += outSize;
          }
      }

      int width = getWidthControlOutputs();
      char *binaryVector = (char*)malloc(width*sizeof(char));

      for (int p = 0; p < width; p++) {
          binaryVector[p] = '?';
      }

      for (int p = 0; p < num; p++) {
          strncpy(&binaryVector[size[p]],num2binary(val[p], getControlOutputWidth(portId[p])).c_str(), getControlOutputWidth(portId[p]));
      }
      std::string retVector(binaryVector, width);
      free(binaryVector);
      free(size);
      return retVector;
  }

  int getWidthControlOutputs() {
      int size = 0;

      int outPorts = getNumberOfOutputs();
      int total_edges = num_edges(behavior->g);
      int found = 0;

      for(int j = 0; j < outPorts; j++) {
          std::pair<merged_graph_t::edge_iterator, merged_graph_t::edge_iterator> range = edges(behavior->g);
          for(merged_graph_t::edge_iterator itr = range.first; itr != range.second; itr++)
          {
            merged_edge_t e = behavior->g[*itr];

            if (e.e1->condition) {
                int c1num = e.e1->condition->conditions->size();
                for (int i = 0; i < c1num; i++) {
                    std::string cCond = *(e.e1->condition->conditions->at(i)->portname);
                    if(cCond.compare(getOutputName(j)) == 0) {
                        size = size + getOutputSize(j);
                        goto over;
                    }
                }
            }

            if (e.e2->condition) {
                int c2num = e.e2->condition->conditions->size();
                for (int i = 0; i < c2num; i++) {
                    std::string cCond = *(e.e2->condition->conditions->at(i)->portname);
                    if(cCond.compare(getOutputName(j)) == 0) {
                        size = size + getOutputSize(j);
                        goto over;
                    }
                }
            }
          }
      over:
          found = 1;
      }
      return size;
  }

  int getWidthControlInputs() {
      int size = 0;

      int inPorts = getNumberOfInputs();
      int total_edges = num_edges(behavior->g);
      int found = 0;

      for(int j = 0; j < inPorts; j++) {
          std::pair<merged_graph_t::edge_iterator, merged_graph_t::edge_iterator> range = edges(behavior->g);
          for(merged_graph_t::edge_iterator itr = range.first; itr != range.second; itr++)
          {
            merged_edge_t e = behavior->g[*itr];

            if (e.e1->condition) {
                int c1num = e.e1->condition->conditions->size();
                for (int i = 0; i < c1num; i++) {
                    std::string cCond = *(e.e1->condition->conditions->at(i)->portname);
                    if(cCond.compare(getInputName(j)) == 0) {
                        size = size + getInputSize(j);
                        goto over;
                    }
                }
            }

            if (e.e2->condition) {
                int c2num = e.e2->condition->conditions->size();
                for (int i = 0; i < c2num; i++) {
                    std::string cCond = *(e.e2->condition->conditions->at(i)->portname);
                    if(cCond.compare(getInputName(j)) == 0) {
                        size = size + getInputSize(j);
                        goto over;
                    }
                }
            }
          }
      over:
          found = 1;
      }
      return size;
  }

  int getNumControlOutputs() {
      int count = 0;

      int outPorts = getNumberOfOutputs();
      int total_edges = num_edges(behavior->g);
      int found = 0;

      for(int j = 0; j < outPorts; j++) {
          std::pair<merged_graph_t::edge_iterator, merged_graph_t::edge_iterator> range = edges(behavior->g);
          for(merged_graph_t::edge_iterator itr = range.first; itr != range.second; itr++)
          {
            merged_edge_t e = behavior->g[*itr];

            if (e.e1->condition) {
                int c1num = e.e1->condition->conditions->size();
                for (int i = 0; i < c1num; i++) {
                    std::string cCond = *(e.e1->condition->conditions->at(i)->portname);
                    if(cCond.compare(getOutputName(j)) == 0) {
                        count++;
                        goto over;
                    }
                }
            }

            if (e.e2->condition) {
                int c2num = e.e2->condition->conditions->size();
                for (int i = 0; i < c2num; i++) {
                    std::string cCond = *(e.e2->condition->conditions->at(i)->portname);
                    if(cCond.compare(getOutputName(j)) == 0) {
                        count++;
                        goto over;
                    }
                }
            }
          }
      over:
          found = 1;
      }
      return count;
  }

  int getNumControlInputs() {
      int count = 0;

      int inPorts = getNumberOfInputs();
      int total_edges = num_edges(behavior->g);
      int found = 0;

      for(int j = 0; j < inPorts; j++) {
          std::pair<merged_graph_t::edge_iterator, merged_graph_t::edge_iterator> range = edges(behavior->g);
          for(merged_graph_t::edge_iterator itr = range.first; itr != range.second; itr++)
          {
            merged_edge_t e = behavior->g[*itr];

            if (e.e1->condition) {
                int c1num = e.e1->condition->conditions->size();
                for (int i = 0; i < c1num; i++) {
                    std::string cCond = *(e.e1->condition->conditions->at(i)->portname);
                    if(cCond.compare(getInputName(j)) == 0) {
                        count++;
                        goto over;
                    }
                }
            }

            if (e.e2->condition) {
                int c2num = e.e2->condition->conditions->size();
                for (int i = 0; i < c2num; i++) {
                    std::string cCond = *(e.e2->condition->conditions->at(i)->portname);
                    if(cCond.compare(getInputName(j)) == 0) {
                        count++;
                        goto over;
                    }
                }
            }
          }
      over:
          found = 1;
      }
      return count;
  }


  std::string getControlInputName(int index) {
      int count = 0;

      int found;
      int inPorts = getNumberOfInputs();
      int total_edges = num_edges(behavior->g);

      for(int j = 0; j < inPorts; j++) {
          std::string inputName = getInputName(j);
          std::pair<merged_graph_t::edge_iterator, merged_graph_t::edge_iterator> range = edges(behavior->g);
          for(merged_graph_t::edge_iterator itr = range.first; itr != range.second; itr++)
          {
            merged_edge_t e = behavior->g[*itr];

            if (e.e1->condition) {
                int c1num = e.e1->condition->conditions->size();
                for (int i = 0; i < c1num; i++) {
                    std::string cCond = *(e.e1->condition->conditions->at(i)->portname);
                    if(cCond.compare(inputName) == 0) {
                        if (count == index) {
                            return cCond;
                        }
                        count++;
                        goto over;
                    }
                }
            }

            if (e.e2->condition) {
                int c2num = e.e2->condition->conditions->size();
                for (int i = 0; i < c2num; i++) {
                    std::string cCond = *(e.e2->condition->conditions->at(i)->portname);
                    if(cCond.compare(inputName) == 0) {
                        if (count == index) {
                            return cCond;
                        }
                        count++;
                        goto over;
                    }
                }
            }
         }
      over:
          found = 1;
      }
      return "";
  }

  int getControlInputWidth(int index) {
      int count = 0;
      int found = 0;

      int inPorts = getNumberOfInputs();
      int total_edges = num_edges(behavior->g);

      for(int j = 0; j < inPorts; j++) {
          std::string inName = getInputName(j);
          std::pair<merged_graph_t::edge_iterator, merged_graph_t::edge_iterator> range = edges(behavior->g);
          for(merged_graph_t::edge_iterator itr = range.first; itr != range.second; itr++)
          {
            merged_edge_t e = behavior->g[*itr];

            if (e.e1->condition) {
                int c1num = e.e1->condition->conditions->size();
                for (int i = 0; i < c1num; i++) {
                    std::string cCond = *(e.e1->condition->conditions->at(i)->portname);
                    if(cCond.compare(inName) == 0) {
                        if (count == index) {
                            return getInputSize(j);
                        }
                        count++;
                        goto over;
                    }
                }
            }

            if (e.e2->condition) {
                int c2num = e.e2->condition->conditions->size();
                for (int i = 0; i < c2num; i++) {
                    std::string cCond = *(e.e2->condition->conditions->at(i)->portname);
                    if(cCond.compare(inName) == 0) {
                        if (count == index) {
                            return getInputSize(j);
                        }
                        count++;
                        goto over;
                    }
                }
            }
          }
      over:
          found = 1;
      }
      return 0;
  }

  int getControlOutputWidth(int index) {
      int count = 0;
      int found = 0;

      int outPorts = getNumberOfOutputs();
      int total_edges = num_edges(behavior->g);

      for(int j = 0; j < outPorts; j++) {
          std::string outName = getOutputName(j);
          std::pair<merged_graph_t::edge_iterator, merged_graph_t::edge_iterator> range = edges(behavior->g);
          for(merged_graph_t::edge_iterator itr = range.first; itr != range.second; itr++)
          {
            merged_edge_t e = behavior->g[*itr];

            if (e.e1->condition) {
                int c1num = e.e1->condition->conditions->size();
                for (int i = 0; i < c1num; i++) {
                    std::string cCond = *(e.e1->condition->conditions->at(i)->portname);
                    if(cCond.compare(outName) == 0) {
                        if (count == index) {
                            return getOutputSize(j);
                        }
                        count++;
                        goto over;
                    }
                }
            }

            if (e.e2->condition) {
                int c2num = e.e2->condition->conditions->size();
                for (int i = 0; i < c2num; i++) {
                    std::string cCond = *(e.e2->condition->conditions->at(i)->portname);
                    if(cCond.compare(outName) == 0) {
                        if (count == index) {
                            return getOutputSize(j);
                        }
                        count++;
                        goto over;
                    }
                }
            }
          }
      over:
          found = 1;
      }
      return 0;
  }

  std::string getControlOutputName(int index) {
      int count = 0;
      int found = 1;

      int outPorts = getNumberOfOutputs();
      int total_edges = num_edges(behavior->g);

      for(int j = 0; j < outPorts; j++) {
          std::string outName = getOutputName(j);
          std::pair<merged_graph_t::edge_iterator, merged_graph_t::edge_iterator> range = edges(behavior->g);
          for(merged_graph_t::edge_iterator itr = range.first; itr != range.second; itr++)
          {
            merged_edge_t e = behavior->g[*itr];

            if (e.e1->condition) {
                int c1num = e.e1->condition->conditions->size();
                for (int i = 0; i < c1num; i++) {
                    std::string cCond = *(e.e1->condition->conditions->at(i)->portname);
                    if(cCond.compare(outName) == 0) {
                        if (count == index) {
                            return cCond;
                        }
                        count++;
                        goto over;
                    }
                }
            }

            if (e.e2->condition) {
                int c2num = e.e2->condition->conditions->size();
                for (int i = 0; i < c2num; i++) {
                    std::string cCond = *(e.e2->condition->conditions->at(i)->portname);
                    if(cCond.compare(outName) == 0) {
                        if (count == index) {
                            return cCond;
                        }
                        count++;
                        goto over;
                    }
                }
            }
          }
      over:
          found = 1;
      }
      assert(0);
      return "";
  }

  std::string getOutputVectorCondition(merged_edge_t e)
  {
      int count = 0;

      int total_conds = getTotalOutputConditions(e);
      int outControlPorts = getNumControlOutputs();
      std::vector<std::string> portname_vector;
      int *val = (int *)malloc(total_conds*sizeof(int));
      int *portId = (int *)malloc(total_conds*sizeof(int));
      int num_conds = 0;

      if (e.e1->condition) {
          int c1num = e.e1->condition->conditions->size();
          for (int i = 0; i < c1num; i++) {
              std::string portname = *(e.e1->condition->conditions->at(i)->portname);
              int value = e.e1->condition->conditions->at(i)->val->num;
              for(int j = 0; j < outControlPorts; j++) {
                  if(portname.compare(getControlOutputName(j)) == 0) {
                      portname_vector.push_back(portname);
                      val[num_conds] = value;
                      portId[num_conds] = j;
                      num_conds++;
                      break;
                  }
              }
          }
      }

      if (e.e2->condition) {
          int c2num = e.e2->condition->conditions->size();
          for (int i = 0; i < c2num; i++) {
              std::string portname = *(e.e2->condition->conditions->at(i)->portname);
              int value = e.e2->condition->conditions->at(i)->val->num;
              for(int j = 0; j < outControlPorts; j++) {
                  if(portname.compare(getControlOutputName(j)) == 0) {
                      portname_vector.push_back(portname);
                      val[num_conds] = value;
                      portId[num_conds] = j;
                      num_conds++;
                      break;
                  }
              }
          }
      }
      if(num_conds != total_conds) {
          assert(0);
      }
      std::string vector = packOutputPort(portname_vector, val, portId, total_conds);
      free(val);
      free(portId);
      return vector;
  }

  std::string getInputVectorCondition(merged_edge_t e)
  {
      int count = 0;

      int total_conds = getTotalInputConditions(e);
      int inControlPorts = getNumControlInputs();
      std::vector<std::string> portname_vector;
      int *val = (int *)malloc(total_conds*sizeof(int));
      int *portId = (int *)malloc(total_conds*sizeof(int));
      int num_conds = 0;

      if (e.e1->condition) {
          int c1num = e.e1->condition->conditions->size();
          for (int i = 0; i < c1num; i++) {
              std::string portname = *(e.e1->condition->conditions->at(i)->portname);
              int value = e.e1->condition->conditions->at(i)->val->num;
              for(int j = 0; j < inControlPorts; j++) {
                  if(portname.compare(getControlInputName(j)) == 0) {
                      portname_vector.push_back(portname);
                      val[num_conds] = value;
                      portId[num_conds] = j;
                      num_conds++;
                      break;
                  }
              }
          }
      }

      if (e.e2->condition) {
          int c2num = e.e2->condition->conditions->size();
          for (int i = 0; i < c2num; i++) {
              std::string portname = *(e.e2->condition->conditions->at(i)->portname);
              int value = e.e2->condition->conditions->at(i)->val->num;
              for(int j = 0; j < inControlPorts; j++) {
                  if(portname.compare(getControlInputName(j)) == 0) {
                      portname_vector.push_back(portname);
                      val[num_conds] = value;
                      portId[num_conds] = j;
                      num_conds++;
                      break;
                  }
              }
          }
      }
      if(num_conds != total_conds) {
          assert(0);
      }
      std::string vector = packInputPort(portname_vector, val, portId, total_conds);
      free(val);
      free(portId);
      return vector;
  }

  int getTotalInputConditions(merged_edge_t e) {

      int count = 0;
      int inPorts = getNumberOfInputs();
      int found = 0;
      for(int j = 0; j < inPorts; j++) {
            if (e.e1->condition) {
                int c1num = e.e1->condition->conditions->size();
                for (int i = 0; i < c1num; i++) {
                    std::string cCond = *(e.e1->condition->conditions->at(i)->portname);
                    if(cCond.compare(getInputName(j)) == 0) {
                        count++;
                        goto over;
                    }
                }
            }

            if (e.e2->condition) {
                int c2num = e.e2->condition->conditions->size();
                for (int i = 0; i < c2num; i++) {
                    std::string cCond = *(e.e2->condition->conditions->at(i)->portname);
                    if(cCond.compare(getInputName(j)) == 0) {
                        count++;
                        goto over;
                    }
                }
            }
      over:
          found = 1 ;
      }
      return count;
  }

  int getTotalOutputConditions(merged_edge_t e) {

      int count = 0;
      int outPorts = getNumberOfOutputs();
      int found = 0;
      for(int j = 0; j < outPorts; j++) {
            if (e.e1->condition) {
                int c1num = e.e1->condition->conditions->size();
                for (int i = 0; i < c1num; i++) {
                    std::string cCond = *(e.e1->condition->conditions->at(i)->portname);
                    if(cCond.compare(getOutputName(j)) == 0) {
                        count++;
                        goto over;
                    }
                }
            }

            if (e.e2->condition) {
                int c2num = e.e2->condition->conditions->size();
                for (int i = 0; i < c2num; i++) {
                    std::string cCond = *(e.e2->condition->conditions->at(i)->portname);
                    if(cCond.compare(getOutputName(j)) == 0) {
                        count++;
                        goto over;
                    }
                }
            }
      over:
          found = 1 ;
      }
      return count;
  }

};

///////// GRAPHVIZ WRITERS /////////

struct merged_vertex_writer_t
{
  // pretty sure ": g(graph)" just initiates our graph
  merged_vertex_writer_t(merged_graph_t& graph) : g(graph)
  {
    // this is our constructor
    // we dont need anything
  };
  void operator()(std::ostream& out, const vid_t& v) const
  {
#if (PRINT_VERTEX)
    out << "[label=\"";

    merged_vertex_t mv = g[v];
    out << mv.v1->to_string();
    out << mv.v2->to_string();

    out << "\"]";
#else
    out << "[width=.25, shape=point]";
#endif
  }

  private:
  merged_graph_t& g;
  // can put stuff here
};

struct merged_edge_writer_t
{
  // pretty sure ": g(graph)" just initiates our graph
  merged_edge_writer_t(merged_graph_t& graph) : g(graph)
  {
    // this is our constructor
    // we dont need anything
  };
  void operator()(std::ostream& out, const eid_t& e) const
  {
#if (PRINT_EDGE)
    out << "[label=\"";

    merged_edge_t me = g[e];
    out << me.e1->to_string();
    out << me.e2->to_string();

    out << "\"]";
#else
    // nothing goes here.
#endif
  }

  private:
  merged_graph_t& g;
  // can put stuff here
};

/////// EXPLORE ////////

typedef enum explore_result {
  EXPLORE_SUCCESS,
  EXPLORE_FAIL,
  EXPLORE_IMM_LOOP
} explore_result_t;

class explore_t {
  public:
  explore_result_t result;
  merged_fsm_t* fsm;

  explore_t();
};

// this is not "does this equal this"
// !comp(a,b) && !comp(b,a) can only be true for same key
struct merged_state_comparator_t {
    bool operator()(const merged_state_t& a, const merged_state_t& b) const {
        // if ( !((a.s1 == b.s1) && (a.s2 == b.s2)) ) { printf("dont have %lu %lu %lu %lu\n", a.s1, a.s2, b.s1, b.s2); }
        // return ( (a.s1 == b.s1) && (a.s2 == b.s2) );
        // return ( (a.s1 <= b.s1) && (a.s2 <= b.s2) );
        unsigned long long a_val = (a.s1 << 32) | a.s2;
        unsigned long long b_val = (b.s1 << 32) | b.s2;
        return a_val < b_val;
    }
};

class explore_table_t {
  public:
  std::map<merged_state_t, merged_fsm_t*, merged_state_comparator_t> vertex_fsm_table;

  bool contains(merged_state_t key);
  merged_fsm_t* get(merged_state_t key);
  void put(merged_state_t key, merged_fsm_t* value);
};

bool sink_path_exists(fsm_t* f1, fsm_t* f2, merged_state_t prev);
bool check_consistency(fsm_t* p1, fsm_t* p2, merged_state_t prev, merged_state_t current);
merged_fsm_t* state_space(fsm_t* p1, fsm_t* p2);
explore_t* state_space_r(fsm_t* p1, fsm_t* p2, merged_state_t prev, merged_state_t current, explore_table_t* t);


void verilog_generate(merged_protocol_t *protocol, std::ofstream& out);
#endif
