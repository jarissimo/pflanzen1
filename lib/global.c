#include "global.h"

#ifndef GLOBAL_C_GUARD
#define GLOBAL_C_GUARD

//XXX this should be const but it seems C forbids this?
nodeid_t NODE_ID;

/* interval of measurements in microseconds*/
uint32_t MEASUREMENT_INTERVAL = 5 * 1000000;

#endif /* end of include guard: GLOBAL_C_GUARD */
