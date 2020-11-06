  

# When adding more tests, list them in GASS_TEST_SOURCES
set(GASS_TEST_SOURCES
	TestMain.cpp
	Core/Test_EnumLookup.cpp
	Core/Test_FilePath.cpp
	Core/Test_FilePath_optional.cpp
	Core/Test_GenericFactory.cpp
	Core/Test_StringUtils.cpp
	#Core/Test_Enum.cpp
	Core/Test_Math.cpp
	Core/Test_Vector.cpp
	Core/Test_Matrix.cpp
	Core/Test_Plane.cpp
	Core/Test_AABox.cpp
	Core/Test_LineSegment.cpp
	Core/Test_RTTI.cpp
	Core/Test_Logger.cpp
	Core/Test_Quaternion.cpp
	Core/Test_NameGenerator.cpp
	#Core/TestReflection_Property.cpp
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

