add_library(
        lib
        city.cpp
        city.h
        interface.cpp
        interface.h
        weather.h
        weather.cpp
)

target_link_libraries(lib PRIVATE cpr::cpr)
target_link_libraries(lib PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(lib PRIVATE ncurses)