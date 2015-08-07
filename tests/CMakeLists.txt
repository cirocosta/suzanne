include_directories(${yabrowser_INCLUDES})

add_executable(styletree_test styletree_test.cc)
add_executable(stylednode_test stylednode_test.cc)
add_executable(layout_test layout_test.cc)

target_link_libraries(styletree_test gtest gtest_main ${yabrowser_LIBS})
target_link_libraries(stylednode_test gtest gtest_main ${yabrowser_LIBS})
target_link_libraries(layout_test gtest gtest_main ${yabrowser_LIBS})

add_test(NAME styletree_test COMMAND styletree_test)
add_test(NAME stylednode_test COMMAND stylednode_test)
add_test(NAME layout_test COMMAND layout_test)

