#include <iostream>
#include <fstream>
#include <string>
#include <zmqpp/zmqpp.hpp>
#include <SFML/Audio.hpp>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>

using namespace std;
using namespace zmqpp;
using namespace sf;

template <class T>
class SafeQueue {
    
    public:
    
    SafeQueue(void)
    : q()
    , m()
    , c()
    {}
    
    ~SafeQueue(void)
    {}
    
    // Add an element to the queue.
    void enqueue(T t)
    {
        std::lock_guard<std::mutex> lock(m);
        q.push(t);
        c.notify_one();
    }
    
    // Get the "front"-element.
    // If the queue is empty, wait till a element is avaiable.
    T dequeue(void)
    {
        std::unique_lock<std::mutex> lock(m);
        while(q.empty()) {
            // release lock as long as the wait and reaquire it afterwards.
            c.wait(lock);
        }
        T val = q.front();
        q.pop();
        return val;
    }
    
    void PrintQueue() {
        std::unique_lock<std::mutex> lock(m);
        if(q.front() == NULL) {
            cout<<"\nNothing to Display\n";
  		}
        else {
            while(q.front()!=NULL) {
                T val = q.front();
  				cout << val;
            }
        }
  	}
    
    private:
    std::queue<T> q;
    mutable std::mutex m;
    std::condition_variable c;
    
};

void messageToFile(const message& msg, const string& fileName, const int numOfParts) {
	int filePointer = 2;
	const void *data;
	size_t size;
    ofstream ofs(fileName, ios::binary);

	while(filePointer <= numOfParts + 2) {
        
        msg.get(&data, filePointer);
		size = msg.size(filePointer);
		ofs.write((char*)data, size);
		filePointer ++;
	}
}

void playbackControlSongs(Music *music, SafeQueue<string> *playList){
    
    while(true) {
        
		string nextSong = playList->dequeue();

		while(music->getStatus() == SoundSource::Playing) {			
		}
        
		context ctx;
		socket s(ctx, socket_type::req);
		s.connect("tcp://localhost:5551");

		message m;
		message answer;
		string result;
        
        m << "play";
		m << nextSong;
        
        s.send(m);			
		s.receive(answer);

		answer >> result;
        
        string strNumOfParts;
        int intNumOfParts;
        answer >> strNumOfParts;
        intNumOfParts = stoi(strNumOfParts);
        messageToFile(answer, "song.ogg", intNumOfParts);
        
		music->openFromFile("song.ogg");
		music->play();
    }
    cout << "Finished!" << endl;
}

int main(int argc, char** argv) {
    
	cout << "This is the client\n";

	context ctx;
	socket s(ctx, socket_type::req);
	cout << "Connecting to tcp port 5551\n";
	s.connect("tcp://localhost:5551");

	Music music;	
	SafeQueue<string> playList;

	thread t1(playbackControlSongs, &music, &playList);
    
    cout << "---------------------" << endl;
    cout << "Operation list:" << endl;
    cout << "* list" << endl;
    cout << "* add" << endl;
    cout << "* play" << endl;
    cout << "* next" << endl;
    cout << "* exit" << endl;
    cout << "---------------------" << endl;

	while(true) {
        
		cout << "\tMusic PLayer\t";
		cout << "\tEnter operation to perform:\t" <<endl;
		string operation;
		cin >> operation;

		message m;
        string result;
		message answer;

		if(operation == "play") {

			string songName;
			cout << "Enter the song's name to be played: " << endl; 
			cin >> songName;
			m << operation;
			m << songName;
            
            s.send(m);
			s.receive(answer);
			answer >> result;
        }
        else if(operation == "add") {
            
            string songName;
            cout << "Enter the song's name to be added: " << endl; 
            cin >> songName; 
            playList.enqueue(songName);
            
            result = "add";
        }
        else if(operation == "list") {
            
            m << operation;
            
            s.send(m);
			s.receive(answer);
			answer >> result;
        }
        else if(operation == "next") {
            
            result = "next";
        }
        else if(operation == "exit") {
            
            cout << "See you soon" << endl;
            return 0;
        }
		
		//here we handle the result's operation 	
		if(result == "list") {
            
            size_t songsNumber;
			answer >> songsNumber;
            string song;
            
			cout << "Available songs: " << songsNumber << endl;
			for(int i = 0; i < songsNumber; i++) {
				song.clear();
				answer >> song;
				cout << song << endl;
			}
		}
        else if(result == "file") {
            
            string strNumOfParts;
            int intNumOfParts;
            answer >> strNumOfParts;
            intNumOfParts = stoi(strNumOfParts);
            messageToFile(answer, "song.ogg", intNumOfParts);
            
			music.openFromFile("song.ogg");
			music.play();					
		}
        else if(result == "add") {
            
			cout << "Element added" << endl;
        }
        else if(result == "next") {
            
            music.pause();
        }
        else if(result == "nomatch") {
            cout << "The name doesn't match any song" << endl;
        }
        else {
            
            cout << "Don't know what to do!!!" << endl;
        }
	}
}
