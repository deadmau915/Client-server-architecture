CC=g++ -std=c++11

ZMQ_PATH=/usr/local

ZMQ_INC=$(ZMQ_PATH)/include
ZMQ_LIB=$(ZMQ_PATH)/lib

all: client server

client:client.cpp
	$(CC) -L$(ZMQ_LIB) -I$(ZMQ_INC) client.cpp -o client.out -lzmq -lzmqpp -lpthread -lsfml-audio

server:server.cpp
	$(CC) -L$(ZMQ_LIB) -I$(ZMQ_INC) server.cpp -o server.out -lzmq -lzmqpp	
