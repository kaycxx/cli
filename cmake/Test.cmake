option(BUILD_TESTING "Build unit tests" ON)
if(NOT BUILD_TESTING)
    return()
endif()

include("${CMAKE_CURRENT_LIST_DIR}/GitDeps.cmake")

git_require(kaycxx::test 0.0.2)

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    test/*.cpp
)
list(SORT TEST_SOURCES)

add_executable(${NAME}-tests
    ${TEST_SOURCES}
)
target_link_libraries(${NAME}-tests PRIVATE
    ${NAMESPACE}::${NAME}
    ${NAMESPACE}::test
)

set(TEST_DISCOVERY_FILE "${CMAKE_CURRENT_BINARY_DIR}/${NAME}-tests.cmake")
file(WRITE "${TEST_DISCOVERY_FILE}" "")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake" "include(\"${TEST_DISCOVERY_FILE}\" OPTIONAL)\n")

add_custom_command(TARGET ${NAME}-tests POST_BUILD
    COMMAND ${NAME}-tests --write-ctest "${TEST_DISCOVERY_FILE}" "$<TARGET_FILE:${NAME}-tests>"
    COMMENT "Discovering ${PACKAGE_NAME} tests"
    VERBATIM
)

add_custom_target(test
    COMMAND ${NAME}-tests
    DEPENDS ${NAME}-tests
    USES_TERMINAL
)

add_custom_target(ctest
    COMMAND ${CMAKE_CTEST_COMMAND} --test-dir "${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS ${NAME}-tests
    USES_TERMINAL
)
