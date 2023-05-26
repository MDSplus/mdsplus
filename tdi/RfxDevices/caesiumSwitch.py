def caesiumSwitch(sigs, dt) :
   from MDSplus import Data
   import numpy as np

   rf1 = sigs[0]
   rf2 = sigs[1]
   rf3 = sigs[2]
   rf4 = sigs[3]

   #Compute RFs references resaple at 1 Hz   
   rf1_r = Data.compile("resample($1,,,1)", rf1 )
   rf2_r = Data.compile("resample($1,,,1)", rf2 )
   rf3_r = Data.compile("resample($1,,,1)", rf3 )
   rf4_r = Data.compile("resample($1,,,1)", rf4 )
   #Sum RFs references resapled at 1 Hz   
   rf_sum = Data.compile("make_signal($1+$2+$3+$4,,$5)", rf1_r,rf2_r,rf3_r,rf4_r,rf4_r.dim_of() )


   tIn = Data.dim_of(rf_sum)
   yIn = Data.data(rf_sum)
   #yIn is 1 where one RFs references is greater than 50. 
   #The offet of 50 is used in case RF references are not set exactly to 0
   yIn = np.asarray([int(1) if v > 50 else int(0) for v in yIn])


   #Check if all RF power are OFF
   if max(yIn) == 0 :
      return Data.compile("build_signal($1,,$2)", np.asarray([0,0]), np.asarray([tIn[0],tIn[-1]]))   


   #Compute the caesium enable signal with pre and post margin
   def checkEdge(val):
      # 0:y 1:y+1 2:y+2 3:t
      if val[0] == 0 and val[1] > 0 :
         return (val[3]-dt, 0), (val[3]-dt, 1) 
      if val[1] > 0 and val[2] == 0 :
         return (val[3]+dt, 1), (val[3]+dt, 0) 

   csSwitch = [ (yp,y,yn,t) for yp,y,yn,t in zip(yIn,yIn[1:],yIn[2:],tIn) ]
   csSwitch = filter(lambda v: v is not None, map( checkEdge, csSwitch ))

   #Set caesium signal boundaries   
   tMin = tIn[0]
   tMax = tIn[-1] 
   if csSwitch[0][0][0] < tMin:
      csSwitch[0] = ((tMin,0),(tMin,1))
   else:
      csSwitch.insert(0,((tMin,0),(tMin,0)))

   if csSwitch[-1][0][0] < tMax:
      csSwitch.append(((tMax,0),(tMax,0)))
   else:
      csSwitch[-1] = ((tMax,1),(tMax,0))
   
   #Remove point when margin is overlapped
   csSw=[]
   skipNext=False
   for v,v1 in zip(csSwitch,csSwitch[1:]):
       if skipNext:
          skipNext=False
          continue            
       if v[0][0] > v1[0][0]:
          skipNext=True
          continue
       else:
          csSw.append(v)
   csSw.append(v1)
  
   #Reshape List as an array even idx is time, odd idx is value
   csSwitch = np.reshape(csSw,len(np.asarray(csSw))*4)
  
   outSig = Data.compile("build_signal($1,,$2)", csSwitch[1::2], csSwitch[0::2] )
       
   return  outSig



