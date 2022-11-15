set(HDRS
alpha.h
assignablevarconv.h
begconv.h
callcc.h
closure.h
compiler.h
constfold.h
constprop.h
context.h
cps.h
copy.h
defconv.h
dump.h
environment.h
error.h
foreign.h
freevaranalysis.h
func.h
gc.h
globdef.h
inlines.h
inputoutput.h
lambdatolet.h
limits.h
macro.h
map.h
memory.h
modules.h
object.h
parser.h
preprocess.h
pool.h
primitives.h
quasiquote.h
quotecollect.h
quoteconv.h
r5rs.h
reader.h
schemerlicht.h
simplifyconv.h
stream.h
string.h
stringvec.h
syscalls.h
tailcall.h
token.h
vector.h
visitor.h
vm.h
)
	
set(SRCS
alpha.c
assignablevarconv.c
begconv.c
callcc.c
closure.c
compiler.c
constfold.c
constprop.c
context.c
cps.c
copy.c
defconv.c
dump.c
environment.c
error.c
foreign.c
freevaranalysis.c
func.c
gc.c
globdef.c
inputoutput.c
lambdatolet.c
macro.c
map.c
memory.c
modules.c
object.c
parser.c
preprocess.c
pool.c
primitives.c
quasiquote.c
quoteconv.c
quotecollect.c
r5rs.c
reader.c
simplifyconv.c
stream.c
string.c
stringvec.c
syscalls.c
tailcall.c
token.c
visitor.c
vm.c
)

if (WIN32)
set(CMAKE_C_FLAGS_DEBUG "/W4 /MP /GF /RTCu /Od /MDd /Zi")
set(CMAKE_C_FLAGS_RELEASE "/W4 /MP /GF /O2 /Ob2 /Oi /Ot /MD /Zi /DNDEBUG")
endif(WIN32)



add_library(schemerlicht STATIC ${HDRS} ${SRCS})
source_group("Header Files" FILES ${hdrs})
source_group("Source Files" FILES ${srcs})

target_include_directories(schemerlicht
    PRIVATE
	  ${CMAKE_CURRENT_SOURCE_DIR}/..
    )	
	
if (UNIX)
target_link_libraries(schemerlicht
    PRIVATE	
    m	  
    )
endif (UNIX)    