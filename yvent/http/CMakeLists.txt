set(SOURCE_FILES
        HttpRequest.cpp
        HttpContext.cpp
        HttpResponse.cpp
        HttpServer.cpp
        )


add_library(yhttp STATIC ${SOURCE_FILES})
target_link_libraries(yhttp)

install(TARGETS yhttp DESTINATION lib)
set(HEADERS
        HttpRequest.h
        HttpContext.h
        HttpResponse.h
        HttpServer.h
        )
install(FILES ${HEADERS} DESTINATION include/http)
