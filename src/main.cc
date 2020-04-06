#include <cstdlib>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
using namespace std;


template <typename T> std::string type_name();


typedef tuple<float, float> point_t;
typedef vector<point_t> points_t;
typedef unordered_map<string, int64_t> dict_int_t;

float rand_float() {
  return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
}

float runif(int lo, int hi) { return lo + rand_float() * (hi - lo); }

// TODO: rnorm
// TODO: randomVector
dict_int_t defaultExtent = {{"width", 1}, {"height", 1}};

//points_t generatePoints(int n, dict_int_t ext) {
  //if (ext)
    //ext = defaultExtent;
//}

int main() {
		cout << defaultExtent["width"] << endl;
}
