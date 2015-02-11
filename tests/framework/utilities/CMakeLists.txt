include_directories(include)

if(UNIX)
  set(CXX_FLAGS "${CXX_FLAGS} -include ${ODATACPP_INCLUDE_DIR}/compat/linux_compat.h")
endif()

add_library(${LIB}utilities
  os_utilities.cpp
  )
  
target_link_libraries(${LIB}utilities
  ${LIB}unittestpp
  )
