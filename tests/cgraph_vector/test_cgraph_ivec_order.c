#include <stdio.h>

#include "cgraph_vector.h"
#include "ut.h"

int main() {
  cgraph_ivec_t v = cgraph_ivec_fromarray((CGRAPH_INTEGER []){1, 0, 2, 2, 3, 1}, 6);
  cgraph_ivec_t v2 = cgraph_ivec_fromarray((CGRAPH_INTEGER []){3, 2, 1, 3, 2, 0}, 6);
  cgraph_ivec_t res = cvector_create_empty();
  cgraph_ivec_init(&res, 6);
  cgraph_ivec_order(v, v2, res);
  if (cvector_size(res) != 6) {
    printf("%s: Failed\n", "Test ivec init size");
    return 1;
  }
  if (!cgraph_ivec_equal(res, (CGRAPH_INTEGER[]){1, 5, 0, 2, 3, 4})) {
    printf("%s: Failed\n", "Test index");
    return 1;
  }
  printf("%s: OK\n", "cgraph_ivec_order test");
  return 0;
}