
Below is the output of the benchmarks.
I ran these on an Intel Core i7 - 10850H CPU @ 2.70Ghz laptop.

These results can be replicated by running the command line call

 > sl.exe benchall.scm
  
where 'sl.exe' is the schemerlicht compiler that is built via CMake as explained
on the main page.

ack
running "ack" (20)
Ran for 17.534000 seconds
Compiled and ran for 17.537000 seconds
array1
running "array1" (2)
Ran for 6.630000 seconds
Compiled and ran for 6.633000 seconds
boyer
running "boyer" (50)
Ran for 21.678000 seconds
Compiled and ran for 21.689000 seconds
browse
running "browse" (600)
Ran for 17.242000 seconds
Compiled and ran for 17.250000 seconds
cat
running "cat" (12)
Ran for 21.685000 seconds
Compiled and ran for 21.687000 seconds
compiler
running "compiler" (500)
Ran for 14.015000 seconds
Compiled and ran for 17.610000 seconds
conform
running "conform" (70)
Ran for 25.890000 seconds
Compiled and ran for 25.905000 seconds
cpstak
running "cpstak" (1700)
Ran for 15.838000 seconds
Compiled and ran for 15.841000 seconds
ctak
running "ctak" (160)
Ran for 6.834000 seconds
Compiled and ran for 6.836000 seconds
dderiv
running "dderiv" (3000000)
Ran for 37.482000 seconds
Compiled and ran for 37.486000 seconds
deriv
running "deriv" (4000000)
Ran for 35.001000 seconds
Compiled and ran for 35.004000 seconds
destruc
running "destruc" (800)
Ran for 11.525000 seconds
Compiled and ran for 11.528000 seconds
diviter
running "diviter" (1200000)
Ran for 27.080000 seconds
Compiled and ran for 27.083000 seconds
dynamic
running "dynamic" (70)
Ran for 14.125000 seconds
Compiled and ran for 14.176000 seconds
earley
running "earley" (400)
Ran for 31.785000 seconds
Compiled and ran for 31.801000 seconds
fft
running "fft" (4000)
Ran for 6.845000 seconds
Compiled and ran for 6.848000 seconds
fib
running "fib" (6)
Ran for 16.833000 seconds
Compiled and ran for 16.835000 seconds
fibc
running "fibc" (900)
Ran for 11.515000 seconds
Compiled and ran for 11.518000 seconds
fibfp
running "fibfp" (2)
Ran for 5.915000 seconds
Compiled and ran for 5.917000 seconds
formattest
running "fibfp" (2)
Ran for 5.831000 seconds
Compiled and ran for 5.832000 seconds
fpsum
running "fpsum" (60)
Ran for 2.901000 seconds
Compiled and ran for 2.903000 seconds
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
Ran for 16.017000 seconds
Compiled and ran for 16.023000 seconds
graphs
running "graphs" (500)
Ran for 21.230000 seconds
Compiled and ran for 21.238000 seconds
lattice
running "lattice" (2)
Ran for 44.653000 seconds
Compiled and ran for 44.659000 seconds
maze
running "maze" (4000)
Ran for 15.583000 seconds
Compiled and ran for 15.618000 seconds
mazefun
running "mazefun" (2500)
Ran for 21.015000 seconds
Compiled and ran for 21.028000 seconds
mbrot
running "mbrot" (120)
Ran for 3.088000 seconds
Compiled and ran for 3.091000 seconds
nboyer
running "nboyer0" (150)
Ran for 32.844000 seconds
Compiled and ran for 32.859000 seconds
nqueens
running "nqueens" (4000)
Ran for 23.550000 seconds
Compiled and ran for 23.553000 seconds
ntakl
running "ntakl" (600)
Ran for 23.001000 seconds
Compiled and ran for 23.004000 seconds
paraffins
running "paraffins" (1800)
Ran for 20.155000 seconds
Compiled and ran for 20.160000 seconds
parsing
running "parsing:parsing-data.scm:360" (360)
Ran for 14.529000 seconds
Compiled and ran for 14.582000 seconds
perm9
running "perm9" (12)
Ran for 10.104000 seconds
Compiled and ran for 10.729000 seconds
peval
running "peval" (400)
Ran for 29.574000 seconds
Compiled and ran for 29.594000 seconds
pnpoly
running "pnpoly" (140000)
Ran for 7.389000 seconds
Compiled and ran for 7.726000 seconds
primes
running "primes" (180000)
Ran for 22.822000 seconds
Compiled and ran for 22.825000 seconds
puzzle
running "puzzle" (180)
Ran for 13.301000 seconds
Compiled and ran for 13.306000 seconds
quicksort
running "quicksort30" (60)
Ran for 11.443000 seconds
Compiled and ran for 11.449000 seconds
ray
running "ray" (5)
Ran for 15.981000 seconds
Compiled and ran for 15.987000 seconds
sboyer
running "sboyer0" (200)
Ran for 40.434000 seconds
Compiled and ran for 40.448000 seconds
scheme
running "scheme" (40000)
Ran for 19.242000 seconds
Compiled and ran for 19.264000 seconds
simplex
running "simplex" (160000)
Ran for 25.592000 seconds
Compiled and ran for 25.737000 seconds
slatex
running "slatex" (30)
Ran for 3.447000 seconds
Compiled and ran for 3.505000 seconds
string
running "string" (4)
Ran for 0.016000 seconds
Compiled and ran for 0.019000 seconds
sum
running "sum" (30000)
Ran for 14.600000 seconds
Compiled and ran for 14.603000 seconds
sum1
running "sum1" (5)
Ran for 4.998000 seconds
15794.975000
Compiled and ran for 5.002000 seconds
sumfp
running "sumfp" (8000)
Ran for 3.776000 seconds
Compiled and ran for 3.778000 seconds
sumloop
running "sumloop" (2)
Ran for 10.982000 seconds
Compiled and ran for 10.984000 seconds
tail
running "tail" (4)
Ran for 11.752000 seconds
Compiled and ran for 11.756000 seconds
tak
running "tak" (3000)
Ran for 28.290000 seconds
Compiled and ran for 28.292000 seconds
takl
running "takl" (500)
Ran for 20.560000 seconds
Compiled and ran for 20.562000 seconds
trav1
running "trav1" (150)
Ran for 13.394000 seconds
Compiled and ran for 13.399000 seconds
trav2
running "trav2" (40)
Ran for 41.173000 seconds
Compiled and ran for 41.261000 seconds
triangl
running "triangl" (12)
Ran for 14.069000 seconds
Compiled and ran for 14.072000 seconds
wc
running "wc" (15)
Ran for 23.335000 seconds
Compiled and ran for 23.337000 seconds
#undefined