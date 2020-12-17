#include "cgraph_constants.h"
#include "cgraph_datatype.h"
#include "cgraph_error.h"
#include "cgraph_interface.h"
#include "cgraph_iqueue.h"
#include "cgraph_ivec.h"
#include "cgraph_paths.h"
#include "cgraph_topology.h"
#include "cgraph_types_internal.h"

#include <stdlib.h>

#include <string.h>

int cgraph_is_dag(const cgraph_t *graph, bool *res) {
  *res = true;

  if(!cgraph_is_directed(graph)) {
    *res = false;
    return 0;
  }

  CGRAPH_INTEGER no_of_vertices = cgraph_vcount(graph);

  for(int i = 0; i < no_of_vertices; i++) {
    /* use bfs to verify if there is a path from one vertex to itself */
    cgraph_iqueue_t source = cgraph_iqueue_create();
    CGRAPH_INTEGER passed[no_of_vertices];
    CGRAPH_INTEGER pop;
    
    memset(passed, 0, no_of_vertices * sizeof(CGRAPH_INTEGER));

    cgraph_iqueue_enqueue(source, i);
    passed[i] = 1;

    while(!cgraph_iqueue_empty(source)) {
      cgraph_iqueue_poll(source, &pop);
      for(int j = graph->os[pop]; j < graph->os[pop + 1]; j++) {
        int node = graph->to[ graph->oi[j] ];

        if(!passed[node]) {
          passed[node] = 1;
          cgraph_iqueue_enqueue(source, node);
        }
        else if(passed[node] && node == i) {
          *res = false;
          cgraph_iqueue_free(&source);
          return 0;
        }

      }
    }

    cgraph_iqueue_free(&source);
  }
  return 0;
}

int cgraph_topological_sorting(const cgraph_t *graph,
                               cgraph_ivec_t *res,
                               cgraph_neimode_t mode)
{
  cgraph_ivec_init(res, 0);
  cgraph_neimode_t mode2;
  if (mode == CGRAPH_OUT)
    mode2 = CGRAPH_IN;
  if (mode == CGRAPH_IN)
    mode2 = CGRAPH_OUT;
  cgraph_ivec_t dgree = cgraph_ivec_create();
  cgraph_degree_all(graph, &dgree, mode2, true);
  CGRAPH_INTEGER ressize = 0;
  bool check = true;
  while (check)
  {
    CGRAPH_INTEGER indx = 0;
    cgraph_ivec_t nei = cgraph_ivec_create();
    while (dgree[indx] != 0 && indx < graph->n)
      indx++;
    if (indx == graph->n)
      check = false;
    else
    {
      dgree[indx]--;
      cgraph_neighbors(graph, &nei, indx, mode);
      for (CGRAPH_INTEGER j = 0; j < cgraph_ivec_size(nei); j++)
        dgree[nei[j]]--;
      cgraph_ivec_push_back(res, indx);
    }
  }
  return 0;
}

int cgraph_get_shortest_path_dijkstra(const cgraph_t *graph,
        cgraph_ivec_t *vertices,
        cgraph_ivec_t *edges,
        CGRAPH_INTEGER from,
        CGRAPH_INTEGER to,
        const double *weights,
        cgraph_neimode_t mode) {

  cgraph_ivec_free(vertices);
  cgraph_ivec_free(edges);
  *vertices = cgraph_ivec_create();
  *edges = cgraph_ivec_create();

  int index = 0;

  double total_weight[graph->n];
  CGRAPH_INTEGER tracking[graph->n];
  CGRAPH_INTEGER passed[graph->n];    /* is this node passed or not? */

  for(int i = 0; i < graph->n; i++) {
    total_weight[i] = (double) 1000000007;
    tracking[i] = -1;
    passed[i] = 0;
  }
  tracking[from] = from;
  
  cgraph_iqueue_t bfs = cgraph_iqueue_create();
  // cgraph_iqueue_enqueue(bfs, from);

  cgraph_ivec_t nei = cgraph_ivec_create();
  /* base step */
  cgraph_neighbors(graph, &nei, from, mode);
  total_weight[from] = 0;
  for(int i = 0; i < cgraph_ivec_size(nei); i++) {
    for(index = 0; index < graph->n; index++) {
      if(mode == CGRAPH_OUT && 
        graph->from[index] == from && graph->to[index] == nei[i]) {
        total_weight[ nei[i] ] = weights[index];
        break;
      }
      else if(mode == CGRAPH_IN &&
            graph->to[index] == from && graph->from[index] == nei[i]) {
        total_weight[ nei[i] ] = weights[index];
        break;
      }
    }
    tracking[ nei[i] ] = from;
    cgraph_iqueue_enqueue(bfs, nei[i]);
  }

  while(!cgraph_iqueue_empty(bfs)) {
    CGRAPH_INTEGER out;
    cgraph_iqueue_poll(bfs, &out);
    passed[out] = 1;

    cgraph_neighbors(graph, &nei, out, mode);
    for(int i = 0; i < cgraph_ivec_size(nei); i++) {
      /* find weight's index */
      for(index = 0; index < cgraph_ivec_size(graph->from); index++) {
        if(mode == CGRAPH_OUT &&
          graph->from[index] == out && graph->to[index] == nei[i]) {
          break;
        }
        else if(mode == CGRAPH_IN &&
              graph->to[index] == from && graph->from[index] == nei[i]) {
          break;
        }
      }
      if(!passed[ nei[i] ] || 
          weights[index] + total_weight[out] < total_weight[ nei[i] ]) {

        cgraph_iqueue_enqueue(bfs, nei[i]);

        if(weights[index] + total_weight[out] < total_weight[ nei[i] ]) {
          total_weight[ nei[i] ] = weights[index] + total_weight[out];
          tracking[ nei[i] ] = out;
        }
      }
    }
  }

  CGRAPH_INTEGER i = to;
  while(true) {
    if(i == from) {
      cgraph_ivec_push_back(vertices, from);
      break;
    }
    else {
      cgraph_ivec_push_back(vertices, i);
      i = tracking[i];
    }
  }
  if(to == 3) {
    printf("size: %d\n", cgraph_ivec_size(*vertices));
    for(int i = 0; i < cgraph_ivec_size(*vertices); i++) {
      printf("%d ", (*vertices)[i]);
    }
  }
  /* we need to reverse the vector vertices */
  for(int i = 0; i < cgraph_ivec_size(*vertices) / 2; i++) {
    int swap = (*vertices)[i];
    (*vertices)[i] = (*vertices)[cgraph_ivec_size(*vertices) - i - 1];
    (*vertices)[cgraph_ivec_size(*vertices) - i - 1] = swap;
  }

  /* vector edges */
  for(int i = 0; i < cgraph_ivec_size(*vertices) - 1; i++) {
      for(index = 0; index < cgraph_ivec_size(graph->from); index++) {
        if(mode == CGRAPH_OUT &&
          graph->from[index] == (*vertices)[i] && 
          graph->to[index] == (*vertices)[i+1]) {
          cgraph_ivec_push_back(edges, index);
          break;
        }
        else if(mode == CGRAPH_IN &&
          graph->to[index] == (*vertices)[i] &&
          graph->from[index] == (*vertices)[i+1]) {
            cgraph_ivec_push_back(edges, index);
            break;
          }
      }
  }

  cgraph_ivec_free(&nei);
  cgraph_iqueue_free(&bfs);

  return 0;
}

int cgraph_get_shortest_path(const cgraph_t *graph,
        cgraph_ivec_t *vertices,
        cgraph_ivec_t *edges,
        CGRAPH_INTEGER from,
        CGRAPH_INTEGER to,
        cgraph_neimode_t mode) {
  CGRAPH_INTEGER no_of_edges = cgraph_ecount(graph);
  cgraph_rvec_t weights = cgraph_rvec_create();
  cgraph_rvec_init(&weights, no_of_edges);
  for (CGRAPH_INTEGER i = 0; i < no_of_edges; ++i) {
    weights[i] = 1.0;
  }
  CGRAPH_INTEGER ret = cgraph_get_shortest_path_dijkstra(graph, vertices, edges, from, to, weights, mode);
  cgraph_rvec_free(&weights);
  return ret;
}