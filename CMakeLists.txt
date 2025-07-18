cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(MySimpleProject
    main.cpp
    dfaState.cpp
    dfaStateMachine.cpp
    nfaState.cpp)
