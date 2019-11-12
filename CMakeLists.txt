cmake_minimum_required (VERSION 2.8)
project(jcsu-client CXX)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(SRC_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/src/client.cc
		${CMAKE_CURRENT_SOURCE_DIR}/src/client-builder.cc
		${CMAKE_CURRENT_SOURCE_DIR}/src/latest-version.cc
		${CMAKE_CURRENT_SOURCE_DIR}/src/file-list.cc
		${CMAKE_CURRENT_SOURCE_DIR}/src/file-download.cc
		${CMAKE_CURRENT_SOURCE_DIR}/src/sign-header-impl.h
		${CMAKE_CURRENT_SOURCE_DIR}/src/sign-header-impl.cc
		${CMAKE_CURRENT_SOURCE_DIR}/include/jcsu/client.h
		${CMAKE_CURRENT_SOURCE_DIR}/include/jcsu/client-handler.h
		${CMAKE_CURRENT_SOURCE_DIR}/include/jcsu/latest-version.h
		${CMAKE_CURRENT_SOURCE_DIR}/include/jcsu/file-item.h
		${CMAKE_CURRENT_SOURCE_DIR}/include/jcsu/file-download.h
		${CMAKE_CURRENT_SOURCE_DIR}/include/jcsu/file-list.h
		${CMAKE_CURRENT_SOURCE_DIR}/include/jcsu/jcsu-intl/uniobj.h
		${CMAKE_CURRENT_SOURCE_DIR}/include/jcsu/jcsu-intl/identity.h
)
add_library(${PROJECT_NAME} ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

target_link_libraries(${PROJECT_NAME} jcu-file)
target_link_libraries(${PROJECT_NAME} jcu-http)
target_link_libraries(${PROJECT_NAME} jsoncpp_lib)
target_link_libraries(${PROJECT_NAME} josepp-static)
