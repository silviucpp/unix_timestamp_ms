# MySQL unix_timestamp_ms

MySQL UDF function implemented in C++ for getting the unix timestamp of a `DATETIME`, `TIME`, `DATE`, `TIMESTAMP` field in miliseconds.

`UNIX_TIMESTAMP_MS(DateTime)` works in the same way the builtin function `UNIX_TIMESTAMP(DateTime)` does the only difference being the fact that returns the timestamp in miliseconds and not in seconds. 

If called with no argument, returns a Unix timestamp (miliseconds since '1970-01-01 00:00:00' UTC) as an unsigned integer. If `UNIX_TIMESTAMP_MS()` is called with a date argument, it returns the value of the argument as miliseconds since '1970-01-01 00:00:00' UTC. date may be a DATE string, a DATETIME string or a TIMESTAMP. The server interprets date as a value in the current time zone and converts it to an internal value in UTC. 

**Compilation**

Just run `make compile` in the project root.

**Installation**

After you compile you must install it and tell MySQL about it ([More details][1]). You need to locate the plugin directory. This directory is given by the value of the `plugin_dir` system variable. Usually located in `/usr/lib/mysql/plugin/` in linux.
 
- Copy the shared object to the server's plugin directory (see above) and name it `unix_timestamp_ms.so`
- Inform mysql about the new function by running: `CREATE FUNCTION unix_timestamp_ms RETURNS INT SONAME 'unix_timestamp_ms.so';`

**Testing**

```
SELECT UNIX_TIMESTAMP_MS("2016-03-30 18:29:09") = UNIX_TIMESTAMP("2016-03-30 18:29:09")*1000
SELECT UNIX_TIMESTAMP_MS(DATE("2016-03-30 18:29:09")) = UNIX_TIMESTAMP(DATE("2016-03-30 18:29:09"))*1000 
SELECT UNIX_TIMESTAMP_MS(TIME("2016-03-30 18:29:09")) = UNIX_TIMESTAMP(TIME("2016-03-30 18:29:09"))*1000
SELECT UNIX_TIMESTAMP_MS() > UNIX_TIMESTAMP()*1000
```

**Notes**

- It keeps track for 3 decimals precision for miliseconds. For example the following are valid: `SELECT UNIX_TIMESTAMP_MS(NOW(3))`, `SELECT UNIX_TIMESTAMP_MS(NOW(2))` or `SELECT UNIX_TIMESTAMP_MS("2016-03-30 18:29:09.452")`
- In case the input is not in a valid format will return NULL or will trigger an error in case there are too many arguments (more than 1).
- The GMT offset is the one from system when the module is loaded and cached. In case you changed the system timezone you need to reload the module by dropping and crating the function back or restart the server.
- The function doesn't accept yet as input a number in the format `YYMMDD` or `YYYYMMDD` as `UNIX_TIMESTAMP` does.

**Benchmarks**

We run 10 Milion times the `UNIX_TIMESTAMP` and `UNIX_TIMESTAMP_MS` with the same arguments and compoared the results. For example:

```
SET @now=NOW();
SET @loops=10000000;
SELECT BENCHMARK(@loops, UNIX_TIMESTAMP(@now));
```

Results:

|Argument            | UNIX_TIMESTAMP (ms)   | UNIX_TIMESTAMP_MS (ms) |
|:------------------:|:---------------------:|:----------------------:|
| NOW()              | 5107 				 | 850                    |
| DATE(NOW())        | 4840                  | 687                    |
| TIMESTAMP(NOW())   | 5123                  | 856                    |
| NOW(3)             | 5991                  | 939   				  |
| No argument        | 124                   | 423                    |


[1]:http://dev.mysql.com/doc/refman/5.7/en/udf-compiling.html
