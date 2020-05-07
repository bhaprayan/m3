<img src="documentation/title.png">

## Evolution Visualization
(Note: GIFs may need few seconds to load)

### Forward Evolution
<img src="documentation/evolution_forward.gif" width="50%">

### Backward Evolution
<img src="documentation/evolution_backwards.gif" width="50%">

## Execution

```
make mapgen
./mapgen -n [nVoxels] -m [nMountains], -t [nThreads]
```

### External Libraries & APIs

* [Generating Fantasy Maps (inspiration)](http://mewo2.com/notes/terrain/)
* [FortuneSweep Voronoi Generation](https://github.com/JCash/voronoi)
* [Graphics.py](https://mcsp.wartburg.edu/zelle/python/graphics.py)
* [Raylib](https://www.raylib.com)
* [EarCut Tesselation](https://github.com/mapbox/earcut)
* [OpenMP](https://www.openmp.org)

## Algorithm Design

<img src="documentation/block_diagram.jpg" width="75%">

The block diagram gives an overview of the map generation and rendering pipeline. Arrows are used to indicate a linear set of dependencies between different stages of the pipeline. CPU-executed components are indicated in yellow, and GPU components are indicated in green.

* **Voronoi** - Compute a randomized Voronoi map with a prespecified number of voxels using the Fortune Sweep algorithm.
* **Parsing** - Iterate over the centroids and edges of the returned Voronoi map and store them in the custom map data structure. Allocate a zeroed out heightmap of the same size as the number of voxels.
* **Slope** - Generate a random 2D vector representation of a line and compute the dot product of each voxel centroid to the line. Store this distance in the voxel's heightmap.
* **Mountain** - Generate 2D mountain locations. For each voxel, compute the distance between the voxel and mountain center. As an effect of this computation, heightmap values of voxels nearer to the mountain will be influenced more.
* **Normalize** - Retrieve the min and max height values across the global heightmap. Subtract the min height from each value in the heightmap, and divide by the difference between the max and min (i.e. heightmap normalization).
* **Mean** - Retrieve the mean of the heightmap list.
* **Render** - Compute the color for each voxel as a function of its height through a threshold and affine transformation based coloring scheme. Before rendering each voxel, tessellate the polygon using the EarCut algorithm, and render each polygon with the computed colors.

### Performance

Results of Fig 1-3 computed on an 8-core Linux machine.<br>
Results of Fig 4 computed on an 4-core Linux machine.

Fig 1. Generation time in μsec as voxels increase, m=25.<br>
<img src="documentation/voxel_speedup.png" width="50%">

Fig 2. Generation time in μsec with n=320000, m=25.<br>
<img src="documentation/voxel_breakdown.png" width="50%">

Fig 3. Generation time in μsec as mountains increase, n=32000.<br>
<img src="documentation/mountain_speedup.png" width="50%">

Fig 4. Generation time in μsec with n=320000, m=32000.<br>
<img src="documentation/mountain_breakdown.png" width="50%">


## Gallery

(Click on the image to reveal a high-res version with the number of voxels and mountains listed.)

<p float="left">
  <img src="documentation/n256m8.png" width="20%" />
  <img src="documentation/n256m16.png" width="20%" />
  <img src="documentation/n256m32.png" width="20%" />
  <img src="documentation/n2048m1.png" width="20%" />
  <img src="documentation/n2048m30.png" width="20%" />
  <img src="documentation/n2048m32.png" width="20%" />
  <img src="documentation/n2048m64.png" width="20%" />
  <img src="documentation/n2048m128.png" width="20%" />
  <img src="documentation/n8192m16.png" width="20%" />
  <img src="documentation/n32000m1600.png" width="20%" />
  <img src="documentation/n32000m3200.png" width="20%" />
  <img src="documentation/n32000m6400.png" width="20%" />
</p>

