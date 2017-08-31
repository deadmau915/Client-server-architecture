CC=g++ -std=c++11

ZMQ_PATH=/usr/local

ZMQ_INC=$(ZMQ_PATH)/include
ZMQ_LIB=$(ZMQ_PATH)/lib

all: Client serverMusic

Client:Client.cpp
	$(CC) -L$(ZMQ_LIB) -I$(ZMQ_INC) Client.cpp -o client.out -lzmq -lzmqpp  -pthread -lsfml-audio

serverMusic:serverMusic.cpp
	$(CC) -L$(ZMQ_LIB) -I$(ZMQ_INC) serverMusic.cpp -o server.out -lzmq -lzmqpp	
