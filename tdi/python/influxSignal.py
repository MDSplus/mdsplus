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

import re
import MDSplus

from os import environ

try:
    from influxdb import InfluxDBClient
except:
    print(
        "You must install the `influxdb` python package.")
    exit(1)

def influxSignal(fieldKey, aggregation, where, series=None, database=None, config=None, shotStartTime=None, shotEndTime=None): 
    tree = MDSplus.Tree()

    debug = False
    if "DEBUG_DEVICES" in environ:
        debug = True

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
    
    if not isinstance(config, str):
        config = config.data()

    """Instantiate a connection to the InfluxDB."""
    username = ''
    password = ''
    try:
        with open(config) as file:
            lines = file.readlines()

            if len(lines) < 2:
                print("Failed to read influx config from file %s" %(config,))
                
            host = lines[0].strip('\n')
            username = lines[1].strip('\n')
            password = lines[2].strip('\n')

    except IOError as e:
        print("Failed to open credentials file %s" %(config,))

    port = 8086
    if ':' in host:
        parts = host.split(':')
        host = parts[0]
        port = int(parts[1])

    if not isinstance(database, str):
        database = database.data()

    if not isinstance(series, str):
        series = series.data()

    if not isinstance(fieldKey, str):
        fieldKey = fieldKey.data()

    if not isinstance(aggregation, str):
        aggregation = aggregation.data()

    if isinstance(shotStartTime, str):
        shotStartTime = int(shotStartTime)
    elif not isinstance(shotStartTime, int):
        shotStartTime = shotStartTime.data()

    if isinstance(shotEndTime, str):
        shotEndTime = int(shotEndTime)
    elif not isinstance(shotEndTime, int):
        shotEndTime = shotEndTime.getDataNoRaise()

    if not isinstance(where, str):
        where = where.data()

    if where == '':
        return

    whereList = [where]

    timeContext = MDSplus.Tree.getTimeContext()

    startTime = shotStartTime
    endTime = shotEndTime
    deltaTime = None

    #print('Getting time context from the tree: %s '%(timeContext,))

    # The time context is in seconds relative to the start of the shot.
    if timeContext[0] is not None:
        startTime = shotStartTime + (float(timeContext[0]) * 1000)

    if timeContext[1] is not None:
        endTime   = shotStartTime + (float(timeContext[1]) * 1000)
    
    if timeContext[2] is not None:
        deltaTime = float(timeContext[2])

    # Clamp the computed start/end time within the time bounds of the shot
    if startTime < shotStartTime:
        startTime = shotStartTime

    if endTime is not None and shotEndTime is not None and endTime > shotEndTime:
        endTime = shotEndTime

    startTimeQuery = ''
    endTimeQuery = ''

    # Convert to nanosecond UNIX timestamp
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

    groupBy = ''
    if deltaTime is not None:
        # Convert deltaTime from seconds to milliseconds, if deltaTime is less than 1ms this will break
        deltaTimeMS = int(deltaTime * 1000)
        groupBy = 'GROUP BY time(%sms)' % (deltaTimeMS,) # The trailing 'ms' is for milliseconds
        # If the fieldKey does not have an aggregation function, Influx will error out
        # Attempt to wrap the field key with an aggregation function, but don't affect
        # any equations stored in it
        # For example:
        #   10*fVal will become 10*MEAN(fVal)
        fieldKey = re.sub(r'([a-zA-Z]+)', '%s(\\1)' % (aggregation,), fieldKey)
        if debug:
            print(aggregation, fieldKey)

    """Instantiate a connection to the InfluxDB."""
    client = InfluxDBClient(host, port, username, password, database)

    query = 'SELECT %s AS value FROM "%s" %s %s;' % (fieldKey, series, where, groupBy)
    if debug:
        print(query)

    result = client.query(query, params={'epoch': 'ms'})

    data = list(result.get_points())

    valueData = [None] * len(data)
    timeData  = [None] * len(data)

    count = 0
    for row in data:
        if debug:
            print(row)

        # If there are no data points within the time specified by deltaTime, influx
        # returns a None for the value
        if row['value'] is None:
            continue

        valueData[count] = float(row['value'])
        timeData[count] = (row['time'] - shotStartTime) / 1000
        count += 1

    # If any data points were skipped, shrink the arrays to the actual number of points
    values = MDSplus.Float32Array(valueData[:count])
    times  = MDSplus.Float32Array(timeData[:count])

    return MDSplus.Signal(values, None, times)

