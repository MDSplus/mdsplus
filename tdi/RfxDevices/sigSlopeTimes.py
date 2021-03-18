def sigSlopeTimes(sig, level, slope ):
   import numpy as np

   yIn = sig.data()
   tIn = sig.dim_of()

   if slope == 'POSITIVE' :
      out = [ t if y <= level and y1 > level else None for y,y1,t in zip(yIn, yIn[1:],tIn)]
   elif slope == 'NEGATIVE' :
      out = [ t if y > level and y1 <= level else None for y,y1,t in zip(yIn, yIn[1:],tIn)]
   else:
      out = [tIn[0]]

   return np.asarray(filter(lambda v : v is not None, out))

