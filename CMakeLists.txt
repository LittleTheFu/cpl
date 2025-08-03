cmake_minimum_required(VERSION 3.20)

project(my_compiler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# -- 设置第三方库路径 --
set(ASIO_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third/asio)
include_directories(${ASIO_ROOT_DIR}/include)

set(JSON_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third/json)
include_directories(${JSON_ROOT_DIR}/include)

# -- 定义核心编译器/VM 的源文件 (不包含 ffi_api.cpp 和 main.cpp) --
set(CORE_SOURCES
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
)

# -- 1. 创建核心静态库 --
# 这个库包含所有编译器和VM的内部实现
add_library(cpl_core STATIC ${CORE_SOURCES})

target_include_directories(cpl_core PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/regluarExpression
    ${PROJECT_SOURCE_DIR}/ast
    ${PROJECT_SOURCE_DIR}/state
    ${PROJECT_SOURCE_DIR}/vm
    ${PROJECT_SOURCE_DIR}/front
    ${PROJECT_SOURCE_DIR}/ir
    # 确保包含了 third party libs 的 include
    ${ASIO_ROOT_DIR}/include
    ${JSON_ROOT_DIR}/include
)

# -- 2. 为 Flutter FFI 创建共享库 --
# 这个库只包含 ffi_api.cpp，并链接 cpl_core 静态库
add_library(cpl_ffi_lib SHARED ffi_api.cpp)

# 设置共享库的输出目录
if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/dll") # 将 DLL 输出到项目根目录下的 'dll' 文件夹
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib") # 将 SO/DYLIB 输出到项目根目录下的 'lib' 文件夹
endif()

target_link_libraries(cpl_ffi_lib PRIVATE cpl_core) # 链接到核心静态库
# 如果你的 ffi_api.cpp 或其内部调用的核心代码需要这些系统库，请在这里添加
# target_link_libraries(cpl_ffi_lib PRIVATE ws2_32 Iphlpapi)

target_include_directories(cpl_ffi_lib PUBLIC
    ${PROJECT_SOURCE_DIR} # 包含所有模块的头文件路径，确保 ffi_api.cpp 可以找到它们
    ${PROJECT_SOURCE_DIR}/regluarExpression
    ${PROJECT_SOURCE_DIR}/ast
    ${PROJECT_SOURCE_DIR}/state
    ${PROJECT_SOURCE_DIR}/vm
    ${PROJECT_SOURCE_DIR}/front
    ${PROJECT_SOURCE_DIR}/ir
    ${ASIO_ROOT_DIR}/include
    ${JSON_ROOT_DIR}/include
)


# -- 3. 构建独立的 C++ 可执行文件 --
# cpl 可执行文件直接链接 cpl_core 静态库
add_executable(cpl main.cpp)
target_link_libraries(cpl PRIVATE cpl_core)
# 如果 main.cpp 需要这些系统库，也要加
# target_link_libraries(cpl PRIVATE ws2_32 Iphlpapi)

target_include_directories(cpl PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/regluarExpression
    ${PROJECT_SOURCE_DIR}/ast
    ${PROJECT_SOURCE_DIR}/state
    ${PROJECT_SOURCE_DIR}/vm
    ${PROJECT_SOURCE_DIR}/front
    ${PROJECT_SOURCE_DIR}/ir
    ${ASIO_ROOT_DIR}/include
    ${JSON_ROOT_DIR}/include
)

# -------------------------------------------------------------------
# -- GTest 测试部分 --
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
    # test/testSimpleParser.cpp
)

target_include_directories(cpl_tests PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/regluarExpression
    ${PROJECT_SOURCE_DIR}/state
    ${PROJECT_SOURCE_DIR}/test
)

target_link_libraries(cpl_tests
    GTest::gtest_main
    cpl_core # 测试也链接核心静态库
    ws2_32
    Iphlpapi
)

include(CTest)
enable_testing()
add_test(NAME cpl_tests COMMAND cpl_tests)