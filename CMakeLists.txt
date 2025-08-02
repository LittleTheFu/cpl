cmake_minimum_required(VERSION 3.20)

project(my_compiler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(ASIO_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third/asio)
include_directories(${ASIO_ROOT_DIR}/include)

set(JSON_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third/json)
include_directories(${JSON_ROOT_DIR}/include)

set(PROJECT_SOURCES
    front/alphaBet.cpp
    front/lexer.cpp
    front/token.cpp
    front/productionRule.cpp
    front/grammarSymbol.cpp
    front/grammar.cpp
    front/lrItem.cpp
    front/lrParserGenerator.cpp
    front/lrState.cpp
    front/predefineSymbol.cpp
    front/parser.cpp

    ir/irInstruction.cpp
    ir/irProgram.cpp
    ir/vmCodeGenerator.cpp

    regluarExpression/regEx.cpp
    regluarExpression/regExParser.cpp
    regluarExpression/regExNode.cpp
    regluarExpression/regExCharNode.cpp
    regluarExpression/regExCharSetNode.cpp
    regluarExpression/regExAlternationNode.cpp
    regluarExpression/regExConcatenationNode.cpp
    regluarExpression/regExKleeneStarNode.cpp
    regluarExpression/regExPlusNode.cpp
    regluarExpression/regExOptionalNode.cpp

    ast/astNode.cpp
    ast/expressionNode.cpp
    ast/binaryOpNode.cpp
    ast/identifierNode.cpp
    ast/intergerLiteralNode.cpp

    state/dfaState.cpp
    state/dfaStateMachine.cpp
    state/nfaState.cpp
    state/nfaStateFragment.cpp

    vm/virtualMachine.cpp
    vm/instruction.cpp

    network/socket_server_module.cpp

    main.cpp
)

add_library(cpl_lib STATIC ${PROJECT_SOURCES})

target_include_directories(cpl_lib PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/regluarExpression
    ${PROJECT_SOURCE_DIR}/ast
    ${PROJECT_SOURCE_DIR}/state
    ${PROJECT_SOURCE_DIR}/vm
    ${PROJECT_SOURCE_DIR}/front
    ${PROJECT_SOURCE_DIR}/ir
)

add_executable(cpl main.cpp)
target_link_libraries(cpl PRIVATE cpl_lib)

# -------------------------------------------------------------------
add_subdirectory(${PROJECT_SOURCE_DIR}/third/googletest)

add_executable(cpl_tests
    test/test.cpp
    # test/testVM.cpp
    # test/testRegEx.cpp
    # test/testRegExParser.cpp
    # test/testFirstSet.cpp
    # test/testFollowSet.cpp
    # test/testLRParserGenerator.cpp
    # test/testParser.cpp
    #  test/testSimpleParser.cpp
)

target_include_directories(cpl_tests PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/regluarExpression
    ${PROJECT_SOURCE_DIR}/state
    ${PROJECT_SOURCE_DIR}/test
)

target_link_libraries(cpl_tests
    GTest::gtest_main
    cpl_lib          
    ws2_32
    Iphlpapi
)

include(CTest)
enable_testing()
add_test(NAME cpl_tests COMMAND cpl_tests)