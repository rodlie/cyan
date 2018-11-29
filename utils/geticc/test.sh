#!/bin/sh
if [ -f "test.jpg.icc" ]; then
  rm -f test.jpg.icc
fi
./geticc test.jpg || exit 1
CHECK=`diff test.icc test.jpg.icc`
if [ "$CHECK" != "" ]; then
  echo "TEST FAILED!"
  exit 1
fi
echo "TEST OK!"
rm -f test.jpg.icc
exit 0
