Formatting
==========

`clang-format` enforces a standardize C code style. You can find a git
pre-commit hook in `tools/git/hook-clang-format` that will ensure every source
code file respect the format.

Tests infrastructure
====================

libtwiddle is backed by standard unit checks implemented with the `check`
library. Minimal python bindings exists to test more thoroughly with the
property testing library [hypothesis].

[hypothesis](https://github.com/DRMacIver/hypothesis)


In order to develop, it is recommanded to use the `Debug` release type as it
will enable symbols for debugging.

```
$ mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug ..
$ make clean all test
...
```

To tests with `hypothesis`, one must prepare a virtualenv and install
dependencies. This is only needed once.

```
$ # setup virtualenv for pip
$ cd python && virtualenv .venv && source .venv/bin/activate
$ # install dependencies
$ pip install pytest hypothesis==1.19.0
$ # run property tests
$ PYTHONPATH=. py.test tests
```

Debugging
---------

`check` forks every test, thus making it hard to debug. To tackle this,
before going into gdb, you can call the following `export CK_FORK=no`
and `check` will stop forking.

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
