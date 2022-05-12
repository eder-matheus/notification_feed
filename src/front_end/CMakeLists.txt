project(front_end)

add_executable(front_end
	../app/app_front_end.cpp
	../util/common.cpp
	../util/notification.cpp
	../util/ui.cpp
	front_end.cpp
)

target_include_directories(front_end
	PUBLIC
	../../include/front_end
	../../include/util
)

target_link_libraries(front_end
	pthread
)

install(TARGETS front_end RUNTIME DESTINATION bin)