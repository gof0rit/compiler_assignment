/* Resizable hash. */

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef const void *HKey;
typedef void *HData;

typedef struct bucket_node
{
  HKey key;
  HData data;
  struct bucket_node *next;
} Bucket;

typedef struct hhead
{
  size_t size;
  size_t capacity;
  Bucket **buckets;
  bool (*keq)(HKey k1, HKey k2);
  unsigned int (*khash)(HKey k, unsigned int m);
  void (*efree)(HData d);
} *HTab;

static inline bool key_eq(HTab t, HKey k1, HKey k2)
{
  return (*t->keq)(k1, k2);
}

static inline unsigned int key_hash(HTab t, HKey k, unsigned int m)
{
  return (*t->khash)(k, m);
}

HTab htab_new(size_t capacity,
              bool (*keq)(HKey k1, HKey k2),
              unsigned int (*khash)(HKey k, unsigned int m),
              void (*efree)(HData d))
{
  HTab t =      malloc(sizeof(struct hhead));
  t->size =     0;
  t->capacity = capacity;
  t->buckets =  calloc(capacity, sizeof(Bucket *));
  t->keq =      keq;
  t->khash =    khash;
  t->efree =    efree;

  return t;
}

static HData hlookup(HTab t, HKey k)
{
  int i = key_hash(t, k, t->capacity);
  Bucket* b = t->buckets[i];
  while (b != NULL)
  {
    if (key_eq(t, b->key, k))
      return b->data;
    else
      b = b->next;
  }

  return NULL;
}

static void htab_resize(HTab t, size_t new_capacity)
{
  Bucket **n_table = calloc(new_capacity, sizeof(Bucket *));
  Bucket **o_table = t->buckets;
  size_t idx = 0;

  for (; idx < t->capacity; idx++)
  {
    Bucket *b = o_table[idx];
    while (b != NULL)
    {
      Bucket *n = b->next;
      HData d = b->data;
      int h = key_hash(t, b->key, new_capacity);
      b->next = n_table[h];
      n_table[h] = b;
      b = n;
    }
  }

  free(o_table);

  /* NOTE: no change on size (the num of existing elems). */
  t->capacity = new_capacity;
  t->buckets = n_table;
  return;
}

static HData hinsert(HTab t, HKey k, HData d)
{
  unsigned int i = key_hash(t, k, t->capacity);

  Bucket *b = t->buckets[i];
  while (b != NULL)
  {
    if (key_eq(t, b->key, k))
    {
      /* NOTE: duplicated key will overwrite existing element and return it */
      HData t = b->data;
      b->data = d;
      return t;
    }
    else
    {
      b = b->next;
    }
  }

  b = malloc(sizeof(Bucket));
  b->key = k;
  b->data = d;
  b->next = t->buckets[i];
  t->buckets[i] = b;
  t->size++;

  if (t->size > t->capacity && t->capacity < UINT_MAX/2)
    htab_resize(t, 2 * t->capacity);

  return NULL;
}

static void bfree(Bucket *b, void (*efree)(HData d))
{
  while (b != NULL)
  {
    if (efree != NULL)
      (*efree)(b->data);

    Bucket *t = b->next;
    free(b);
    b = t;
  }

  return;
}

static void hfree(HTab t)
{
  size_t idx = 0;

  for (; idx < t->capacity; idx++)
    if (t->buckets[idx] != NULL)
      bfree(t->buckets[idx], t->efree);

  free(t->buckets);
  free(t);
}

/* Generic string compare. */
static bool hash_eq_str(HKey k1, HKey k2)
{
  return strcmp((char *)k1, (char *)k2) == 0;
}

/* Generate hash from string. */
static unsigned int hash_gen_str(HKey k, unsigned int m)
{
  unsigned int a = 1664525, b = 1013904223;
  unsigned int h = 0, idx = 0;
  char *s = (char *)k;
  size_t len = strlen(s);

  for (; idx < len; idx++)
  {
    h = h + (unsigned int)(s[idx]);
    h = h * a + b;
  }

  h = h % m;

  return h;
}

/* The initial size. */
#define STRMAP_SIZE 32
typedef struct hhead StrMap;

StrMap *strmap_new()
{
  StrMap *sm = htab_new(STRMAP_SIZE, &hash_eq_str, &hash_gen_str, NULL);
  return sm;
}

void strmap_free(StrMap *sm)
{
  hfree(sm);
}

void strmap_insert(StrMap *sm, const char *key, void *data)
{
  hinsert(sm, key, data);
}

void *strmap_find(StrMap *sm, const char *key)
{
  return hlookup(sm, key);
}


