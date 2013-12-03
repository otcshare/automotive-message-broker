#!/bin/bash

TABLE="CREATE TABLE IF NOT EXISTS data (key TEXT, value BLOB, source TEXT, time REAL, sequence REAL);"

rm generated.db
rm generated.db.sql
echo $TABLE > generated.db.sql
echo "BEGIN IMMEDIATE TRANSACTION;" >> generated.db.sql

for (( i=0; i<120; i++ ))
do
T=$(echo 1385571956 + $i)
	for (( n=1; n<1000; n++ ))
	do
		MS=$(echo $n \* 0.001 | bc)
		TIME=$(echo $T + $MS | bc)
		echo "insert into data values('VehicleSpeed',$(( ( RANDOM % 300 )  + 1 )),'test', $TIME, -1);" >> generated.db.sql
	done
done

echo "END TRANSACTION;" >> generated.db.sql

sqlite3 generated.db < generated.db.sql
