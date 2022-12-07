
Below is the output of the benchmarks.
I ran these on an Intel Core i7 - 10850H CPU @ 2.70Ghz laptop.

These results can be replicated by running the command line call

 > sl.exe benchall.scm
  
where 'sl.exe' is the schemerlicht compiler that is built via CMake as explained
on the main page.

ack
running "ack" (20)
Ran for 19.361000 seconds
Compiled and ran for 19.364000 seconds
array1
running "array1" (2)
Ran for 8.518000 seconds
Compiled and ran for 8.521000 seconds
boyer
running "boyer" (50)
Ran for 28.278000 seconds
Compiled and ran for 28.289000 seconds
browse
running "browse" (600)
Ran for 19.963000 seconds
Compiled and ran for 19.971000 seconds
cat
running "cat" (12)
Ran for 23.047000 seconds
Compiled and ran for 23.050000 seconds
compiler
running "compiler" (500)
Ran for 15.960000 seconds
Compiled and ran for 19.448000 seconds
conform
running "conform" (70)
Ran for 30.819000 seconds
Compiled and ran for 30.834000 seconds
cpstak
running "cpstak" (1700)
Ran for 20.267000 seconds
Compiled and ran for 20.270000 seconds
ctak
running "ctak" (160)
Ran for 8.009000 seconds
Compiled and ran for 8.012000 seconds
dderiv
running "dderiv" (3000000)
Ran for 43.188000 seconds
Compiled and ran for 43.192000 seconds
deriv
running "deriv" (4000000)
Ran for 39.826000 seconds
Compiled and ran for 39.829000 seconds
destruc
running "destruc" (800)
Ran for 14.616000 seconds
Compiled and ran for 14.619000 seconds
diviter
running "diviter" (1200000)
Ran for 31.604000 seconds
Compiled and ran for 31.607000 seconds
dynamic
running "dynamic" (70)
Ran for 16.677000 seconds
Compiled and ran for 16.727000 seconds
earley
running "earley" (400)
Ran for 41.389000 seconds
Compiled and ran for 41.405000 seconds
fft
running "fft" (4000)
Ran for 9.394000 seconds
Compiled and ran for 9.397000 seconds
fib
running "fib" (6)
Ran for 21.865000 seconds
Compiled and ran for 21.867000 seconds
fibc
running "fibc" (900)
Ran for 14.347000 seconds
Compiled and ran for 14.352000 seconds
fibfp
running "fibfp" (2)
Ran for 7.412000 seconds
Compiled and ran for 7.415000 seconds
formattest
running "fibfp" (2)
Ran for 7.416000 seconds
Compiled and ran for 7.417000 seconds
fpsum
running "fpsum" (60)
Ran for 3.538000 seconds
Compiled and ran for 3.542000 seconds
gcbench
The garbage collector should touch about 32 megabytes of heap storage.
The use of more or less memory will skew the results.
running "GCBench18" (2)
Garbage Collector Test
 Stretching memory with a binary tree of depth 18
 Total memory available= ???????? bytes  Free memory= ???????? bytes
GCBench: Main
 Creating a long-lived binary tree of depth 16
 Creating a long-lived array of 524284 inexact reals
 Total memory available= ???????? bytes  Free memory= ???????? bytes
Creating 33824 trees of depth 4
GCBench: Top down construction
GCBench: Bottom up construction
Creating 8256 trees of depth 6
GCBench: Top down construction
GCBench: Bottom up construction
Creating 2052 trees of depth 8
GCBench: Top down construction
GCBench: Bottom up construction
Creating 512 trees of depth 10
GCBench: Top down construction
GCBench: Bottom up construction
Creating 128 trees of depth 12
GCBench: Top down construction
GCBench: Bottom up construction
Creating 32 trees of depth 14
GCBench: Top down construction
GCBench: Bottom up construction
Creating 8 trees of depth 16
GCBench: Top down construction
GCBench: Bottom up construction
 Total memory available= ???????? bytes  Free memory= ???????? bytes
Garbage Collector Test
 Stretching memory with a binary tree of depth 18
 Total memory available= ???????? bytes  Free memory= ???????? bytes
GCBench: Main
 Creating a long-lived binary tree of depth 16
 Creating a long-lived array of 524284 inexact reals
 Total memory available= ???????? bytes  Free memory= ???????? bytes
Creating 33824 trees of depth 4
GCBench: Top down construction
GCBench: Bottom up construction
Creating 8256 trees of depth 6
GCBench: Top down construction
GCBench: Bottom up construction
Creating 2052 trees of depth 8
GCBench: Top down construction
GCBench: Bottom up construction
Creating 512 trees of depth 10
GCBench: Top down construction
GCBench: Bottom up construction
Creating 128 trees of depth 12
GCBench: Top down construction
GCBench: Bottom up construction
Creating 32 trees of depth 14
GCBench: Top down construction
GCBench: Bottom up construction
Creating 8 trees of depth 16
GCBench: Top down construction
GCBench: Bottom up construction
 Total memory available= ???????? bytes  Free memory= ???????? bytes
Ran for 22.194000 seconds
Compiled and ran for 22.200000 seconds
graphs
running "graphs" (500)
Ran for 29.480000 seconds
Compiled and ran for 29.487000 seconds
lattice
running "lattice" (2)
Ran for 58.860000 seconds
Compiled and ran for 58.865000 seconds
maze
running "maze" (4000)
Ran for 20.097000 seconds
Compiled and ran for 20.136000 seconds
mazefun
running "mazefun" (2500)
Ran for 26.902000 seconds
Compiled and ran for 26.917000 seconds
mbrot
running "mbrot" (120)
Ran for 4.066000 seconds
Compiled and ran for 4.069000 seconds
nboyer
running "nboyer0" (150)
Ran for 43.612000 seconds
Compiled and ran for 43.630000 seconds
nqueens
running "nqueens" (4000)
Ran for 30.132000 seconds
Compiled and ran for 30.135000 seconds
ntakl
running "ntakl" (600)
Ran for 27.350000 seconds
Compiled and ran for 27.352000 seconds
paraffins
running "paraffins" (1800)
Ran for 25.795000 seconds
Compiled and ran for 25.801000 seconds
parsing
running "parsing:parsing-data.scm:360" (360)
Ran for 19.541000 seconds
Compiled and ran for 19.599000 seconds
perm9
running "perm9" (12)
Ran for 13.081000 seconds
Compiled and ran for 13.841000 seconds
peval
running "peval" (400)
Ran for 38.818000 seconds
Compiled and ran for 38.840000 seconds
pnpoly
running "pnpoly" (140000)
Ran for 10.655000 seconds
Compiled and ran for 11.027000 seconds
primes
running "primes" (180000)
Ran for 29.622000 seconds
Compiled and ran for 29.624000 seconds
puzzle
running "puzzle" (180)
Ran for 18.295000 seconds
Compiled and ran for 18.301000 seconds
quicksort
running "quicksort30" (60)
Ran for 15.706000 seconds
Compiled and ran for 15.716000 seconds
ray
running "ray" (5)
Ran for 21.181000 seconds
Compiled and ran for 21.190000 seconds
sboyer
running "sboyer0" (200)
Ran for 55.982000 seconds
Compiled and ran for 55.997000 seconds
scheme
running "scheme" (40000)
Ran for 24.448000 seconds
Compiled and ran for 24.478000 seconds
simplex
running "simplex" (160000)
Ran for 34.584000 seconds
Compiled and ran for 34.749000 seconds
slatex
running "slatex" (30)
Ran for 4.190000 seconds
Compiled and ran for 4.270000 seconds
string
running "string" (4)
Ran for 0.023000 seconds
Compiled and ran for 0.026000 seconds
sum
running "sum" (30000)
Ran for 18.392000 seconds
Compiled and ran for 18.397000 seconds
sum1
running "sum1" (5)
Ran for 5.051000 seconds
15794.975000
Compiled and ran for 5.055000 seconds
sumfp
running "sumfp" (8000)
Ran for 5.046000 seconds
Compiled and ran for 5.049000 seconds
sumloop
running "sumloop" (2)
Ran for 14.410000 seconds
Compiled and ran for 14.412000 seconds
tail
running "tail" (4)
Ran for 15.668000 seconds
Compiled and ran for 15.670000 seconds
tak
running "tak" (3000)
Ran for 30.842000 seconds
Compiled and ran for 30.846000 seconds
takl
running "takl" (500)
Ran for 24.721000 seconds
Compiled and ran for 24.726000 seconds
trav1
running "trav1" (150)
Ran for 18.932000 seconds
Compiled and ran for 18.938000 seconds
trav2
running "trav2" (40)
Ran for 63.264000 seconds
Compiled and ran for 63.387000 seconds
triangl
running "triangl" (12)
Ran for 18.139000 seconds
Compiled and ran for 18.145000 seconds
wc
running "wc" (15)
Ran for 27.819000 seconds
Compiled and ran for 27.823000 seconds
#undefined
>