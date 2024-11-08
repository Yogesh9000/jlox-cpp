add_executable(generate_ast generate_ast.cpp lexer.cpp)
target_link_libraries(generate_ast PRIVATE project_settings)
set_target_properties(generate_ast PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

add_executable(main main.cpp lexer.cpp)
target_link_libraries(main PRIVATE project_settings)
set_target_properties(main PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
