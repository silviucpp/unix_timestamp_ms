
compile:
	gcc -O3 -Wall -Wextra -fPIC -o unix_timestamp_ms.so -shared unix_timestamp_ms.cc -I /usr/include/mysql/

