import torch
import numpy as np
from torch.nn import Sigmoid


def mountain_render_list():
    min_sig, max_sig = -6, 6
    min_mount, max_mount = 0, 32000
    act = Sigmoid()
    gen = torch.Tensor([i for i in range(min_mount, max_mount, 256)])
    gen = gen * ((max_sig - min_sig) / (max_mount - min_mount)) + min_sig
    gen = act(gen)
    gen = gen * (max_mount - min_mount)
    gen = gen.type(torch.int32).numpy()
    gen = np.concatenate(
        (np.array([i for i in range(0, 70, 5)]), gen, np.array([32000]))
    )
    return gen

if __name__ == "__main__":
    print(mountain_render_list())
