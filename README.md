# UNIX_TIMESTAMP_MS for MySQL

MySQL UDF function implemented in C++ for getting the value of a `DATETIME`, `TIME`, `DATE`, `TIMESTAMP` field in miliseconds.

`UNIX_TIMESTAMP_MS(DateTime)` is the same as `UNIX_TIMESTAMP(DateTime)*1000`

Also `UNIX_TIMESTAMP_MS()` returns the current unix timestamp in miliseconds.

**Compilation**

Just run `make compile` in the project root.

**Installation**

After you compile you must install it and tell MySQL about it ([More details][1]).
 
-Copy the shared object to the server's plugin directory and name it `unix_timestamp_ms.so`. This directory is given by the value of the `plugin_dir` system variable. Usually located in `/usr/lib/mysql/plugin/` in linux.
-Inform mysql about the new function by running: `CREATE FUNCTION UNIX_TIMESTAMP_MS RETURNS INT SONAME 'unix_timestamp_ms.so';`

**Testing**

```
SELECT UNIX_TIMESTAMP_MS("2016-03-30 18:29:09") = UNIX_TIMESTAMP("2016-03-30 18:29:09")*1000
SELECT UNIX_TIMESTAMP_MS(DATE("2016-03-30 18:29:09")) = UNIX_TIMESTAMP(DATE("2016-03-30 18:29:09"))*1000 
SELECT UNIX_TIMESTAMP_MS(TIME("2016-03-30 18:29:09")) = UNIX_TIMESTAMP(TIME("2016-03-30 18:29:09"))*1000
SELECT UNIX_TIMESTAMP_MS() > UNIX_TIMESTAMP()*1000
```

[1]:http://dev.mysql.com/doc/refman/5.7/en/udf-compiling.html
