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
from MDSplus import mdsExceptions, Device, Data, Int64, Int64Array, Uint64, Event
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
            for sigIdx in range(self.numSignals):
              print('signal_'+str(sigIdx + 1)+'_in_signal')
              self.inSignals.append(getattr(device, 'signal_'+str(sigIdx + 1)+'_in_signal'))
              self.snapshots.append(getattr(device, 'signal_'+str(sigIdx + 1)+'_snapshots'))
              try:
                self.startTimes.append(getattr(device, 'signal_'+str(sigIdx + 1)+'_start').data() + self.times)
                self.endTimes.append(getattr(device, 'signal_'+str(sigIdx + 1)+'_end').data() + self.times)
              except:
                Data.execute('DevLogErr($1,$2)', device.getNid(), 'Error reading start or end for  signal '+str(sigIdx+1))
                raise DevCOMM_ERROR
              self.isEmpty.append(True)
            self.stopReq = False

        def computeMean(self, samples, times, startTime, endTime):
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
          for sigIdx in range(len(self.inSignals)):
            inSignal = self.inSignals[sigIdx]
            snapshots = self.snapshots[sigIdx]
            startTimes = self.startTimes[sigIdx]
            endTimes = self.endTimes[sigIdx]
            print('Signal '+str(sigIdx))
            print('START TIMES: ' + str(startTimes))
            print('END TIMES: ' + str(endTimes))
            try:
 #             inSignal.getTree().setTimeContext(Float32(startTimes[0]), None, None)
              inSignal.getTree().setTimeContext(None, None, None)
              signalEnd = inSignal.getDimensionAt(0).data()[-1]
            except:
              print('READOUT OF ' + inSignal.getFullPath()+'   FAILED')
              continue	
            if endTimes[0] > signalEnd:
              print('SIGNAL TERMINATES AT '+ str(signalEnd))
            currData = inSignal.data()
            currTimes = inSignal.getDimensionAt(0)
            while len(endTimes) > 0 and endTimes[0] <= signalEnd:
              print('CONTEXT START : ' + str(startTimes[0]))
              print('CONTEXT END : ' + str(endTimes[0]))
#              inSignal.getTree().setTimeContext(startTimes[0], endTimes[0], None)
#              meanData = currData[0:1]
#              for i in range(1,len(currData)):
#                meanData += currData[i:i+1]
#              meanData = meanData / len(currData)
              meanData = self.computeMean(currData, currTimes, startTimes[0], endTimes[0])

              if self.isEmpty[sigIdx]:
                self.isEmpty[sigIdx] = False
                inShape = currData.shape
                shape = []
                for s in inShape:
                  shape.append(s)
                shape[0] = len(startTimes)
                print('SHAPE: ' + str(shape))
                print(self.times)
                snapshots.beginSegment(self.times[0], self.times[-1], self.times, np.zeros(shape, currData.dtype))
              print('MEAN: '+ str(meanData))
              if len(meanData) > 0:
                snapshots.putSegment(meanData)
              startTimes = startTimes[1:]
              endTimes = endTimes[1:]
              inSignal.getTree().setTimeContext(None, None, None)
            #endwhile
            self.startTimes[sigIdx] = startTimes
            self.endTimes[sigIdx] = endTimes
            if len(endTimes) > 0:
              allDone = False
          #endfor
          return allDone

        def run(self):
          while not self.stopReq:
            sleep(self.updateSecs)
            allDone = self.check()
            print('ALL DONE: ' + str(allDone))
            if allDone:
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
      worker.configure(self)
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
      worker.configure(self)
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
          names.append(getattr(self, 'signal_'+str(sigIdx + 1)+'_col_name').data())
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
      createCommand = 'CREATE TABLE IF NOT EXISTS '+table+' (id SERIAL PRIMARY KEY, experiment VARCHAR, shot INTEGER, sig_idx INTEGER, snap_time REAL'
      for sigIdx in range(numSignals):
        createCommand += ','+names[sigIdx]+'_path VARCHAR'
        if(len(signals[sigIdx]) == 0  or len(signals[sigIdx].shape) >= 3): #Do not write frames or empty signals
            continue
        createCommand += ',' + names[sigIdx] + ' REAL'
        if len(signals[sigIdx].shape) == 2:
          createCommand += '[]'
      createCommand += ')'
      print(createCommand)
      print('\n\n')
      try:
        cursor.execute(createCommand)
      except:
        Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot create table: ')
        raise DevCOMM_ERROR

      print(createCommand)
      for timeIdx in range(len(times)):
        insertCommand = 'INSERT INTO ' + table + ' VALUES(DEFAULT,\''+self.getTree().name+'\','+str(self.getTree().shot)
        insertCommand += ','+ str(timeIdx)
        insertCommand += ','+str(times[timeIdx])
        for sigIdx in range(len(signals)):
          if len(signals[sigIdx]) == 0:
            continue  #do not update empty signals 
          insertCommand += ',\''+ paths[sigIdx]+'\''
          if len(signals[sigIdx].shape) == 1:
            if len(signals[sigIdx]) > timeIdx:
              insertCommand += ', '+str(signals[sigIdx][timeIdx])
            else:
              insertCommand += ', '
          elif len(signals[sigIdx].shape) == 2:
 #           insertCommand += ',ARRAY['+str(signals[sigIdx][timeIdx][0])
            print(signals[sigIdx].shape)
            insertCommand += ',ARRAY['+str(signals[sigIdx][timeIdx][0])
            for i in range(1, len(signals[sigIdx][timeIdx])):
              insertCommand += ','+str(signals[sigIdx][timeIdx][i])
            insertCommand += ']'
          #Nothing written if frame
        #endfor
        insertCommand += ')'
        print('\n\n\n')
        print(insertCommand)
        print('\n\n\n')
        try:
          cursor.execute(insertCommand)
        except:
          Data.execute('DevLogErr($1,$2)', self.getNid(), 'Cannot insert in table('+insertCommand+'): ')
          print(sys.exc_info()[0])
          raise DevCOMM_ERROR
      #endfor
      conn.commit()