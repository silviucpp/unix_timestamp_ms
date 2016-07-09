
compile:
	gcc -O3 -Wall -Wextra -fPIC -o unix_timestamp_ms.so -shared src/unix_timestamp_ms.cc src/fast_atoi.cc src/timestamp.cc -I /usr/include/mysql 

clean:
	rm unix_timestamp_ms.so

