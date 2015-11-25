Adding a new data structure
===========================

Assume you want to add a new data structure with shortname `ds` in group
`ds_group`.

Creating a new data strucute involves mulitple steps:

  * add header file in `include/ds_group/ds.h`
  * add implementation file in `src/ds_group/ds.c`
  * update `src/CMakeLists.txt` and add reference to previous file

Afterward, one must implement tests.

  * create unit test in `tests/test-ds.c`
  * update `tests/CMakeLists.txt` and add reference to previous file
  * create usage example in `tests/examples/example-ds.c`
  * update `tests/examples/CMakeLists.txt` and add reference to previous file

Finally, python bindings and tests must be added.

  * update python-c bindings in `python/twiddle/c.py`
  * add python class in `python/twiddle/ds.py`
  * add fuzzing test in `python/tests/test_ds.py`
