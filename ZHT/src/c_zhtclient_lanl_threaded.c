/*
 *   Created on: Apr 24, 2013
 *      Author: Xiaobing Zhou
 */

/*
 The following is sequence you need to run this test case(c_zhtclient_lanl_threaded.c).
 You can also only run step 3 to see what will happened.

 1. make

 2. to insert 1000 nodes, run
 ./c_zhtclient_lanl_threaded -n neighbor -z zht.cfg -p TCP -i

 3. to concurrently allocate nodes by comp_and_swap, run
 ./c_zhtclient_lanl_threaded -n neighbor -z zht.cfg -p TCP


 => I changed the startup arguments only for this test programs, e.g. ./c_zhtclient_lanl_threaded -n neighbor -z zht.cfg -p TCP
 to see the usage, run
 ./c_zhtclient_lanl_threaded or
 ./c_zhtclient_lanl_threaded -h


 4. to to check nodes finally available, run

 ./c_zhtclient_lanl_threaded -n neighbor -z zht.cfg -p TCP -l


 ###########special attention###########
 1. Since the ZHT Client API is not thread-safe, this testcase is a expediency to make sure your use case(if based on the code here) works ,at least.

 2. I used the criticial section to make sure your application is thread-safe, although ZHT Client API is not.

 3. copy the code and adjust it on your demand, but take care the critical section code.

 */

#include   <stdbool.h>
#include   <stdlib.h>
#include   <stdio.h>

#include   <string.h>
#include "c_zhtclient.h"
#include "meta.pb-c.h"

#include <pthread.h>
#include <assert.h>
#include <stdio.h>

/*concurrent threads to request c_zht_compare_and_swap*/
#define CONCUR_DEGREE 5

/*the number of nodes every thread requests*/
#define NODES_ALLOC  100

/*return code, means no more nodes available*/
#define RC_NO_NODES_AVAIL -1000

/*the number of nodes initially in the pool*/
char *NODES_POOL = "1000";

/*the key for nodes resource*/
char *key = "node_res";

/*size of buffer to store lookup result*/
const int LOOKUP_SIZE = 1024 * 2; //size of buffer to store lookup result, larger enough than TOTAL_SIZE

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int lookup_nodes() {

	size_t ln;
	char *result = (char*) calloc(LOOKUP_SIZE, sizeof(char));

	if (result != NULL) {

		int lret = c_zht_lookup2(key, result, &ln); //1 for look up, 2 for remove, 3 for insert, 4 for append

		/*		fprintf(stdout, "[%d] c_zht_lookup, return code(length): %d(%lu)\n",
		 seq_num, lret, ln);*/

		fprintf(stdout,
				"[%lu] c_zht_lookup, return {key}:{value}=> {%s}:{%s}, rc(%d)\n",
				pthread_self(), key, result, lret);
	}

	int nodes = atoi(result);

	free(result);

	return nodes;
}

int compare_swap_internal() {

	int nodes_avail = lookup_nodes();

	int nodes_left = nodes_avail - NODES_ALLOC; //allocate 100 nodes

	/*todo: guard the case: nodes_left < 0, means no nodes available*/
	if (nodes_left < 0)
		return RC_NO_NODES_AVAIL; //for example, -1000 means no nodes available

	char p_nodes_avail[20];
	sprintf(p_nodes_avail, "%d", nodes_avail);

	char p_nodes_left[20];
	sprintf(p_nodes_left, "%d", nodes_left);

	int rc = c_zht_compare_and_swap(key, p_nodes_avail, p_nodes_left);

	fprintf(stdout,
			"[%lu] c_zht_compare_and_swap, {seen_value}:{new_value} => {%s}:{%s}, rc(%d)\n",
			pthread_self(), p_nodes_avail, p_nodes_left, rc);

	return rc;
}

void* compare_swap(void *arg) {

	pthread_mutex_lock(&mutex);

	int rc = compare_swap_internal();

	if (rc == 1) {

//		return NULL;
	} else if (rc == RC_NO_NODES_AVAIL) {

//		return NULL; //todo: here,  you may need to return RC_NO_NODES_AVAIL to caller application
	} else {

		while (rc == 0) {

			compare_swap_internal();
		}
	}

	pthread_mutex_unlock(&mutex);
}

void compare_swap_final() {

	int nodes_avail = lookup_nodes();

	int nodes_pool = atoi(NODES_POOL);

	/*nodes_avail = (1000 - 5 * 100), since 5 threads request 100 nodes each once a time*/
	assert(nodes_avail == (nodes_pool - CONCUR_DEGREE * NODES_ALLOC));
}

void insert_resource() {

	char *nodes = NODES_POOL;

	int rc = c_zht_insert2(key, nodes);

	fprintf(stdout, "[%lu] init [%s] nodes available, rc(%d)\n", pthread_self(),
			nodes, rc);
}

void test_compare_swap() {

	insert_resource();

	int tc = CONCUR_DEGREE;
	pthread_t threads[tc];

	int i;
	for (i = 0; i < tc; i++) {

		int th_num = i + 1;
		pthread_create(&threads[i], NULL, compare_swap, (void*) &th_num);
	}

	for (i = 0; i <= tc; i++) {
		pthread_join(threads[i], NULL);
	}

	compare_swap_final();
}

void printUsage(char *argv_0);

int main(int argc, char **argv) {

	extern char *optarg;

	double us = 0;
	int printHelp = 0;

	char *neighbor = NULL;
	char *zht_cfg = NULL;
	char *protocol = NULL;
	int is_init = 0;
	int is_lookup = 0;

	int c;
	while ((c = getopt(argc, argv, "n:z:p:ilh")) != -1) {
		switch (c) {
		case 'n':
			neighbor = optarg;
			break;
		case 'z':
			zht_cfg = optarg;
			break;
		case 'p':
			protocol = optarg;
			break;
		case 'i':
			is_init = 1;
			break;
		case 'l':
			is_lookup = 1;
			break;
		case 'h':
			printHelp = 1;
			break;
		default:
			fprintf(stdout, "Illegal argument \"%c\"\n", c);
			printUsage(argv[0]);
			exit(1);
		}
	}

	if (printHelp) {
		printUsage(argv[0]);
		exit(1);
	}

	if (neighbor != NULL && zht_cfg != NULL && protocol != NULL) {

		bool useTCP = false;

		if (!strcmp("TCP", protocol)) {
			useTCP = true;
		} else {
			useTCP = false;
		}

		/*init...*/
		c_zht_init(neighbor, zht_cfg, useTCP); //neighbor zht.cfg TCP

		if (is_init) {

			/*e.g. init node pool with 1000 nodes*/
			insert_resource();
		} else if (is_lookup) {

			/*lookup nodes available*/
			lookup_nodes();
		} else {

			/*test concurrent compare_and_swap*/
			test_compare_swap();
		}

		/*clear...*/
		c_zht_teardown();

	} else {

		printUsage(argv[0]);
		exit(1);
	}

	return 0;
}

void printUsage(char *argv_0) {

	fprintf(stdout, "Usage:\n%s %s\n", argv_0,
			"{-n neighbor -z zht.cfg -p TCP|UDP -i(init)} | -l(lookup) | {-h(help)}");
}

