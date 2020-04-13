#include <cstdlib>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <tuple>
#include <numeric>
#include <time.h>
#include <algorithm>
#include <math.h>
#include <cmath>

using namespace std;


template <typename T> std::string type_name();




// -------------------- CONSTRUCTORS -------------------- //

struct point_t {
  float x;
  float y;
};

struct edge_t {
  point_t v1;
  point_t v2;
};

typedef vector<point_t> points_t;
typedef vector<edge_t> edges_t;


// point constructor
point_t point(float x, float y) {
  point_t p;
  p.x = x;
  p.y = y;
  return p;
}

// edge constructor
edge_t edge(point_t v1, point_t v2) {
  edge_t e;
  e.v1 = v1;
  e.v2 = v2;
  return e;
}




struct mesh_t {
  points_t vcenter;             // size |v|
  edges_t edges;                // size |v| * deg(v)
  vector<float> heightmap;      // size |v|
  vector<bool> type;            // size |v|
  int width;
  int height;
};


// -------------------- END CONSTRUCTORS -------------------- //


// -------------------- HELPER FUNCTIONS -------------------- //


float rand_float() { // TODO: not generating random numbers
  return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}


point_t rand_point(int width, int height) {
  return point((rand_float() - 0.5) * width, (rand_float() - 0.5) * height);
}


float dot(point_t a, point_t b) {
  return a.x * b.x + a.y * b.y;
}



point_t sub(point_t a, point_t b) {
  return point(a.x - b.x, a.y - b.y);
}



// -------------------- END HELPER FUNCTIONS -------------------- //








points_t generatePoints(int n, mesh_t mesh) {
  points_t pts;
  for (int i = 0; i < n; i++) {
    pts.push_back( rand_point(mesh.width, mesh.height) );
  }
  return pts;
}



mesh_t slope(mesh_t mesh, point_t direction) {
  for(int i = 0; i < mesh.vcenter.size(); i++) {
    mesh.heightmap[i] = dot(mesh.vcenter[i], direction);
  }
  return mesh;
}


mesh_t cone(mesh_t mesh) {

  point_t c = point(0,0);

  for(int i = 0; i < mesh.vcenter.size(); i++) {
    point_t d = sub(mesh.vcenter[i], c);
    mesh.heightmap[i] = sqrt(dot(d,d)) * mesh.heightmap[i];
  }
  return mesh;
}


mesh_t mountains(mesh_t mesh, int n, float r) {

  float limit = 1200.0;
  float weight = 5.0;

  points_t mounts;
  for(int i = 0; i < n; i++) {
    mounts.push_back( rand_point(mesh.width, mesh.height) );
  }

  for(int i = 0; i < mesh.vcenter.size(); i++) {
    point_t v = mesh.vcenter[i];
    for(int j = 0; j < n; j++) {
      point_t m = mounts[j];
      point_t mv = sub(m,v);

      float t = r * r  / dot(mv,mv);

      //if(t > 100.0)
      //printf("%f\n", t);
      if(t > limit) { t = limit; }

      mesh.heightmap[i] += weight * t;

    }
  }

  return mesh;
}



mesh_t normalize(mesh_t mesh) {

  float min = *min_element(mesh.heightmap.begin(), mesh.heightmap.end());
  float max = *max_element(mesh.heightmap.begin(), mesh.heightmap.end());

  for(int i = 0; i < mesh.heightmap.size(); i++) {
    mesh.heightmap[i] -= min;
    mesh.heightmap[i] *= 1.0 / (max - min);
  }

  return mesh;
}

int main() {

	srand(time(NULL));

	int n = 1000;


  // create mesh
  mesh_t mesh;
  mesh.width = 100;
  mesh.height = 100;
  mesh.vcenter = generatePoints(n, mesh);


  for(int i = 0; i < mesh.vcenter.size(); i++) {
    mesh.heightmap.push_back(1.0);
  }


  mesh = slope(mesh, point(0, 100));
  //mesh = cone(mesh);
  mesh = mountains(mesh, 15, 100);
  mesh = normalize(mesh);



  for(int i = 0; i < mesh.vcenter.size(); i++) {
    printf("%f %f %f\n", mesh.vcenter[i].x, mesh.vcenter[i].y, mesh.heightmap[i]);
  }


}
