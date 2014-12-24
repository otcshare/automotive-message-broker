#!/bin/bash

TABLE="@database_table_create@;"

if [ -f generated.db ];
  then
   rm generated.db
fi

echo $TABLE > generated.db.sql
echo "BEGIN IMMEDIATE TRANSACTION;" >> generated.db.sql

for (( i=0; i<130; i++ ))
do
T=$(echo 1385571956 + $i)
	for (( n=1; n<1000; n++ ))
	do
		MS=$(echo $n \* 0.001 | bc)
		TIME=$(echo $T + $MS | bc)
		echo "insert into data values('VehicleSpeed', $(( ( RANDOM % 300 )  + 1 )),'test', 0, $TIME, -1, 'alskejroanpnfansadfasdfa');" >> generated.db.sql
	done
done

echo "END TRANSACTION;" >> generated.db.sql

sqlite3 generated.db < generated.db.sql

exit
