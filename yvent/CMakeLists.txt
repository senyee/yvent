set(SOURCE_FILES
        EventLoop.cpp EventLoop.h
        )

add_library(yvent STATIC ${SOURCE_FILES})
target_link_libraries(yvent pthread)

install(TARGETS yvent DESTINATION lib)
set(HEADERS
        EventLoop.h
        Logging.h
        )
install(FILES ${HEADERS} DESTINATION include)