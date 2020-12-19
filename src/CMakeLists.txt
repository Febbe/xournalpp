cmake_minimum_required(VERSION 3.11)

# Todo: add to target by generator expression
#if (DEV_ENABLE_GCOV)
#    add_definitions(--coverage)
#endif (DEV_ENABLE_GCOV)

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
##############################
# xournalpp-core
##############################
add_subdirectory(core)

##############################
# xournalpp main program
##############################

add_executable(xournalpp core/Xournalpp.cpp)
target_link_libraries(xournalpp PRIVATE xoj::core)

install(TARGETS xournalpp
        RUNTIME DESTINATION bin
        COMPONENT xournalpp
        )

if (ENABLE_CPPUNIT)
    add_subdirectory(${CMAKE_SOURCE_DIR}/test ${CMAKE_BINARY_DIR}/test)
endif (ENABLE_CPPUNIT)

add_subdirectory(xoj-preview-extractor)