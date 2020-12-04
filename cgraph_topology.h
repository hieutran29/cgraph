#ifndef CGRAPH_TOPOLOGY_H_
#define CGRAPH_TOPOLOGY_H_

#include "cgraph_datatype.h"

int cgraph_is_dag(const cgraph_t *graph, bool *res);
int cgraph_topological_sorting(const cgraph_t *graph, 
                               cgraph_ivec_t *res,
                               cgraph_neimode_t mode);

#endif  // CGRAPH_TOPOLOGY_H_