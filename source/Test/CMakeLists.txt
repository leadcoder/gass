ENABLE_TESTING(true)  

# When adding more tests, list them in GASS_TEST_SOURCES
set(GASS_TEST_SOURCES 
	${PROJECT_SOURCE_DIR}/source/Test/TestMain.cpp
	${PROJECT_SOURCE_DIR}/source/Test/Core/TestUtils_FilePath.cpp
	#${PROJECT_SOURCE_DIR}/source/Test/Core/TestUtils_FilePath_Optional.cpp
	${PROJECT_SOURCE_DIR}/source/Test/Core/TestUtils_StringUtils.cpp
)

create_source_group("Test Files" "${CMAKE_CURRENT_SOURCE_DIR}" ${GASS_TEST_SOURCES})

# Setup catch test framework, which only consist of one header file.
add_library(Catch INTERFACE)
set(CATCH_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/source/Test/catch CACHE INTERNAL "Path to include folder for Catch")
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

add_executable(GassTest ${GASS_TEST_SOURCES})
target_link_libraries(GassTest Catch GASSCore)

add_test(NAME TestBase COMMAND GassTest)

