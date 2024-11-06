add_executable(main main.cpp lexer.cpp)
target_link_libraries(main PRIVATE project_settings)
set_target_properties(main PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
