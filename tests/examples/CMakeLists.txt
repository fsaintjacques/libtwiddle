add_c_test(example-bitmap)
add_c_test(example-bitmap-rle)
add_c_test(example-bloomfilter)
add_c_test(example-bloomfilter-a2)
add_c_test(example-hyperloglog)

add_c_example(bf-uniq)
add_c_example(hll-wc)

find_package(PythonInterp)

if (PYTHON_EXECUTABLE)
    file(GLOB_RECURSE TESTS "${CMAKE_CURRENT_SOURCE_DIR}/*.t")
    foreach(TEST ${TESTS})
        get_filename_component(TEST_NAME "${TEST}" NAME_WE)
        add_test(
            ${TEST_NAME}
            ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR}
            ${CMAKE_BINARY_DIR}/../tools/cram/ccram
                --python ${PYTHON_EXECUTABLE}
                --root  ${CMAKE_CURRENT_SOURCE_DIR}
                --build ${CMAKE_BINARY_DIR}/tests/examples
                --tests ${TEST}
        )
    endforeach(TEST)
else (PYTHON_EXECUTABLE)
    message(WARNING "Unable to find Python; skipping cram tests.")
endif (PYTHON_EXECUTABLE)
