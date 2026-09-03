cmake_minimum_required(VERSION 3.20)
project(Logging CXX)

set(Logging_public_headers
	Logger.h
	SinkConfig.h
	LoggerMacros.h
)

set(Logging_private_headers
	detail/LogUtils.h
	detail/SinkFactory.h
	detail/YamlLoggerConfigReader.h
)

set(Logging_source
	detail/Logger.cpp
	detail/LogUtils.cpp
	detail/SinkFactory.cpp
	detail/YamlLoggerConfigReader.cpp
)

add_library(Logging STATIC ${Logging_source} ${Logging_public_headers} ${Logging_private_headers})
target_include_directories(Logging PUBLIC ${CMAKE_SOURCE_DIR})

if(TARGET spdlog::spdlog_header_only)
	set(LOGGING_SPDLOG_TARGET spdlog::spdlog_header_only)
	message(STATUS "spdlog::spdlog_header_only target found!")
elseif(TARGET spdlog::spdlog)
	message(WARNING "spdlog::spdlog_header_only target not found; falling back to spdlog::spdlog")
	set(LOGGING_SPDLOG_TARGET spdlog::spdlog)
else()
	message(FATAL_ERROR "No usable spdlog target found")
endif()

target_link_libraries(Logging PUBLIC ${LOGGING_SPDLOG_TARGET} yaml-cpp)