CC := g++
CFLAGS := -std=c++11 -Wall -Wextra


LIBS :=


INCLUDES := -I.


SRCDIR := .

# List of source files
SERVER_SRC := $(SRCDIR)/server.cpp
CLIENT_SRC := $(SRCDIR)/client.cpp

# Object files
SERVER_OBJ := server.o
CLIENT_OBJ := client.o

# Targets
SERVER_TARGET := libserver.a
CLIENT_TARGET := libclient.a

.PHONY: all clean

all: $(SERVER_TARGET) $(CLIENT_TARGET)

$(SERVER_TARGET): $(SERVER_OBJ)
	ar rcs $(SERVER_TARGET) $(SERVER_OBJ)

$(CLIENT_TARGET): $(CLIENT_OBJ)
	ar rcs $(CLIENT_TARGET) $(CLIENT_OBJ)

$(SERVER_OBJ): $(SERVER_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(CLIENT_OBJ): $(CLIENT_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(SERVER_OBJ) $(CLIENT_OBJ) $(SERVER_TARGET) $(CLIENT_TARGET)



