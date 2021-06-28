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

def influxSignal(dbname, measurement, field_value, where, address, credentials):
    """Instantiate a connection to the InfluxDB."""
    host     = address.data()
    port     = 8086

    username = ''
    password = ''
    try:
        with open(credentials.data()) as cred_file:
            lines = cred_file.readlines()

            if len(lines) < 2:
                print("Failed to read credentials from file %s" %(credentials.data(),))

            username = lines[0].strip('\n')
            password = lines[1].strip('\n')

    except IOError as e:
        print("Failed to open credentials file %s" %(credentials.data(),))

    dbname      = dbname.data()
    measurement = measurement.data()
    field_value = field_value.data()

    if where == '':
        return

    whereList = [where.data()]

    # We get the start_time and the end_time of the query from the defined MDSplus tree time context:
    start_end_times = MDSplus.Tree.getTimeContext()

    print('Getting time context from the tree: %s '%(start_end_times,))
    start = int(start_end_times[0])
    end   = int(start_end_times[1])

    startTimeQuery = ''
    endTimeQuery = ''

    # Convert to nanosecond UNIX timestamp
    startTimeQuery = 'time > %d' % (start * 1000000,)

    # Convert to nanosecond UNIX timestamp
    endTimeQuery = 'time < %d' % (end * 1000000,)

    if startTimeQuery != '':
        whereList.append(startTimeQuery)

    if endTimeQuery != '':
        whereList.append(endTimeQuery)

    where = ''
    if len(whereList) > 0:
        where = 'WHERE %s' % (' AND '.join(whereList),)

    client = InfluxDBClient(host, port, username, password, dbname)

    # example influxDB query:
    # dbname      = 'NOAA_water_database' 
    # measurement = h2o_feet == Table
    # field_value = water_level
    # 'SELECT "water_level" FROM "h2o_feet" WHERE time >= 1568745000000000000 AND time <= 1568750760000000000;'
    query = 'SELECT "%s" AS value FROM "%s" %s;' % (field_value, measurement, where)
    print('Query: %s' % query)

    result = client.query(query, params={'epoch': 'ms'})

    data = list(result.get_points())

    valueData = [None] * len(data)
    timeData  = [None] * len(data)

    i = 0
    for row in data:
        valueData[i] = float(row['value'])
        timeData[i] = row['time']
        i += 1

    values = MDSplus.Float32Array(valueData)
    times  = MDSplus.Uint64Array(timeData)

    return MDSplus.Signal(values, None, times)

