#COMPLILER USED
CC = g++

#VARIABLR
INCLUDE=./
LIB=./
 
#COMPILE FLAGS
ADD_INCL = -I$(INCLUDE)
ADD_LIB = -L$(LIB)

#LAST FLAG
CFLAGS = -Wall -g -DLINUX  -funsigned-char -lstdc++
CFLAGS  += $(ADD_INCL)
LDFLAGS += $(ADD_LIB)

SRC_PATH=./src

OBJ=$(patsubst %.c, %.o, $(wildcard $(SRC_PATH)/*.c))

OUTPUT=capture

BIN_DIR=./bin
 
#MAKE RULE
$(OUTPUT):$(OBJ)
	$(CC) -o $@ $<  $(CFLAGS) $(LDFLAGS)

%.o:%.c
	$(CC) -c -o $@ $<  $(CFLAGS) $(LDFLAGS)

clean:
	$(RM) $(OBJ) $(OUTPUT)

install:
ifneq ($(strip $(OUTPUT)),)
	mv  $(OUTPUT) $(BIN_DIR)
endif
