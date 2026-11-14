#ifndef DB_H
#define DB_H

#include <stddef.h>

const char* db_get(const char* key, size_t key_len, size_t* value_len);
int db_set(const char* key, size_t key_len, const char* value,
           size_t value_len);

#endif
