  

# When adding more tests, list them in GASS_TEST_SOURCES
set(GASS_TEST_SOURCES 
	TestMain.cpp
	Core/TestUtils_EnumLookup.cpp
	Core/TestUtils_FilePath.cpp
	Core/TestUtils_FilePath_optional.cpp
	Core/TestUtils_StringUtils.cpp
	Core/TestUtils_Math.cpp
	Core/TestUtils_Vector.cpp
	Core/TestUtils_Matrix.cpp
	Core/TestUtils_Plane.cpp
	Core/TestUtils_RTTI.cpp
	Core/TestUtils_Logger.cpp
)

set(GASS_MATH_TEST_SOURCES 
	Core/TestUtils_Vector.cpp
)

#create_source_group("Test Files" "${CMAKE_CURRENT_SOURCE_DIR}" ${GASS_TEST_SOURCES})

# Setup catch test framework, which only consist of one header file.
add_library(Catch INTERFACE)
set(CATCH_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/source/Test/catch CACHE INTERNAL "Path to include folder for Catch")
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})
add_executable(GassTest ${GASS_TEST_SOURCES})
target_link_libraries(GassTest Catch GASSCore)
add_test(NAME TestBase COMMAND GassTest)

#add_library(Catch INTERFACE)
#target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})
#add_executable(GassMathTest ${GASS_MATH_TEST_SOURCES})
#target_link_libraries(GassMathTest Catch GASSCore)
#add_test(NAME MathTest COMMAND GassMathTest)

