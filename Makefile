$(shell chmod -x *.c *.h)
CC=gcc
CFLAGS=-Wall

DEPEND=tc_fpga.c process.c 

TEST_EXEC=server
TEST_SRC=server_fpga.c $(DEPEND)

$(TEST_EXEC):$(TEST_SRC)
		$(CC) $^ $(CFLAGS) -o $@

clean:
		rm -f *.o $(EXEC) $(TEST_EXEC)
