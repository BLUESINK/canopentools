
install :
	sed 's?path?'$(shell pwd)'/canopentools.py?' install/canopentools > /usr/local/bin/canopentools
	chmod a+x /usr/local/bin/canopentools

.PHONY : install
