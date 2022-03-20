project(client)

add_executable(client
	../app/app_client.cpp
	../util/common.cpp
	../util/notification.cpp
	../util/ui.cpp
	client.cpp
)

target_include_directories(client
	PUBLIC
	../../include/client
	../../include/util
)

target_link_libraries(client
	pthread
)

install(TARGETS client RUNTIME DESTINATION bin)