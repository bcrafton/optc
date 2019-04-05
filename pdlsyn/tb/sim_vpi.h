#ifndef SIM_VPI_H
#define SIM_VPI_H

#include <vpi_user.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "vec_file.h"

PLI_INT32 next(char* user_data);
PLI_INT32 has_next(char* user_data);
PLI_INT32 get(char* user_data);
PLI_INT32 set(char* user_data);
PLI_INT32 ready(char* user_data);
PLI_INT32 test(char* user_data);

#endif
