
Below is the output of the benchmarks.
I ran these on an Intel Core i7 - 10850H CPU @ 2.70Ghz laptop.

These results can be replicated by running the command line call

 > sl.exe benchall.scm
  
where 'sl.exe' is the schemerlicht compiler that is built via CMake as explained
on the main page.

ack
running "ack" (20)
Ran for 17.431000 seconds
Compiled and ran for 17.434000 seconds
array1
running "array1" (2)
Ran for 7.087000 seconds
Compiled and ran for 7.090000 seconds
boyer
running "boyer" (50)
Ran for 21.628000 seconds
Compiled and ran for 21.639000 seconds
browse
running "browse" (600)
Ran for 18.012000 seconds
Compiled and ran for 18.022000 seconds
cat
running "cat" (12)
Ran for 23.461000 seconds
Compiled and ran for 23.464000 seconds
compiler
running "compiler" (500)
Ran for 14.528000 seconds
Compiled and ran for 18.242000 seconds
conform
running "conform" (70)
Ran for 28.261000 seconds
Compiled and ran for 28.277000 seconds
cpstak
running "cpstak" (1700)
Ran for 17.080000 seconds
Compiled and ran for 17.082000 seconds
ctak
running "ctak" (160)
Ran for 6.798000 seconds
Compiled and ran for 6.802000 seconds
dderiv
running "dderiv" (3000000)
Ran for 37.982000 seconds
Compiled and ran for 37.986000 seconds
deriv
running "deriv" (4000000)
Ran for 36.775000 seconds
Compiled and ran for 36.778000 seconds
destruc
running "destruc" (800)
Ran for 12.562000 seconds
Compiled and ran for 12.567000 seconds
diviter
running "diviter" (1200000)
Ran for 28.496000 seconds
Compiled and ran for 28.499000 seconds
dynamic
running "dynamic" (70)
Ran for 14.424000 seconds
Compiled and ran for 14.474000 seconds
earley
running "earley" (400)
Ran for 33.273000 seconds
Compiled and ran for 33.289000 seconds
fft
running "fft" (4000)
Ran for 7.869000 seconds
Compiled and ran for 7.872000 seconds
fib
running "fib" (6)
Ran for 17.691000 seconds
Compiled and ran for 17.693000 seconds
fibc
running "fibc" (900)
Ran for 11.526000 seconds
Compiled and ran for 11.530000 seconds
fibfp
running "fibfp" (2)
Ran for 6.007000 seconds
Compiled and ran for 6.011000 seconds
formattest
running "fibfp" (2)
Ran for 6.236000 seconds
Compiled and ran for 6.242000 seconds
fpsum
running "fpsum" (60)
Ran for 3.089000 seconds
Compiled and ran for 3.091000 seconds
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
Ran for 16.417000 seconds
Compiled and ran for 16.428000 seconds
graphs
running "graphs" (500)
Ran for 22.916000 seconds
Compiled and ran for 22.923000 seconds
lattice
running "lattice" (2)
Ran for 44.993000 seconds
Compiled and ran for 44.997000 seconds
maze
running "maze" (4000)
Ran for 16.126000 seconds
Compiled and ran for 16.164000 seconds
mazefun
running "mazefun" (2500)
Ran for 21.189000 seconds
Compiled and ran for 21.202000 seconds
mbrot
running "mbrot" (120)
Ran for 3.131000 seconds
Compiled and ran for 3.133000 seconds
nboyer
running "nboyer0" (150)
Ran for 34.917000 seconds
Compiled and ran for 34.933000 seconds
nqueens
running "nqueens" (4000)
Ran for 26.547000 seconds
Compiled and ran for 26.551000 seconds
ntakl
running "ntakl" (600)
Ran for 23.633000 seconds
Compiled and ran for 23.647000 seconds
paraffins
running "paraffins" (1800)
Ran for 21.914000 seconds
Compiled and ran for 21.922000 seconds
parsing
running "parsing:parsing-data.scm:360" (360)
Ran for 16.206000 seconds
Compiled and ran for 16.275000 seconds
perm9
running "perm9" (12)
Ran for 10.495000 seconds
Compiled and ran for 11.161000 seconds
peval
running "peval" (400)
Ran for 31.096000 seconds
Compiled and ran for 31.118000 seconds
pnpoly
running "pnpoly" (140000)
Ran for 8.040000 seconds
Compiled and ran for 8.383000 seconds
primes
running "primes" (180000)
Ran for 24.082000 seconds
Compiled and ran for 24.086000 seconds
puzzle
running "puzzle" (180)
Ran for 15.198000 seconds
Compiled and ran for 15.205000 seconds
quicksort
running "quicksort30" (60)
Ran for 12.243000 seconds
Compiled and ran for 12.252000 seconds
ray
running "ray" (5)
Ran for 16.613000 seconds
Compiled and ran for 16.621000 seconds
sboyer
running "sboyer0" (200)
Ran for 43.194000 seconds
Compiled and ran for 43.207000 seconds
scheme
running "scheme" (40000)
Ran for 20.564000 seconds
Compiled and ran for 20.589000 seconds
simplex
running "simplex" (160000)
Ran for 27.737000 seconds
Compiled and ran for 27.896000 seconds
slatex
running "slatex" (30)
Ran for 3.947000 seconds
Compiled and ran for 4.011000 seconds
string
running "string" (4)
Ran for 0.016000 seconds
Compiled and ran for 0.020000 seconds
sum
running "sum" (30000)
Ran for 15.489000 seconds
Compiled and ran for 15.493000 seconds
sum1
running "sum1" (5)
Ran for 5.211000 seconds
15794.975000
Compiled and ran for 5.215000 seconds
sumfp
running "sumfp" (8000)
Ran for 4.151000 seconds
Compiled and ran for 4.153000 seconds
sumloop
running "sumloop" (2)
Ran for 11.950000 seconds
Compiled and ran for 11.952000 seconds
tail
running "tail" (4)
Ran for 12.874000 seconds
Compiled and ran for 12.877000 seconds
tak
running "tak" (3000)
Ran for 24.953000 seconds
Compiled and ran for 24.955000 seconds
takl
running "takl" (500)
Ran for 21.002000 seconds
Compiled and ran for 21.006000 seconds
trav1
running "trav1" (150)
Ran for 14.630000 seconds
Compiled and ran for 14.636000 seconds
trav2
running "trav2" (40)
Ran for 46.400000 seconds
Compiled and ran for 46.500000 seconds
triangl
running "triangl" (12)
Ran for 15.616000 seconds
Compiled and ran for 15.620000 seconds
wc
running "wc" (15)
Ran for 24.148000 seconds
Compiled and ran for 24.152000 seconds
#undefined
>