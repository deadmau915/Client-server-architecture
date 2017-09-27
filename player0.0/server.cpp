#include <zmqpp/zmqpp.hpp>
#include <iostream>
#include <string>
#include <cassert>
#include <unordered_map>
#include <fstream>
#include <dirent.h>
#include <sstream>

using namespace std;
using namespace zmqpp;

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

size_t fileSize(const string& fileName) {
    ifstream ifs(fileName, ios::binary | ios::ate);
    ifstream::pos_type pos = ifs.tellg();
    return pos;
}

////////////////////////////////////////////////////////////////////////
void chunkFile(const string& filePath, const string& chunkName, size_t chunkSize, message& msg) {
    
    ifstream ifs(filePath, ios::binary);
    vector<char> chunkBuffer(chunkSize);
    string fullChunkName;
    int partsCounter = 0;
    string partsDirectory = "splits/";
    
    while(!ifs.eof()) {
        ifs.seekg(partsCounter * chunkSize, ios::beg);
        ifs.read(chunkBuffer.data(), chunkSize);
        
        fullChunkName.clear();
        fullChunkName.append(partsDirectory);
        fullChunkName.append(chunkName);
        fullChunkName.append(".");

        // Convert counter integer into string and append to name.
        stringstream ss;
        ss << partsCounter;
        string myString = ss.str();
        fullChunkName.append(myString);
            
        ofstream ofs(fullChunkName, ios::binary);
        ofs.write(chunkBuffer.data(), ifs.gcount());
        
        fileToMesage(fullChunkName, msg);
        partsCounter ++;
    }
}

void readDirectory() {
    string aux;
    if((dir = opendir(fullPath.c_str())) != NULL) {
        
        /* print all the files and directories within directory */
        while((ent = readdir(dir)) != NULL) {
            
            aux = path + ent->d_name;
            songs.insert({ent->d_name, aux});
        }
        closedir(dir);
    }
    else {
        /* could not open directory */
        perror ("");
        return EXIT_FAILURE;
    }
}
////////////////////////////////////////////////////////////////////////
    
int main(int argc, char** argv) {
    context ctx;
    socket s(ctx, socket_type::rep);
    s.bind("tcp://*:5551");
    string path(argv[1]);
    int chunkSize = 512000;
    
    unordered_map<string,string> songs;
    
    DIR *dir;
    struct dirent *ent;
    
    string fullPath = "./" + path;
    
    ////////////////////////////////////////////////////////////////////
    //~ string aux;
    //~ if((dir = opendir(fullPath.c_str())) != NULL) {
        
        //~ /* print all the files and directories within directory */
        //~ while((ent = readdir(dir)) != NULL) {
            
            //~ aux = path + ent->d_name;
            //~ songs.insert({ent->d_name, aux});
        //~ }
        //~ closedir(dir);
    //~ }
    //~ else {
        //~ /* could not open directory */
        //~ perror ("");
        //~ return EXIT_FAILURE;
    //~ }
    ////////////////////////////////////////////////////////////////////
    
    cout << "Start serving requests!\n";
    
    while(true) {
        
        message m;
        s.receive(m);
        
        string op;
        m >> op;
        
        cout << "Action:  " << op << endl;
        
        if(op == "list") {  
            // Use case 1: Send the songs
            message n;
            n << "list" << songs.size();
            
            for(const auto& p : songs) {
                n << p.first;
            }
            s.send(n);
        }
        else if(op == "play") {
            // Use case 2: Send song file
            
            string songName;
            m >> songName;
            message n;
            if(songs.count(songName)) {
                cout << "sending song " << songName << " at " << songs[songName] << endl;
                n << "file";
                size_t fileSizeVarible = fileSize(songs[songName]);
                size_t numOfParts = fileSizeVarible/chunkSize;
                n << to_string(numOfParts);
                chunkFile(songs[songName], songName, chunkSize, n);
                s.send(n);
            }
            else {
                n << "nomatch";
                s.send(n);
            }
        }
        else {
            cout << "Invalid operation requested!!\n";
        }
    }
    
    cout << "Finished\n";
    return 0;
}
