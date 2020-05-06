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



Fig 1. Generation time in microsec as voxels increase, m=25.<br>
<img src="documentation/voxel_speedup.png" width="50%">

Fig 2. Generation time in microsec with n=320000, m=25.<br>
<img src="documentation/voxel_breakdown.png" width="50%">

Fig 3. Generation time in microsec as mountains increase, n=32000.<br>
<img src="documentation/mountain_speedup.png" width="50%">

Fig 4. Generation time in microsec with n=320000, m=32000.<br>
<img src="documentation/mountain_breakdown.png" width="50%">
