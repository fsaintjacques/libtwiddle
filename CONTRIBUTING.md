Formatting
==========

`clang-format` enforces a standard style for C code. You can find a git
pre-commit hook in `tools/git/hook-clang-format` that will ensure every source
code file respects the style.

Test infrastructure
===================

libtwiddle is backed by standard unit checks implemented with the Check
library. Minimal Python bindings exist to test more thoroughly with the
property-testing library [Hypothesis](https://github.com/DRMacIver/hypothesis).


When developing, we recommend using the `Debug` release type, as it
will enable debugging symbols:

```
$ mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug ..
$ make clean
$ make all
$ make test
...
```

One can also invoke the test suite with the `tools/travis/test` script which
should create a grid of build directories: `build-{portable,avx,avx2}` and run
the tests on each build directory. It should automagicaly detect CPU support for
instruction sets required.

To test with Hypothesis, one must prepare a virtualenv and install
dependencies. This is only needed once.

```
$ # setup virtualenv for pip
$ cd python && virtualenv .venv && source .venv/bin/activate
$ # install dependencies
$ pip install pytest hypothesis==1.19.0
$ # run property tests
$ PYTHONPATH=. py.test tests
```

One can also invoke the travis test script to run integration test:
`RUN_HYPOTHESIS=true tools/travis/test`.

Travis
------

Travis will try to build and run the test infrastructure on each commit.
To fasten up feedback, integration tests are only run on pull-request branches
from github.

Debugging
---------

Check forks for each test, making it hard to debug. To work around this,
before entering gdb, you can do `export CK_FORK=no`
and Check will stop forking.


Adding a new data structure
===========================

Assume you want to add a new data structure with shortname `ds` in group
`ds_group`.

Creating a new data structure involves the following steps:

  * add header file `include/ds_group/ds.h`
  * add implementation file `src/ds_group/ds.c`
  * update `src/CMakeLists.txt` and add reference to previous file

Then, one must implement tests:

  * create unit test in `tests/test-ds.c`
  * update `tests/CMakeLists.txt` and add reference to previous file
  * create usage example in `tests/examples/example-ds.c`
  * update `tests/examples/CMakeLists.txt` and add reference to previous file

Finally, Python bindings and tests must be added:

  * update python-c bindings in `python/twiddle/c.py`
  * add Python class in `python/twiddle/ds.py`
  * add fuzzing test in `python/tests/test_ds.py`
