
#include "sim_vpi.h"

#define uint32_t unsigned int
#define uint8_t unsigned char 

static uint32_t count;
static GTree* ports;

PLI_INT32 init(char* user_data)
{    
  ports = load_table();
  return 0; 
}

PLI_INT32 next(char* user_data)
{    
  assert(user_data == NULL);

  count++;

  return 0; 
}

PLI_INT32 has_next(char* user_data)
{    
  assert(user_data == NULL);
  vpiHandle vhandle;
  vhandle = vpi_handle(vpiSysTfCall, NULL);

  /////////////////////////////////////////////////////////////
  // create the output vector
  s_vpi_value out;
  out.format = vpiVectorVal;
  out.value.vector = (s_vpi_vecval*) malloc(sizeof(s_vpi_vecval) * 1);

  out.value.vector[0].aval = 1;
  out.value.vector[0].bval = 0;

  /////////////////////////////////////////////////////////////
  // put the output vector
  vpi_put_value(vhandle, &out, NULL, vpiNoDelay);

  /////////////////////////////////////////////////////////////

  return 0; 
}

PLI_INT32 get(char* user_data)
{    
  assert(user_data == NULL);
  vpiHandle vhandle, iterator, arg;
  vhandle = vpi_handle(vpiSysTfCall, NULL);

  s_vpi_value inval;
  
  char* name;

  /////////////////////////////////////////////////////////////
  // get the NAME
  iterator = vpi_iterate(vpiArgument, vhandle);

  arg = vpi_scan(iterator);
  inval.format = vpiStringVal;
  vpi_get_value(arg, &inval);
  name = inval.value.str;
  if(name == NULL) {
    fprintf(stderr, "Expected a name\n");
    assert(name != NULL);
  }

  /////////////////////////////////////////////////////////////
  // create the output vector
  s_vpi_value out;
  out.format = vpiVectorVal;
  out.value.vector = (s_vpi_vecval*) malloc( sizeof(s_vpi_vecval) * 2 );

  out.value.vector[0].bval = 0;
  out.value.vector[1].bval = 0;

  GString* key = g_string_new_len(name, sizeof(char) * 50);
  if ( g_tree_lookup(ports, key) == NULL ) {
     out.value.vector[0].aval = 0;
     out.value.vector[1].aval = 0;
  }
  else {
    // thought we wud have to do count-1 here. but we dont.
    GArray* vals = g_tree_lookup(ports, key);
    if (count < vals->len) {
      uint64_t get_val = g_array_index(vals, uint64_t, count);
      out.value.vector[0].aval = get_val;
      out.value.vector[1].aval = (get_val >> 32);
    }
    else {
      out.value.vector[0].aval = 0;
      out.value.vector[1].aval = 0;
    }
  }

  /////////////////////////////////////////////////////////////
  // put the output vector
  vpi_put_value(vhandle, &out, NULL, vpiNoDelay);

  /////////////////////////////////////////////////////////////

  return 0; 
}

PLI_INT32 set(char* user_data)
{    
  assert(user_data == NULL);
  vpiHandle vhandle, iterator, arg;
  vhandle = vpi_handle(vpiSysTfCall, NULL);

  s_vpi_value inval;
  
  // does not work, need a deep copy.
  // char* name;
  char name[100];
  unsigned int value;

  /////////////////////////////////////////////////////////////
  // get the NAME
  iterator = vpi_iterate(vpiArgument, vhandle);

  arg = vpi_scan(iterator);
  inval.format = vpiStringVal;
  vpi_get_value(arg, &inval);

  // need a deep copy.
  // name = inval.value.str;

  if(inval.value.str == NULL) {
    fprintf(stderr, "Expected a name\n");
    assert(inval.value.str != NULL);
  } 
  else {
    strcpy(name, inval.value.str);
  }

  /////////////////////////////////////////////////////////////
  // get the value
  arg = vpi_scan(iterator);
  inval.format = vpiVectorVal;
  vpi_get_value(arg, &inval);
  value = inval.value.vector[0].aval;
  if(inval.value.vector[0].bval > 0) {
    // fprintf(stderr, "Expected no garbage : %s\n", name);
    // assert( !(inval.value.vector[0].bval > 0) );
  }

  return 0; 
}

PLI_INT32 ready(char* user_data)
{    
  assert(user_data == NULL);
  vpiHandle vhandle;
  vhandle = vpi_handle(vpiSysTfCall, NULL);

  /////////////////////////////////////////////////////////////
  // create the output vector
  s_vpi_value out;
  out.format = vpiVectorVal;
  out.value.vector = (s_vpi_vecval*) malloc(sizeof(s_vpi_vecval) * 1);

  out.value.vector[0].aval = 1;
  out.value.vector[0].bval = 0;

  /////////////////////////////////////////////////////////////
  // put the output vector
  vpi_put_value(vhandle, &out, NULL, vpiNoDelay);

  /////////////////////////////////////////////////////////////

  return 0; 
}

PLI_INT32 test(char* user_data)
{    
  assert(user_data == NULL);
  vpiHandle vhandle, iterator, arg;
  vhandle = vpi_handle(vpiSysTfCall, NULL);

  s_vpi_value inval;
  
  // does not work, need a deep copy.
  // char* name;
  char name[100];
  unsigned int value;

  /////////////////////////////////////////////////////////////
  // get the NAME
  iterator = vpi_iterate(vpiArgument, vhandle);

  arg = vpi_scan(iterator);
  inval.format = vpiStringVal;
  vpi_get_value(arg, &inval);

  // need a deep copy.
  // name = inval.value.str;

  if(inval.value.str == NULL) {
    fprintf(stderr, "Expected a name\n");
    assert(inval.value.str != NULL);
  } 
  else {
    strcpy(name, inval.value.str);
  }

  /////////////////////////////////////////////////////////////
  // get the value
  arg = vpi_scan(iterator);
  inval.format = vpiVectorVal;
  vpi_get_value(arg, &inval);
  value = inval.value.vector[0].aval;
  if(inval.value.vector[0].bval > 0) {
    // fprintf(stderr, "Expected no garbage : %s\n", name);
    // assert( !(inval.value.vector[0].bval > 0) );
  }

  return 0; 
}

PLI_INT32 get_ret_size(PLI_BYTE8 *user_data)
{
  return 64;
}

void init_register(void)
{
  s_vpi_systf_data tf_data;
  tf_data.type        = vpiSysFunc;
  tf_data.sysfunctype = vpiIntFunc;
  tf_data.tfname    = "$init";
  tf_data.calltf    = init;
  tf_data.compiletf = 0;
  tf_data.sizetf    = 0;
  tf_data.user_data = 0;
  vpi_register_systf(&tf_data);
}

void next_register(void)
{
  s_vpi_systf_data tf_data;
  tf_data.type        = vpiSysFunc;
  tf_data.sysfunctype = vpiIntFunc;
  tf_data.tfname    = "$next";
  tf_data.calltf    = next;
  tf_data.compiletf = 0;
  tf_data.sizetf    = 0;
  tf_data.user_data = 0;
  vpi_register_systf(&tf_data);
}

void has_next_register(void)
{
  s_vpi_systf_data tf_data;
  tf_data.type        = vpiSysFunc;
  tf_data.sysfunctype = vpiIntFunc;
  tf_data.tfname    = "$has_next";
  tf_data.calltf    = has_next;
  tf_data.compiletf = 0;
  tf_data.sizetf    = 0;
  tf_data.user_data = 0;
  vpi_register_systf(&tf_data);
}

void get_register(void)
{
  s_vpi_systf_data tf_data;
  tf_data.type        = vpiSysFunc;
  tf_data.sysfunctype = vpiSizedFunc;
  tf_data.tfname      = "$get";
  tf_data.calltf      = get;
  tf_data.compiletf   = 0;
  tf_data.sizetf      = get_ret_size;
  tf_data.user_data   = 0;
  vpi_register_systf(&tf_data);
}

void set_register(void)
{
  s_vpi_systf_data tf_data;
  tf_data.type        = vpiSysFunc;
  tf_data.sysfunctype = vpiIntFunc;
  tf_data.tfname    = "$set";
  tf_data.calltf    = set;
  tf_data.compiletf = 0;
  tf_data.sizetf    = 0;
  tf_data.user_data = 0;
  vpi_register_systf(&tf_data);
}

void ready_register(void)
{
  s_vpi_systf_data tf_data;
  tf_data.type        = vpiSysFunc;
  tf_data.sysfunctype = vpiIntFunc;
  tf_data.tfname    = "$ready";
  tf_data.calltf    = ready;
  tf_data.compiletf = 0;
  tf_data.sizetf    = 0;
  tf_data.user_data = 0;
  vpi_register_systf(&tf_data);
}

void test_register(void)
{
  s_vpi_systf_data tf_data;
  tf_data.type        = vpiSysFunc;
  tf_data.sysfunctype = vpiIntFunc;
  tf_data.tfname    = "$test";
  tf_data.calltf    = test;
  tf_data.compiletf = 0;
  tf_data.sizetf    = 0;
  tf_data.user_data = 0;
  vpi_register_systf(&tf_data);
}

void (*vlog_startup_routines[])() = {
  init_register,
  next_register,
  has_next_register,
  get_register,
  set_register,
  ready_register,
  test_register,
  0
};

