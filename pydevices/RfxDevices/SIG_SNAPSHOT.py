#
# Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

"""
RfxDevices
==========
@authors: Gabriele Manduchi (Consorzio RFX Padova)
@copyright: 2019
@license: GNU GPL
"""
from MDSplus import mdsExceptions, Device, Data, Int64, Int64Array, Uint64, Event, Float32, Float64, Float32Array
from MDSplus.mdsExceptions import DevCOMM_ERROR
from MDSplus.mdsExceptions import DevBAD_PARAMETER
from threading import Thread
from ctypes import CDLL, c_int, byref, c_byte, c_ulonglong, c_ubyte
from time import sleep
import sys
import numpy as np
import select
import os
try:
  import psycopg2
except:
  pass
class SIG_SNAPSHOT(Device):
    """National Instrument 6683 device. Generation of clock and triggers and recording of events """
    parts = [{'path':':COMMENT', 'type':'text'},
             {'path':':DATABASE', 'type':'text'},
             {'path':':DB_SERVER', 'type':'text'},
             {'path':':DB_USER', 'type':'text'},
             {'path':':DB_PASSWD', 'type':'text'},
             {'path':':TABLE', 'type':'text'},
             {'path':':TIMES', 'type':'numeric'},
             {'path':':NUM_SIGNALS', 'type':'numeric', 'value': 0},
             {'path':':UPDATE_SECS', 'type':'numeric', 'value': 1}]
    for sigIdx in range(500):
        parts.append({'path':'.SIGNAL_'+str(sigIdx+1), 'type':'structure'})
        parts.append({'path':'.SIGNAL_'+str(sigIdx+1)+':COL_NAME', 'type':'text'})
        parts.append({'path':'.SIGNAL_'+str(sigIdx+1)+':ALGORITHM', 'type':'text', 'value':'MEAN'})
        parts.append({'path':'.SIGNAL_'+str(sigIdx+1)+':PARAMS', 'type':'numeric'})
        parts.append({'path':'.SIGNAL_'+str(sigIdx+1)+':START', 'type':'numeric', 'value' : -1})
        parts.append({'path':'.SIGNAL_'+str(sigIdx+1)+':END', 'type':'numeric', 'value' : 1})
        parts.append({'path':'.SIGNAL_'+str(sigIdx+1)+':IN_SIGNAL', 'type':'numeric'})
        parts.append({'path':'.SIGNAL_'+str(sigIdx+1)+':SNAPSHOTS', 'type':'signal'})
    for sigIdx in range(500):
        parts.append({'path':'.SIGNAL_'+str(sigIdx+1)+':USE_ABS_TIME', 'type':'text','value':'NO'})
    for sigIdx in range(500):
        parts.append({'path':'.SIGNAL_'+str(sigIdx+501), 'type':'structure'})
        parts.append({'path':'.SIGNAL_'+str(sigIdx+501)+':COL_NAME', 'type':'text'})
        parts.append({'path':'.SIGNAL_'+str(sigIdx+501)+':ALGORITHM', 'type':'text', 'value':'MEAN'})
        parts.append({'path':'.SIGNAL_'+str(sigIdx+501)+':PARAMS', 'type':'numeric'})
        parts.append({'path':'.SIGNAL_'+str(sigIdx+501)+':START', 'type':'numeric', 'value' : -1})
        parts.append({'path':'.SIGNAL_'+str(sigIdx+501)+':END', 'type':'numeric', 'value' : 1})
        parts.append({'path':'.SIGNAL_'+str(sigIdx+501)+':IN_SIGNAL', 'type':'numeric'})
        parts.append({'path':'.SIGNAL_'+str(sigIdx+501)+':SNAPSHOTS', 'type':'signal'})
    for sigIdx in range(500):
        parts.append({'path':'.SIGNAL_'+str(sigIdx+501)+':USE_ABS_TIME', 'type':'text','value':'NO'})
    parts.append({'path':':WRITE_TOI', 'type':'text'})
    parts.append({'path':':BLIP_SIG', 'type':'numeric'})
    parts.append({'path':':START_STORE','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','PON',51,None),Method(None,'start_store',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':STOP_STORE','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','PPC',50,None),Method(None,'stop_store',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':ST_RDB_STORE','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','PPC',50,None),Method(None,'start_rdb_store',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':SP_RDB_STORE','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','PPC',50,None),Method(None,'stop_rdb_store',head))",
        'options':('no_write_shot',)})
    
    workerDict = {}
    workerRdbDict = {}

    class AsynchRdbStore(Thread):

        def configure(self, device):
            self.device = device
            self.updateSecs = device.update_secs.data()
            self.stopReq = False
            
        def run(self):
          while not self.stopReq:
            for i in range(self.updateSecs):
              sleep(1)
              if self.stopReq:
                break
            if self.stopReq:
                self.device.rdb_store()
                return
            self.device.rdb_store()
            if  self.stopReq:
              return


        def stop(self):
          self.stopReq = True
################End class AsynchRdbStore 


    class AsynchStore(Thread):

        def configure(self, device):
            self.device = device
            try:
              self.times = device.times.data()
            except:
              Data.execute('DevLogErr($1,$2)', device.getNid(), 'Error reading snapshot times')
              raise DevCOMM_ERROR
            try:
              self.numSignals = device.num_signals.data()
            except:
              Data.execute('DevLogErr($1,$2)', device.getNid(), 'Error reading Number of signals')
              raise DevCOMM_ERROR
            
            try:
              self.updateSecs = device.update_secs.data()
            except:
              self.updateSecs = 1
            self.useAbsTime = []
            self.inSignals = []
            self.snapshots = []
            self.endTimes = []
            self.startTimes = []
            self.isEmpty = []
            self.lastTimeServed = []
            self.algorithms = []
            for sigIdx in range(self.numSignals):
              print('signal_'+str(sigIdx + 1)+'_in_signal')
              self.inSignals.append(getattr(device, 'signal_'+str(sigIdx + 1)+'_in_signal'))
              self.snapshots.append(getattr(device, 'signal_'+str(sigIdx + 1)+'_snapshots'))
              self.useAbsTime.append(getattr(device, 'signal_'+str(sigIdx + 1)+'_use_abs_time'))
              self.algorithms.append(getattr(device, 'signal_'+str(sigIdx + 1)+'_algorithm'))
              self.lastTimeServed.append(self.times[0] - 1)
              try:
                self.startTimes.append(getattr(device, 'signal_'+str(sigIdx + 1)+'_start').data().astype('float64') + self.times)
                self.endTimes.append(getattr(device, 'signal_'+str(sigIdx + 1)+'_end').data().astype('float64') + self.times)
              except:
                Data.execute('DevLogErr($1,$2)', device.getNid(), 'Error reading start or end for  signal '+str(sigIdx+1))
                raise DevCOMM_ERROR
              self.isEmpty.append(True)
            self.stopReq = False

        def computeMean(self, samples, times, startTime, endTime):
          return np.mean(samples[np.logical_and(times >= startTime, times <= endTime)])
          
        def computeStd(self, samples, times, startTime, endTime):
          return np.std(samples[np.logical_and(times >= startTime, times <= endTime)])
          

        def check(self):
          allDone = True
          minStart = 1E6
          for sigIdx in range(len(self.inSignals)):
            if len(self.startTimes[sigIdx]) > 0 and self.startTimes[sigIdx][0] < minStart:
              minStart =  self.startTimes[sigIdx][0] 
          if minStart == 1E6: # no left times
            return True
          print('MIN START: '+str(minStart))
           
          for sigIdx in range(len(self.inSignals)):
            inSignal = self.inSignals[sigIdx]
            snapshots = self.snapshots[sigIdx]
            startTimes = self.startTimes[sigIdx]
            endTimes = self.endTimes[sigIdx]
            if len(startTimes) == 0:  #this signal is done
              continue
 #           print (inSignal.getData())
            signalEnd = 4000
            firstLoop = True
            while len(endTimes) > 0 : #and endTimes[0] <= signalEnd:
              

              try:
                if self.useAbsTime[sigIdx].data() == 'NO':
                  print("TIME CONTEXT: ",startTimes[0],endTimes[0]) 
                  if len(endTimes) == 1:
                    inSignal.getTree().setTimeContext(Float64(startTimes[0]), Float64(endTimes[0]), None)
                  else:
                    inSignal.getTree().setTimeContext(Float64(startTimes[0]), Float64(endTimes[1]), None)
                  print('READING....')
                  currData = inSignal.data()
                  if len(currData)== 0:
                    break;
                  print('READING DIM...')
                  currTimes = inSignal.getDimensionAt(0).data().flatten()
                  print('READ')
                else:
                  print("ABSOLUTE TIMES: NO TIME CONTEXT") 
                  inSignal.getTree().setTimeContext(None, None, None)
                  if firstLoop:
                    print('READING....')
                    currData = inSignal.data()
                    print('READING DIM...')
                    currTimes = inSignal.getDimensionAt(0).data().flatten()
                    print('READ')
                    firstLoop = False
               
                if(len(currData.shape) > 1 and currData.shape[0] == 1):
                  currData = currData.flatten()
                signalEnd = currTimes.flatten()[-1]
                print('SIGNAL '+ inSignal.getPath() + ' TERMINATES AT '+ str(signalEnd))
                if currData.shape[0] > currTimes.shape[0]:
                  currData = currData[:currTimes.shape[0]]
                if currData.shape[0] < currTimes.shape[0]:
                  currTimes = currTimes[:currData.shape[0]]
            
              except:
                print('READOUT OF ' + inSignal.getFullPath()+'   FAILED')
                break  
      
              roi = currData[np.logical_and(currTimes >= startTimes[0], currTimes <= endTimes[0])]
              if roi.size >  0:
                resultData = 0 
                if(len(roi) > 0):
                  if self.algorithms[sigIdx].data() == 'MEAN':
                    resultData = np.mean(roi,0)
                  if self.algorithms[sigIdx].data() == 'STD':
                    resultData = np.std(roi,0)
 
                if self.isEmpty[sigIdx]:
                  self.isEmpty[sigIdx] = False
                  inShape = currData.shape
                  shape = []
                  for s in inShape:
                    shape.append(s)
                  shape[0] = len(self.times)
                  snapshots.beginSegment(Float32(self.times[0]), Float32(self.times[-1]), Float32Array(self.times), Float32Array(np.zeros(shape, currData.dtype)))
                if np.isscalar(resultData):
                   snapshots.putSegment(Float32(resultData))
                else:
                   snapshots.putSegment(Float32Array(resultData))
              else:
                break
              #else: #for any reason roi has no elements, save in any case the first sample if the interval considered
                #if self.isEmpty[sigIdx]:
                  #self.isEmpty[sigIdx] = False
                  #inShape = currData.shape
                  #shape = []
                  #for s in inShape:
                    #shape.append(s)
                  #shape[0] = len(self.times)
                  #snapshots.beginSegment(Float32(self.times[0]), Float32(self.times[-1]), Float32Array(self.times), Float32Array(np.zeros(shape, currData.dtype)))
                #if np.isscalar(currData[0]):
                   #snapshots.putSegment(Float32(currData[0]))
                #else: 
                   #snapshots.putSegment(Float32Array(currData[0]))
              startTimes = startTimes[1:]
              endTimes = endTimes[1:]
              if len(startTimes) > 1 and signalEnd <= startTimes[0]:
                break
            #endwhile
            self.startTimes[sigIdx] = startTimes
            self.endTimes[sigIdx] = endTimes
            inSignal.getTree().setTimeContext(None, None, None)
          #endfor
          for sigIdx in range(len(self.inSignals)):
            endTimes = self.endTimes[sigIdx]
            if len(endTimes) > 0:
              allDone = False

          inSignal.getTree().setTimeContext(None, None, None)
          return allDone

        def checkOnline(self):
          allDone = True
          minStart = 1E6
          for sigIdx in range(len(self.inSignals)):
            if len(self.startTimes[sigIdx]) > 0 and self.startTimes[sigIdx][0] < minStart:
              minStart =  self.startTimes[sigIdx][0] 
          if minStart == 1E6: # no left times
            return True
          print('MIN START: '+str(minStart))
           
          for sigIdx in range(len(self.inSignals)):
            inSignal = self.inSignals[sigIdx]
            snapshots = self.snapshots[sigIdx]
            startTimes = self.startTimes[sigIdx]
            endTimes = self.endTimes[sigIdx]
            if len(startTimes) == 0:  #this signal is done
              continue
            print (inSignal.getData())
            signalEnd = 4000
            firstLoop = True
            while len(endTimes) > 0 : #and endTimes[0] <= signalEnd:
              

              try:
                if firstLoop:
                  if self.useAbsTime[sigIdx].data() == 'NO':
                    #print("START TIME CONTEXT: ",startTimes[0]) 
                    inSignal.getTree().setTimeContext(Float64(startTimes[0]), None, None)
                    #print('READING....')
                    currData = inSignal.data()
                    if len(currData)== 0:
                      break;
                   #print('READING DIM...')
                    currTimes = inSignal.getDimensionAt(0).data().flatten()
                    #print('READ')
                  else:
                    print("ABSOLUTE TIMES: NO TIME CONTEXT") 
                    inSignal.getTree().setTimeContext(None, None, None)
                   # print('READING....')
                    currData = inSignal.data()
                  #  print('READING DIM...')
                    currTimes = inSignal.getDimensionAt(0).data().flatten()
                   # print('READ')
               
                  if(len(currData.shape) > 1 and currData.shape[0] == 1):
                    currData = currData.flatten()
                  signalEnd = currTimes.flatten()[-1]
                  #print('SIGNAL '+ inSignal.getPath() + ' TERMINATES AT '+ str(signalEnd))
                  if currData.shape[0] > currTimes.shape[0]:
                    currData = currData[:currTimes.shape[0]]
                  if currData.shape[0] < currTimes.shape[0]:
                    currTimes = currTimes[:currData.shape[0]]
                  firstLoop = False
                  
              except:
                print('READOUT OF ' + inSignal.getFullPath()+'   FAILED')
                break  
      
              roi = currData[np.logical_and(currTimes >= startTimes[0], currTimes <= endTimes[0])]
              if roi.size >  0:
                resultData = 0 
                if(len(roi) > 0):
                  if self.algorithms[sigIdx].data() == 'MEAN':
                    resultData = np.mean(roi,0)
                  if self.algorithms[sigIdx].data() == 'STD':
                    resultData = np.std(roi,0)
 
                if self.isEmpty[sigIdx]:
                  self.isEmpty[sigIdx] = False
                  inShape = currData.shape
                  shape = []
                  for s in inShape:
                    shape.append(s)
                  shape[0] = len(self.times)
                  snapshots.beginSegment(Float32(self.times[0]), Float32(self.times[-1]), Float32Array(self.times), Float32Array(np.zeros(shape, currData.dtype)))
                if np.isscalar(resultData):
                   snapshots.putSegment(Float32(resultData))
                else:
                   snapshots.putSegment(Float32Array(resultData))
              else:
                print('WARNING!! EMPTY ROI!!!!')
                break
              #else: #for any reason roi has no elements, save in any case the first sample if the interval considered
                #if self.isEmpty[sigIdx]:
                  #self.isEmpty[sigIdx] = False
                  #inShape = currData.shape
                  #shape = []
                  #for s in inShape:
                    #shape.append(s)
                  #shape[0] = len(self.times)
                  #snapshots.beginSegment(Float32(self.times[0]), Float32(self.times[-1]), Float32Array(self.times), Float32Array(np.zeros(shape, currData.dtype)))
                #if np.isscalar(currData[0]):
                   #snapshots.putSegment(Float32(currData[0]))
                #else: 
                   #snapshots.putSegment(Float32Array(currData[0]))
              startTimes = startTimes[1:]
              endTimes = endTimes[1:]
              if len(startTimes) > 1 and signalEnd <= startTimes[0]:
                break
            #endwhile
            self.startTimes[sigIdx] = startTimes
            self.endTimes[sigIdx] = endTimes
            inSignal.getTree().setTimeContext(None, None, None)
          #endfor
          for sigIdx in range(len(self.inSignals)):
            endTimes = self.endTimes[sigIdx]
            if len(endTimes) > 0:
              allDone = False

          inSignal.getTree().setTimeContext(None, None, None)
          return allDone



        def run(self):
          while not self.stopReq:
            for i in range(self.updateSecs):
              sleep(1)
              if self.stopReq:
                break
            if self.stopReq:
                self.check()
                return
            allDone = self.checkOnline()
            print('ALL DONE: ' + str(allDone))
            if allDone or self.stopReq:
              return


        def stop(self):
          self.stopReq = True
################End class AsynchStore 


    
    def start_store(self):
      nSignals = self.num_signals.data()
      #Clear output signals first
      for sigIdx in range(nSignals):
        getattr(self, 'signal_'+str(sigIdx + 1)+'_snapshots').deleteData()
      
      try:
        worker = SIG_SNAPSHOT.workerDict[self.nid]
        if worker.isAlive():
          worker.stop
 #         worker.join()
      except:
        pass
      worker = self.AsynchStore()
      SIG_SNAPSHOT.workerDict[self.nid] = worker
      worker.configure(self.copy())
      worker.daemon = True
      worker.start()
     
      
    def stop_store(self):
      worker = SIG_SNAPSHOT.workerDict[self.nid]
      if worker.isAlive():
        print("SIG_SNAPSHOT stop_worker")
        worker.stop()
 #       worker.join()
        return 1

    def store(self):
      nSignals = self.num_signals.data()
      #Clear output signals first
      for sigIdx in range(nSignals):
        print(getattr(self, 'signal_'+str(sigIdx + 1)+'_snapshots'))
        getattr(self, 'signal_'+str(sigIdx + 1)+'_snapshots').deleteData()
      worker = self.AsynchStore()
      worker.configure(self.copy())
      worker.check()
      return 1
      
    def getBlipNumbers(self):
     # try:
      #print('LEGGO BLIPS')
      blips = self.getTree().getNode('\SPIDER::TOP.PLANTS.POWER.CAESIUM.CONTROL:SWITCH_SIG').data()
      #print('LETTO BLIPS')
      blipTimes = self.getTree().getNode('\SPIDER::TOP.PLANTS.POWER.CAESIUM.CONTROL:SWITCH_SIG').dim_of().data()
      #print('LETTO TIMES')
      #except:
        #return []
      times = self.times.data()
      blipNumbers = []
      for time in times:
        bp = 0
        for currIdx in range(1, len(blips)): 
          if blipTimes[currIdx] > time:
            break
          if blips[currIdx - 1] == 0 and blips[currIdx] == 1:
            bp = bp + 1
        blipNumbers.append(bp)
      print(blipNumbers)
      return blipNumbers  


    def rdb_store(self):
      try:
        times = self.times.data()
      except:
        return 1
      if len(times) < 1:
        return 1
      signals = []
      names = []
      paths = []
      numSignals = self.num_signals.data()
      for sigIdx in range(numSignals):
        try:
            signals.append(getattr(self, 'signal_'+str(sigIdx + 1)+'_snapshots').data())
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'No snapshot acquired for signal '+str(sigIdx+1))
          signals.append([])
          paths.append("")
          names.append("")
          continue
        paths.append(getattr(self, 'signal_'+str(sigIdx + 1)+'_snapshots').getFullPath())
        try:
          names.append(getattr(self, 'signal_'+str(sigIdx + 1)+'_col_name').data().lower())
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot read column name for signal '+ str(sigIdx+1))
          raise DevCOMM_ERROR

      try:
        host = self.db_server.data()
      except:
        host = ''
      try:
        database = self.database.data()
        user = self.db_user.data()
        password = self.db_passwd.data()
        table = self.table.data()
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot read database access configuration')
        raise DevCOMM_ERROR
      
      try:
        conn = psycopg2.connect(host=host,database=database, user=user, password=password)
      except :
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot connect to database server '+host+'; ')
        raise DevCOMM_ERROR
      
      cursor = conn.cursor()
      
      for sigIdx in range(len(signals)):
        if names[sigIdx] == '':
          continue  
             # This command creates the table and the indices if needed
        tableName = names[sigIdx].upper() + '_table'
        createCommand =  "DO $$ BEGIN "
        createCommand += "IF NOT EXISTS ("
        createCommand += "SELECT 1 FROM pg_tables "
        createCommand += "WHERE tablename = '" + tableName.lower() + "'"
        createCommand += ") THEN "
        createCommand += "CREATE TABLE " + tableName + " ("
        createCommand += "id SERIAL PRIMARY KEY, "
        createCommand += "experiment VARCHAR, "
        createCommand += "shot INTEGER, "
        createCommand += "sig_idx INTEGER, "
        createCommand += "snap_time REAL, "
        createCommand += "snap_idx INTEGER, "
        createCommand += names[sigIdx] + "_path VARCHAR, "
        createCommand += names[sigIdx] + " REAL"
        
        if len(signals[sigIdx].shape) == 2:
          createCommand += '[]'

        createCommand += '); ' 

        createCommand += "CREATE INDEX idx2" + names[sigIdx] + " ON " + tableName + "(shot, snap_idx); "
	createCommand += "GRANT SELECT ON " + tableName + " IN SCHEMA public TO sadiag; "
        createCommand += "END IF; "
        createCommand += "END $$;"

        #print(createCommand)
        try:
          cursor.execute(createCommand)
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot create table: ')
          raise DevCOMM_ERROR

      
      
      for timeIdx in range(len(times)):
        for sigIdx in range(len(signals)):
          if names[sigIdx] == '':
            continue  
          if len(signals[sigIdx]) <= timeIdx:
            continue  #do not update empty signals 
          checkQuery='SELECT experiment from '+ names[sigIdx]+'_table WHERE shot = '+ str(self.getTree().shot) + ' AND sig_idx = '+str(sigIdx) +' AND snap_time = '+str(times[timeIdx]) 
          try:
            cursor.execute(checkQuery)
          except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot make select query: ')
            raise DevCOMM_ERROR
          exps = cursor.fetchall()
          sqlOk = True
          if len(exps) == 0:
            insertCommand = 'INSERT INTO ' + names[sigIdx]+'_table (experiment, shot, sig_idx, snap_time)  VALUES(\''+self.getTree().name+'\','
            insertCommand += str(self.getTree().shot)+', '+str(sigIdx)+', '+str(times[timeIdx])+')'
            #print(insertCommand)
            try:
              cursor.execute(insertCommand)
            except:
              Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot insert in table('+insertCommand+'): ')
              print(sys.exc_info()[0])
              raise DevCOMM_ERROR
        
          insertCommand = 'UPDATE '+ names[sigIdx]+'_table SET '+names[sigIdx]+'_path = \''+ paths[sigIdx]+'\','+names[sigIdx]+' = '
          if len(signals[sigIdx].shape) == 1:
            if len(signals[sigIdx]) > timeIdx:
              if np.isnan(signals[sigIdx][timeIdx]) or np.isinf(signals[sigIdx][timeIdx]):
                sqlOk = False
              insertCommand += str(signals[sigIdx][timeIdx])
            else:
              insertCommand += '0'
          elif len(signals[sigIdx].shape) == 2:
 #           insertCommand += ',ARRAY['+str(signals[sigIdx][timeIdx][0])
            #print(signals[sigIdx].shape)
            insertCommand += 'ARRAY['+str(signals[sigIdx][timeIdx][0])
            for i in range(1, len(signals[sigIdx][timeIdx])):
              if np.isnan(signals[sigIdx][timeIdx]) or  np.isinf(signals[sigIdx][timeIdx]):
                sqlOk = False
              insertCommand += ','+str(signals[sigIdx][timeIdx][i])
            insertCommand += ']'
          insertCommand += ' WHERE experiment = \''+self.getTree().name+'\' AND shot = '+str(self.getTree().shot)+' AND sig_idx = '+str(sigIdx) +' AND snap_time = '+str(times[timeIdx])
          #print(insertCommand)
          if sqlOk:
            try:
              cursor.execute(insertCommand)
            except:
              Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot insert in table('+insertCommand+'): ')
              print(sys.exc_info()[0])
              raise DevCOMM_ERROR
          
        #endfor
      #endfor
      
      
      #create/update table with TOI absolute times
        fill_toi_table = False
        try:
          if self.write_toi.data() == 'YES':
            fill_toi_table = True
        except:
          pass
        if fill_toi_table:
          try:
            startAbsTime = self.getTree().tdiExecute('getSpiderShotTime(,1)').data()
            #print('start abs time: ' + str(startAbsTime))
            createCommand = 'CREATE TABLE IF NOT EXISTS toi_time_table (id SERIAL PRIMARY KEY, experiment VARCHAR, shot INTEGER, toi_idx INTEGER, toi_abs_time BIGINT, snap_idx INTEGER)'
            #print(createCommand)
            try:
              cursor.execute(createCommand)
            except:
              Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot create table toi_time_table ')
              raise DevCOMM_ERROR
            for toiIdx in range(0, len(times)):
              checkQuery='SELECT experiment from toi_time_table WHERE shot = '+ str(self.getTree().shot) + ' AND toi_idx = '+str(toiIdx) 
              #print(checkQuery)
              try:
                cursor.execute(checkQuery)
              except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot make select query ')
                raise DevCOMM_ERROR
              exps = cursor.fetchall()
              if len(exps) == 0:
                insertCommand = 'INSERT INTO toi_time_table (experiment, shot, toi_idx, toi_abs_time, snap_idx)  VALUES(\''+self.getTree().name+'\','+str(self.getTree().shot)+','+str(toiIdx)+','+str(int(times[toiIdx]*1000000000) + startAbsTime)+','+str(timeIdx)+')'
                #print(insertCommand)
                try:
                  cursor.execute(insertCommand)
                except:
                  Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot insert in table toi_times_table ')
                  print(sys.exc_info()[0])
                  raise DevCOMM_ERROR
          #end for toiIdx
          except:
            print('Cannot get spider abs time')

          #create/update table with toi_blib_table
          blipNumbers = self.getBlipNumbers()
          if blipNumbers == []:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot get blip numbers ')
            print(sys.exc_info()[0])
            raise DevCOMM_ERROR
          else:
            createCommand = 'CREATE TABLE IF NOT EXISTS toi_blip_table (id SERIAL PRIMARY KEY, experiment VARCHAR, shot INTEGER, toi_idx INTEGER, toi_blip_number INTEGER, snap_idx INTEGER)'
            #print(createCommand)
            try:
              cursor.execute(createCommand)
            except:
              Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot create table toi_blip_table ')
              raise DevCOMM_ERROR

            for toiIdx in range(0, len(times)):
              checkQuery='SELECT experiment from toi_blip_table WHERE shot = '+ str(self.getTree().shot) + ' AND toi_idx = '+str(toiIdx)
              #print(createCommand)
              try:
                cursor.execute(checkQuery)
              except:
                Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot make select query ')
                raise DevCOMM_ERROR
              exps = cursor.fetchall()
              if len(exps) == 0:
                insertCommand = 'INSERT INTO toi_blip_table (experiment, shot, toi_idx, toi_blip_number, snap_idx)  VALUES(\''+self.getTree().name+'\','+str(self.getTree().shot)+','+str(toiIdx)+','+str(blipNumbers[toiIdx])+','+str(timeIdx)+')'
                #print(insertCommand)
                try:
                  cursor.execute(insertCommand)
                except:
                  Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot insert in table toi_blip_table ')
                  print(sys.exc_info()[0])
                  raise DevCOMM_ERROR
            #end for toiIdx
          #endif
      #endif
      conn.commit()
      
      
      
    def start_rdb_store(self):
      nSignals = self.num_signals.data()
      #Clear output signals first
      for sigIdx in range(nSignals):
        getattr(self, 'signal_'+str(sigIdx + 1)+'_snapshots').deleteData()
      
      try:
        worker = SIG_SNAPSHOT.workerRdbDict[self.nid]
        if worker.isAlive():
          worker.stop
         # worker.join()
      except:
        pass
      worker = self.AsynchRdbStore()
      SIG_SNAPSHOT.workerRdbDict[self.nid] = worker
      worker.configure(self.copy())
      worker.daemon = True
      worker.start()
     
      
    def stop_rdb_store(self):
      worker = SIG_SNAPSHOT.workerRdbDict[self.nid]
      if worker.isAlive():
        print("SIG_SNAPSHOT stop_rdb_worker")
        worker.stop()
        
##ADDED COMMAND TO UPDATE TABLE shot_times_table
        
        
        #worker.join()
  
      os.system('fill_time_info.py '+ str(self.getTree().shot)+' '+str(self.getTree().shot))
     
      
      return 1


      
      
