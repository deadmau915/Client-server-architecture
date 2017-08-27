#include <zmqpp/zmqpp.hpp>
#include <iostream>
#include <string>
#include <cassert>

using namespace std;
using namespace zmqpp;

int main() {
  cout << "This is the server\n";

  context ctx;
  socket s(ctx, socket_type::rep);

  cout << "Binding socket to tcp port 5555\n";
  s.bind("tcp://*:5555");

  cout << "Start serving requests!\n";

  while(true) {
    message m;
    cout << "HOLA 11111111111111111" << endl;
    s.receive(m);

    // Each message has three parts: <operation|operand1|operand2>
    assert(m.parts() == 3);

    string operation;
    int operand1;
    int operand2;

    int answer = 0;

    m >> operation >> operand1 >> operand2;
    cout << "Working on  " << operation << endl;
    if (operation == "add") {
      answer = operand1 + operand2;
    } else if(operation == "sub") {
      answer = operand1 - operand2;
    } else {
      cout << "Invalid operation requested!!\n";
    }

    cout << "Sending response  " << answer << endl;

    message response;
    response << answer;
    s.send(response);
  }

  cout << "Finished\n";
  return 0;
}
