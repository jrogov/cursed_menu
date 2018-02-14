CC = gcc
CFLAGS = --std=gnu99 -pedantic -Wall -Wextra -Werror
LIBS = -lncursesw
INCS = -I /usr/include/ncursesw

SRCS = menu.c newgame.c
OBJS = $(SRCS:.c=.o)

TARGET = newgame

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $^ $(CFLAGS) $(LIBS) $(INCS) -o $@

%.o: %.c
	$(CC) -c $< $(CFLAGS) $(LIBS) $(INCS) -o $@

clean:
	$(RM) $(OBJS) $(TARGETS)