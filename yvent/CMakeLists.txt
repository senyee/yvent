set(SOURCE_FILES
        EventLoop.cpp
        Poll.cpp
        Channel.cpp
        Acceptor.cpp
        InetAddr.cpp
        Socket.cpp
        TcpServer.cpp
        TcpConnection.cpp
        )

add_library(yvent STATIC ${SOURCE_FILES})
target_link_libraries(yvent pthread)

install(TARGETS yvent DESTINATION lib)
set(HEADERS
        noncopyable.h
        Logging.h
        EventLoop.h
        Poll.h
        Channel.h
        Acceptor.h
        InetAddr.h
        Socket.h
        TcpServer.h
        Callbacks.h
        TcpConnection.h
        )
install(FILES ${HEADERS} DESTINATION include)