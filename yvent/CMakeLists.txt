set(SOURCE_FILES
        EventLoop.cpp
        Poll.cpp
        Channel.cpp
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
        )
install(FILES ${HEADERS} DESTINATION include)