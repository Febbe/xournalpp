cmake_minimum_required(VERSION 3.11)

##############################
# xoj default option target
##############################
add_library(defaults INTERFACE)
target_compile_options(defaults INTERFACE -Wall -Wreturn-type -Wuninitialized -Wunused-value -Wunused-variable)
target_include_directories(defaults INTERFACE ${PROJECT_BINARY_DIR}/src)
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

add_subdirectory(xoj-preview-extractor)

if (DEV_ENABLE_GCOV)
    add_definitions(--coverage)
endif (DEV_ENABLE_GCOV)

# Used to compile xournalpp and xournalpp-test
set(xournalpp_LDFLAGS
        ${xournalpp_LDFLAGS}
        )

##############################
# xournalpp-core
##############################

# These dirs are xournalpp only so it's safe to add them recursively
unset(xournalpp_SOURCES_RECURSE)
file(GLOB_RECURSE xournalpp_SOURCES_RECURSE
        audio/*.cpp
        control/*.cpp
        enums/*.cpp
        gui/*.cpp
        io/*.cpp
        model/*.cpp
        plugin/*.cpp
        undo/*.cpp
        view/*.cpp
        audio/*.h
        control/*.h
        enums/*.h
        gui/*.h
        io/*.h
        model/*.h
        plugin/*.h
        undo/*.h
        view/*.h
        )

# Here GLOB_RECURSE wouldn't be safe
unset(xournalpp_SOURCES)
file(GLOB xournalpp_SOURCES
        pdf/base/*.cpp
        pdf/base/*.h
        )

# Concatenate SOURCES lists
set(xournalpp_SOURCES ${xournalpp_SOURCES_RECURSE} ${xournalpp_SOURCES})
unset(xournalpp_SOURCES_RECURSE)

file(GLOB xournalpp_SOURCES_RECURSE pdf/popplerapi/*.cpp)

# Concatenate SOURCES lists
set(xournalpp_SOURCES ${xournalpp_SOURCES_RECURSE} ${xournalpp_SOURCES})
unset(xournalpp_SOURCES_RECURSE)

set(xournalpp_include_dirs "${PROJECT_SOURCE_DIR}/src")

# Used for xournalpp and xournalpp-test
add_library(xournalpp-core STATIC ${xournalpp_SOURCES})
target_compile_features(xournalpp-core PUBLIC ${PROJECT_CXX_FEATURES})
target_link_libraries(xournalpp-core
        PRIVATE xoj::util ${xournalpp_LDFLAGS}
        PUBLIC xoj::stacktrace xoj::defaults)
target_include_directories(xournalpp-core PRIVATE)
target_include_directories(xournalpp-core PRIVATE ${xournalpp_include_dirs})

##############################
# xournalpp main program
##############################

add_executable(xournalpp Xournalpp.cpp)
target_link_libraries(xournalpp PRIVATE xournalpp-core)

install(TARGETS xournalpp
        RUNTIME DESTINATION bin
        COMPONENT xournalpp
        )

if (ENABLE_CPPUNIT)
    add_subdirectory(${CMAKE_SOURCE_DIR}/test ${CMAKE_BINARY_DIR}/test)
endif (ENABLE_CPPUNIT)

