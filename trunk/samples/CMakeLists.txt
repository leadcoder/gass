#Add dependencies
find_package(FindGASSSharedLibraries)
FILE(COPY ${ALL_BIN_FILES_RELEASE}  DESTINATION  ${CMAKE_BINARY_DIR}/out/release) 

ADD_SUBDIRECTORY( sim_demo)
#ADD_SUBDIRECTORY( physics)
#ADD_SUBDIRECTORY( vehicle_demo)

