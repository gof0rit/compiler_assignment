#ifndef C_EXTLIB_STRMAP
#define C_EXTLIB_STRMAP

struct hhead;
typedef struct hhead StrMap;

StrMap *strmap_new();
void strmap_free(StrMap *sm);

void strmap_insert(StrMap *sm, const char *name, void *data);
void *strmap_find(StrMap *sm, const char *name);

#endif

