

#include "defines.h"
#include "ast.h"
#include "transition.h"

using namespace std;
using namespace boost;

int main()
{
  value_t* val = new value_t(VALUE_TYPE_NUMBER, NULL, 150);
  value_t* val_cp = val->copy();
  assert(val != val_cp && val->num == val_cp->num);

  string* portname = new string("string");
  state_t* state = new state_t(portname, val);
  state_t* state_cp = state->copy();
  assert(state != state_cp && state->portname != state_cp->portname);

  state_list_t* states = new state_list_t();
  states->push_back(state);
  state_list_t* states_cp = states->copy();
  assert(states_cp != states);
  assert(states_cp->at(0) != states->at(0));
  assert(states_cp->at(0)->portname != states->at(0)->portname);
  assert(states_cp->at(0)->val->num == states->at(0)->val->num);

  states_cp->at(0)->val->num = 5;

  string state_list_string = states_cp->to_string();
  printf("%s\n", state_list_string.c_str());

  state_list_string = states->to_string();
  printf("%s\n", state_list_string.c_str());
}
