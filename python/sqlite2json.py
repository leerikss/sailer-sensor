import sqlite3
import json

sql = "SELECT sat_time,latitude,longitude,altitude,satellites,epx,epy FROM gps;"
args=(-1,)
database = "../c++/data/sailerlog.sqlite"

def main():
    try:
        conn = sqlite3.connect(database)
        conn.text_factory = str
        cursor = conn.cursor()
        rows = []
        for row in cursor.execute(sql):
            rows.append( { 'time': row[0], 'latitude': row[1], 'longitude': row[2], \
                               'altitude': row[3], 'satellites': row[4], 'epx': row[5], 'epy': row[6] });
        print(json.dumps(rows) )

    except Exception as ex:
        print("Error: Storage: Could not open database %s: %s" % (database, ex))
        raise ex

if __name__ == "__main__":
    main()
