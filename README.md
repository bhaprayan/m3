<img src="documentation/title.png">

## Evolution Visualization
(Note: GIFs may need few seconds to load)

### Forward Evolution
<img src="documentation/evolution_forward.gif" width="50%">

### Backwards Evolution
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

<img src="documentation/block_diagram.png" width="50%">

### Performance

<img src="documentation/voxel_speedup.png" width="50%">
Generation time varied over the num-ber of voxels with the number of mountains fixedto m=25.   Horizontal axis controls the numberof voxels and Vertical axis measure the executiontime in microseconds.
<img src="documentation/voxel_breakdown.png" width="50%">
<img src="documentation/mountain_speedup.png" width="50%">
<img src="documentation/mountain_breakdown.png" width="50%">
