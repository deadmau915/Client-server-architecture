# Client-server-architecture

## Folder hw

hello world using the client-server architecture and the zmq and zmq library

## Folder player0.0

small implementation of a music player using the client-server architecture and the zmq and zmqpp library

## Folder matmult

development of a program to multiply matrices that becomes progressively concurrent, the first versions do sequentially, the last will do concurrently intelligent

#### mult0
Multiplication of matrices sequentially.

to copile
```
g++ -std=c++11 file_name.cpp -o file_name.out
```
to execute
```
./file_name.out graph1.dimacs
```

#### mult1
Multiplication of matrices throwing a number of threads equal to the number of elements in the matrix

#### mult2
Multiplication of matrices by throwing a number of threads equal to the number of rows or columns in the matrix

#### mult3
Multiplication of matrices throwing a number of threads equal to the number of cores of the computer

