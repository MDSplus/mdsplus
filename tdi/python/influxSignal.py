import MDSplus

try:
    from influxdb import InfluxDBClient
except:
    print(
        "You must install the `influxdb` python package.")
    exit(1)

def influxSignal(dbname, measurement, field_value):
    """Instantiate a connection to the InfluxDB."""
    host     = 'localhost'
    port     = 8086
    user     = 'admin'
    password = 'password'

    dbname      = dbname.data()
    measurement = measurement.data()
    field_value = field_value.data()

    MDSplus.Data.execute('TreeOpen("influx", 0)')
    start_end_times = MDSplus.Tree.getTimeContext()
    print('Getting time context from the tree: {} '.format(start_end_times))
    start_time = str(int(start_end_times[0]))
    end_time   = str(int(start_end_times[1]))
    
    client = InfluxDBClient(host, port, user, password, dbname)
    # example influxDB query:
    # dbname      = 'NOAA_water_database' 
    # measurement = h2o_feet
    # field_value = water_level
    # 'SELECT "water_level" FROM "h2o_feet" WHERE time >= 1568745000000000000 AND time <= 1568750760000000000;'
    query = 'SELECT "%s" FROM "%s" WHERE time >= %s AND time <= %s;' % (field_value, measurement, start_time, end_time)
    print('Query: %s' % query)

    result = client.query(query, params={'epoch': 'ms'})

    data = list(result.get_points())

    valueData = [None] * len(data)
    timeData  = [None] * len(data)

    i = 0
    for row in data:
        valueData[i] = float(row['water_level'])
        timeData[i] = row['time']
        i += 1

    values = MDSplus.Float32Array(valueData)
    times  = MDSplus.Uint64Array(timeData)

    return MDSplus.Signal(values, None, times)