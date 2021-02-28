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
from MDSplus import mdsExceptions, Device, Data, Int64, Int64Array, Uint64, Event, Float32
from MDSplus.mdsExceptions import DevCOMM_ERROR
from MDSplus.mdsExceptions import DevBAD_PARAMETER
from threading import Thread
from ctypes import CDLL, c_int, byref, c_byte, c_ulonglong, c_ubyte
from time import sleep
import sys
import numpy as np
import select
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
    for sigIdx in range(512):
        parts.append({'path':'.SIGNAL_'+str(sigIdx+1), 'type':'structure'})
        parts.append({'path':'.SIGNAL_'+str(sigIdx+1)+':COL_NAME', 'type':'text'})
        parts.append({'path':'.SIGNAL_'+str(sigIdx+1)+':ALGORITHM', 'type':'text', 'value':'MEAN'})
        parts.append({'path':'.SIGNAL_'+str(sigIdx+1)+':PARAMS', 'type':'numeric'})
        parts.append({'path':'.SIGNAL_'+str(sigIdx+1)+':START', 'type':'numeric', 'value' : -1})
        parts.append({'path':'.SIGNAL_'+str(sigIdx+1)+':END', 'type':'numeric', 'value' : 1})
        parts.append({'path':'.SIGNAL_'+str(sigIdx+1)+':IN_SIGNAL', 'type':'numeric'})
        parts.append({'path':'.SIGNAL_'+str(sigIdx+1)+':SNAPSHOTS', 'type':'signal'})
    parts.append({'path':':START_STORE','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','PON',51,None),Method(None,'start_store',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':STOP_STORE','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','PPC',50,None),Method(None,'stop_store',head))",
        'options':('no_write_shot',)})
    parts.append({'path':':RDB_STORE','type':'action',
        'valueExpr':"Action(Dispatch('PXI_SERVER','PPC',50,None),Method(None,'rdb_store',head))",
        'options':('no_write_shot',)})
    
    workerDict = {}

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
            self.inSignals = []
            self.snapshots = []
            self.endTimes = []
            self.startTimes = []
            self.isEmpty = []
            self.lastTimeServed = []
            for sigIdx in range(self.numSignals):
              print('signal_'+str(sigIdx + 1)+'_in_signal')
              self.inSignals.append(getattr(device, 'signal_'+str(sigIdx + 1)+'_in_signal'))
              self.snapshots.append(getattr(device, 'signal_'+str(sigIdx + 1)+'_snapshots'))
              self.lastTimeServed.append(self.times[0] - 1)
              try:
                self.startTimes.append(getattr(device, 'signal_'+str(sigIdx + 1)+'_start').data() + self.times)
                self.endTimes.append(getattr(device, 'signal_'+str(sigIdx + 1)+'_end').data() + self.times)
              except:
                Data.execute('DevLogErr($1,$2)', device.getNid(), 'Error reading start or end for  signal '+str(sigIdx+1))
                raise DevCOMM_ERROR
              self.isEmpty.append(True)
            self.stopReq = False

        def computeMean(self, samples, times, startTime, endTime):
          return np.mean(samples[np.logical_and(times >= startTime, times <= endTime)])
          
        def computeMeanVECCHIA(self, samples, times, startTime, endTime):
          summed = None
          nMean = 0
          nSamples = len(samples)
          if nSamples > len(times):
            nSamples = len(times)
          for idx in range(nSamples):
            if times[idx] >= startTime and times[idx] <= endTime:
              if summed == None:
                summed = samples[idx:idx+1]
              else:
                summed += samples[idx:idx+1]
              nMean += 1
            if times[idx] > endTime:
              break
          if summed == None:
            return []
          return summed / nMean
          

        def check(self):
          allDone = True
          minStart = 1E6
          for sigIdx in range(len(self.inSignals)):
            if len(self.startTimes[sigIdx]) > 0 and self.startTimes[sigIdx][0] < minStart:
              minStart =  self.startTimes[sigIdx][0] 
          if minStart == 1E6: # no left times
            return True
          print('MIN START: '+str(minStart))
 #         self.device.getTree().setTimeContext(Float32(minStart), None, None)
          
          for sigIdx in range(len(self.inSignals)):
            inSignal = self.inSignals[sigIdx]
            snapshots = self.snapshots[sigIdx]
            startTimes = self.startTimes[sigIdx]
            endTimes = self.endTimes[sigIdx]
            if len(startTimes) == 0:  #this signal is done
              continue
            print('Signal '+str(sigIdx))
            print('START TIMES: ' + str(startTimes))
            print('END TIMES: ' + str(endTimes))
            #self.device.getTree().setTimeContext(Float32(startTimes[0]), None, None)
            try:
 #             inSignal.getTree().setTimeContext(Float32(startTimes[0]), None, None)
 #             inSignal.getTree().setTimeContext(None, None, None)
              currData = inSignal.data()
              currTimes = inSignal.getDimensionAt(0).data()
              signalEnd = currTimes[-1]
            except:
              print('READOUT OF ' + inSignal.getFullPath()+'   FAILED')
              continue	
            #if len(endTimes) > 0 and endTimes[0] > signalEnd:
            print('SIGNAL '+ inSignal.getPath() + ' TERMINATES AT '+ str(signalEnd))
            #currData = inSignal.data()
            #currTimes = inSignal.getDimensionAt(0)
            if currData.shape[0] > currTimes.shape[0]:
              currData = currData[:currTimes.shape[0]]
            if currData.shape[0] < currTimes.shape[0]:
              currTimes = currTimes[:currData.shape[0]]
	    
            while len(endTimes) > 0 and endTimes[0] <= signalEnd:
              print('CONTEXT START : ' + str(startTimes[0]))
              print('CONTEXT END : ' + str(endTimes[0]))
#              inSignal.getTree().setTimeContext(startTimes[0], endTimes[0], None)
#              meanData = currData[0:1]
#              for i in range(1,len(currData)):
#                meanData += currData[i:i+1]
#              meanData = meanData / len(currData)

	
              roi = currData[np.logical_and(currTimes >= startTimes[0], currTimes <= endTimes[0])]
              if roi.size >  0:
                meanData = np.mean(roi,0)
             # meanData = self.computeMean(currData, currTimes, startTimes[0], endTimes[0])

                if self.isEmpty[sigIdx]:
                  self.isEmpty[sigIdx] = False
                  inShape = currData.shape
                  shape = []
                  for s in inShape:
                    shape.append(s)
                  shape[0] = len(self.times)
                  print('SHAPE: ' + str(shape))
                  print('DATA SHAPE: ' + str(currData.shape))
                  print(self.times)
                  snapshots.beginSegment(self.times[0], self.times[-1], self.times, np.zeros(shape, currData.dtype))
                print('MEAN: '+ str(meanData) + str( currData.shape) + str(currTimes.shape))
                try:   
                  snapshots.putSegment(meanData)
                  print('PUT SEGMENT FATTA')
                except:
                  print('PUT SEGMENT FALITA')
              startTimes = startTimes[1:]
              endTimes = endTimes[1:]
              inSignal.getTree().setTimeContext(None, None, None)
            #endwhile
            self.startTimes[sigIdx] = startTimes
            self.endTimes[sigIdx] = endTimes
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
            allDone = self.check()
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
          worker.join()
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
        worker.join()
        return 1

    def store(self):
      nSignals = self.num_signals.data()
      #Clear output signals first
      for sigIdx in range(nSignals):
        getattr(self, 'signal_'+str(sigIdx + 1)+'_snapshots').deleteData()
      worker = self.AsynchStore()
      worker.configure(self.copy())
      worker.check()
      return 1
      



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
      except err:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot connect to database server '+host+'; '+str(err))
        raise DevCOMM_ERROR
      
      cursor = conn.cursor()
      
      for sigIdx in range(len(signals)):
        if names[sigIdx] == '':
          continue  
        createCommand = 'CREATE TABLE IF NOT EXISTS '+names[sigIdx].upper()+'_table (id SERIAL PRIMARY KEY, experiment VARCHAR, shot INTEGER, sig_idx INTEGER, snap_time REAL'
        createCommand += ','+names[sigIdx]+'_path VARCHAR,' + names[sigIdx] + ' REAL'
        if len(signals[sigIdx].shape) == 2:
          createCommand += '[]'
        createCommand += ')'
        print(createCommand)
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
          if len(exps) == 0:
            insertCommand = 'INSERT INTO ' + names[sigIdx]+'_table (experiment, shot, sig_idx, snap_time)  VALUES(\''+self.getTree().name+'\','
            insertCommand += str(self.getTree().shot)+', '+str(sigIdx)+', '+str(times[timeIdx])+')'
            print(insertCommand)
            try:
              cursor.execute(insertCommand)
            except:
              Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot insert in table('+insertCommand+'): ')
              print(sys.exc_info()[0])
              raise DevCOMM_ERROR
        
          insertCommand = 'UPDATE '+ names[sigIdx]+'_table SET '+names[sigIdx]+'_path = \''+ paths[sigIdx]+'\','+names[sigIdx]+' = '
          if len(signals[sigIdx].shape) == 1:
            if len(signals[sigIdx]) > timeIdx:
              insertCommand += str(signals[sigIdx][timeIdx])
            else:
              insertCommand += '0'
          elif len(signals[sigIdx].shape) == 2:
 #           insertCommand += ',ARRAY['+str(signals[sigIdx][timeIdx][0])
            print(signals[sigIdx].shape)
            insertCommand += 'ARRAY['+str(signals[sigIdx][timeIdx][0])
            for i in range(1, len(signals[sigIdx][timeIdx])):
              insertCommand += ','+str(signals[sigIdx][timeIdx][i])
            insertCommand += ']'
          insertCommand += ' WHERE experiment = \''+self.getTree().name+'\' AND shot = '+str(self.getTree().shot)+' AND sig_idx = '+str(sigIdx) +' AND snap_time = '+str(times[timeIdx])
          print(insertCommand)
          try:
            cursor.execute(insertCommand)
          except:
            Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot insert in table('+insertCommand+'): ')
            print(sys.exc_info()[0])
            raise DevCOMM_ERROR
          
        #endfor
      #endfor
      conn.commit()