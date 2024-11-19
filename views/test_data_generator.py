import mysql.connector
import random
from datetime import datetime
import time

with open('mysql_password.txt', 'r', encoding='utf-8-sig') as fp:
    mysql_password = fp.read().rstrip()

mydb = mysql.connector.connect(
    host="16.16.145.115",
    user="Python_Importer",
    password=mysql_password,
    database="BMS"
)

mycursor = mydb.cursor()

sql = ("INSERT INTO measurments (temperature1, temperature2, voltage1, voltage2, voltage3, voltage4, "
       "current_charge, current_discharge, time) VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s)")


def write_to_mysql(input_values):
    ts = time.time()
    timestamp = datetime.fromtimestamp(ts).strftime('%Y-%m-%d %H:%M:%S')

    val = (input_values[0], input_values[1], input_values[2], input_values[3],
           input_values[4], input_values[5], input_values[6], input_values[7], timestamp)
    mycursor.execute(sql, val)

    mydb.commit()

    print(timestamp, end=" | ")
    for i in input_values:
        print(i, end=" | ")
    print()


values = [0] * 8

while 1:
    values[0] = random.randint(180, 200) / 10
    values[1] = random.randint(230, 250) / 10
    values[2] = random.randint(265, 280) / 100
    values[3] = random.randint(280, 300) / 100
    values[4] = random.randint(300, 335) / 100
    values[5] = random.randint(335, 350) / 100
    values[6] = random.randint(10, 90) / 100
    values[7] = random.randint(200, 300) / 100

    write_to_mysql(values)

    time.sleep(3)

