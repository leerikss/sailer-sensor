from bottle import route, run, response, request, get, static_file
import sqlite3
import json

SQL_DATES="select distinct strftime('%d.%m.%Y',time) from gps;"

SQL_DATA_BY_DAYS="select time,latitude,longitude,altitude,satellites,epx,epy \
from gps where time between date('%s') and date('%s','+1 day') and id %% %d = 0;"
INTERVAL=10

DATABASE = "/home/pi/sailer-sensor/rpi/data/sailerlog.sqlite"

# @route('/gpsplot')
@route('/:filename#.*#')
def index(filename):
    return static_file(filename, root='.')

@route('/getDates')
def getDates():
    cursor = conn_db()
    rows = []
    for row in cursor.execute(SQL_DATES):
        rows.append( { 'date': row[0] } )
    response.content_type = 'application/json'
    return json.dumps(rows)

@route('/getDataByDate')
def getDataByDate():
    cursor = conn_db()
    rows = []
    date = request.query.get('date')
    sql = SQL_DATA_BY_DAYS % (date, date, INTERVAL)
    for row in cursor.execute(sql):
        rows.append( { 'time': row[0], 'latitude': row[1], 'longitude': row[2], \
                           'altitude': row[3], 'satellites': row[4], \
                           'epx': row[5], 'epy': row[6] });
    response.content_type = 'application/json'
    return json.dumps(rows)

def conn_db():
    try:
        conn = sqlite3.connect(DATABASE)
        conn.text_factory = str
        cursor = conn.cursor()
        return cursor

    except Exception as ex:
        print("Error: Storage: Could not open database %s: %s" % (DATABASE, ex))
        raise ex


run(host='pi.cable', port=80)
