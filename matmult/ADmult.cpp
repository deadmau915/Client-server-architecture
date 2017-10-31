#include "graphreader.hh"
#include "stats.hh"
#include "timer.hh"
#include <cassert>
#include <iostream>
#include <string>

using namespace std;
using Mat = vector<vector<int>>;

void ad0(const Mat &m1, const Mat &m2, Mat &res) {
  int i = m1.size();    // number of rows in m1
  int j = m1[0].size(); // number of cols in m1
  int k = m2.size();    // number of rows in m2
  int l = m2[0].size(); // number of cols in m2

  assert(j == k);

  for (int a = 0; a < i; a++)
    for (int b = 0; b < l; b++)
      for (int c = 0; c < j; c++) {
        if (m1[a][c] != numeric_limits<int>::max() &&
            m2[c][b] != numeric_limits<int>::max())
          res[a][b] = min(res[a][b], m1[a][c] + m2[c][b]);
      }
}

void printMat(const Mat &m) {
  for (int i = 0; i < m.size(); i++) {
    for (int j = 0; j < m[0].size(); j++)
      if (m[i][j] == numeric_limits<int>::max())
        cout << " "
             << "F";
      else
        cout << " " << m[i][j];
    cout << endl;
  }
}

void benchmark(int times, const string &fileName) {
  Mat g = readGraphW(fileName);
  for (int i = 0; i < g.size(); i++)
    for (int j = 0; j < g[0].size(); j++) {
      if (g[i][j] == 0)
        g[i][j] = numeric_limits<int>::max();
      if (i == j)
        g[i][j] = 0;
    }

  Mat r, empty;
  r.resize(g.size());
  empty.resize(g.size());
  for (int i = 0; i < g.size(); i++)
  {
    r[i].resize(g.size(), numeric_limits<int>::max());
    empty[i].resize(g.size(), numeric_limits<int>::max());
  }

  int timesMult = ceil(log2(g.size()));

  for (int i = 0; i < timesMult; ++i)
  {
    ad0(g, g, r);
    g = r;
    r = empty;
  }

  printMat(g);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    cerr << "Error!!" << endl;
  }
  string fileName(argv[1]);
  benchmark(1, fileName);
  return 0;
}