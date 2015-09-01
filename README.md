# ChibiTest - A C Unit testing framework

## Description and Philosophy

ChibiTest is a C unit testing framework that aims to be simple
to use, portable and without dependencies apart from the C standard
library for its core functionality (3rd party depdencies will
be required for optional features like use within Continuous Integration
systems, e.g. XML/JSON output).

While developed on a GNU/Linux system, the minimum system requirement is
an AmigaOS 1.x system with 512 KB RAM (using VBCC), and it should run on
any modern computer system with a standards compliant C compiler.

The way the system is integrated in a project is inspired by CuTest -
simply put a header and C source code file into your project.

## Usage

Copy chibi.h and chibi.c to your project source and use as part of
your project.

### Example

```C
#include "chibi.h"

CHIBI_TEST(MyTest)
{
  chibi_assert(1 == 1);
}

int main(int argc, char **argv)
{
  chibi_suite *suite = chibi_suite_new();
  chibi_suite_add_test(suite, MyTest);
  chibi_suite_run(suite);
  chibi_suite_summary(suite);
  chibi_suite_delete(suite);
  return 1;
}
```

## History and Motivation

After researching and evaluating a couple of C unit testing frameworks
for a while, I decided to write my own, mostly because I wanted
to have something that doesn't depend on any other libraries and is
simple and small.

Since a lot of my C development happens on an AmigaOS compatible system,
a unit testing framework that runs equally well on a classic Motorola 68k
system as on an x64, ARM or PPC GNU/Linux system.

In addition there is some room for experimentation.
