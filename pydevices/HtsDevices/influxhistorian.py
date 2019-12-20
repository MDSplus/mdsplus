import MDSplus
import time
from datetime import datetime
from influxdb import InfluxDBClient

class INFLUXHISTORIAN(MDSplus.Device):

    DATA_COUNT = 10

    parts = [
        { 'path': ':ADDRESS',    'type': 'text',                                         'options':('no_write_shot') },
        { 'path': ':DATABASE',   'type': 'text',                                         'options':('no_write_shot') },
        { 'path': ':SERIES',     'type': 'text',     'value': 'scada-ignition.default',  'options':('no_write_shot') },
        { 'path': ':DATA_EVENT', 'type': 'text',     'value': 'INFLUXDB_TREND',          'options':('no_write_shot') },
        { 'path': ':last_read',  'type': 'numeric',                                      'options':('write_shot') },
        { 'path': ':DATA',       'type': 'text'},
    ]

    for i in range(DATA_COUNT):
        name = ":DATA:D{:02d}".format(i + 1)
        parts.append({ 'path': name,                'type': 'signal',                       'options':('write_shot') })
        parts.append({ 'path': name + ":TAG_PATH",  'type': 'text',                         'options':('no_write_shot') })
        parts.append({ 'path': name + ":FUNCTION",  'type': 'text',     'value': 'fVal',    'options':('no_write_shot') })

    def debugging(self):
        import os
        if self.debug == None:
            self.debug=os.getenv("DEBUG_DEVICES")
        return(self.debug)

    def trend(self):
        if not self.on:
            return

        self.store(self.last_read.data())
        self.last_read.record = int(round(time.time() * 1000))
    TREND=trend

    def store(self, start = 0, end = 0):
        if not self.on:
            return

        address = self.address.data()
        parts = address.split(":", 2)

        address = parts[0]
        port = 8086
        if len(parts) > 1:
            port = int(parts[1])

        client = InfluxDBClient(address, port, 'root', 'root', self.database.data())

        for i in range(self.DATA_COUNT):
            try:
                node    = self.__getattr__("data_d{:02d}".format(i + 1))
                tagPath = self.__getattr__("data_d{:02d}_tag_path".format(i + 1))
                if not node.on:
                    continue

                whereList = []
                if tagPath != '':
                    whereList.append('tagPath = \'{}\''.format(tagPath.data()))

                if start > 0:
                    start /= 1000.0
                    whereList.append('time > \'{}000Z\''.format(datetime.fromtimestamp(start).strftime("%Y-%m-%dT%H:%M:%S.%f")))
                if end > 0:
                    end /= 1000.0
                    whereList.append('time < \'{}000Z\''.format(datetime.fromtimestamp(end).strftime("%Y-%m-%dT%H:%M:%S.%f")))

                where = ''
                if len(whereList) > 0:
                    where = 'WHERE {}'.format(' AND '.join(whereList))
                
                query = 'SELECT {} AS value FROM "{}" {}'.format(
                    node.FUNCTION.data(), 
                    self.series.data(), 
                    where
                )

                if self.debugging():
                    print(query)

                result = client.query(query)
                
                for row in result.get_points():
                    # TODO: Cleanup
                    # Strip off ###Z from nanosecond times
                    timestamp = row['time'][0:row['time'].find('.') + 4]
                    if timestamp[-1] == 'Z':
                        timestamp = timestamp[0:-1]
                    # Parse into datetime
                    timestamp = datetime.strptime(timestamp, '%Y-%m-%dT%H:%M:%S.%f')
                    # Convert to millisecond UNIX timestamp
                    timestamp = time.mktime(timestamp.timetuple()) * 1000
                    
                    node.putRow(1000, MDSplus.Float32(float(row['value'])), MDSplus.Int64(timestamp))
                
            except MDSplus.TreeNODATA:
                pass
            except Exception as e:
                print(e)

        MDSplus.Event.setevent(self.data_event.data())
    STORE=store