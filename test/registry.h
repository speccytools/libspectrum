#ifndef TEST_REGISTRY_H
#define TEST_REGISTRY_H

#include <stddef.h>

#include "test.h"

typedef test_return_t (*test_fn)( void );

typedef struct test_description {

  test_fn test;
  const char *name;
  const char *description;
  int active;

} test_description;

extern test_description tests[];
extern size_t test_count;

#endif
