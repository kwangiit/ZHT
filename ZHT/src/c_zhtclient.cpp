/*
 #include "../inc/c_zhtclient.h"
 #include "../inc/c_zhtclientStd.h"
 */

#include "c_zhtclient.h"
#include "c_zhtclientStd.h"
#include "lock_guard.h"
#include <pthread.h>

ZHTClient_c zhtClient;

pthread_mutex_t c_zht_client_mutex;

int c_zht_init(const char *memberConfig, const char *zhtConfig, bool tcp) {

	pthread_mutex_init(&c_zht_client_mutex, NULL);

	return c_zht_init_std(&zhtClient, memberConfig, zhtConfig, tcp);
}

int c_zht_lookup(const char *pair, char *result, size_t *n) {

	lock_guard lock(&c_zht_client_mutex);

	return c_zht_lookup_std(zhtClient, pair, result, n);
}

int c_zht_lookup2(const char *key, char *result, size_t *n) {

	lock_guard lock(&c_zht_client_mutex);

	return c_zht_lookup2_std(zhtClient, key, result, n);
}

int c_zht_remove(const char *pair) {

	lock_guard lock(&c_zht_client_mutex);

	return c_zht_remove_std(zhtClient, pair);
}

int c_zht_remove2(const char *key) {

	lock_guard lock(&c_zht_client_mutex);

	return c_zht_remove2_std(zhtClient, key);
}

int c_zht_insert(const char *pair) {

	lock_guard lock(&c_zht_client_mutex);

	return c_zht_insert_std(zhtClient, pair);
}

int c_zht_insert2(const char *key, const char *value) {

	lock_guard lock(&c_zht_client_mutex);

	return c_zht_insert2_std(zhtClient, key, value);
}

int c_zht_append(const char *pair) {

	lock_guard lock(&c_zht_client_mutex);

	return c_zht_append_std(zhtClient, pair);
}

int c_zht_append2(const char *key, const char *value) {

	lock_guard lock(&c_zht_client_mutex);

	return c_zht_append2_std(zhtClient, key, value);
}

int c_zht_teardown() {

	pthread_mutex_destroy(&c_zht_client_mutex);

	return c_zht_teardown_std(zhtClient);
}

int c_zht_compare_and_swap(const char *key, const
char *seen_value, const char *new_value) {

	lock_guard lock(&c_zht_client_mutex);

	return c_zht_compare_and_swap_std(zhtClient, key, seen_value, new_value);
}

