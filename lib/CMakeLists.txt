set (tempTest ${test})
set (test OFF)

add_subdirectory(yacss-parser)
add_subdirectory(yahtml-parser)
add_subdirectory(yahttp-client)

set(test ${tempTest})

