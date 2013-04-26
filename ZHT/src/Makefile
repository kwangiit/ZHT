
TARGETS = zht_c_binding zht_benchmark zht_server c_zhtclient_lanl c_zhtclient_lanl_threaded c_zhtclient_threaded_test
CC = gcc
CCFLAGS = -g -I${USER_INCLUDE} -L${USER_LIB}
LIBFLAGS = -lstdc++ -lrt -lpthread -lm -lc -lprotobuf -lprotobuf-c

all:	$(TARGETS)
	
c_zhtclient_threaded_test: c_zhtclient_threaded_test.o lock_guard.o c_zhtclient.o c_zhtclientStd.o cpp_zhtclient.o meta.pb-c.o lru_cache.o meta.pb.o net_util.o novoht.o zht_util.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)

c_zhtclient_lanl_threaded: c_zhtclient_lanl_threaded.o lock_guard.o c_zhtclient.o c_zhtclientStd.o cpp_zhtclient.o meta.pb-c.o lru_cache.o meta.pb.o net_util.o novoht.o zht_util.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)

c_zhtclient_lanl: c_zhtclient_lanl.o lock_guard.o c_zhtclient.o c_zhtclientStd.o cpp_zhtclient.o meta.pb-c.o lru_cache.o meta.pb.o net_util.o novoht.o zht_util.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)

zht_c_binding: c_zhtclient_main.o lock_guard.o c_zhtclient.o c_zhtclientStd.o cpp_zhtclient.o meta.pb-c.o lru_cache.o meta.pb.o net_util.o novoht.o zht_util.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)


zht_benchmark: benchmark_client.o cpp_zhtclient.o meta.pb-c.o lru_cache.o meta.pb.o net_util.o novoht.o zht_util.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)

zht_server: server_general.o meta.pb-c.o lru_cache.o meta.pb.o net_util.o novoht.o zht_util.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)



%.o: %.c
	$(CC) $(CCFLAGS) -c $< $^ $(LIBFLAGS)
	
%.o: %.cpp
	$(CC) $(CCFLAGS) -c $< $^ $(LIBFLAGS)
	
%.o: %.cc
	$(CC) $(CCFLAGS) -c $< $^ $(LIBFLAGS)

c_zhtclient_threaded_test.o: c_zhtclient_threaded_test.c
c_zhtclient_lanl_threaded.o: c_zhtclient_lanl_threaded.c
c_zhtclient_lanl.o:		c_zhtclient_lanl.c
c_zhtclient_main.o:		c_zhtclient_main.c
lock_guard.o:			lock_guard.cpp
c_zhtclient.o:			c_zhtclient.cpp
c_zhtclientStd.o:		c_zhtclientStd.cpp

benchmark_client.o:		benchmark_client.cpp
cpp_zhtclient.o:		cpp_zhtclient.cpp

server_general.o:		server_general.cpp

meta.pb-c.o:			meta.pb-c.c
lru_cache.o:			lru_cache.cpp	
meta.pb.o:				meta.pb.cc
net_util.o:				net_util.cpp
novoht.o:				novoht.cpp
zht_util.o:				zht_util.cpp

.PHONY:	clean
	

clean:
	rm *.o
	rm $(TARGETS)
	rm *.gch*
	rm *.bin*