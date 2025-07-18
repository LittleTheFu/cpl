cmake_minimum_required(VERSION 3.20)

project(my_compiler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(PROJECT_SOURCES
    main.cpp
    dfaState.cpp
    dfaStateMachine.cpp
    nfaState.cpp
    nfaStateFragment.cpp
    regEx.cpp
    regExNode.cpp
    regExAlternationNode.cpp
    regExConcatenationNode.cpp
    regExKleeneStarNode.cpp
)

add_library(cpl_lib STATIC ${PROJECT_SOURCES})
target_include_directories(cpl_lib PUBLIC ${PROJECT_SOURCE_DIR}) # 假设头文件和源文件在同一目录

add_executable(cpl main.cpp) # 只包含 main.cpp，因为其他代码在库中
target_link_libraries(cpl PRIVATE cpl_lib)

# -------------------------------------------------------------------
add_subdirectory(${PROJECT_SOURCE_DIR}/third/googletest)

add_executable(cpl_tests
    test/test.cpp
)

target_include_directories(cpl_tests PRIVATE ${PROJECT_SOURCE_DIR})

target_link_libraries(cpl_tests
    GTest::gtest_main
    cpl_lib
)

include(CTest)
enable_testing()
add_test(NAME cpl_tests COMMAND cpl_tests)
