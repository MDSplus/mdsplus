def spiderBlipNumFromTime(time) :

   from MDSplus import Data
   import numpy as np

   sigs = Data.compile('[\RF1_PREF:WAVE, \RF2_PREF:WAVE, \RF3_PREF:WAVE, \RF4_PREF:WAVE]')
   s = Data.compile('caesiumSwitch($1, 0)', sigs)

   tIn = s.dim_of()
   yIn = s.data() 

   blipsOn = [(tIn[i], tIn[i+1]) if yIn[i] > 0 and yIn[i+1] > 0 else None for i in range(len(tIn) - 1)]
   blipsOn = filter(lambda v: v is not None, blipsOn)
   blipsIdx = np.asarray( [ 1 if bl[0] < time and bl[1] > time else 0 for bl in blipsOn ] )

   if sum(blipsIdx) == 0:
      return np.asarray( [ 0, len(blipsIdx)], dtype=np.int32 ) 
   else:
      return np.asarray( [ np.nonzero(blipsIdx)[0][0]+1, len(blipsIdx)], dtype=np.int32 ) 

   
