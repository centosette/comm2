CC          = gcc
CLINKER     = $(CC)
INCLUDE     = -I .
LINK_OPT_RT = -lrt
LINK_OPT_M  = -lm
DEBUG_FLAGS = -ggdb
LINK_OPT_GMP = -lgmp
COMMON_DEPS    = utils.h config.h protocol.h ipc/tcp.h
SERVER_SPECIFIC_DEPS = 
CLIENT_SPECIFIC_DEPS = client.h

SERVER_DEPS    = $(COMMON_DEPS) $(SERVER_SPECIFIC_DEPS)
CLIENT_DEPS    = $(COMMON_DEPS) $(CLIENT_SPECIFIC_DEPS)
COMMON_OBJECTS = protocol.o utils.o 
CLIENT_SPECIFIC_OBJECTS = client.o ipc/tcpclient.o
SERVER_SPECIFIC_OBJECTS = ipc/tcpserver.o listener.o
CLIENT_OBJECTS = $(COMMON_OBJECTS) $(CLIENT_SPECIFIC_OBJECTS)
SERVER_OBJECTS = $(COMMON_OBJECTS) $(SERVER_SPECIFIC_OBJECTS)
ALL_OBJECTS = $(CLIENT_OBJECTS) $(SERVER_SPECIFIC_OBJECTS) $(CLIENT_SPECIFIC_OBJECTS)
ALL_DEPS = $(COMMON_DEPS) $(CLIENT_SPECIFIC_DEPS) $(SERVER_SPECIFIC_DEPS)


alltest: test_client test_server test_bigint
	touch ./makealltest

test_client: $(CLIENT_OBJECTS) $(CLIENT_DEPS) test_client.c test_client.h
	$(CC) -o test_client $^\
 $(INCLUDE) \
 $(LINK_OPT_RT) \
 $(DEBUG_FLAGS)

test_server: $(SERVER_OBJECTS) $(SERVER_DEPS) test_server.c
	$(CC) -o test_server $^\
 $(INCLUDE) \
 $(LINK_OPT_RT) \
 $(DEBUG_FLAGS)

test_bigint: bigint.h bigint.c test_bigint.c
	$(CC) -o test_bigint $^\
 $(INCLUDE) \
 $(LINK_OPT_GMP) \
 $(DEBUG_FLAGS)

test_clean:
	/bin/rm -f *.o test_client test_server


%.o: %.c $(ALL_DEPS)
	$(CC) -c $^ $(LINK_OPT_M) $(LINK_OPT_RM)



