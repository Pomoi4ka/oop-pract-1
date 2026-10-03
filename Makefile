CFLAGS = -Wall -Wextra -pedantic -std=c89 -pipe -g -MMD -MP
LIBS   = -lreadline

SRCS = $(shell find src/ -type f -name '*.c')
OBJS = $(SRCS:.c=.o)
DEPS = $(OBJS:.o=.d)

all: main

main: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

-include $(DEPS)
