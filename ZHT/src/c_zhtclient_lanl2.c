#include   <stdbool.h>
#include   <stdlib.h>
#include   <stdio.h>

#include   <string.h>
#include "c_zhtclient.h"
#include "meta.pb-c.h"

#include <pthread.h>
#include <assert.h>
#include <stdio.h>

char *key = "node_resource";

const int LOOKUP_SIZE = 1024 * 2; //size of buffer to store lookup result, larger enough than TOTAL_SIZE

int lookup_nodes(int thread_num) {

	size_t ln;
	char *result = (char*) calloc(LOOKUP_SIZE, sizeof(char));

	if (result != NULL) {

		int lret = c_zht_lookup2(key, result, &ln); //1 for look up, 2 for remove, 3 for insert, 4 for append

		/*		fprintf(stdout, "[%d] c_zht_lookup, return code(length): %d(%lu)\n",
		 seq_num, lret, ln);*/

		fprintf(stdout,
				"[%d] c_zht_lookup, return {key}:{value}=> {%s}:{%s}, rc(%d)\n",
				thread_num, key, result, lret);
	}

	int nodes = atoi(result);

	free(result);

	return nodes;
}

int compare_swap_internal(int thread_num) {

	int nodes_avail = lookup_nodes(thread_num);

	int nodes_left = nodes_avail - 100;

	char p_nodes_avail[20];
	sprintf(p_nodes_avail, "%d", nodes_avail);

	char p_nodes_left[20];
	sprintf(p_nodes_left, "%d", nodes_left);

	int rc = c_zht_compare_and_swap(key, p_nodes_avail, p_nodes_left);

	return rc;
}

void* compare_swap(void *arg) {

//	int thread_num = *((int*) arg);
	int *pthread_num = (int*) arg;
	int thread_num = *pthread_num;

	int rc = compare_swap_internal(thread_num);

	while (rc == -5) {

		compare_swap_internal(thread_num);
	}
}

void compare_swap_final() {

	int thread_mock = 6;
	int nodes_avail = lookup_nodes(thread_mock);

	assert(nodes_avail == 900);
}

void insert_resource() {

	char *nodes = "1000";

	int rc = c_zht_insert2(key, nodes);

	fprintf(stdout, "init [%s] nodes available, rc(%d)\n", nodes, rc);
}

void test_compare_swap() {

	insert_resource();

	int tc = 1;
	pthread_t threads[tc];

	int i;
	for (i = 1; i <= tc; i++) {

		pthread_create(&threads[i], NULL, compare_swap, (void*) &i);
	}

	for (i = 1; i <= tc; i++) {
		pthread_join(threads[i], NULL);
	}

//	compare_swap_final();

}

int main(int argc, char **argv) {

	if (argc < 4) {

		printf("Usage: %s", "###.exe neighbor zht.cfg TCP\n");

		return -1;
	}

	bool useTCP = false;
	char *tcpFlag = argv[3];

	if (!strcmp("TCP", tcpFlag)) {
		useTCP = true;
	} else {
		useTCP = false;
	}

	c_zht_init(argv[1], argv[2], useTCP); //neighbor zht.cfg TCP

	test_compare_swap();

	c_zht_teardown();

	return 0;
}
