# NodeJS

**Version:**

```bash
$ node -v

v8.10.0
```

**Script:**

`index.js`:
```js
const http = require('http');

const hostname = '127.0.0.1';
const port = 8080;

const server = http.createServer((req, res) => {
    res.statusCode = 200;
    res.setHeader('Content-Type', 'text/html');
    res.end("<html><head><title>Hello world</title></head><body>Hello world</body></html>");
});

server.listen(port, hostname, () => {
    console.log(`Server running at http://${hostname}:${port}/`);
});
````

**Run:**

```bash
$ node index.js
 
Server running at http://127.0.0.1:8080/
```

# Nginx

**Version:**

```bash
$ nginx -v

nginx version: nginx/1.14.0
```

**nginx.conf:**

```
...

worker_processes auto;
worker_cpu_affinity auto;
events {
    worker_connections  10000;
}
http {
    access_log off;
    keepalive_timeout 65;
}

...
```

**NOTE:** remember to disable all nginx logs. 

# Sagui

**Version:**

```bash
$ pkg-config libsagui --modversion
 
1.0.0
```

**Build:**

```bash
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang ..
make example_httpsrv_benchmark
```

**Run:**

```bash
$ ./examples/example_httpsrv_benchmark
 
Number of processors: 8
Connections limit: 1000
Server running at http://localhost:42587
```

# Environment

```bash
$ lscpu

Architecture:        x86_64
CPU op-mode(s):      32-bit, 64-bit
Byte Order:          Little Endian
CPU(s):              8
On-line CPU(s) list: 0-7
Thread(s) per core:  2
Core(s) per socket:  4
Socket(s):           1
NUMA node(s):        1
Vendor ID:           GenuineIntel
CPU family:          6
Model:               58
Model name:          Intel(R) Core(TM) i7-3632QM CPU @ 2.20GHz
Stepping:            9
CPU MHz:             1509.846
CPU max MHz:         3200.0000
CPU min MHz:         1200.0000
BogoMIPS:            4390.30
Virtualization:      VT-x
L1d cache:           32K
L1i cache:           32K
L2 cache:            256K
L3 cache:            6144K
NUMA node0 CPU(s):   0-7
Flags:               fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx rdtscp lm constant_tsc arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 cx16 xtpr pdcm pcid sse4_1 sse4_2 x2apic popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm cpuid_fault epb pti ibrs ibpb stibp tpr_shadow vnmi flexpriority ept vpid fsgsbase smep erms xsaveopt dtherm ida arat pln pts
```

```
SO: Ubuntu 18.04.1 LTS (kernel 4.15.0-30-generic)
```

# ApacheBench logs

**NodeJS:**

```
$ ab -n 10000 -c 1000 -k http://127.0.0.1:8080/

This is ApacheBench, Version 2.3 <$Revision: 1807734 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking 127.0.0.1 (be patient)
Completed 1000 requests
Completed 2000 requests
Completed 3000 requests
Completed 4000 requests
Completed 5000 requests
Completed 6000 requests
Completed 7000 requests
Completed 8000 requests
Completed 9000 requests
Completed 10000 requests
Finished 10000 requests


Server Software:
Server Hostname:        127.0.0.1
Server Port:            8080

Document Path:          /
Document Length:        76 bytes

Concurrency Level:      1000
Time taken for tests:   1.897 seconds
Complete requests:      10000
Failed requests:        0
Keep-Alive requests:    0
Total transferred:      1760000 bytes
HTML transferred:       760000 bytes
Requests per second:    5272.85 [#/sec] (mean)
Time per request:       189.651 [ms] (mean)
Time per request:       0.190 [ms] (mean, across all concurrent requests)
Transfer rate:          906.27 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0   79 269.8      0    1036
Processing:     8   44 108.3     18     849
Waiting:        7   44 108.3     18     849
Total:         14  122 353.8     18    1864

Percentage of the requests served within a certain time (ms)
  50%     18
  66%     21
  75%     23
  80%     25
  90%    105
  95%   1225
  98%   1447
  99%   1858
 100%   1864 (longest request)
```

**Nginx:**

```
$ ab -n 10000 -c 1000 -k http://127.0.0.1:80/

This is ApacheBench, Version 2.3 <$Revision: 1807734 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking 127.0.0.1 (be patient)
Completed 1000 requests
Completed 2000 requests
Completed 3000 requests
Completed 4000 requests
Completed 5000 requests
Completed 6000 requests
Completed 7000 requests
Completed 8000 requests
Completed 9000 requests
Completed 10000 requests
Finished 10000 requests


Server Software:        nginx/1.14.0
Server Hostname:        127.0.0.1
Server Port:            80

Document Path:          /
Document Length:        76 bytes

Concurrency Level:      1000
Time taken for tests:   0.232 seconds
Complete requests:      10000
Failed requests:        0
Keep-Alive requests:    10000
Total transferred:      3120000 bytes
HTML transferred:       760000 bytes
Requests per second:    43109.77 [#/sec] (mean)
Time per request:       23.197 [ms] (mean)
Time per request:       0.023 [ms] (mean, across all concurrent requests)
Transfer rate:          13135.01 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    3  10.2      0      52
Processing:     1   16  13.5     14      33
Waiting:        1   16  13.5     14      33
Total:          1   19  16.9     28      66

Percentage of the requests served within a certain time (ms)
  50%     28
  66%     31
  75%     31
  80%     31
  90%     32
  95%     48
  98%     57
  99%     60
 100%     66 (longest request)
```

**Sagui:**

```
$ ab -n 10000 -c 1000 -k http://127.0.0.1:32933/

This is ApacheBench, Version 2.3 <$Revision: 1807734 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking 127.0.0.1 (be patient)
Completed 1000 requests
Completed 2000 requests
Completed 3000 requests
Completed 4000 requests
Completed 5000 requests
Completed 6000 requests
Completed 7000 requests
Completed 8000 requests
Completed 9000 requests
Completed 10000 requests
Finished 10000 requests


Server Software:
Server Hostname:        127.0.0.1
Server Port:            32933

Document Path:          /
Document Length:        76 bytes

Concurrency Level:      1000
Time taken for tests:   0.214 seconds
Complete requests:      10000
Failed requests:        0
Keep-Alive requests:    10000
Total transferred:      2010000 bytes
HTML transferred:       760000 bytes
Requests per second:    46673.14 [#/sec] (mean)
Time per request:       21.426 [ms] (mean)
Time per request:       0.021 [ms] (mean, across all concurrent requests)
Transfer rate:          9161.43 [Kbytes/sec] received

Connection Times (ms)
              min  mean[+/-sd] median   max
Connect:        0    3   9.2      0      48
Processing:     0   15  14.2     13      42
Waiting:        0   15  14.2     13      42
Total:          0   17  17.0     27      62

Percentage of the requests served within a certain time (ms)
  50%     27
  66%     27
  75%     28
  80%     32
  90%     40
  95%     42
  98%     52
  99%     55
 100%     62 (longest request)
```

# Wrk logs

**NodeJS:**

```
$ wrk -t10 -c1000 -d10s --latency http://127.0.0.1:8080/

Running 10s test @ http://127.0.0.1:8080/
  10 threads and 1000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    68.78ms   55.19ms 968.08ms   98.28%
    Req/Sec     1.47k   359.24     2.65k    77.60%
  Latency Distribution
     50%   65.43ms
     75%   66.54ms
     90%   68.12ms
     99%  286.45ms
  146106 requests in 10.04s, 28.01MB read
Requests/sec:  14557.41
Transfer/sec:      2.79MB
```

**Nginx:**

```
$ wrk -t10 -c1000 -d10s --latency http://127.0.0.1:80/

Running 10s test @ http://127.0.0.1:80/
  10 threads and 1000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     9.99ms    5.11ms 221.85ms   75.55%
    Req/Sec     9.63k     1.45k   25.26k    71.91%
  Latency Distribution
     50%    8.53ms
     75%   12.50ms
     90%   15.88ms
     99%   26.13ms
  956751 requests in 10.09s, 284.64MB read
Requests/sec:  94804.50
Transfer/sec:     28.20MB
```

**Sagui:**

```
$ wrk -t10 -c1000 -d10s --latency http://127.0.0.1:34187

Running 10s test @ http://127.0.0.1:34187
  10 threads and 1000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     9.22ms   15.42ms 467.42ms   96.05%
    Req/Sec    12.34k     2.29k   29.15k    78.31%
  Latency Distribution
     50%    5.19ms
     75%   12.12ms
     90%   20.20ms
     99%   33.60ms
  1214358 requests in 10.10s, 232.78MB read
Requests/sec: 120254.65
Transfer/sec:     23.05MB
```

# JMeter logs

Take a look at the [CVS](https://en.wikipedia.org/wiki/Comma-separated_values) files in [benchmark/](https://github.com/risoflora/libsagui/tree/master/examples/benchmark).