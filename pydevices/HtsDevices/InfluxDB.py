
import MDSplus
import time
from datetime import datetime
from influxdb import InfluxDBClient

class INFLUXDB(MDSplus.Device):

    parts = [
        { 'path': 'ADDRESS',    'type': 'text',                                         'options':('no_write_shot') },
        { 'path': 'DATABASE',   'type': 'text',                                         'options':('no_write_shot') },
        { 'path': 'SERIES',     'type': 'text',     'value': 'scada-ignition.default',  'options':('no_write_shot') },
        { 'path': 'DATA_EVENT', 'type': 'text',     'value': 'INFLUXDB_TREND',          'options':('no_write_shot')})
        { 'path': 'LAST_READ',  'type': 'numeric',                                      'options':('write_shot') },
        { 'path': 'DATA',       'type': 'text'},
    ]

    for i in range(10):
        name = "DATA:D{:02d}".format(i + 1)
        parts.append({ 'path': name,                'type': 'signal',                       'options':('write_shot') })
        parts.append({ 'path': name + ":TAG_PATH",  'type': 'text',                         'options':('no_write_shot') })
        parts.append({ 'path': name + ":FUNCTION",  'type': 'text',     'value': 'fVal',    'options':('no_write_shot') })

    def debugging(self):
        import os
        if self.debug == None:
            self.debug=os.getenv("DEBUG_DEVICES")
        return(self.debug)

    def trend(self):
        self.store(self.LAST_READ.data())
        self.LAST_READ.record = int(round(time.time() * 1000))
    TREND=trend

    def store(self, start = 0, end = 0):
        address = self.ADDRESS.data()
        parts = address.split(":", 2)

        address = parts[0]
        port = 8086
        if len(parts) > 1:
            port = int(parts[1])

        client = InfluxDBClient(address, port, 'root', 'root', self.DATABASE.data())

        for i in range(10):
            try:
                node = self.getNode("DATA:D{:02d}".format(i + 1))

                whereList = []
                tagPath = node.TAG_PATH.data()
                if tagPath != '':
                    whereList.append('tagPath == \'{}\''.format(tagPath))

                if start > 0:
                    start /= 1000.0
                    whereList.append('time > "{}"'.format(datetime.fromtimestamp(start).strftime("%Y-%m-%dT%H:%M:%S.%f")))
                if end > 0:
                    end /= 1000.0
                    whereList.append('time < "{}"'.format(datetime.fromtimestamp(end).strftime("%Y-%m-%dT%H:%M:%S.%f")))

                where = ''
                if len(whereList) > 0:
                    where = 'WHERE {}'.format(' AND '.join(whereList))
                
                query = 'SELECT {} AS value FROM \'{}\' {}'.format(
                    node.FUNCTION.data(), 
                    self.SERIES.data(), 
                    where
                )

                if self.debugging():
                    print(query)

                result = client.query(query)
                
                for row in result.get_points():
                    # Strip off ###Z from nanosecond times
                    timestamp = row['time'][0:-4]
                    # Parse into datetime
                    timestamp = datetime.strptime(timestamp, '%Y-%m-%dT%H:%M:%S.%f')
                    # Convert to millisecond UNIX timestamp
                    timestamp = time.mktime(timestamp.timetuple()) * 1000
                    
                    node.putRow(1, MDSplus.Float32(row['value']), MDSplus.Int64(timestamp))
                
            except MDSplus.TreeNODATA:
                pass
            except Exception as e:
                print(e)

        MDSplus.Event.setevent(self.DATA_EVENT.data())
    STORE=store
