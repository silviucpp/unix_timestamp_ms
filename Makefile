
compile:
	gcc -O3 -Wall -Wextra -fPIC -o unix_timestamp_ms.so -shared src/unix_timestamp_ms.cc -I /usr/include/mysql/

