include_directories(${INCLUDES})
message(MY_INCS ${INCLUDES})

add_executable(main main.cc)

add_library(yabrowserlib StyleTree.cc Layout.cc)
target_link_libraries(yabrowserlib
  ${yahtml-parser_LIBS}
  ${yacss-parser_LIBS}
  ${yahttp-client_LIBS}
)

set(LIBS "yabrowserlib" PARENT_SCOPE)

