cmake_minimum_required(VERSION 3.2)
set(CMAKE_SYSTEM_VERSION 7.0 CACHE TYPE INTERNAL FORCE)

set(AppName main)
project(${AppName})

include(ExternalProject)

set(AppSourcesDir src)
set(AppHeadersDir include)
set(WebDriverXxDir ../include)

# deps headers
#set(CurlIncludeDirs $ENV{CURL_PATH}/include)

aux_source_directory(${AppSourcesDir} AppSourcesList)
file(GLOB_RECURSE AppHeadersList "include/*.h")

list(REMOVE_ITEM AppSourcesList src/stdafx.cpp)

add_msvc_precompiled_header("stdafx.h" "src/stdafx.cpp" AppSourcesList)
source_group("Precompiled Headers" FILES include/stdafx.h src/stdafx.cpp)

configure_msvc_runtime()

add_executable(
	${AppName} 
	${AppSourcesList}
	${AppHeadersList}
	${AppFormsList} 
	${AppResourcesList}
	${AppFormsHeadersList}
	${AppHeadersMocList}
	${AppResourcesListAdded}
)

if(MSVC)
	add_definitions(
		/EHsc
		/MP
		/std:c++latest
		/Zi
		/W4
		/WX
		-D_CRT_SECURE_NO_WARNINGS
		-D_SCL_SECURE_NO_WARNINGS
	)
	
	set_target_properties(
		${AppName}
		PROPERTIES COMPILE_FLAGS "/FI\"stdafx.h\""
	)
	
	set_target_properties(
		${AppName} 
		PROPERTIES
		LINK_FLAGS "/DEBUG /ENTRY:mainCRTStartup /MAP"
	)
endif()

#picojson
externalproject_add(picojson_project
	PREFIX picojson
	GIT_REPOSITORY https://github.com/kazuho/picojson
	GIT_TAG 806c5740e2c673189fee7e53acc0429af0b24691
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
	INSTALL_COMMAND ""
	UPDATE_COMMAND ""
)
	
externalproject_get_property(picojson_project source_dir)
include_directories("${source_dir}")
list(APPEND DEPS picojson_project)

# curl
externalproject_add(curl_project
	URL D:/curl-7.29.0.tar.gz
	PREFIX curl
	CMAKE_ARGS -DCURL_STATICLIB=ON -DBUILD_SHARED_LIBS=OFF -DBUILD_CURL_EXE=OFF -DBUILD_CURL_TESTS=OFF -DCMAKE_USE_OPENSSL=OFF -DCURL_ZLIB=OFF -DHTTP_ONLY=ON
	INSTALL_COMMAND ""
	UPDATE_COMMAND ""
)

externalproject_get_property(curl_project source_dir binary_dir)
include_directories("${source_dir}/include")
link_directories("${binary_dir}/lib")
list(APPEND LIBS libcurl wsock32 ws2_32)
list(APPEND DEPS curl_project)
add_definitions(-DCURL_STATICLIB)

include_directories(
	${AppHeadersDir}
	${WebDriverXxDir}
)

add_dependencies(${AppName} ${DEPS})

target_link_libraries(
	${AppName}
	${LIBS}
)