#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <numeric>
#include <string>
#include <time.h>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <stdint.h>
#include <stdio.h> // printf
#include <stdlib.h>

#if defined(_MSC_VER)
#include <malloc.h>
#define alloca _alloca
#else
#include <alloca.h>
#endif

#ifndef JC_VORONOI_IMPLEMENTATION
#define JC_VORONOI_IMPLEMENTATION
// If you wish to use doubles
//#define JCV_REAL_TYPE double
//#define JCV_FABS fabs
//#define JCV_ATAN2 atan2
#include "../voronoi/src/jc_voronoi.h"
#endif

#ifndef JC_VORONOI_CLIP_IMPLEMENTATION
#define JC_VORONOI_CLIP_IMPLEMENTATION
#include "../voronoi/src/jc_voronoi_clip.h"
#endif

#ifdef HAS_MODE_FASTJET
#include "../test/fastjet/voronoi.h"
#include <vector>
#endif

using namespace std;

static void relax_points(const jcv_diagram *diagram, jcv_point *points) {
  const jcv_site *sites = jcv_diagram_get_sites(diagram);
  for (int i = 0; i < diagram->numsites; ++i) {
    const jcv_site *site = &sites[i];
    jcv_point sum = site->p;
    int count = 1;

    const jcv_graphedge *edge = site->edges;

    while (edge) {
      sum.x += edge->pos[0].x;
      sum.y += edge->pos[0].y;
      ++count;
      edge = edge->next;
    }

    points[site->index].x = sum.x / count;
    points[site->index].y = sum.y / count;
  }
}

jcv_diagram return_voronoi(int count) {
  // Number of sites to generate
  // Image dimension
  int width = 512;
  int height = 512;
  int numrelaxations = 0;

  jcv_point *points = 0;
  jcv_rect *rect = 0;

  points = (jcv_point *)malloc(sizeof(jcv_point) * (size_t)count);

  int pointoffset = 10; // move the points inwards, for aestetic reasons

  srand(0);

  for (int i = 0; i < count; ++i) {
    points[i].x = (float)(pointoffset + rand() % (width - 2 * pointoffset));
    points[i].y = (float)(pointoffset + rand() % (height - 2 * pointoffset));
  }

  jcv_clipper *clipper = 0;

  for (int i = 0; i < numrelaxations; ++i) {
    jcv_diagram diagram;
    memset(&diagram, 0, sizeof(jcv_diagram));
    jcv_diagram_generate(count, (const jcv_point *)points, rect, clipper,
                         &diagram);

    relax_points(&diagram, points);

    jcv_diagram_free(&diagram);
  }

  jcv_diagram diagram;
  memset(&diagram, 0, sizeof(jcv_diagram));
  jcv_diagram_generate(count, (const jcv_point *)points, rect, clipper,
                       &diagram);

  return diagram;
}


static inline jcv_point remap(const jcv_point *pt, const jcv_point *min,
                              const jcv_point *max, const jcv_point *scale) {
  jcv_point p;
  p.x = (pt->x - min->x) / (max->x - min->x) * scale->x;
  p.y = (pt->y - min->y) / (max->y - min->y) * scale->y;
  return p;
}








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
  vector<edges_t> edges;        // size |v| * deg(v)
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


mesh_t mountains(mesh_t mesh, int n, float r, float limit) {

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


  int n = 1000;

  // Image dimension
  int width = 512;
  int height = 512;

  mesh_t mesh;
  mesh.width = width;
  mesh.height = height;


  jcv_point dimensions;
  dimensions.x = (jcv_real)width;
  dimensions.y = (jcv_real)height;


  jcv_diagram diagram = return_voronoi(500);

  const jcv_site *sites = jcv_diagram_get_sites(&diagram);

  for (int i = 0; i < diagram.numsites; ++i) {
    const jcv_site *site = &sites[i];

    srand((unsigned int)site->index); // for generating colors for the triangles

    jcv_point s = remap(&site->p, &diagram.min, &diagram.max, &dimensions);

    mesh.vcenter.push_back(point(s.x, s.y));
    edges_t edges;

    const jcv_graphedge *e = site->edges;
    while (e) {
      jcv_point p0 = remap(&e->pos[0], &diagram.min, &diagram.max, &dimensions);
      //printf("ep0 x: %f ep0 y: %f\n", p0.x, p0.y);
      jcv_point p1 = remap(&e->pos[1], &diagram.min, &diagram.max, &dimensions);
      //printf("ep1 x: %f ep1 y: %f\n", p1.x, p1.y);

      point_t p0_t = point(p0.x, p0.y);
      point_t p1_t = point(p1.x, p1.y);

      edges.push_back( edge(p0_t, p1_t) );

      e = e->next;
    }

    mesh.edges.push_back( edges );
  }


  for(int i = 0; i < mesh.vcenter.size(); i++) {
    mesh.heightmap.push_back(1.0);
  }


  mesh = slope(mesh, point(0, 100));
  //mesh = cone(mesh);
  mesh = normalize(mesh);
  mesh = mountains(mesh, 15, 100, 30.0);
  mesh = normalize(mesh);

  printf("%d\n", mesh.vcenter.size());
  /*
  for (int i = 0; i < mesh.vcenter.size(); i++) {
      printf("v %f %f %f\n", mesh.vcenter[i].x, mesh.vcenter[i].y, mesh.heightmap[i]);

      for (int j = 0; j < mesh.edges[i].size(); j++) {
        printf("e %f %f %f %f\n", mesh.edges[i][j].v1.x, mesh.edges[i][j].v1.y, mesh.edges[i][j].v2.x, mesh.edges[i][j].v2.y);
      }
  }
  */


}
