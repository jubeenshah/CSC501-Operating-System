if [ -f vn ]; then
	expr `cat vn` + 1 > vn
else
	echo 0 > vn
fi
if [ -z "$USER" ]; then
       USER=`whoami`
fi
HOST=`hostname`
HOST=`basename $HOST .ncsu.edu`
VN=`cat vn`
echo "(CSC501 1-2009 base) #$VN ("$USER"@"$HOST") "`date` > version
