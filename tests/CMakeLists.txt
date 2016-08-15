add_subdirectory(check)

add_c_test(test-bitmap)
add_c_test(test-bitmap-rle)
add_c_test(test-bloomfilter)
add_c_test(test-bloomfilter-a2)
add_c_test(test-hyperloglog)
add_c_test(test-minhash)

add_subdirectory(benchmarks)
add_subdirectory(examples)
