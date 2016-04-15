import numpy as np
def raspi_rgb(r,g,b):
    shape = r.data().shape + (3,)
    ans = np.empty(shape, dtype=np.uint8)
    ans[...,0]=r.data()
    ans[...,1]=g.data()
    ans[...,2]=b.data()
    return ans
