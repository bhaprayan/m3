#include <cstdlib>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <tuple>
#include <numeric>
#include <time.h>
using namespace std;


template <typename T> std::string type_name();


typedef tuple<float, float> point_t;
typedef vector<point_t> points_t;
typedef unordered_map<string, int64_t> dict_int_t;

float rand_float() { // TODO: not generating random numbers
  return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

float runif(int lo, int hi) { return lo + rand_float() * (hi - lo); }

// TODO: rnorm
// TODO: randomVector

dict_int_t defaultExtent = {{"width", 1}, {"height", 1}};

points_t generatePoints(int n, dict_int_t ext) {
  if(ext.empty()) { ext = defaultExtent; }
  points_t pts;
  for (int i = 0; i < n; i++) {
  	pts.push_back( point_t((rand_float() - 0.5) * ext["width"], (rand_float() - 0.5) * ext["height"]) );
  }
  return pts;
}

point_t centroid(points_t pts) {
	float x = 0;
	float y = 0;
	for (int i = 0; i < pts.size(); i++) {
        x += get<0>(pts[i]);
        y += get<1>(pts[i]);
    }
    point_t center = point_t(x/pts.size(),y/pts.size());
    return center;
}

// TODO: improve points







int main() {

	srand(time(NULL));

	dict_int_t base;
	base = {{"width", 100}, {"height", 100}};
	int n = 10;
	points_t pts = generatePoints(n, base);
	point_t center = centroid(pts);

	printf("%f, %f\n", get<0>(center), get<1>(center));

}
