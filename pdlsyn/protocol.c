
#include "protocol.h"

using namespace std;
using namespace boost;

extern config_t config;

// scheduling object

/*
class sched_t {
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
*/

sched_t::sched_t(uint32_t size)
{
  this->size = size;

  this->valid = new bool[size];
  this->add =   new bool[size];
  this->needs = new bool[size];
  this->ready = new bool[size];
  this->none =  new bool[size];
  this->dep =   new bool[size];
  this->edge =  new edge_t*[size];
  this->eid =   new eid_t[size];

  uint32_t i;
  for(i=0; i<size; i++) {
    this->valid[i] = false;
    this->add[i] = false;
    this->needs[i] = false;
    this->ready[i] = false;
    this->none[i] = false;
    this->dep[i] = false;
    this->edge[i] = NULL;
  }
}

/*
class sched_thread_t {
  public:

  protocol_t* translator;
  fsm_t** scheduled;
  protocol_t* p1;
  protocol_t* p2;
  vector_t<path_set_t*> psets1;
  vector_t<path_set_t*> psets2;

  sched_thread_t(protocol_t* translator, fsm_t** scheduled, protocol_t* p1, protocol_t* p2, path_set_t* next, vector_t<path_set_t*> psets);
};
*/

sched_thread_t::sched_thread_t(protocol_t* translator, fsm_t** scheduled, protocol_t* p1, protocol_t* p2, vector_t<path_set_t*> psets1, vector_t<path_set_t*> psets2)
{
  this->translator = translator;
  this->scheduled = scheduled;
  this->p1 = p1;
  this->p2 = p2;
  this->psets1 = psets1;
  this->psets2 = psets2;
}

/*
class sched_thread_args_t {
  public:

  uint32_t start_i;
  uint32_t end_i;
  sched_thread_t* sched_thread;

  sched_thread_args_t(uint32_t start_i, uint32_t end_i, sched_thread_t* sched_thread);
};
*/

// want to put the lock in here as well.
sched_thread_args_t::sched_thread_args_t(uint32_t start_i, uint32_t end_i, sched_thread_t* sched_thread)
{
  this->start_i = start_i;
  this->end_i = end_i;
  this->sched_thread = sched_thread;
}

// protocol

protocol_t::protocol_t()
{
  this->ports = NULL;
  this->maps = new value_map_list_t();
  // this->behavior = new fsm_t();
}

protocol_t::protocol_t(protocol_t* p1, protocol_t* p2, config_t* config)
{
  this->name += p1->name;
  this->name += "_to_";
  this->name += p2->name;

  this->p1 = p1;
  this->p2 = p2;

  this->num_fsms = p1->behavior.size() + p2->behavior.size();
  this->p1_offset = p1->behavior.size();
  this->num_states = new uint32_t[num_fsms];
  this->state_width = new uint32_t[num_fsms];

  uint32_t i;
  for(i=0; i<p1->behavior.size(); i++) {
    fsm_t* fsm = p1->behavior.at(i);

    this->num_states[i] = fsm->size();
    this->state_width[i] = log2number(this->num_states[i]);
  }
  uint32_t offset = p1->behavior.size();
  for(i=0; i<p2->behavior.size(); i++) {
    fsm_t* fsm = p2->behavior.at(i);

    this->num_states[offset+i] = fsm->size();
    this->state_width[offset+i] = log2number(this->num_states[offset+i]);
  }

  // this->set_fsm_states();

  this->ports = new port_list_t(p1->ports, p2->ports);
  this->connections = config->connections;
  // this->maps = new maps_t(p1->maps, p2->maps);

  this->behavior = this->schedule(p1, p2);
  // this->behavior = this->schedule(p1->behavior, p2->behavior);

  this->set_control();
  this->set_muxes();
  this->set_blocks();
  this->set_registers();
}

uint32_t protocol_t::size()
{
  // it should never be used in merging, only when we want to see total # of states at the end.

  // fprintf(stderr, "Dont think this should ever be used\n");
  // assert(false);

  uint32_t i;
  uint32_t num_states;

  num_states = 0;
  for(i=0; i<this->num_fsms; i++) {
    num_states += this->num_states[i];
  }

  return num_states;
}

/*
  question of whether we can return just a path set or if we must return a list of path sets.
  sticking with just 1 path set for now
  but trouble with realizing connection between the path sets.
*/

static pair<bool, path_set_t*> make_path_set(uint32_t set_size, path_set_t* p, vector_t<path_set_t*> psets, connections_t* connections)
{
  int i, j, k;

  path_set_t* res = new path_set_t(set_size);

  while( !p->equals(res, connections) ) {
    bool champ_path_set = false;
    path_set_t* champ_path = NULL;
    float champ_value = 0.0;

    for(i=0; i<psets.size(); i++) {
      path_set_t* next_pset = psets.at(i);
      path_set_t* combine = res->copy();
      combine->add( next_pset );

      if( p->contains(combine, connections) ) {
        float value = combine->value(); 

        if(value > champ_value) {
          if (champ_path) {
            // delete champ_path;
          }
          champ_path = next_pset; 
          champ_value = value;
          champ_path_set = true;
        }
      }
      delete combine;
    } // for(i=0; i<fsms.size(); i++) {
    if (champ_path_set) {
      res->add(champ_path);
    }
    else {
      return pair<bool, path_set_t*>(false, (path_set_t*) NULL);
    }
  } // while( !path.equals(match_path, connections) ) {

  return pair<bool, path_set_t*>(true, res);
}

static void* schedule_path_set(void* args)
{
  sched_thread_args_t* thread_args = (sched_thread_args_t*) args;
  int i, j, k; 
 
  uint32_t start_i = thread_args->start_i;
  uint32_t end_i = thread_args->end_i;

  protocol_t* translator =       thread_args->sched_thread->translator;
  fsm_t** scheduled =            thread_args->sched_thread->scheduled;
  protocol_t* p1 =               thread_args->sched_thread->p1;
  protocol_t* p2 =               thread_args->sched_thread->p2;
  vector_t<path_set_t*> psets1 = thread_args->sched_thread->psets1;
  vector_t<path_set_t*> psets2 = thread_args->sched_thread->psets2;

  for(i=start_i; i<end_i; i++) {
    path_set_t* next = psets1.at(i);

    pair<bool, path_set_t*> ret = make_path_set(p2->behavior.size(), next, psets2, translator->connections);
    if ( !ret.first ) { continue; }
    // assert(ret.first);
    path_set_t* match = ret.second;
    // match->print_transactions();

    path_set_t* new_pset = new path_set_t(next, match);
    translator->psets.push_back(new_pset);

    path_set_t* accum = next->accum();
    path_set_t* match_accum = match->accum();

    transaction_slice_sum_t slice_sum;

    pair<bool, vid_t>* last_vid = new pair<bool, vid_t>[translator->num_fsms];
    for(j=0; j<translator->num_fsms; j++) { last_vid[j].first = false; }

    bool built = false;
    while(!built) {

      sched_t target_sched(next->size);
      sched_t match_sched(match->size);

      for(j=0; j<next->size; j++) {
        if (next->has(j)) {
          assert(accum->has(j));

          path_t* curr_path = next->at(j);
          path_t* curr_accum = accum->at(j);

          if ( curr_path->num_cycles() > curr_accum->num_cycles() ) {
            uint32_t current_state = curr_accum->num_cycles();

            target_sched.valid[j] = true;
            target_sched.eid[j] = curr_path->edge_at(current_state).first;
            target_sched.edge[j] = curr_path->edge_at(current_state).second;
          }
          else {
            target_sched.valid[j] = false;
          }
        }
      } // for(j=0; j<next->size; j++) {

      for(j=0; j<match->size; j++) {
        if (match->has(j)) {
          assert(match_accum->has(j));

          path_t* curr_path = match->at(j);
          path_t* curr_accum = match_accum->at(j);

          if ( curr_path->num_cycles() > curr_accum->num_cycles() ) {
            uint32_t current_state = curr_accum->num_cycles();

            match_sched.valid[j] = true;
            match_sched.eid[j] = curr_path->edge_at(current_state).first;
            match_sched.edge[j] = curr_path->edge_at(current_state).second;
          }
          else {
            match_sched.valid[j] = false;
          }
        }
      } // for(j=0; j<match->size; j++) {

      for(j=0; j<next->size; j++) {
        if (next->has(j) && target_sched.valid[j]) {
          for(k=0; k<match->size; k++) {
            if (match->has(k) && match_sched.valid[k]) {

              bool need = slice_sum.needs(target_sched.edge[j], match_sched.edge[k], translator->connections);
              if (need) {
                target_sched.needs[j] = true;

                string new_cond_name = "state";
                new_cond_name += std::to_string( (unsigned long long int) j);
                vid_t state_num = source(target_sched.eid[j], p1->behavior.at(j)->g);
                uint32_t state_width = translator->state_width[j];

                condition_t* new_cond = new state_condition_t(new_cond_name, state_width, value_t(VALUE_TYPE_NUMBER, state_num));
                match_sched.edge[k]->conditions.push_back(new_cond);
              }
            }
          }

          target_sched.ready[j] = slice_sum.ready(target_sched.edge[j], translator->connections);
          target_sched.none[j] = (!target_sched.edge[j]->has_send() && !target_sched.edge[j]->has_receive());
          target_sched.dep[j] = slice_sum.dep(target_sched.edge[j]);

          if ( (target_sched.needs[j] || target_sched.ready[j] || target_sched.none[j]) && target_sched.dep[j] ) {
            target_sched.add[j] = true;
            accum->at(j)->add(target_sched.eid[j], target_sched.edge[j]);
          }
          else { 
            target_sched.add[j] = false;
          }
        }
      } // for(j=0; j<next->size; j++) {

      for(j=0; j<match->size; j++) {
        if (match->has(j) && match_sched.valid[j]) {
          for(k=0; k<next->size; k++) {
            if (next->has(k) && target_sched.valid[k]) {
              bool need_match = slice_sum.needs(match_sched.edge[j], target_sched.edge[k], translator->connections);
              if (need_match) {
                match_sched.needs[j] = true;

                string new_cond_name = "state";
                new_cond_name += std::to_string( (unsigned long long int) next->size + j);
                vid_t state_num = source(match_sched.eid[j], p2->behavior.at(j)->g);
                uint32_t state_width = translator->state_width[translator->p1_offset + j];

                condition_t* new_cond = new state_condition_t(new_cond_name, state_width, value_t(VALUE_TYPE_NUMBER, state_num));
                target_sched.edge[k]->conditions.push_back(new_cond);
              }
            }
          }

          match_sched.ready[j] = slice_sum.ready(match_sched.edge[j], translator->connections);
          match_sched.none[j] = (!match_sched.edge[j]->has_send() && !match_sched.edge[j]->has_receive());
          match_sched.dep[j] = slice_sum.dep(match_sched.edge[j]);

          if ( (match_sched.needs[j] || match_sched.ready[j] || match_sched.none[j]) && match_sched.dep[j] ) {
            match_sched.add[j] = true;
            match_accum->at(j)->add(match_sched.eid[j], match_sched.edge[j]);
          }
          else { // not needed
            match_sched.add[j] = false;
          }
        }
      } // for(j=0; j<match->size; j++) {

  /*
      // this is for the test bench.
      edge_t** new_row = new edge_t*[num_fsms];

      for(j=0; j<next->size; j++) {
        if (add[j]) {
          new_row[j] = edge[j];
        }
        else {
          new_row[j] = (edge_t*)NULL;
        }
      }
      for(j=0; j<match->size; j++) {
        if (add_match[j]) {
          new_row[next->size + j] = match_edge[j];
        }
        else {
          new_row[next->size + j] = (edge_t*)NULL;
        }
      }
      sched.push_back(new_row);
      // this is for the test bench.
  */

      bool check_add = false;
      for(j=0; j<next->size; j++) {
        check_add = check_add || target_sched.add[j];

        if (next->has(j) && target_sched.valid[j] && target_sched.add[j]) {
          slice_sum.add(target_sched.edge[j], translator->connections);

          // we are adding the stuff to our fsm
          uint32_t cycle = accum->at(j)->num_cycles();

          // transaction sums
          transaction_sum_t dest_state;
          transaction_sum_t source_state;

          // old vids
          vid_t src;
          vid_t dst;
          vector<vid_t> src_vids;
          vector<vid_t> dst_vids;
          if (cycle == 0) {
            src = source(target_sched.eid[j], p1->behavior.at(j)->g);
            dst = src;
          }
          else {
            src = source(target_sched.eid[j], p1->behavior.at(j)->g);
            dst = target(target_sched.eid[j], p1->behavior.at(j)->g);
          }
          src_vids.push_back(src);
          dst_vids.push_back(dst);

          // new vids
          vid_t src_vid;
          vid_t dst_vid;
          if ( num_vertices(scheduled[j]->g) == 0 ) {
            src_vid = scheduled[j]->add_root(src_vids, source_state);
          }
          else if (!last_vid[j].first) {
            src_vid = scheduled[j]->add_vertex(src_vids, source_state);
          }
          else {
            src_vid = last_vid[j].second;
          }

          dst_vid = scheduled[j]->add_vertex(dst_vids, dest_state);
          last_vid[j].first = true;
          last_vid[j].second = dst_vid;

          pair<eid_t, bool> e = boost::edge(src_vid, dst_vid, scheduled[j]->g);
          bool set_edge = !( e.second && (source(e.first, scheduled[j]->g) == src_vid) && (target(e.first, scheduled[j]->g) == dst_vid) );
          if ( set_edge ) {
            scheduled[j]->add_edge(src_vid, dst_vid, target_sched.edge[j]);
          }
          else {
            // printf("Edge already exists\n");
          }
        }
      }


      for(j=0; j<match->size; j++) {
        check_add = check_add || match_sched.add[j];

        if (match->has(j) && match_sched.valid[j] && match_sched.add[j]) {
          slice_sum.add(match_sched.edge[j], translator->connections);

          // we are adding the stuff to our fsm
          uint32_t offset = p1->behavior.size();
          uint32_t cycle = match_accum->at(j)->num_cycles();

          // transaction sums
          transaction_sum_t dest_state;
          transaction_sum_t source_state;

          // old vids
          vid_t match_src;
          vid_t match_dst;
          vector<vid_t> src_vids;
          vector<vid_t> dst_vids;
          if (cycle == 0) {
            match_src = source(match_sched.eid[j], p2->behavior.at(j)->g);
            match_dst = match_src;
          }
          else {
            match_src = source(match_sched.eid[j], p2->behavior.at(j)->g);
            match_dst = target(match_sched.eid[j], p2->behavior.at(j)->g);
          }
          src_vids.push_back(match_src);
          dst_vids.push_back(match_dst);

          // new vids
          vid_t src_vid;
          vid_t dst_vid;
          if ( num_vertices(scheduled[offset + j]->g) == 0 ) {
            src_vid = scheduled[offset + j]->add_root(src_vids, source_state);
          }
          else if (!last_vid[offset + j].first) {
            src_vid = scheduled[offset + j]->add_vertex(src_vids, source_state);
          }
          else {
            src_vid = last_vid[offset + j].second;
          }

          dst_vid = scheduled[offset + j]->add_vertex(dst_vids, dest_state);
          last_vid[offset + j].first = true;
          last_vid[offset + j].second = dst_vid;

          pair<eid_t, bool> e = boost::edge(src_vid, dst_vid, scheduled[offset + j]->g);

          bool set_edge = !( e.second && (source(e.first, scheduled[offset + j]->g) == src_vid) && (target(e.first, scheduled[offset + j]->g) == dst_vid) );
          if ( set_edge ) {
            scheduled[offset + j]->add_edge(src_vid, dst_vid, match_sched.edge[j]);
          }
          else {
            // assert(e.second);
            // edge_t* old_edge = scheduled[offset + j]->g[e.first];
            // old_edge->add(match_edge[j]);
          }
        }
      }
      assert(check_add);

      built = true;
      for(j=0; j<next->size; j++) {
        if (next->has(j)) { // DO NOT put && valid[j] here
          path_t* curr_path = next->at(j);
          path_t* curr_accum = accum->at(j);
          built = built && (curr_path->num_cycles() == curr_accum->num_cycles());
        }
      } // for(j=0; j<next->size; j++) {
      for(j=0; j<match->size; j++) { 
        if (match->has(j)) { // DO NOT put && match_valid[j] here
          path_t* curr_path = match->at(j);
          path_t* curr_accum = match_accum->at(j);
          built = built && (curr_path->num_cycles() == curr_accum->num_cycles());
        }
      } // for(j=0; j<match->size; j++) {
    } // while(!built) {
  } //   for(i=start_i; i<end_i; i++) {
  return NULL;
}

vector_t<fsm_t*> protocol_t::schedule(protocol_t* p1, protocol_t* p2)
{
  uint32_t i;

  fsm_t** scheduled = new fsm_t*[this->num_fsms];
  for(i=0; i<this->num_fsms; i++) { scheduled[i] = new fsm_t(); }

  vector_t<path_set_t*> psets1 = p1->path_sets();
  vector_t<path_set_t*> psets2 = p2->path_sets();
  printf("num path sets: %u %u\n", psets1.size(), psets2.size());

  sched_thread_t* sched_thread = new sched_thread_t(this, scheduled, p1, p2, psets1, psets2);

  uint32_t num_path_sets = psets1.size();
  uint32_t num_per_thread = num_path_sets / NUM_THREADS;
  // uint32_t left_over = num_path_sets % NUM_THREADS;

  pthread_t thread[NUM_THREADS];

  for(i=0; i<NUM_THREADS; i++) {
    uint32_t start_i = i * num_per_thread;
    uint32_t end_i = (i+1) * num_per_thread;

    // create the args struct
    sched_thread_args_t* args = new sched_thread_args_t(start_i, end_i, sched_thread);

    // create the thread
    pthread_create(&thread[i], NULL, &schedule_path_set, (void*)args);
  }

  for(i=0; i<NUM_THREADS; i++) {
    // join the threads before continuing.
    pthread_join(thread[i], NULL);
  }

  vector_t<fsm_t*> ret;
  for(i=0; i<num_fsms; i++) {
    ret.push_back(scheduled[i]);
  }

  printf("Finished scheduling\n");

  return ret;
}

vector_t<path_set_t*> protocol_t::path_sets()
{
  uint32_t i, j, ii, jj;
  vector_t<path_set_t*> ret;
  uint32_t set_size = this->behavior.size();

  paths_t** paths_list = new paths_t*[set_size];
  for(i=0; i<set_size; i++) {
    paths_t* paths = behavior.at(i)->paths();
    paths_list[i] = paths;
  }

  if ( set_size == 1 ) {
    paths_t* paths = paths_list[0];
    for(j=0; j<paths->size(); j++) {
      path_t* p = paths->at(j)->copy();

      path_set_t* pset = new path_set_t(set_size); 
      pset->set(0, p);
      ret.push_back(pset);
    }
  }
  else {
    for(i=0; i<set_size; i++) {
      paths_t* paths1 = paths_list[i];

      for(ii=0; ii<paths1->size(); ii++) {
        path_t* p1 = paths1->at(ii);
        p1->set_id();
        p1->set_dep();

        bool used = false;

        for(j=0; j<set_size; j++) {
          paths_t* paths2 = paths_list[j];

          if(i != j) {
            for(jj=0; jj<paths2->size(); jj++) {
              path_t* p2 = paths2->at(jj);

              if (i==0) {
                p2->set_id();
                p2->set_dep();
              }

              if (p1->id_set.equals(p2->dep_set)) {
                used = true;
                path_set_t* pset = new path_set_t(set_size);
                pset->set(i, p1->copy()); 
                pset->set(j, p2->copy());
                ret.push_back(pset);
              }
            } // for(jj=0; jj<paths2->size(); jj++) {
          } // if(i != j) {
        } // for(j=0; j<set_size; j++) {
        if ( (!used) && (p1->dep_set.size()==0) ) {
          path_set_t* pset = new path_set_t(set_size);
          pset->set(i, p1->copy());
          ret.push_back(pset);
        }
      } // for(ii=0; ii<paths1->size(); ii++) {
    } // for(i=0; i<set_size; i++) {
  } // else {

  return ret;
}

/*
think we only need one function to do both.
*/

void protocol_t::set_control()
{
  this->has_output_state = new bool[this->behavior.size()];
  this->has_input_state = new bool[this->behavior.size()];

  uint32_t i, j;
  for(i=0; i<this->behavior.size(); i++) {
    fsm_t* fsm = this->behavior.at(i);

    // may as well be a set
    // we were an idiot and had this at the top
    // need to remake it for each fsm
    map_comp_t<condition_t*, uint32_t, state_comparator_t> set;

    uint32_t start_control_in = 0;
    uint32_t start_control_out = 0;

    std::pair<graph_t::edge_iterator, graph_t::edge_iterator> range = edges(fsm->g);
    for(graph_t::edge_iterator itr = range.first; itr != range.second; itr++) {
      edge_t* edge = fsm->g[*itr];
      
      for(j=0; j<edge->conditions.size(); j++) {
        condition_t* cond = edge->conditions.at(j);

        if (!set.contains(cond)) {
          set.put(cond, 1);


          if (cond->type()==COND_CONTROL) {
            uint32_t len;
            if (cond->has_cut) {
              len = cond->msb - cond->lsb + 1;
              // printf("cut.len : %u\n", len);
            } 
            else {
              port_t* port = this->ports->find( cond->top() );
              len = port->width;
              // printf("port.len : %u\n", len);
            }
            if (cond->get_dir() == PORT_DIR_IN) {
              control_pos_t pos(cond, start_control_in, len);
              fsm->control_in.push_back(pos);
              start_control_in += len;
            } 
            else {
              assert(cond->get_dir() == PORT_DIR_OUT);
              control_pos_t pos(cond, start_control_out, len);
              fsm->control_out.push_back(pos);
              start_control_out += len;
            }
          }
          else {
            assert(cond->type()==COND_STATE);
            assert(cond->get_dir() == PORT_DIR_NULL);

            uint32_t len = cond->get_width();
            control_pos_t pos(cond, start_control_out, len);
            fsm->control_out.push_back(pos);
            start_control_out += len;
          }


        } // if (!set.contains(cond)) {
      } // for(j=0; j<edge->conditions.size(); j++) {
    } // for(graph_t::edge_iterator itr = range.first; itr != range.second; itr++) {

    has_output_state[i] = fsm->control_in.size() != 0;
    has_input_state[i] = fsm->control_out.size() != 0;

  } // for(i=0; i<this->behavior.size(); i++) {
}



void protocol_t::set_muxes()
{
  int i, j, k;

  // not sure if these loops are right
  // why shud we have so many fifos / increments.

  for(i=0; i<this->behavior.size(); i++) {
    fsm_t* fsm = this->behavior.at(i);

    // sender muxes
    for(j=0; j<this->connections->size(); j++) {
      connection_t connect = this->connections->at(j);
      vector_t<transaction_t*> unique_locations = fsm->unique_locations( &connect.send_ref );

      if (unique_locations.size() > 0) {
        send_mux_t* new_send_mux = new send_mux_t(unique_locations, i);

        if ( !this->send_muxes.contains(&connect.send_ref) ) {
          this->send_muxes.push_back(new_send_mux);
        }
        else {
          this->send_muxes.add(&connect.send_ref, unique_locations);
        }
      }
    } // for(j=0; j<this->connections->size(); j++) {

    // receiver muxes
    for(j=0; j<this->connections->size(); j++) {
      connection_t connect = this->connections->at(j);
      vector_t<transaction_t*> unique_locations = fsm->unique_locations( &connect.rec_ref );

      for(k=0; k<unique_locations.size(); k++) {
        transaction_t* unique_receive = unique_locations.at(k);
        vector_t<port_ref_t*> unique_matches = fsm->unique_matches( unique_receive );

        if (unique_matches.size() > 0) {
          if (!this->receive_muxes.contains(unique_receive)) {

            receive_mux_t* new_rec_mux = new receive_mux_t(unique_receive, unique_matches, i);
            this->receive_muxes.push_back(new_rec_mux);
          }
          else {

            this->receive_muxes.add(unique_receive, unique_matches);
          }
        }
      } // for(k=0; k<unique_locations.size(); k++) {
    } // for(j=0; j<this->connections->size(); j++) {

  } // for(i=0; i<this->behavior.size(); i++) {
}

void protocol_t::set_blocks()
{
  int i, j, k;

  // not sure if these loops are right
  // why shud we have so many fifos / increments.

  for(i=0; i<this->behavior.size(); i++) {
    fsm_t* fsm = this->behavior.at(i);

    for(j=0; j<this->connections->size(); j++) {
      connection_t connect = this->connections->at(j);

      vector_t<transaction_t*> unique_sources = fsm->unique_locations( &connect.send_ref );
      vector_t<transaction_t*> unique_dests;

      if (unique_sources.size() > 0) {

        pair<bool, string> parameter;
        uint32_t in_width = 0;
        uint32_t out_width = 0;

        // find the in width from the muxes
        for(k=0; k<this->send_muxes.size(); k++) {
          send_mux_t* mux = this->send_muxes.at(k);

          if ( (mux->name == connect.send_ref.get_name()) && (in_width < mux->width) ) {
            in_width = mux->width;
          }
        }
        // make sure in width is good
        if (in_width == 0) {
          fprintf(stderr, "in_width = %u\n", in_width);
          assert(in_width > 0);
        }
        // find the out width from the muxes
        for(k=0; k<this->receive_muxes.size(); k++) {
          receive_mux_t* mux = this->receive_muxes.at(k);

          if ( mux->contains( &connect.send_ref ) && (out_width < mux->width) ) {
            out_width = mux->width;
            unique_dests.push_back(mux->receive);
          }
        } 
        // make sure out width is good
        if (out_width == 0) {
          fprintf(stderr, "out_width = %u\n", out_width);
          assert(out_width > 0);
        }
        // figure out what type of block this is.
        // there is probably a more efficient way of doing this.
        for(k=0; k<unique_sources.size(); k++) {
          if (!parameter.first) {
            parameter.first = true;
            parameter.second = unique_sources.at(k)->parameter();
          }
          else {
            assert(parameter.second == unique_sources.at(k)->parameter());
          }
        }

        string name = connect.send_ref.get_name();

        if (parameter.second == "offset") {
          increment_t* new_increment = new increment_t(name, unique_dests, in_width, out_width, i);
          if ( !this->blocks.contains(name) ) {
            this->blocks.push_back(new_increment);
          }
          else {
            // this->blocks.add();
          }
        }
        else if (parameter.second == "range") {
          fifo_t* new_fifo = new fifo_t(name, unique_dests, in_width, out_width, i);
          if ( !this->blocks.contains(name) ) {
            this->blocks.push_back(new_fifo);
          }
          else {
            // this->blocks.add();
          }
        }
        else {
          fprintf(stderr, "We do not support any other parameters yet\n");
          assert(false);
        }
      }
    } // for(i=0; i<this->connections->size(); i++) {
  }
}

void protocol_t::set_registers()
{
  uint32_t i;

  for(i=0; i<this->behavior.size(); i++) {
    fsm_t* fsm = this->behavior.at(i);
    uint32_t state_width = this->state_width[i];

    string reg_name;
    uint32_t width;

    // state
    reg_name = "state" + std::to_string( (unsigned long long int) i);
    width = state_width - 1;
    this->registers.push_back(new reg_t(reg_name, width, 0));

    // next state
    // reg_name = "next_state" + std::to_string( (unsigned long long int) i);
    // width = state_width - 1;
    // this->registers.push_back(new reg_t(reg_name, width, 0));

    // output state
    if (this->has_output_state[i]) {
      reg_name = "output_state" + std::to_string( (unsigned long long int) i);
      width = fsm->control_in_width() - 1;
      this->registers.push_back(new reg_t(reg_name, width, 0));

      // next output state
      // reg_name = "next_output_state" + std::to_string( (unsigned long long int) i);
      // width = fsm->control_in_width() - 1;
      // this->registers.push_back(new reg_t(reg_name, width, 0));

    }
  }

  this->registers.push_back(this->send_muxes.registers());
  this->registers.push_back(this->receive_muxes.registers());
  this->registers.push_back(this->blocks.registers());
}

void protocol_t::to_file(string filename)
{
  int i;

  ofstream file;
  file.open(filename.c_str());

  file << "PORTS:\n";
  file << this->ports->to_string();
  file << "\n";

  // file << "BEHAVIOR:\n";
  // file << this->behavior->to_string();

  file.close();
}

void protocol_t::print()
{
  int i;

  printf("PORTS:\n");
  this->ports->print();
  printf("\n");

  printf("MAPS:\n");
  this->maps->print();
  printf("\n");

  // printf("BEHAVIOR:\n");
  // this->behavior->print();
}

void protocol_t::add_body(body_t* body)
{
  switch (body->type) {
    case BODY_TYPE_PORTS:
      // we are handling ports differently
      // assert(this->ports == NULL);
      // this->ports = body->ports;
      break;
    case BODY_TYPE_MAP:
      // assert(this->maps == NULL);
      this->maps->push_back(body->map);
      break;
    case BODY_TYPE_BEHAVIOR:
      // assert(this->behavior == NULL);
      // this->behavior = body->behavior;
      break;
    case BODY_TYPE_PACKET:
      this->packets.put( body->packet->type, body->packet );
      break;
    default:
      fprintf(stderr, "Invalid body type: %d\n", body->type);
      assert(0);
  }
}

void protocol_t::set_name(string protocol_name)
{
  int i, j;

  this->name = protocol_name;

  for(i=0; i<this->ports->size(); i++) {
    this->ports->at(i)->set_protocol_name(protocol_name);
  }

  for(i=0; i<this->behavior.size(); i++) {

    fsm_t* behavior = this->behavior.at(i);

    std::pair<graph_t::edge_iterator, graph_t::edge_iterator> edge_range;
    edge_range = edges( behavior->g );
    for(graph_t::edge_iterator itr = edge_range.first; itr != edge_range.second; itr++)
    {
      edge_t* edge = behavior->g[*itr];
 
      for(j=0; j<edge->sends.size(); j++) {
        transaction_t* send = edge->sends.at(j);
        send->set_protocol_name(protocol_name);
      }

      for(j=0; j<edge->receives.size(); j++) {
        transaction_t* rec = edge->receives.at(j);
        rec->set_protocol_name(protocol_name);
      }
    }
  } // for(i=0; i<this->behavior.size(); i++) {

}

void protocol_t::to_visualize(std::string filename)
{
  graph_t viz_graph;

  std::ofstream outfile(filename.c_str());

  uint32_t i;
  for(i=0; i<this->behavior.size(); i++) {
    fsm_t* fsm = this->behavior.at(i);

    map_t<uint64_t, uint64_t> vid_map;

    // we will add all the vertexes to our graph/map
    std::pair<graph_t::vertex_iterator, graph_t::vertex_iterator> vertex_range = vertices(fsm->g);
    for(graph_t::vertex_iterator itr = vertex_range.first; itr != vertex_range.second; itr++) {
      // create a new vid
      vid_t new_vid = add_vertex(viz_graph);
      
      // get the old vid, and its vertex
      vid_t old_vid = *itr; 
      vertex_t* vertex = fsm->g[old_vid];

      // set the new vid to the vertex
      viz_graph[new_vid] = vertex;

      // put the old vid -> new vid mapping in our map
      vid_map.put(old_vid, new_vid);
    }

    // now we will add the edges.
    std::pair<graph_t::edge_iterator, graph_t::edge_iterator> range = edges(fsm->g);
    for(graph_t::edge_iterator itr = range.first; itr != range.second; itr++) {
      // get the old eid, source, target, and edge.
      eid_t old_eid = *itr;
      vid_t old_source = old_eid.m_source;
      vid_t old_target = old_eid.m_target;
      edge_t* edge = fsm->g[old_eid];

      // get the new source/target 
      assert(vid_map.contains(old_source));
      vid_t new_source = vid_map.get(old_source);

      assert(vid_map.contains(old_target));
      vid_t new_target = vid_map.get(old_target);
        
      // add our edge to viz graph
      pair<eid_t, bool> ret = boost::add_edge(new_source, new_target, viz_graph);
      eid_t new_eid = ret.first;
      assert(ret.second);

      viz_graph[new_eid] = edge;
    }
  }

  write_graphviz(outfile, viz_graph, vertex_writer_t(viz_graph), edge_writer_t(viz_graph));
}

string protocol_t::protocol_name()
{
  return this->name;
}

// dont think we need this
string protocol_t::clk_name()
{
  return "clk";
}

// dont think we need this
string protocol_t::reset_name()
{
  return "reset";
}

/*
string protocol_t::output_condition(edge_t* edge)
{
  string output_string;

  int i;
  for(i=0; i<this->ports->size(); i++) {
    if(this->ports->at(i)->type == PORT_TYPE_CONTROL && 
       this->ports->at(i)->get_dir() == PORT_DIR_OUT) {

      string portname = this->ports->at(i)->get_name();
      if(edge->contains_condition(portname)) {
        uint32_t value = edge->get(portname);
        // we prepend here because we want the order to be lsb->msb when indexing
        output_string = num2binary(value, this->ports->at(i)->width) + output_string;
      }
      else {
        // we prepend here because we want the order to be lsb->msb when indexing
        output_string = std::string(this->ports->at(i)->width, '?') + output_string;
      }
    }
  }

  return output_string;
}

string protocol_t::input_condition(edge_t* edge)
{
  string input_string;

  int i;
  for(i=0; i<this->ports->size(); i++) {
    if(this->ports->at(i)->type == PORT_TYPE_CONTROL && 
       this->ports->at(i)->get_dir() == PORT_DIR_IN) {

      string portname = this->ports->at(i)->get_name();
      if(edge->contains_condition(portname)) {
        uint32_t value = edge->get(portname);
        // we prepend here because we want the order to be lsb->msb when indexing
        input_string = num2binary(value, this->ports->at(i)->width) + input_string;
      }
      else {
        // we prepend here because we want the order to be lsb->msb when indexing
        input_string = std::string(this->ports->at(i)->width, '?') + input_string;
      }
    }
  }

  return input_string;
}
*/

/*
BUFFER VECTOR NOTE

plan wud really just be
create a new port map in config OR port map
that just handles bufferd ports
and we add do it when we say set buffer
then on lookups we go and see if its there.

we cud do that right now. but its really not worth it.

all it gives us is the size of the push state verilog vector
and we can iterate over it and see if we are to push into it.
instead of iterating over all the port mappings.

so all we are doing now is adding extra 0s and it works fine. 
*/

// need to do this in order of ports IN config.

/*
string protocol_t::push_state(edge_t* edge, connections_t* connections) {
  int i, j;
  string push_string;

  // need to make this for buffers.
  for (i=0; i<connections->size(); i++) {
    connection_t connect = connections->at(i);

    string push_value = "0";
    if (edge->contains_send( &connect.send_ref )) {
      push_value = "1";
    }

    push_string = push_value + push_string;
  }

  return push_string;
}

// need to do this in order of ports IN config.
/*
string protocol_t::pop_state(edge_t* edge, connections_t* connections) {
  int i, j;
  string pop_string;

  // we need to do this over connections
  // because that is best way to create the pop string.
  // so it is same length and consistent

  // need to make this for buffers.
  for (i=0; i<connections->size(); i++) {
    connection_t connect = connections->at(i);

    string pop_value = "0";
    if (edge->contains_receive( &connect.rec_ref )) {
      pop_value = "1";
    }

    pop_string = pop_value + pop_string;
  }

  return pop_string;
}
*/

string protocol_t::to_verilog_states()
{
  string state_string;

  uint32_t i;

  //////////// STATES ////////////

  for(i=0; i<this->behavior.size(); i++) {
    fsm_t* behavior = this->behavior.at(i);
    uint32_t state_width = this->state_width[i];

    state_string += "// fsm #";
    state_string += std::to_string( (unsigned long long int) i);
    state_string += "\n";

    std::pair<graph_t::vertex_iterator, graph_t::vertex_iterator> vertex_range = vertices(behavior->g);
    for(graph_t::vertex_iterator itr = vertex_range.first; itr != vertex_range.second; itr++) 
    {
      vertex_t* vertex = behavior->g[*itr];
      state_string += "localparam fsm" + std::to_string( (unsigned long long int) i) + "_" + vertex->to_string();
      state_string += " = ";
      state_string += std::to_string( (unsigned long long int) state_width) + "'b" + num2binary(vertex->id(), state_width) + ";\n";
    }
    state_string += "\n";
  }

  state_string += "\n";

  return state_string;
}

string protocol_t::to_verilog_fsm()
{
  string fsm_string;

  uint32_t i;

  for(i=0; i<this->behavior.size(); i++) {
    fsm_t* behavior = this->behavior.at(i);
    uint32_t state_width = this->state_width[i];

    fsm_string += this->to_verilog_fsm_help(state_width, behavior, i);
    fsm_string += "\n";
  }

  return fsm_string;
}

string protocol_t::to_verilog_fsm_help(uint32_t state_width, fsm_t* fsm, uint32_t fsm_id)
{
  string fsm_string;
  uint32_t i, j;

  uint32_t num_fsms = this->behavior.size();

  fsm_string += "always @(*) begin\n";
  fsm_string += "  if (reset) begin\n";
  // fsm_string += "    next_state";
  // fsm_string += std::to_string( (unsigned long long int) fsm_id);
  // fsm_string += " = 0;\n";
  fsm_string += "  end else begin\n";
  
  if (this->has_input_state[fsm_id]) {
    fsm_string += "    casex({input_vector";
    fsm_string += std::to_string( (unsigned long long int) fsm_id);
    fsm_string += ", state";
    fsm_string += std::to_string( (unsigned long long int) fsm_id);
    fsm_string += "})\n";
  }
  else {
    fsm_string += "    casex(state";
    fsm_string += std::to_string( (unsigned long long int) fsm_id);
    fsm_string += ")\n";
  }

  std::pair<graph_t::vertex_iterator, graph_t::vertex_iterator> vertex_range = vertices(fsm->g);
  for(graph_t::vertex_iterator itr = vertex_range.first; itr != vertex_range.second; itr++) 
  {
    vertex_t* vertex = fsm->g[*itr];
    std::string vertex_name = vertex->to_string();

    std::pair<graph_t::out_edge_iterator, graph_t::out_edge_iterator> edge_range;
    edge_range = out_edges(*itr, fsm->g);
    if (edge_range.first == edge_range.second) {
      edge_range = out_edges(fsm->root, fsm->g);
    }

    if(vertex->type == FSM_SINK) {
      // this case would work, but it never happens
      assert(false);
    }
    else {
      edge_range = out_edges(*itr, fsm->g);
    }

    for(graph_t::out_edge_iterator itr = edge_range.first; itr != edge_range.second; itr++)
    {
      edge_t* e = fsm->g[*itr];
      vid_t dst_vid = target(*itr, fsm->g);
      vertex_t* dst_vertex = fsm->g[dst_vid];
      std::string dst_vertex_name = dst_vertex->to_string();

      if (this->has_input_state[fsm_id]) {
        fsm_string += "      {" + std::to_string( (unsigned long long int) fsm->control_out_width() ) + "'b";
        fsm_string += fsm->input_condition(e);
        fsm_string += ",";
        fsm_string += "fsm";
        fsm_string += std::to_string( (unsigned long long int) fsm_id);
        fsm_string += "_";
        fsm_string += vertex_name;
        fsm_string += "}: begin\n";
      }
      else {
        fsm_string += "      fsm";
        fsm_string += std::to_string( (unsigned long long int) fsm_id);
        fsm_string += "_";
        fsm_string += vertex_name;
        fsm_string += ": begin\n";
      }

      fsm_string += "        next_state";
      fsm_string += std::to_string( (unsigned long long int) fsm_id);
      fsm_string += " = fsm";
      fsm_string += std::to_string( (unsigned long long int) fsm_id);
      fsm_string += "_";
      fsm_string += dst_vertex_name;
      fsm_string += ";\n";

      if (this->has_output_state[fsm_id]) {
        fsm_string += "        next_output_state";
        fsm_string += std::to_string( (unsigned long long int) fsm_id);
        fsm_string += " = ";
        fsm_string += std::to_string( (unsigned long long int) fsm->control_in_width() ) + "'b" + fsm->output_condition(e) + ";\n";
      }

      for(i=0; i<this->blocks.size(); i++) {
        block_t* block = this->blocks.at(i);

        for(j=0; j<e->sends.size(); j++) {
          transaction_t* send = e->sends.at(j);

          if (block->push_state(send)) {
            fsm_string += "        ";
            fsm_string += block->next_push();
            fsm_string += " = 1;\n";
          }
        }

        for(j=0; j<e->receives.size(); j++) {
          transaction_t* rec = e->receives.at(j);
          // right here we need to add the ports that we have to pop on
          // so for write_address incr, we are gonna need to pop on awaddr.
          // so we shud put the unique_locations in fifo and increment or something

          if (block->pop_state(rec)) {
            fsm_string += "        ";
            fsm_string += block->next_pop();
            fsm_string += " = 1;\n";
          }
        }
      }

      // this is where we need to add the mux sel logic
      int i;
      for(i=0; i<this->send_muxes.size(); i++) {
        std::pair<bool, string> mux_sel = this->send_muxes.at(i)->mux_sel(e);
        if ( mux_sel.first ) {
          fsm_string += "        ";
          fsm_string += mux_sel.second;
          fsm_string += "\n";
        }
      }

      for(i=0; i<this->receive_muxes.size(); i++) {
        std::pair<bool, string> mux_sel = this->receive_muxes.at(i)->mux_sel(e);
        if ( mux_sel.first ) {
          fsm_string += "        ";
          fsm_string += mux_sel.second;
          fsm_string += "\n";
        }
      }

      fsm_string += "      end\n";
    }
  }

  fsm_string += "    endcase\n";
  fsm_string += "  end\n";
  fsm_string += "end\n";

  return fsm_string;
}

string protocol_t::to_verilog_trans()
{
  string seq_string;
  uint32_t i;
  uint32_t num_fsms = this->behavior.size();

  seq_string += "always @(posedge clk) begin\n";
  seq_string += "  if (reset) begin\n";

/*
  for(i=0; i<this->behavior.size(); i++) {
    fsm_t* fsm = this->behavior.at(i);

    if (this->has_output_state[i]) {
      seq_string += "    output_state";
      seq_string += std::to_string( (unsigned long long int) i );
      seq_string += " <= 0;\n";
    }
  }
*/

  for(i=0; i<this->registers.size(); i++) {
    reg_t* reg = this->registers.at(i);

    seq_string += "    ";
    seq_string += reg->name;
    seq_string += " <= 0;\n";
  }

  seq_string += "  end else begin\n\n";

  for(i=0; i<this->behavior.size(); i++) {
    fsm_t* fsm = this->behavior.at(i);

    seq_string += "    // fsm #";
    seq_string += std::to_string( (unsigned long long int) i);
    seq_string += "\n";

    if (this->has_output_state[i]) {
      seq_string += "    output_state";
      seq_string += std::to_string( (unsigned long long int) i );
      seq_string += " <= next_output_state";
      seq_string += std::to_string( (unsigned long long int) i );
      seq_string += ";\n"; 
    }

    seq_string += "    state";
    seq_string += std::to_string( (unsigned long long int) i );
    seq_string += " <= next_state";
    seq_string += std::to_string( (unsigned long long int) i );
    seq_string += ";\n"; 
  }

  /*
  the muxes are bound to the merged protocol, they may need to be named to say which fsms they are going between tho
  this wud have to happen before we got here.

  i guess all the fifos and increment blocks share the push/pop logic vector
  so we dont actually have to split that up ...

  yes, although the stuff is split up
  */

  seq_string += "    // mux control logic\n";

  for(i=0; i<this->send_muxes.size(); i++) {
    seq_string += "    ";
    seq_string += this->send_muxes.at(i)->sel();
    seq_string += " <= ";
    seq_string += this->send_muxes.at(i)->next_sel();
    seq_string += ";\n";
  }

  for(i=0; i<this->receive_muxes.size(); i++) {
    seq_string += "    ";
    seq_string += this->receive_muxes.at(i)->sel();
    seq_string += " <= ";
    seq_string += this->receive_muxes.at(i)->next_sel();
    seq_string += ";\n";
  }

  seq_string += "    // block control logic\n";
  
  for(i=0; i<this->blocks.size(); i++) {
    seq_string += "    ";
    seq_string += this->blocks.at(i)->push();
    seq_string += " <= ";
    seq_string += this->blocks.at(i)->next_push();
    seq_string += ";\n";

    seq_string += "    ";
    seq_string += this->blocks.at(i)->pop();
    seq_string += " <= ";
    seq_string += this->blocks.at(i)->next_pop();
    seq_string += ";\n";
  }

  seq_string += "  end\n"; 
  seq_string += "end\n";

  return seq_string;
}

/*
string protocol_t::to_verilog_reset()
{
  string reset_string;

  uint32_t i;

  reset_string += "always @(*) begin\n";
  reset_string += "  if (reset) begin\n";

  // for(i=0; i<this->behavior.size(); i++) {
  //   fsm_t* fsm = this->behavior.at(i);

  //  if (this->has_output_state[i]) {
  //    reset_string += "    output_state";
  //    reset_string += std::to_string( (unsigned long long int) i );
  //    reset_string += " = 0;\n";
  //  }
  // }

  for(i=0; i<this->registers.size(); i++) {
    reg_t* reg = this->registers.at(i);

    reset_string += "    ";
    reset_string += reg->name;
    reset_string += " = 0;\n";
  }

  reset_string += "  end\n";
  reset_string += "end\n";

  return reset_string;
}
*/

string protocol_t::to_verilog_vars()
{
  string vars_string;

  uint32_t num_fsms = this->behavior.size();

  uint32_t i;
  for(i=0; i<this->behavior.size(); i++) {
    fsm_t* fsm = this->behavior.at(i);
    uint32_t state_width = this->state_width[i];

    vars_string += "// fsm #";
    vars_string += std::to_string( (unsigned long long int) i);
    vars_string += "\n";

    // state
    vars_string += "reg [" + std::to_string( (unsigned long long int) (state_width - 1) ) + ":0] state";
    vars_string += std::to_string( (unsigned long long int) i);
    vars_string += ";\n";

    // next state
    vars_string += "reg [" + std::to_string( (unsigned long long int) (state_width - 1) ) + ":0] next_state";
    vars_string += std::to_string( (unsigned long long int) i);
    vars_string += ";\n";

    // output state
    if (this->has_output_state[i]) {
      vars_string += "reg [" + std::to_string( (unsigned long long int) (fsm->control_in_width() - 1) ) + ":0] output_state";
      vars_string += std::to_string( (unsigned long long int) i);
      vars_string += ";\n";
      // next output state
      vars_string += "reg [" + std::to_string( (unsigned long long int) (fsm->control_in_width() - 1) ) + ":0] next_output_state";
      vars_string += std::to_string( (unsigned long long int) i);
      vars_string += ";\n";
    }

    // input vector
    if (this->has_input_state[i]) {
      vars_string += "wire [" + std::to_string( (unsigned long long int) (fsm->control_out_width() - 1) ) + ":0] input_vector";
      vars_string += std::to_string( (unsigned long long int) i );
      vars_string += ";\n";
    }
  }

  vars_string += "\n\n";

  /*
  I am not sure if each fsm gets its own push/pop state or its own muxes
  BUT we should be using connection->buffers->size().
  */

  for(i=0; i<this->send_muxes.size(); i++) {
    vars_string += "// ";
    vars_string += send_muxes.at(i)->name;
    vars_string += "\n";
    vars_string += send_muxes.at(i)->output_dec();
    vars_string += "\n";
    vars_string += send_muxes.at(i)->sel_dec();
    vars_string += "\n";
    vars_string += send_muxes.at(i)->next_sel_dec();
    vars_string += "\n";
  }



  for(i=0; i<this->receive_muxes.size(); i++) {
    vars_string += "// ";
    vars_string += receive_muxes.at(i)->name;
    vars_string += "\n";
    vars_string += receive_muxes.at(i)->output_dec();
    vars_string += "\n";
    vars_string += receive_muxes.at(i)->sel_dec();
    vars_string += "\n";
    vars_string += receive_muxes.at(i)->next_sel_dec();
    vars_string += "\n";
  }



  for(i=0; i<this->blocks.size(); i++) {
    block_t* block = this->blocks.at(i);
    vars_string += "// ";
    vars_string += block->name;
    vars_string += "\n";

    vars_string += block->output_dec();
    vars_string += "\n";

    vars_string += block->next_push_dec();
    vars_string += "\n";
    vars_string += block->next_pop_dec();
    vars_string += "\n";
    vars_string += block->push_dec();
    vars_string += "\n";
    vars_string += block->pop_dec();
    vars_string += "\n";
  }

  return vars_string;
}
// need to check who is input and who is output here.
string protocol_t::to_verilog_assign()
{
  string assign_string;

  int i, j;
  uint32_t num_fsms = this->behavior.size();

  // output vector

  for(i=0; i<this->behavior.size(); i++) {
    fsm_t* fsm = this->behavior.at(i);

    assign_string += "\n";

    for(j=0; j<fsm->control_in.size(); j++) {
      assert(this->has_output_state[i]);

      control_pos_t pos = fsm->control_in.at(j);
      condition_t* next = pos.cond;

      assign_string += "assign ";
      assign_string += next->to_verilog();
      assign_string += " = output_state";
      assign_string += std::to_string( (unsigned long long int) i );
      assign_string += "[";
      assign_string += std::to_string( (unsigned long long int) pos.start + pos.len - 1 );
      assign_string += ":";
      assign_string += std::to_string( (unsigned long long int) pos.start );
      assign_string += "];\n";
    }
    assign_string += "\n";
  }

  // input vector

  for(i=0; i<this->behavior.size(); i++) {
    fsm_t* fsm = this->behavior.at(i);

    if (this->has_input_state[i]) {
      assign_string += "\n";
      assign_string += "assign input_vector";
      assign_string += std::to_string( (unsigned long long int) i );
      assign_string += " = {";

      for(j=0; j<fsm->control_out.size(); j++) {
        condition_t* next = fsm->control_out.at(j).cond;
        assign_string += next->to_verilog();
        if (j < fsm->control_out.size()-1) {
          assign_string += ", ";
        }
      }
      assign_string += "};";
      assign_string += "\n";
    }
  }

  // assign output connections to the receive muxes.

  assign_string += "\n// output\n";

  for(i=0; i<this->receive_muxes.size(); i++) {
    assign_string += this->receive_muxes.at(i)->output_assign();
    assign_string += "\n";
  }

  // blocks : fifos and increment

  assign_string += "\n// blocks\n";

  for(i=0; i<this->blocks.size(); i++) {
    assign_string += this->blocks.at(i)->to_verilog();
    assign_string += "\n";
  }

  // muxes

  assign_string += "\n// send muxes\n";

  for(i=0; i<this->send_muxes.size(); i++) {
    assign_string += this->send_muxes.at(i)->to_verilog();
    assign_string += "\n";
  }

  assign_string += "\n// receive muxes\n";

  for(i=0; i<this->receive_muxes.size(); i++) {
    assign_string += this->receive_muxes.at(i)->to_verilog();
    assign_string += "\n";
  }

  return assign_string;
}

string protocol_t::to_verilog()
{
  string protocol_string;

  // interface
  protocol_string += "module top(\n";
  protocol_string += "input wire clk,\n";
  protocol_string += "input wire reset,\n";
  protocol_string += this->ports->to_verilog_ports();
  protocol_string += ");\n\n";

  // internal variables
  protocol_string += this->to_verilog_vars();
  protocol_string += "\n";
  
  // assignments
  // use the maps to do output data assignments!
  // data

  // control   
  protocol_string += this->to_verilog_assign();
  protocol_string += "\n";

  // states
  protocol_string += this->to_verilog_states();
  protocol_string += "\n";

  // fsm / casex
  protocol_string += this->to_verilog_fsm();
  protocol_string += "\n";

  // reset
  // protocol_string += this->to_verilog_reset();

  // sequential logic transition
  protocol_string += this->to_verilog_trans();
  protocol_string += "\n";

  // end module
  protocol_string += "endmodule\n";

  // return our verilog code.
  return protocol_string;
}

///////////////////////////
//// TEST BENCH ///////////
///////////////////////////

string protocol_t::test_bench()
{
  string test_bench_string;

  // testbench variables
  test_bench_string += "module " + this->protocol_name() + "_tb();\n";
  test_bench_string += "\n";

  test_bench_string += this->test_bench_head();
  test_bench_string += "\n\n";

  // instantiate translator
  test_bench_string += this->test_bench_translator();
  test_bench_string += "\n\n";

  // body   
  test_bench_string += this->test_bench_body();
  test_bench_string += "\n\n";

  // end module
  test_bench_string += "endmodule\n";

  // return our verilog code.
  return test_bench_string;
}

string protocol_t::test_bench_head()
{
  string test_bench_string;

  test_bench_string += "  reg clk;\n";
  test_bench_string += "  reg reset;\n";
  test_bench_string += this->ports->test_bench_head();

  return test_bench_string;
}

string protocol_t::test_bench_translator()
{
  string test_bench_string;

  test_bench_string += "  ";
  test_bench_string += this->protocol_name() + " DUT(\n";
  test_bench_string += "  .clk(clk),\n";
  test_bench_string += "  .reset(reset),\n";
  test_bench_string += this->ports->test_bench_translator();
  test_bench_string += "  );\n\n";

  return test_bench_string;
}

string protocol_t::test_bench_body_init()
{
  string test_bench_string;

  test_bench_string += "  reg init_bit;\n";
  test_bench_string += "  reg next_bit;\n";
  test_bench_string += "  reg set_bit;\n";
  test_bench_string += "  reg test_bit;\n";
  test_bench_string += "\n";
  test_bench_string += "  initial begin\n\n";
  test_bench_string += "    init_bit = $init( $time );\n\n";
  test_bench_string += "    $dumpfile(\"test.vcd\");\n";
  test_bench_string += "    $dumpvars(0, " + this->protocol_name() + "_tb);\n";
  test_bench_string += "    \n";
  test_bench_string += "    $monitor(\"time=%d clk=%d reset=%d\",\n";
  test_bench_string += "      $time,\n";
  test_bench_string += "      clk,\n";
  test_bench_string += "      reset\n";
  test_bench_string += "    );\n";
  test_bench_string += "    \n";
  test_bench_string += "    clk = 0;\n";
  test_bench_string += "    reset = 1;\n";
  test_bench_string += "    #4\n";
  test_bench_string += "    reset = 0;\n";
  test_bench_string += "  end\n\n";

  return test_bench_string;
}

string protocol_t::test_bench_body_always()
{
  string test_bench_string;

  test_bench_string += "  always @(posedge clk) begin\n\n";
  test_bench_string += "    if ( !reset && $has_next( $time ) ) begin\n";
  test_bench_string += "      next_bit = $next( $time );\n";
  test_bench_string += "    end\n\n";
  test_bench_string += this->test_bench_body_always_next();
  test_bench_string += "\n";
  // test_bench_string += this->test_bench_body_always_set();
  test_bench_string += "\n";
  test_bench_string += "    if ( $ready( $time ) ) begin\n";
  test_bench_string += this->test_bench_body_always_test();
  test_bench_string += "    end\n";
  test_bench_string += "\n";
  test_bench_string += "    if ( $time > 20 ) begin\n";
  test_bench_string += "      $finish;\n";
  test_bench_string += "    end\n\n";
  test_bench_string += "  end\n";

  return test_bench_string;
}

string protocol_t::test_bench_body_always_next()
{
  string test_bench_string;

  uint32_t i, j;

  for(i=0; i<this->behavior.size(); i++) {
    fsm_t* fsm = this->behavior.at(i);

    test_bench_string += "    // fsm";
    test_bench_string += std::to_string( (unsigned long long int) i);
    test_bench_string += "\n";

    for(j=0; j<fsm->control_out.size(); j++) {
      condition_t* cond = fsm->control_out.at(j).cond;

      if (cond->type() == COND_CONTROL) {
        test_bench_string += "    ";
        test_bench_string += cond->get_name();
        test_bench_string += " = $get(\"";
        test_bench_string += cond->get_name();
        test_bench_string += "\");";
        test_bench_string += "\n";
      }
    }
  }

  // this part dosnt look right given that we have packet protocols.

  test_bench_string += "\n    //data in\n";
  for(i=0; i<this->ports->size(); i++) {
    port_t* port = this->ports->at(i);
    if (port->get_dir() == PORT_DIR_IN && port->type == PORT_TYPE_DATA) {
      test_bench_string += "    ";
      test_bench_string += port->get_name();
      test_bench_string += " = $get(\"";
      test_bench_string += port->get_name();
      test_bench_string += "\");";
      test_bench_string += "\n";
    }
  }

  return test_bench_string;
}

string protocol_t::test_bench_body_always_set()
{
  string test_bench_string;
  test_bench_string += "    //control out\n";

  uint32_t i;
  for(i=0; i<this->ports->size(); i++) {
    port_t* port = this->ports->at(i);
    if (port->get_dir() == PORT_DIR_OUT && port->type == PORT_TYPE_CONTROL) {
      test_bench_string += "    set_bit = $set(\"";
      test_bench_string += port->get_name();
      test_bench_string += "\", ";
      test_bench_string += port->get_name();
      test_bench_string += ");\n";
    }
  }

  return test_bench_string;
}

string protocol_t::test_bench_body_always_test()
{
  string test_bench_string;

  // dont need to do testing now.

/*
  test_bench_string += "      //data out\n";

  uint32_t i;
  for(i=0; i<this->ports->size(); i++) {
    port_t* port = this->ports->at(i);
    if (port->get_dir() == PORT_DIR_OUT && port->type == PORT_TYPE_DATA) {
      test_bench_string += "      test_bit = $test(\"";
      test_bench_string += port->get_name();
      test_bench_string += "\", ";
      test_bench_string += port->get_name();
      test_bench_string += ");\n";
    }
  }
*/

  return test_bench_string;
}

string protocol_t::test_bench_body()
{
  string test_bench_string;

  test_bench_string += "  always #1 clk = ~clk;\n";
  test_bench_string += "\n";
  test_bench_string += this->test_bench_body_init();
  test_bench_string += "\n\n";
  test_bench_string += this->test_bench_body_always();

  return test_bench_string;
}

/*
we currently dont include data
we only do control
this is because we dont want to double do control and data right now
  for packets
that requires work
we can manually enter that in and its fine.
*/

string protocol_t::test_bench_vec()
{
  string test_bench_string;

  int i, j, k, l;

  // header
  test_bench_string += "vec begin\n";

  // just going to use a set and a vector to do this.

  // could be smart and make the vector have a contains function
  // or 
  // could see if the set itr is in order

  vector_t<string> vec_header;
  set_t<string> vec_header_set;

  for(i=0; i<this->behavior.size(); i++) {
    fsm_t* fsm = this->behavior.at(i);

    for(j=0; j<fsm->control_out.size(); j++) {
      condition_t* cond = fsm->control_out.at(j).cond;

      if (cond->type() == COND_CONTROL) {
        string name = cond->get_name();

        if (!vec_header_set.contains(name)) {
          test_bench_string += name;
          test_bench_string += "\n";

          vec_header_set.put(name);
          vec_header.push_back(name);
        }
      }
    } // for(j=0; j<fsm->control_out.size(); j++) {
  } // for(i=0; i<this->behavior.size(); i++) {
  test_bench_string += "end\n\n";

  // vectors

/*
  for(i=0; i<this->psets.size(); i++) {
    path_set_t* next_pset = this->psets.at(i);

    for(j=0; j<next_pset->size; j++) {
      if (next_pset->has(j)) {
        path_t* next_p = next_pset->at(j);

        for(k=0; k<next_p->num_cycles(); k++) {
          edge_t* edge = next_p->edge_at(k).second;

          for(l=0; l<vec_header.size(); l++) {
            string cond = vec_header.at(l);

            if (edge->contains_condition(cond)) {
              uint32_t num = edge->get(cond);
              test_bench_string += std::to_string( (long long unsigned int) num );
              test_bench_string += " ";
            }
            else {
              test_bench_string += "0 ";
            }

          } // for(l=0; l<vec_header.size(); l++) {

        } // for(k=0; k<next_p->num_cycles(); k++) {

      } // if (next_pset->has(j)) {

    } // for(j=0; j<next_pset->size; j++) {

    test_bench_string += "\n";
  } // for(i=0; i<this->psets.size(); i++) {
*/

  for(i=0; i<this->sched.size(); i++) {
    edge_t** cycle = this->sched.at(i);
    
    map_t<string, uint32_t> cycle_vals;

    for(j=0; j<this->behavior.size(); j++) {
      edge_t* edge = cycle[j];
      if (edge == NULL) { continue; }

      for(k=0; k<vec_header.size(); k++) {
        string cond = vec_header.at(k);

        if (edge->contains_condition(cond)) {
          uint32_t num = edge->get(cond);
          cycle_vals.put(cond, num);
        }
      } // for(k=0; k<vec_header.size(); k++) {
    } // for(j=0; j<this->behavior.size(); j++) {
    for(j=0; j<vec_header.size(); j++) {
      string cond = vec_header.at(j);

      if (cycle_vals.contains(cond)) {
        uint32_t num = cycle_vals.get(cond);
        test_bench_string += std::to_string( (long long unsigned int) num );
        test_bench_string += " ";
      }
      else {
        test_bench_string += "0 ";
      }
    } // for(j=0; j<vec_header.size(); j++) {

    test_bench_string += "\n";
  } // for(i=0; i<this->sched.size(); i++) {

  return test_bench_string;

}

string protocol_t::makefile()
{

  string makefile_string;

  string translator_filename = this->protocol_name() + ".v";
  string testbench_filename = this->protocol_name() + "_tb.v";

  makefile_string += "verilog_depends = " + testbench_filename + " " + translator_filename + " ../tb/up_fifo.v ../tb/down_fifo.v ../tb/mux.v ../tb/increment.v\n\n";

  makefile_string += "vpi_depends = ../tb/sim_vpi.c ../tb/vec_file.c\n\n";

  makefile_string += "all: vpi_compile verilog_compile\n\n";

  makefile_string += "vpi_compile: $(vpi_depends)\n";
	makefile_string += "\tiverilog-vpi $(vpi_depends) `pkg-config --cflags --libs glib-2.0`\n\n";

  makefile_string += "verilog_compile: $(verilog_depends)\n";
	makefile_string += "\tiverilog -o sim_vpi.vvp $(verilog_depends) ../tb/sizes.sft\n\n";

  makefile_string += "run:\n";
	makefile_string += "\tvvp -M. -m sim_vpi sim_vpi.vvp\n\n";

  makefile_string += "clean:\n";
	makefile_string += "\trm *.vvp *.vpi *.o *.out *.vcd\n";

  return makefile_string;

}












