#include <iostream>
#include <stdlib.h>
#include <zmqpp/zmqpp.hpp>
#include <unordered_map>
#include "json.hpp"

#include <unistd.h>
#include <random>

using namespace zmqpp;
using namespace std;
using json = nlohmann::json;

static std::unordered_map<string, string> hashTable;
static std::unordered_map<string, socket*> fingerTable;
static string self_ip, self_port, neighbour_ip, neighbour_port, server_endPoint, client_endPoint;
static int lowerBound, upperBound, self_id, neighbour_id;
static context ctx;
static poller p;

int getRandom()
{
	random_device rand_dev;
	mt19937 generator(rand_dev());
	uniform_int_distribution<int> distr(0, 30);
	return distr(generator);
}

bool in_range(int id)
{
	if (upperBound >= lowerBound)
	{
		return (id > lowerBound && id <= upperBound);
	} else {
		return (id > lowerBound || id <= upperBound);
	}
}

int main(int argc, char const** argv)
{
	if (argc != 6)
	{
		cout << "Usage: \"<new_ring(1 or 0)>\" \"<local_ip>\" \"<local_port>\" \"<remote_ip>\" \"<remote_port>\"" << endl;
		return 1;
	}

	int base_case = atoi(argv[1]);
	self_ip = argv[2];
	self_port = argv[3];
	neighbour_ip = argv[4];
	neighbour_port = argv[5];
	self_id = getRandom();
	neighbour_id = -1;

	server_endPoint = "tcp://" + self_ip + ":" + self_port;
	client_endPoint = "tcp://" + neighbour_ip + ":" + neighbour_port;

	socket s_server(ctx, socket_type::rep); //listening
	socket s_client(ctx, socket_type::req); //asking

	s_server.bind(server_endPoint);
	s_client.connect(client_endPoint);

	// p.add(s_server, poller::poll_in);
	// p.add(s_client, poller::poll_in);
	p.add(s_server);
	p.add(s_client);

	if (base_case)
	{
		cout << "creating the ring..." << endl;
		json request = {
			{"op", "send_id"},
			{"self_id", self_id}
		};
		json j_answer;
		string s_answer;
		s_client.send(request.dump());
		s_server.receive(s_answer);
		j_answer = json::parse(s_answer);
		if (j_answer["op"] == "send_id")
		{
			neighbour_id = j_answer["self_id"];
			lowerBound = self_id;
			upperBound = j_answer["self_id"];
		} else {
			cout << "unknown option: " << j_answer["op"] << endl;
			return EXIT_FAILURE;
		}
	} else {
		cout << "connecting to existing ring..." << endl;
		json request = {
			{"op", "add node"},
			{"self_id", self_id},
			{"self _ip", self_ip},
			{"self_port", self_port}
		};
		json j_answer;
		string s_answer;
		s_client.send(request.dump());
		s_server.receive(s_answer);
		j_answer = json::parse(s_answer);
		if (j_answer["op"] == "add node ack")
		{
			neighbour_ip = j_answer["neighbour_ip"];
			neighbour_port = j_answer["neighbour_port"];
			neighbour_id = j_answer["neighbour_id"];
			lowerBound = self_id;
			upperBound = j_answer["neighbour_id"];
			s_client.disconnect(client_endPoint);
			client_endPoint = "tcp://" + neighbour_ip + ":" + neighbour_port;
			s_client.connect(client_endPoint);
		} else {
			cout << "unknown option: " << j_answer["op"] << endl;
			return EXIT_FAILURE;
		}
	}

	while (true)
	{
		cout << "checkpoint 1" << endl;
		if (p.poll())
		{
			cout << "checkpoint 2" << endl;
			if (p.has_input(s_server))
			{
				cout << "checkpoint 3" << endl;
				string s_request;
				s_server.receive(s_request);
				json j_request = json::parse(s_request);
				json j_response;
				if (j_request["op"] == "add node")
				{
					if (in_range(j_request["self_id"]))
					{
						cout << "message: in my range, adding node..." << endl;
						j_response = {
							{"op", "add node ack"},
							{"neighbour_ip", neighbour_ip},
							{"neighbour_port", neighbour_port},
							{"neighbour_id", neighbour_id}
						};
						neighbour_ip = j_request["self_ip"];
						neighbour_port = j_request["self_port"];
						neighbour_id = j_request["self_id"];
						upperBound = j_request["self_id"];
						s_client.disconnect(client_endPoint);
						client_endPoint = "tcp://" + neighbour_ip + ":" + neighbour_port;
						s_client.connect(client_endPoint);
						s_client.send(j_response.dump());
					} else {
						cout << "message: not in my range, delegating..." << endl;
						s_client.send(s_request);
					}
				} else {
					cout << "---pass---" << endl;	
				}
			}
		}

		cout << "node id: " << self_id << endl;
		cout << "neighbour id: " << neighbour_id << endl;
		cout << "low: " << lowerBound << " up: " << upperBound << endl;
		usleep(5000000);
	}

	// while (true)
	// {
	//  	cout << "node id: " << self_id << endl;
	// 	cout << "neighbour id: " << neighbour_id << endl;
	// 	cout << "low: " << lowerBound << " up: " << upperBound << endl;
	// 	int tmp_id = getRandom();
	// 	cout << "id: " << tmp_id << " in range?: " << in_range(tmp_id) << endl;
	// 	cout << endl;
	// 	usleep(5000000);
	// }

	return 0;
}