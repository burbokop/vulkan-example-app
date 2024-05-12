function(add_shader_target target)
  cmake_parse_arguments(PARSE_ARGV 1 arg "" "OUTPUT_DIR" "SOURCES")

  if(NOT DEFINED arg_OUTPUT_DIR)
    message("error" FATAL_ERROR)
  endif()

  file(MAKE_DIRECTORY ${arg_OUTPUT_DIR})
  set(ext spv)

  set(output_file_paths)
  foreach(source_file_path ${arg_SOURCES})

    get_filename_component(source_file_name ${source_file_path} NAME)
    get_filename_component(
      output_file_path ${arg_OUTPUT_DIR}/${source_file_name}.${ext} ABSOLUTE)

    message(
      "source_file_path: ${source_file_path}, output_file_path: ${output_file_path}"
    )

    add_custom_command(
      DEPENDS ${source_file_path}
      COMMAND glslc ${source_file_path} -o ${output_file_path}
      OUTPUT ${output_file_path})

    list(APPEND output_file_paths ${output_file_path})

  endforeach()

  message("output_file_paths: ${output_file_paths}")

  add_custom_target(
    ${target}
    SOURCES ${arg_SOURCES}
    DEPENDS ${output_file_paths})

endfunction()
