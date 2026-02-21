CC     = gcc
CFLAGS = -Wall -Wextra
TARGET = sqlite
DB     = mydb.db

SRCS = main.c node.c row.c pager.c cursor.c btree.c repl.c
OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

run: $(TARGET)
	./$(TARGET) $(DB)

clean:
	rm -f $(TARGET) $(OBJS)
