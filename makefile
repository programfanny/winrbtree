CC = gcc
WINDRES = windres
TARGET = $(shell basename $(CURDIR))
SRCS = $(wildcard *.c)
RC_SRCS = $(wildcard *.rc)
OBJS = $(SRCS:.c=.o)
RES_OBJ = $(TARGET)_res.o

CFLAGS = -Wall -O2 -Iinclude
LDFLAGS = -mwindows -lopengl32 -lglu32 -lws2_32 -lcomctl32

all: $(TARGET).exe

$(TARGET).exe: $(OBJS) $(RES_OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(RES_OBJ): $(RC_SRCS)
	$(WINDRES) $< -O coff -o $@

clean:
	rm -f $(OBJS) $(RES_OBJ) $(TARGET).exe

.PHONY: all clean
