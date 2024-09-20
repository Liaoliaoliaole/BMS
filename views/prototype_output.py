import mysql.connector
import random
from datetime import datetime
import time

with open('mysql_password.txt', 'r', encoding='utf-8-sig') as fp:
    mysql_password = fp.read().rstrip()

mydb = mysql.connector.connect(
  host="localhost",
  user="Python_Importer",
  password=mysql_password,
  database="bms_control"
)

mycursor = mydb.cursor()

sql = ("INSERT INTO measurments (temperature1, temperature2, voltage1, voltage2, voltage3, voltage4, voltage_FULL, "
       "current_FULL, time) VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s)")

# current_time = 0

while 1:
    temperature1 = random.randint(200, 220) / 10
    temperature2 = random.randint(200, 220) / 10
    voltage1 = random.randint(300, 320)/100
    voltage2 = random.randint(300, 320) / 100
    voltage3 = random.randint(300, 320) / 100
    voltage4 = random.randint(300, 320) / 100
    voltage_FULL = voltage1 + voltage2 + voltage3 + voltage4
    current_FULL = random.randint(100, 200) / 100
    ts = time.time()
    timestamp = datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')
    # print(timestamp)

    val = (temperature1, temperature2, voltage1, voltage2, voltage3, voltage4, voltage_FULL, current_FULL, timestamp)
    mycursor.execute(sql, val)

    mydb.commit()

    print(timestamp, temperature1, temperature2)

    time.sleep(5)

    # current_time = current_time + 1


