#include "db.h"

#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 1024

typedef struct entry {
  char* key;
  size_t klen;
  char* val;
  size_t vlen;
  struct entry* next;
} entry_t;

static entry_t* table[TABLE_SIZE];

static char* dup_bytes(const char* src, size_t len) {
  char* p = malloc(len ? len : 1);
  if (p) memcpy(p, src, len);
  return p;
}

static unsigned long hash_key(const char* key, size_t len) {
  unsigned long hash = 5381;

  for (size_t i = 0; i < len; i++) {
    hash = hash * 33 + (unsigned char)key[i];
  }
  return hash;
}

static entry_t* find_entry(const char* key, size_t key_len) {
  size_t idx = hash_key(key, key_len) % TABLE_SIZE;
  entry_t* entry = table[idx];

  while (entry) {
    if (entry->klen == key_len &&
        memcmp(entry->key, key, key_len) == 0) {
      return entry;
    }
    entry = entry->next;
  }
  return NULL;
}

const char* db_get(const char* key, size_t key_len, size_t* value_len) {
  entry_t* entry = find_entry(key, key_len);
  if (!entry) return NULL;

  *value_len = entry->vlen;
  return entry->val;
}

int db_set(const char* key, size_t key_len, const char* value,
           size_t value_len) {
  entry_t* entry = find_entry(key, key_len);
  if (entry) {
    char* new_value = dup_bytes(value, value_len);
    if (!new_value) return -1;
    free(entry->val);
    entry->val = new_value;
    entry->vlen = value_len;
    return 0;
  }

  entry = malloc(sizeof(entry_t));
  if (!entry) return -1;
  entry->key = dup_bytes(key, key_len);
  entry->val = dup_bytes(value, value_len);
  if (!entry->key || !entry->val) {
    free(entry->key);
    free(entry->val);
    free(entry);
    return -1;
  }

  entry->klen = key_len;
  entry->vlen = value_len;
  size_t idx = hash_key(key, key_len) % TABLE_SIZE;
  entry->next = table[idx];
  table[idx] = entry;
  return 0;
}
