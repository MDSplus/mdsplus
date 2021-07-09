#
# Copyright (c) 2021, Massachusetts Institute of Technology All rights reserved.
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

import MDSplus

try:
    from influxdb import InfluxDBClient
except:
    print(
        "You must install the `influxdb` python package.")
    exit(1)

def influxSignal(fieldKey, where, series=None, database=None, config=None, shotStartTime=None, shotEndTime=None): 
    tree = MDSplus.Tree()

    if series is None:
        series = tree.getNode('\INFLUX_SERIES')
    
    if database is None:
        database = tree.getNode('\INFLUX_DATABASE')
    
    if config is None:
        config = tree.getNode('\INFLUX_CONFIG')
    
    if shotStartTime is None:
        shotStartTime = tree.getNode('\INFLUX_START_TIME')
    
    if shotEndTime is None:
        shotEndTime = tree.getNode('\INFLUX_END_TIME')

    """Instantiate a connection to the InfluxDB."""
    username = ''
    password = ''
    try:
        with open(config.data()) as file:
            lines = file.readlines()

            if len(lines) < 2:
                print("Failed to read influx config from file %s" %(config.data(),))
                
            host = lines[0].strip('\n')
            username = lines[1].strip('\n')
            password = lines[2].strip('\n')

    except IOError as e:
        print("Failed to open credentials file %s" %(config.data(),))

    port = 8086
    if ':' in host:
        parts = host.split(':')
        host = parts[0]
        port = int(parts[1])

    database = database.data()
    series = series.data()
    fieldKey = fieldKey.data()
    shotStartTime = shotStartTime.data()
    shotEndTime = shotEndTime.data()

    if where == '':
        return

    whereList = [where.data()]

    timeContext = MDSplus.Tree.getTimeContext()

    startTime = shotStartTime
    endTime = shotEndTime

    #print('Getting time context from the tree: %s '%(timeContext,))

    # The time context is in seconds relative to the start of the shot.
    if timeContext[0] is not None:
        startTime = shotStartTime + (int(timeContext[0]) * 1000)
    if timeContext[1] is not None:
        endTime   = shotStartTime + (int(timeContext[1]) * 1000)
    
    # TODO: timeContext[2] is the interval which influx supports, therefore we should support it too.

    # Clamp the computed start/end time within the time bounds of the shot
    if startTime < shotStartTime:
        startTime = shotStartTime
    if endTime > shotEndTime:
        endTime = shotEndTime

    startTimeQuery = ''
    endTimeQuery = ''

    # Convert to nanosecond UNIX timestamp
    if startTime is not None:
        startTimeQuery = 'time > %d' % (startTime * 1000000,)

    # Convert to nanosecond UNIX timestamp
    if endTime is not None:
        endTimeQuery = 'time < %d' % (endTime * 1000000,)

    if startTimeQuery != '':
        whereList.append(startTimeQuery)

    if endTimeQuery != '':
        whereList.append(endTimeQuery)

    where = ''
    if len(whereList) > 0:
        where = 'WHERE %s' % (' AND '.join(whereList),)

    client = InfluxDBClient(host, port, username, password, database)

    query = 'SELECT "%s" AS value FROM "%s" %s;' % (fieldKey, series, where)
    #print('Query: %s' % query)

    result = client.query(query, params={'epoch': 'ms'})

    data = list(result.get_points())

    valueData = [None] * len(data)
    timeData  = [None] * len(data)

    i = 0
    for row in data:
        valueData[i] = float(row['value'])
        timeData[i] = (row['time'] - shotStartTime) / 1000
        i += 1

    values = MDSplus.Float32Array(valueData)
    times  = MDSplus.Float32Array(timeData)

    return MDSplus.Signal(values, None, times)

