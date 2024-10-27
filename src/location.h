#ifndef PANDA_LOCATION_H
#define PANDA_LOCATION_H

typedef struct {
  char *file;
  int line;
  int column;
} Location;

#endif
