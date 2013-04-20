import sqlite3
import json

sql = "SELECT latitude,longitude,altitude FROM gps;"
args=(-1,)
database = "../c++/data/sailorlog.sqlite"

def main():
    try:
        conn = sqlite3.connect(database)
        conn.text_factory = str
        cursor = conn.cursor()
        rows = []
        for row in cursor.execute(sql):
            rows.append( {'latitude': row[0], 'longitude': row[1], 'altitude': row[2]} )
        print(json.dumps(rows) )

    except Exception as ex:
        print("Error: Storage: Could not open database %s: %s" % (database, ex))
        raise ex

if __name__ == "__main__":
    main()
