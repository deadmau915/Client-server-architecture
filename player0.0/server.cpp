#include <zmqpp/zmqpp.hpp>
#include <iostream>
#include <string>
#include <cassert>
#include <unordered_map>
#include <fstream>
#include <dirent.h>
//#include <list>
//#include <filesystem>
//#include <experimental/filesystem> 


using namespace std;
using namespace zmqpp;
//namespace fs = std::filesystem;

vector<char> readFileToBytes(const string& fileName) {
	ifstream ifs(fileName, ios::binary | ios::ate);
	ifstream::pos_type pos = ifs.tellg();

	vector<char> result(pos);

	ifs.seekg(0, ios::beg);
	ifs.read(result.data(), pos);

	return result;
}

void fileToMesage(const string& fileName, message& msg) {
	vector<char> bytes = readFileToBytes(fileName);
	msg.add_raw(bytes.data(), bytes.size());
}
    

int main(int argc, char** argv) {

  context ctx;
  socket s(ctx, socket_type::rep);
  s.bind("tcp://*:5555");

  string path(argv[1]);

  //list<string> SongsList;
  unordered_map<string,string> songs;
  string aux;

  DIR *dir;
struct dirent *ent;
if ((dir = opendir ("/home/jhon/Cliente-Servidor/Proyect_1/player0.0/music")) != NULL) {
  /* print all the files and directories within directory */
  while ((ent = readdir (dir)) != NULL) {
    //printf ("%s\n", ent->d_name);
    aux = path + ent->d_name;
    //printf("%s\n",aux );
    songs.insert({ent->d_name,aux});
  }
  closedir (dir);
} else {
  /* could not open directory */
  perror ("");
  return EXIT_FAILURE;
}
  cout << "Start serving requests!\n";
  while(true) {
    message m;
    s.receive(m);

    string op;
    m >> op;

    cout << "Action:  " << op << endl;
    if (op == "list") {  // Use case 1: Send the songs
      message n;
      n << "list" << songs.size();
      for(const auto& p : songs)
        n << p.first;
      s.send(n);
    } else if(op == "play") {
      // Use case 2: Send song file
      string songName;
      m >> songName;
      cout << "sending song " << songName
           << " at " << songs[songName] << endl;
			message n;
			n << "file";
			fileToMesage(songs[songName], n);
			s.send(n);
    } else {
      cout << "Invalid operation requested!!\n";
    }
  }

  cout << "Finished\n";
  return 0;
}
