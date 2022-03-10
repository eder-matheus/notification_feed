project(client)

add_executable(client
	../app/app_client.cpp
	client.cpp
	ui.cpp)

target_include_directories(client
	PUBLIC
	../../include/client
	../../include/util)

install(TARGETS client RUNTIME DESTINATION bin)