file(GLOB examples_sources "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
list(REMOVE_ITEM examples_sources ${CMAKE_CURRENT_SOURCE_DIR}/all.cpp)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/all.cpp
    COMMAND sed '/int main/,/^}/d' ${examples_sources} >
        ${CMAKE_CURRENT_BINARY_DIR}/all.cpp
    COMMAND cat ${CMAKE_CURRENT_SOURCE_DIR}/all.cpp.in |
        python ${CMAKE_CURRENT_SOURCE_DIR}/make-all.py >>
        ${CMAKE_CURRENT_BINARY_DIR}/all.cpp
)

list(APPEND examples_sources ${CMAKE_CURRENT_BINARY_DIR}/all.cpp)

add_custom_command(OUTPUT ${local_headers}
    COMMAND ${CMAKE_COMMAND}
    -E copy_directory ${PROJECT_SOURCE_DIR}/src/ ${local_headers_dir}
    DEPENDS ${PROJECT_SOURCE_DIR}/src/*.hpp)

foreach(example_path ${examples_sources})
    include_directories(${local_include_dir})
    get_filename_component(example ${example_path} NAME_WE)
    add_executable(${example}.wt ${example_path} ${local_headers})
    target_link_libraries(${example}.wt
        wtclasses
        wthttp
    )
    install(FILES ${example_path} DESTINATION lib/libwtclasses/examples)
    install(TARGETS ${example}.wt DESTINATION lib/libwtclasses/examples)
endforeach()

