#include "../earcut.hpp/include/mapbox/earcut.hpp"
#include "raylib.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <numeric>
#include <string>
#include <sys/time.h>
#include <thread>
#include <time.h>
#include <tuple>
#include <unordered_map>
#include <vector>

#include <ctime>
#include <functional>
#include <random>

#include <omp.h>

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
using namespace std::chrono;

// -------------------- CONSTRUCTORS -------------------- //

struct point_t {
  float x;
  float y;
};

struct edge_t {
  point_t v1;
  point_t v2;
};

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

typedef vector<edge_t> edges_t;

struct mesh_t {
  point_t *vcenter; // size |v|
  edges_t *edges;   // size |v| * deg(v)
  float *heightmap; // size |v|
  int nVoxels;
  int width;
  int height;
  // Color *color; // size |v|
};

// -------------------- END CONSTRUCTORS -------------------- //

// -------------------- HELPER FUNCTIONS -------------------- //

int GetRN(int min = 0, int max = RAND_MAX) {
  std::uniform_int_distribution<int> distribution(min, max);
  static std::mt19937 engine(time(NULL));
  return distribution(engine);
}

float rand_float() {
  float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
  return r;
}

float rand_int(int min, int max) {
  int i = (rand() % (max - min));
  return i + min;
}

point_t rand_point(int width, int height) {
  return point(rand_float() * width, rand_float() * height);
}

float dot(point_t a, point_t b) { return a.x * b.x + a.y * b.y; }

point_t sub(point_t a, point_t b) { return point(a.x - b.x, a.y - b.y); }

// -------------------- END HELPER FUNCTIONS -------------------- //

// -------------------- VORONOI FUNCTIONS -------------------- //

static void relax_points(const jcv_diagram *diagram, jcv_point *points) {
  const jcv_site *sites = jcv_diagram_get_sites(diagram);

  int i = 0;
#pragma omp parallel for schedule(static)
  for (i = 0; i < diagram->numsites; ++i) {
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

  int i = 0;
#pragma omp parallel for schedule(static)
  for (i = 0; i < count; ++i) {
    points[i].x = (float)(pointoffset + GetRN() % (width - 2 * pointoffset));
    points[i].y = (float)(pointoffset + GetRN() % (height - 2 * pointoffset));
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

// -------------------- END VORONOI FUNCTIONS -------------------- //

void slope(mesh_t *mesh, point_t direction) {
  int i = 0;
#pragma omp parallel for schedule(static)
  for (i = 0; i < mesh->nVoxels; i++) {
    mesh->heightmap[i] = dot(mesh->vcenter[i], direction);
  }
}

void mountains(mesh_t *mesh, int n, float r, float h_limit, float r_limit) {

  point_t *mounts = (point_t *)calloc(n, sizeof(point_t));
  int i = 0;
#pragma omp parallel for schedule(static)
  for (i = 0; i < n; i++) {
    point_t pt = rand_point(mesh->width, mesh->height);
    mounts[i] = pt;
  }

#pragma omp parallel for schedule(static)
  for (i = 0; i < mesh->nVoxels; i++) {
    point_t v = mesh->vcenter[i];
    for (int j = 0; j < n; j++) {
      point_t m = mounts[j];
      point_t mv = sub(m, v);

      float dist = dot(mv, mv);
      if (dist < r_limit) {

        float t = r * r / dist;
        if (t > h_limit) {
          t = h_limit;
        }

        mesh->heightmap[i] += t;
      }
    }
  }
}

void normalize(mesh_t *mesh) {

  float max_val = mesh->heightmap[0];
  float min_value = mesh->heightmap[0];
#pragma omp parallel for reduction(min : min_value) reduction(max : max_val)
  for (int i = 0; i < mesh->nVoxels; i++) {
    if (max_val < mesh->heightmap[i]) {
      max_val = mesh->heightmap[i];
    }
    if (min_value > mesh->heightmap[i]) {
      min_value = mesh->heightmap[i];
    }
  }

  float norm = 1.0 / (max_val - min_value);
  int i = 0;
#pragma omp parallel for schedule(static)
  for (i = 0; i < mesh->nVoxels; i++) {
    mesh->heightmap[i] -= min_value;
    mesh->heightmap[i] *= norm;
  }
}

float get_mean(mesh_t *mesh) {

  float sum = 0.0;
#pragma omp parallel for reduction(+ : sum)
  for (int i = 0; i < mesh->nVoxels; i++) {
    sum += mesh->heightmap[i];
  }

  return sum / (mesh->nVoxels);
}

float get_variance(mesh_t *mesh) {
  float mean = get_mean(mesh);
  float var = 0;
#pragma omp parallel for reduction(+ : var)
  for (int i = 0; i < mesh->nVoxels; i++) {
    var += (mesh->heightmap[i] - mean) * (mesh->heightmap[i] - mean);
  }
  return var / (mesh->nVoxels);
}

float get_max_height(mesh_t *mesh) {
  float max_val = mesh->heightmap[0];
  float min_value = mesh->heightmap[0];
#pragma omp parallel for reduction(min : min_value) reduction(max : max_val)
  for (int i = 0; i < mesh->nVoxels; i++) {
    if (max_val < mesh->heightmap[i]) {
      max_val = mesh->heightmap[i];
    }
    if (min_value > mesh->heightmap[i]) {
      min_value = mesh->heightmap[i];
    }
  }
  return max_val;
}

float get_min_height(mesh_t *mesh) {
  float max_val = mesh->heightmap[0];
  float min_value = mesh->heightmap[0];
#pragma omp parallel for reduction(min : min_value) reduction(max : max_val)
  for (int i = 0; i < mesh->nVoxels; i++) {
    if (max_val < mesh->heightmap[i]) {
      max_val = mesh->heightmap[i];
    }
    if (min_value > mesh->heightmap[i]) {
      min_value = mesh->heightmap[i];
    }
  }
  return min_value;
}

void HSVtoRGB(int H, double S, double V, int output[3]) {
  double C = S * V;
  double X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
  double m = V - C;
  double Rs, Gs, Bs;

  if (H >= 0 && H < 60) {
    Rs = C;
    Gs = X;
    Bs = 0;
  } else if (H >= 60 && H < 120) {
    Rs = X;
    Gs = C;
    Bs = 0;
  } else if (H >= 120 && H < 180) {
    Rs = 0;
    Gs = C;
    Bs = X;
  } else if (H >= 180 && H < 240) {
    Rs = 0;
    Gs = X;
    Bs = C;
  } else if (H >= 240 && H < 300) {
    Rs = X;
    Gs = 0;
    Bs = C;
  } else {
    Rs = C;
    Gs = 0;
    Bs = X;
  }

  output[0] = (Rs + m) * 255;
  output[1] = (Gs + m) * 255;
  output[2] = (Bs + m) * 255;
}

Color get_color(float height, float mean, float var, float min_height,
                float max_height) {
  if (height >= mean) {
    // land
    // light green: 110 100 50
    // dark green: 110 100 25
    float rescale_max = 0.50; // light green
    float rescale_min = 0.25; // dark green
    float min_height = mean;
    float value = height;
    int output[3];
    int H = 110;
    double S = 1.0;
    float V = ((rescale_max - rescale_min) * (value - min_height) /
               (max_height - min_height)) +
              rescale_min;
    // printf("land %f\n", V);
    HSVtoRGB(H, S, V, output);
    unsigned char a = 255;
    Color element_color =
        (Color){(unsigned char)output[0], (unsigned char)output[1],
                (unsigned char)output[2], a};
    return element_color;
    // if (height > (mean + 2 * var)) {
    // return BROWN;
    //} else if (height > (mean + var)) {
    // return DARKGREEN;
    //} else {
    // return GREEN;
    //}
  } else {
    // water
    float rescale_max = 0.65; // light blue
    float rescale_min = 0.25; // dark blue
    float max_height = mean;
    float value = height;
    int output[3];
    int H = 205;
    double S = 1.0;
    float V = ((rescale_max - rescale_min) * (value - min_height) /
               (max_height - min_height)) +
              rescale_min;
    // printf("water %f\n", V);
    HSVtoRGB(H, S, V, output);
    unsigned char a = 255;
    Color element_color =
        (Color){(unsigned char)output[0], (unsigned char)output[1],
                (unsigned char)output[2], a};
    return element_color;
    // if (height < (mean - 2 * var)) {
    // return DARKBLUE;
    //} else if (height < (mean - var)) {
    // return BLUE;
    //} else {
    // return SKYBLUE;
    //}
  }
}

int main(int argc, char **argv) {
  auto MAIN_START = high_resolution_clock::now();

  int nVoxels = 500;
  int nMountains = 25;
  int nThreads = 8;
  bool instrument = false;

  bool render = true;

  int i = 1;
  while (i < argc) {
    string arg = argv[i];
    if (arg == "-n") {
      i++;
      nVoxels = atoi(argv[i]);
    } else if (arg == "-m") {
      i++;
      nMountains = atoi(argv[i]);
    } else if (arg == "-t") {
      i++;
      nThreads = atoi(argv[i]);
    } else if (arg == "-I") {
      instrument = true;
    } else if (arg == "-h") {
      printf("-n   number of voxels\n");
      printf("-m   number of mountains\n");
      printf("-t   number of threads\n");
      return 0;
    }
    i++;
  }

  if (instrument)
    printf("Generating map with %d voxels and %d mountains with %d threads.\n",
           nVoxels, nMountains, nThreads);

  omp_set_num_threads(nThreads);

  struct timeval tp;
  gettimeofday(&tp, NULL);
  unsigned int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;

  // Image dimension
  int width = 900;
  int height = 900;

  mesh_t *mesh = new mesh_t();
  mesh->width = width;
  mesh->height = height;

  auto VORONOI_START = high_resolution_clock::now();

  jcv_point dimensions;
  dimensions.x = (jcv_real)width;
  dimensions.y = (jcv_real)height;

  jcv_diagram diagram = return_voronoi(nVoxels);
  const jcv_site *sites = jcv_diagram_get_sites(&diagram);

  auto VORONOI_END = high_resolution_clock::now();

  auto PARSING_START = high_resolution_clock::now();

  mesh->nVoxels = diagram.numsites;
  mesh->vcenter = (point_t *)calloc(mesh->nVoxels, sizeof(point_t));
  mesh->edges = (edges_t *)calloc(mesh->nVoxels, sizeof(edges_t));
  // mesh->color = (Color *)calloc(mesh->nVoxels, sizeof(Color));

#pragma omp parallel for schedule(static)
  for (i = 0; i < diagram.numsites; ++i) {
    const jcv_site *site = &sites[i];

    jcv_point s = remap(&site->p, &diagram.min, &diagram.max, &dimensions);

    mesh->vcenter[i] = point(s.x, s.y);

    edges_t edges;

    const jcv_graphedge *e = site->edges;
    while (e) {
      jcv_point p0 = remap(&e->pos[0], &diagram.min, &diagram.max, &dimensions);
      jcv_point p1 = remap(&e->pos[1], &diagram.min, &diagram.max, &dimensions);

      point_t p0_t = point(p0.x, p0.y);
      point_t p1_t = point(p1.x, p1.y);

      edges.push_back(edge(p0_t, p1_t));

      e = e->next;
    }

    mesh->edges[i] = edges;
  }

  auto PARSING_END = high_resolution_clock::now();

  mesh->heightmap = (float *)calloc(mesh->nVoxels, sizeof(int));

  gettimeofday(&tp, NULL);
  ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
  srand(ms);

  auto SLOPE_START = high_resolution_clock::now();
  slope(mesh, point(rand_int(-100, 100), rand_int(-100, 100)));
  auto SLOPE_END = high_resolution_clock::now();

  auto NORM1_START = high_resolution_clock::now();
  normalize(mesh);
  auto NORM1_END = high_resolution_clock::now();

  auto MOUNTAINS_START = high_resolution_clock::now();
  mountains(mesh, nMountains, 10, 30, 50000);
  auto MOUNTAINS_END = high_resolution_clock::now();

  auto NORM2_START = high_resolution_clock::now();
  normalize(mesh);
  auto NORM2_END = high_resolution_clock::now();

  auto MEAN_START = high_resolution_clock::now();
  float mean = get_mean(mesh);
  auto MEAN_END = high_resolution_clock::now();

  float var = get_variance(mesh);
  float min_height = get_min_height(mesh);
  float max_height = get_max_height(mesh);

  auto MAIN_END = high_resolution_clock::now();

  using Coord = float;

  using N = uint32_t;

  using Point = std::array<Coord, 2>;

  const int screenWidth = 900;
  const int screenHeight = 900;

  // TODO: big assumption! this could break the code at some point.
  // Color *cell_colors = (Color *)calloc(32, sizeof(Color));

  if (render) {
    InitWindow(screenWidth, screenHeight, "m3. merlins multicore maps");

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    // Main game loop
    int i = 0;
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
      auto RENDER_START = high_resolution_clock::now();
      BeginDrawing();

      ClearBackground(RAYWHITE);

      // 1. triangulation over the voronoi heightmap
      // 2. compute color gradient based on the heightmap
      // 3. render
      //#pragma omp parallel for schedule(static)
      // for (int i = 0; i < mesh->nVoxels; i++) {
      // Color color =
      // get_color(mesh->heightmap[i], mean, var, min_height, max_height);
      // mesh->color[i] = color;
      //}
      for (int i = 0; i < mesh->nVoxels; i++) {
        std::vector<std::vector<Point>> polygon;
        std::vector<Point> poly_points;
        for (int j = 0; j < mesh->edges[i].size(); j++) {
          Point p = {mesh->edges[i][j].v1.x, mesh->edges[i][j].v1.y};
          poly_points.push_back(p);
        }
        polygon.push_back(poly_points);

        std::vector<N> indices = mapbox::earcut<N>(polygon);
        int j = 0;
        // Color color = mesh->color[i];
        Color color =
            get_color(mesh->heightmap[i], mean, var, min_height, max_height);
        for (j = 0; j < indices.size() - 2; j++) {
          // returned vertices are in CW order. raylib expects CCW.
          Point tv_0 = poly_points[indices[j + 2]];
          Point tv_1 = poly_points[indices[j + 1]];
          Point tv_2 = poly_points[indices[j + 0]];
          DrawTriangle((Vector2){tv_0[0], tv_0[1]}, (Vector2){tv_1[0], tv_1[1]},
                       (Vector2){tv_2[0], tv_2[1]}, color);
          // cell_colors[j] = color;
        }

        // for (j = 0; j < indices.size() - 2; j++) {
        // Point tv_0 = poly_points[indices[j + 2]];
        // Point tv_1 = poly_points[indices[j + 1]];
        // Point tv_2 = poly_points[indices[j + 0]];
        // Color color = cell_colors[j];
        // DrawTriangle((Vector2){tv_0[0], tv_0[1]}, (Vector2){tv_1[0],
        // tv_1[1]}, (Vector2){tv_2[0], tv_2[1]}, color);
        //}
      }
      // i = 0;
      EndDrawing();
      //----------------------------------------------------------------------------------
      auto RENDER_END = high_resolution_clock::now();
      if (i == 0) {
        auto render_duration =
            duration_cast<microseconds>(RENDER_END - RENDER_START).count();
        auto voronoi_duration =
            duration_cast<microseconds>(VORONOI_END - VORONOI_START).count();

        auto main_duration =
            duration_cast<microseconds>(MAIN_END - MAIN_START).count();

        auto parsing_duration =
            duration_cast<microseconds>(PARSING_END - PARSING_START).count();
        auto slope_duration =
            duration_cast<microseconds>(SLOPE_END - SLOPE_START).count();
        auto mountains_duration =
            duration_cast<microseconds>(MOUNTAINS_END - MOUNTAINS_START)
                .count();
        auto norm_duration =
            duration_cast<microseconds>(NORM1_END - NORM1_START).count() +
            duration_cast<microseconds>(NORM2_END - NORM2_START).count();
        auto mean_duration =
            duration_cast<microseconds>(MEAN_END - MEAN_START).count();

        auto misc_duration = main_duration - voronoi_duration -
                             parsing_duration - slope_duration -
                             mountains_duration - norm_duration - mean_duration;

        cout << "Render      | " << render_duration << " microseconds" << endl;
        cout << "Voronoi   | " << voronoi_duration << " microseconds" << endl;
        cout << "Parsing   | " << parsing_duration << " microseconds" << endl;
        cout << "Slope     | " << slope_duration << " microseconds" << endl;
        cout << "Mountains | " << mountains_duration << " microseconds" << endl;
        cout << "Norm      | " << norm_duration << " microseconds" << endl;
        cout << "Mean      | " << mean_duration << " microseconds" << endl;
        cout << "Misc      | " << misc_duration << " microseconds" << endl;
        cout << "Main      | " << main_duration + render_duration
             << " microseconds" << endl;

        i += 1;
      }
    }
    std::chrono::milliseconds timewait(1000);

    std::this_thread::sleep_for(timewait);

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
  }

  if (!instrument) {

    printf("%f\n", mean);

    for (int i = 0; i < mesh->nVoxels; i++) {

      printf("%f", mesh->heightmap[i]);
      for (int j = 0; j < mesh->edges[i].size(); j++) {
        printf(" %f %f", mesh->edges[i][j].v1.x, mesh->edges[i][j].v1.y);
      }
      printf("\n");
    }
  }

  else {

    auto voronoi_duration =
        duration_cast<microseconds>(VORONOI_END - VORONOI_START).count();
    auto main_duration =
        duration_cast<microseconds>(MAIN_END - MAIN_START).count();

    auto parsing_duration =
        duration_cast<microseconds>(PARSING_END - PARSING_START).count();
    auto slope_duration =
        duration_cast<microseconds>(SLOPE_END - SLOPE_START).count();
    auto mountains_duration =
        duration_cast<microseconds>(MOUNTAINS_END - MOUNTAINS_START).count();
    auto norm_duration =
        duration_cast<microseconds>(NORM1_END - NORM1_START).count() +
        duration_cast<microseconds>(NORM2_END - NORM2_START).count();
    auto mean_duration =
        duration_cast<microseconds>(MEAN_END - MEAN_START).count();

    auto misc_duration = main_duration - voronoi_duration - parsing_duration -
                         slope_duration - mountains_duration - norm_duration -
                         mean_duration;

    cout << "Voronoi   | " << voronoi_duration << " microseconds" << endl;
    cout << "Parsing   | " << parsing_duration << " microseconds" << endl;
    cout << "Slope     | " << slope_duration << " microseconds" << endl;
    cout << "Mountains | " << mountains_duration << " microseconds" << endl;
    cout << "Norm      | " << norm_duration << " microseconds" << endl;
    cout << "Mean      | " << mean_duration << " microseconds" << endl;
    cout << "Misc      | " << misc_duration << " microseconds" << endl;
    cout << "Main      | " << main_duration << " microseconds" << endl;

    /*
    cout << voronoi_duration << endl;
    cout << parsing_duration << endl;
    cout << slope_duration << endl;
    cout << mountains_duration << endl;
    cout << norm_duration << endl;
    cout << mean_duration << endl;
    cout << misc_duration << endl;
    cout << main_duration << endl;
    */
  }
}
