#!/bin/sh

make

for f in `ls [0-9]*.sql`
do
  echo $f
  sudo -u postgres psql mssqlserver -f $f || exit
done
