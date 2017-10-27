#include "sparseMat.hh"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

// using std::string;
// using std::ifstream;

using namespace std;

void readGraphS(string fileName, sparseMat<int> *m) {
  ifstream infile(fileName);

  string line;
  while (getline(infile, line)) {
    istringstream iss(line);
    if (line[0] == 'p') {
      string s1, s2;
      int nodes;
      iss >> s1 >> s2 >> nodes;

      m->setDimension(nodes);

      // m.resize(nodes);
      // for (int i = 0; i < nodes; i++) {
      //   m[i].resize(nodes);
      // }
      // cout << "Graph with " << nodes << " nodes" << endl;
    } else if (line[0] == 'e') {
      char e;
      int row, col, weight;
      iss >> e >> row >> col >> weight;

      m->setValue(weight);
      m->setCol_ind(col);
      m->setRow_ptr(row);
    }
  }
}

void quickSort() {
  
}

int main(int argc, char const **argv)
{
	sparseMat<int> Mat1;

  string fileName(argv[1]);
  readGraphS(fileName, &Mat1);

  cout << Mat1.getDimension() << endl;
  cout << Mat1.getValue(15) << endl;
  Mat1;
}