project(server)

add_executable(server
	../app/app_server.cpp
	../util/common.cpp
	../util/notification.cpp
	../util/ui.cpp
	election.cpp
  server.cpp
	user.cpp
)

target_include_directories(server
	PUBLIC
	../../include/server
	../../include/util
)

target_link_libraries(server
	pthread
)

install(TARGETS server RUNTIME DESTINATION bin)
