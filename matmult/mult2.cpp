#include "graphreader.hh"
#include "stats.hh"
#include "timer.hh"
#include <cassert>
#include <iostream>
#include <string>
#include <thread>

using namespace std;
using Mat = vector<vector<int>>;

void dot(const Mat &m1, const Mat &m2, int a, Mat &res) {
  int j = m1[0].size(); // number of cols
  int l = m2[0].size(); // number of cols

  for (int b = 0; b < l; b++) {
    for (int c = 0; c < j; c++) {
      res[a][b] += m1[a][c] * m2[c][b];
    }
  }
}

void mult2(const Mat &m1, const Mat &m2, Mat &res) {
  int i = m1.size();    // number of rows in m1
  int j = m1[0].size(); // number of cols in m1
  int k = m2.size();    // number of rows in m2
  int l = m2[0].size(); // number of cols in m2

  assert(j == k);
  vector<thread> ts;
  ts.reserve(i * l);
  for (int a = 0; a < i; a++) {
    ts.push_back(thread(dot, cref(m1), cref(m2), a, ref(res)));
  }

  for (thread &t : ts)
    t.join();
}

void benchmark(int times, const string &fileName) {
  Mat g = readGraph(fileName);
  Mat r;
  r.resize(g.size());
  for (int i = 0; i < g.size(); i++)
    r[i].resize(g.size());

  vector<long> runningTimes;
  runningTimes.reserve(times);

  for (int i = 0; i < times; i++) {
    Timer t("mult2");
    mult2(g, g, r);
    runningTimes.push_back(t.elapsed());
  }
  double am = arithmeticMean(runningTimes);
  double sd = StandardDeviation(runningTimes);
  cout << "mean: " << am << " s.dev: " << sd << endl;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    cerr << "Error!!" << endl;
  }
  string fileName(argv[1]);
  benchmark(100, fileName);
  return 0;
}
