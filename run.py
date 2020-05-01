import shlex
import ipdb
import pandas as pd

import subprocess


if __name__ == "__main__":
    N = [512000]
    M = [25]
    I = True
    T = [i for i in range(8)]
    p = subprocess.run(
        shlex.split("make clean"), stdout=subprocess.PIPE, universal_newlines=True
    )
    print(p.stdout)
    p = subprocess.run(
        shlex.split("make mapgen"), stdout=subprocess.PIPE, universal_newlines=True
    )
    ipdb.set_trace()
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
    if I:
        for n in N:
            for m in M:
                for t in T:
                    p = subprocess.run(
                        shlex.split("./mapgen -n {} -m {} -I -t {}".format(n, m, t)),
                        stdout=subprocess.PIPE,
                        universal_newlines=True,
                    )
                    print(p.stdout)
    else:
        for n in N:
            for m in M:
                for t in T:
                    p = subprocess.run(
                        shlex.split("./mapgen -n {} -m {} -t {}".format(n, m, t)),
                        stdout=subprocess.PIPE,
                        universal_newlines=True,
                    )
                    print(p.stdout)
