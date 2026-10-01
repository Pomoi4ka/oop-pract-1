CFLAGS = -Wall -Wextra -pedantic -std=c89 -pipe -g
LIBS   = -lreadline

SRCS != find src/ -type f -name '*.c'
OBJS = $(SRCS:.c=.o)

all: main

main: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)
