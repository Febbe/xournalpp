cmake_minimum_required(VERSION 3.11)

##############################
# xoj default option target
##############################
add_library(defaults INTERFACE)
target_compile_definitions(defaults INTERFACE -Wall -Wreturn-type -Wuninitialized -Wunused-value -Wunused-variable)
add_library(xoj::defaults ALIAS defaults)

##############################
# stacktrace library
##############################
# Todo use platform independent stacktrace library
# Currently this works only for linux systems
add_library(gnu_stacktrace INTERFACE)
target_link_options(gnu_stacktrace INTERFACE $<$<AND:$<NOT:$<PLATFORM_ID:Windows>>,$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>>:-rdynamic>)
add_library(xoj::stacktrace ALIAS gnu_stacktrace)

##############################
# util
##############################

add_subdirectory(util)

# include_directories (
#        "${PROJECT_BINARY_DIR}/src"
#        "${PROJECT_SOURCE_DIR}/src"
#        "${PROJECT_SOURCE_DIR}/src/util"
#        ${xournalpp_INCLUDE_DIRS}
#)

# Build xournalpp-thumbnailer as it doesn't need CXX_FLAGS from below
add_subdirectory(xoj-preview-extractor)

if (DEV_ENABLE_GCOV)
add_definitions(--coverage)
endif (DEV_ENABLE_GCOV)

# Used to compile xournalpp and xournalpp-test
set(xournalpp_LDFLAGS
util
${xournalpp_LDFLAGS}
)

## Build util lib ##


## Sources ##

# These dirs are xournalpp only so it's safe to add then recursively
unset(xournalpp_SOURCES_RECURSE)
file(GLOB_RECURSE xournalpp_SOURCES_RECURSE
control/*.cpp
enums/*.cpp
gui/*.cpp
io/*.cpp
model/*.cpp
plugin/*.cpp
undo/*.cpp
view/*.cpp
)

# Here GLOB_RECURSE wouldn't be safe
unset(xournalpp_SOURCES)
file(GLOB xournalpp_SOURCES
pdf/base/*.cpp
)

# Concatenate SOURCES lists
set(xournalpp_SOURCES ${xournalpp_SOURCES_RECURSE} ${xournalpp_SOURCES})
unset(xournalpp_SOURCES_RECURSE)

file(GLOB xournalpp_SOURCES_RECURSE
pdf/popplerapi/*.cpp
)

# Concatenate SOURCES lists
set(xournalpp_SOURCES ${xournalpp_SOURCES_RECURSE} ${xournalpp_SOURCES})
unset(xournalpp_SOURCES_RECURSE)

## Core library ##

# Used for xournalpp and xournalpp-test
add_library(xournalpp-core OBJECT ${xournalpp_SOURCES})
add_dependencies(xournalpp-core util)
target_compile_features(xournalpp-core PUBLIC ${PROJECT_CXX_FEATURES})
# Can't use target_link_directories since we require CMake >= 3.10
target_include_directories(xournalpp-core PRIVATE $<TARGET_PROPERTY:util,INTERFACE_INCLUDE_DIRECTORIES>)

## xournalpp main program ##
add_executable(xournalpp
$<TARGET_OBJECTS:xournalpp-core>
Xournalpp.cpp
)

add_dependencies(xournalpp
xournalpp-core
util
)

target_link_libraries(xournalpp ${xournalpp_LDFLAGS})

install(TARGETS xournalpp
RUNTIME DESTINATION bin
COMPONENT xournalpp
)

if (ENABLE_CPPUNIT)
add_subdirectory(${CMAKE_SOURCE_DIR}/test ${CMAKE_BINARY_DIR}/test)
endif (ENABLE_CPPUNIT)

