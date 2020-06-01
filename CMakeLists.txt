cmake_minimum_required(VERSION 3.5)
project(mlog_prj LANGUAGES CXX)
add_definitions("-Wall -g")
add_definitions(-std=c++11)
add_library(mlog
	STATIC
		src/Log.cc
		src/Log.h
		src/LogAsync.cc
		src/LogAsync.h
		src/LogCommon.cc
		src/LogCommon.h
		src/LogFile.cc
		src/LogFile.h
		src/LogStream.cc
		src/LogStream.h
		src/LogThread.cc
		src/LogThread.h
		src/LogTimestamp.cc
		src/LogTimestamp.h
	)

add_executable(demo example/demo.cc)
target_link_libraries(demo mlog)
#target_link_libraries(demo rt)
target_link_libraries(demo pthread)
