# ------8<---[LINES TO MODIFY WHEN CHANGING MODULE]----->8-----
set(MODULE_UNDER_TEST websocket)
project(${MODULE_UNDER_TEST}_tests)
FILE(GLOB SRC src/WebSocketObserverTest.cpp
              src/BoostAsioTest.cpp
              src/WebSocketPPTest1_endpoint.cpp
              src/WebSocketPPTest2_server.cpp
              src/string2vectorTest.cpp
              )
# ------8<---------------------------------------------->8-----

# Include directories
include_directories(inc)
include_directories(${${MODULE_UNDER_TEST}_INCLUDE_DIRS})
include_directories(${SSC_INCLUDE_DIRS})
include_directories(SYSTEM ${GTEST_INCLUDE_DIRS})

add_library(${PROJECT_NAME} OBJECT ${SRC})

INCLUDE_DIRECTORIES(SYSTEM ${Boost_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(SYSTEM websocketpp)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++0x")

FOREACH(f #DemoWebSocket
          DemoWebSocketBug
          DemoWebSocketClient
          DemoWebSocketServerSingleClient
          # DemoWebSocketServerMultiClient
        )
    ADD_EXECUTABLE(${f}
        src/${f}.cpp)
    TARGET_LINK_LIBRARIES(${f} ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_RANDOM_LIBRARY} ${Boost_CHRONO_LIBRARY})
    IF(WIN32)
        TARGET_LINK_LIBRARIES(${f} wsock32 ws2_32)
    ELSE()
        TARGET_LINK_LIBRARIES(${f} pthread rt)
    ENDIF()
ENDFOREACH()

