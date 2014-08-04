#!/bin/bash

grep ofs_open /home/alja/xrd/hdfs/hdfs.log > /tmp/ofs_errors.log

if [ -s /tmp/ofs_errors.log ]; then
   cat /tmp/ofs_errors.log  | mail -s "xrootd-proxy errors" 'amraktadel@ucsd.edu mtadel@ucsd.edu '
fi


