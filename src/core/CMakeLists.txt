##############################
# xournalpp-core
##############################

# These dirs are xournalpp only so it's safe to add them recursively
unset(xournalpp_sources)
file(GLOB_RECURSE xournalpp_sources
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
unset(xournalpp_sources_pdf)
file(GLOB xournalpp_sources_pdf
        pdf/base/*.cpp
        pdf/base/*.h
        )
list(APPEND xournalpp_sources ${xournalpp_sources_pdf})

unset(xournalpp_sources_pdf)
file(GLOB xournalpp_sources_pdf pdf/popplerapi/*.cpp pdf/popplerapi/*.h)
list(APPEND xournalpp_sources ${xournalpp_sources_pdf})

set(xournalpp_include_dirs "${CMAKE_CURRENT_SOURCE_DIR}")

add_library(xournalpp-core STATIC ${xournalpp_sources})
target_compile_features(xournalpp-core PUBLIC ${PROJECT_CXX_FEATURES})
target_link_libraries(xournalpp-core
        PRIVATE xoj::util ${xournalpp_LDFLAGS}
        PUBLIC xoj::stacktrace xoj::defaults)
target_include_directories(xournalpp-core PRIVATE)
target_include_directories(xournalpp-core PRIVATE ${xournalpp_include_dirs})
add_library(xoj::core ALIAS xournalpp-core)
