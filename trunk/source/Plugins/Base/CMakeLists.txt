include_directories ("${BOOST_DIR}"  
	"${PROJECT_SOURCE_DIR}/Source" 
	"${GASS_DEPENDENCIES_DIR}/tinyxml"
	"${GASS_DEPENDENCIES_DIR}/tbb/include"
)

file(GLOB_RECURSE CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE H_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
create_source_group("Source Files"  "${CMAKE_CURRENT_SOURCE_DIR}" ${CPP_FILES})
create_source_group("Header Files"  "${CMAKE_CURRENT_SOURCE_DIR}" ${H_FILES})

link_directories   ("${BOOST_DIR}/lib/"
					"${GASS_DEPENDENCIES_DIR}/lib/")

add_library (GASSBasePlugin SHARED  ${CPP_FILES} ${H_FILES})

set(LINK_LIBRARY optimized tinyxml debug tinyxml_d)
target_link_libraries(GASSBasePlugin GASSSim GASSCore ${LINK_LIBRARY})

install (TARGETS GASSBasePlugin DESTINATION lib)		
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION include/plugins FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)

