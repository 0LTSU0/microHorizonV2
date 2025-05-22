add_executable(microHorizonApp main.cpp
							   configurator.h
							   configurator.cpp)
							   
target_link_libraries(microHorizonApp
					  Tracer
				      posInputWorker
					  osmLoadWorker
					  horizonWorker) 
					  
if (UNIX)
	#TODO: check why linux build fails wihtout link to atomic
	target_link_libraries(microHorizonApp atomic)
endif()

target_include_directories(microHorizonApp PUBLIC
						   "${CMAKE_CURRENT_SOURCE_DIR}"
						   "${CMAKE_CURRENT_SOURCE_DIR}/../../ThirdParty/nlohmann_json-3.11.3")
					  