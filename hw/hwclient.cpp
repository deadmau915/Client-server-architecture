#include <iostream>
#include <string>
#include <zmqpp/zmqpp.hpp>

using namespace std;
using namespace zmqpp;

int main(int argc, char** argv) {
	cout << "This is the client\n";

	if (argc != 4) {
		cerr << "Should be called: " << argv[0] << " <op> operand1 operand2\n";
		return 1;
	}

	context ctx;
	socket s(ctx, socket_type::req);

	cout << "Connecting to tcp port 5555\n";
	s.connect("tcp://localhost:5555");

	cout << "Sending  some work!\n";

	message m;
	string operation(argv[1]);
	string operand1(argv[2]);
	string operand2(argv[3]);

	m << operation << stoi(operand1) << stoi(operand2);
	s.send(m);

	message answer;
	s.receive(answer);

	int a;
	answer >> a;
	cout << "Answer is " << a << endl;

    cout << "Finished\n";
	return 0;
}
