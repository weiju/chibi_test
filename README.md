# chibitest.c - A lightweight C Unit testing framework [![Build Status](https://travis-ci.org/weiju/chibi_test.svg)](https://travis-ci.org/weiju/chibi_test)

## Description

chibitest.c is a C unit testing framework that aims to be simple
to use, flexible, portable and without dependencies apart from the C standard
library for its core functionality.

## Features

  * small, simple, easy integration
  * no global state
  * tests itself
  * supports TAP protocol etc. for automake
  * supports setup() and teardown()
  * nested test suites to support grouping
  * can generate JUnit compatible XML test reports

## System Requirements

While primary developed on a GNU/Linux system, the minimum system requirement is
an AmigaOS 1.x system with 512 KB RAM (using VBCC), and it should run on
any modern computer system with a standards compliant C compiler.

## Usage

Copy chibi.h and chibi.c to your project source and use as part of
your project. For more information, consult the project [wiki](https://github.com/weiju/chibi_test/wiki).

### Example

```C
#include "chibi.h"

CHIBI_TEST(MyTest)
{
  chibi_assert(1 == 1);
}

int main(int argc, char **argv)
{
  chibi_suite *suite = chibi_suite_new("MySuite");
  chibi_suite_add_test(suite, MyTest);
  chibi_suite_run(suite, NULL);
  chibi_suite_delete(suite);
  return 0;
}
```

## History and Motivation

I decided to write my own unit test framework, mostly because I wanted
to have something that doesn't depend on any other libraries and is
simple and small, yet flexible enough to be used in a variety of project
setups.

Since a lot of my C development is also targetting AmigaOS compatible systems,
I wanted to ensure that the framework will enable programmers to do
test-first C development on a classic Amiga if they so wish.

