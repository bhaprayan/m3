import shlex
import ipdb
import math
from rescale import mountain_render_list

total_M = int(math.log2(8196 * 4))

import subprocess


if __name__ == "__main__":
    epochs = 3
    N = [500, 1000, 2000, 4000, 8000, 16000, 32000]
    # M = mountain_render_list()
    M = [3200]
    print("total mountains:", len(M))
    # T = [i for i in range(8)]
    T = [1]
    p = subprocess.run(
        shlex.split("make clean"), stdout=subprocess.PIPE, universal_newlines=True
    )
    print(p.stdout)
    p = subprocess.run(
        shlex.split("make mapgen"), stdout=subprocess.PIPE, universal_newlines=True
    )
    print(p.stdout)
    """
    Voronoi | 1348682 microseconds
    Parsing | 29075 microseconds
    Slope | 320 microseconds
    Mountains | 4123175 microseconds
    Norm | 665 microseconds
    Mean | 207 microseconds
    Misc | 178 microseconds
    Main | 5502302 microseconds
    """
    # func_timing = p.stdout.split() # newlines
    # func_timing = list(map(labmda x: x.split(' '), func_timing)) # individual functions
    for n in N:
        for m in M:
            for t in T:
                for r in range(epochs):
                    print("epoch:", r)
                    print("vcells:", n)
                    print("mountains:", m)
                    print("threads:", t)
                    p = subprocess.run(
                        shlex.split("./mapgen -n {} -m {} -t {}".format(n, m, t)),
                        stdout=subprocess.PIPE,
                        universal_newlines=True,
                    )
                    print(p.stdout)
                    print("-" * 50)
