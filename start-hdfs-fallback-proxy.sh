#! /bin/bash
set -x
export base=/home/alja/xrd/hdfs
export LD_LIBRARY_PATH=$base/ixrd/lib64/
export PATH=$base/ixrd/bin/:$PATH

# log only caching proxy messages, topic == App
export XRD_LOGLEVEL=Info
export XRD_LOGMASK=AppMsg

#xrootd -I v4 -c $base/fileBlock.cfg -p 2020 -l $base/hdfs.log -b
#xrootd -I v4 -c $base/entireFile.cfg  -p 2020   -l $base/hdfs.log -b
xrootd -I v4 -c $base/onlyproxy.cfg  -p 2020 -l $base/hdfs.log -b

