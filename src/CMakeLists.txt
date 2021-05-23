set (CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/autd3/client/include ${EIGEN_PATH})

add_library(blas_backend STATIC
  ${PROJECT_SOURCE_DIR}/src/blas_backend.cpp
  ${PROJECT_SOURCE_DIR}/include/blas_backend.hpp
)

include_directories(${BLAS_INCLUDE_DIR})
target_link_directories(blas_backend PUBLIC ${BLAS_DEPEND_LIB_DIR})
target_link_libraries(blas_backend ${BLAS_LIBRARIES})
if(NOT WIN32)
target_link_libraries(blas_backend gfortran)
endif()

set_target_properties(blas_backend
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

if(WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4190")
else()
  set_target_properties(blas_backend PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
