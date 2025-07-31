cmake_minimum_required(VERSION 3.20)

project(my_compiler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

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

    main.cpp
)

add_library(cpl_lib STATIC ${PROJECT_SOURCES})

target_include_directories(cpl_lib PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/regluarExpression
    ${PROJECT_SOURCE_DIR}/ast
    ${PROJECT_SOURCE_DIR}/state
    ${PROJECT_SOURCE_DIR}/vm
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
)

include(CTest)
enable_testing()
add_test(NAME cpl_tests COMMAND cpl_tests)