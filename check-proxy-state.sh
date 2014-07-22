#!/bin/bash
xpid=`ps --no-headers -opid -C "xrootd -I v4"`
ps -f -p $xpid 

echo -e "\n"


#echo "==================================================================="
	
top -b -n 1 -p $xpid 


# echo -e '++++++++++++++\n\n\n'

echo "==================================================================="
lname=$HOME/xrd/hdfs/hdfs.log

nl=`grep login $lname  |wc -l`
echo "Logins ($nl)x"

echo -e "\n"

na=`grep Attach $lname  |wc -l`
echo "Attach  ($na)x"
 grep Atta $lname | tail -1 | perl -ne 'if(/\[(\d+)-(\d+)-(\d+) (.*) (\-\d+)\]\[Info(.*)Attach\(\)(.*)root\?/){print "[$4] $7root\n";}'

echo -e "\n"

nd=`grep Detach $lname  |wc -l`
echo "Detach ($nd)x"
 grep Deta $lname | tail -1 | perl -ne 'if(/\[(\d+)-(\d+)-(\d+) (.*) (\-\d+)\]\[Info(.*)Detach\(\)(.*)root\?/){print "[$4]  $7root\n";}'


echo -e "\nTRAFFIC"
echo "http://xrootd.t2.ucsd.edu/display?NS=xrootd&_ga=GA1.2.1489768832.1339116143&imgsize=1024x600&interval.max=0&interval.min=43200000&jlinkphyam0=phyam0&log=0&modules=ucsd_servers%2Ftraffic_eth0_in_chart&modules=ucsd_servers%2Ftraffic_eth0_out_chart&page=ucsd_servers%2Ftraffic&plot_series=xrootd-proxy.t2.ucsd.edu&sum=0&tooltips.enabled=0"
