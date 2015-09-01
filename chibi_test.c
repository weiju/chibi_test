#include <stdio.h>

#include "chibi.h"

CHIBI_TEST(TestMakeSuite)
{
  chibi_suite *suite = chibi_suite_new();
  chibi_assert_not_null(suite);
  chibi_assert(suite->head == NULL);
  chibi_suite_delete(suite);
}

CHIBI_TEST(TestRunNothing)
{
  chibi_suite *suite = chibi_suite_new();
  chibi_summary_data summary;
  chibi_suite_run_silently(suite);
  chibi_suite_summary_data(suite, &summary);
  chibi_assert(summary.num_runs == 0);
  chibi_assert(summary.num_pass == 0);
  chibi_assert(summary.num_failures == 0);
  chibi_suite_delete(suite);
}


/* This test is meant to be run from within TestFail() */
CHIBI_TEST(DummyFail) {
  chibi_fail("just fail");
}

CHIBI_TEST(TestFail)
{
  chibi_summary_data summary;
  chibi_suite *suite = chibi_suite_new();
  chibi_suite_add_test(suite, DummyFail);
  chibi_suite_run_silently(suite);
  chibi_suite_summary_data(suite, &summary);
  chibi_assert(summary.num_failures == 1);
  chibi_suite_delete(suite);
}

int main(int argc, char **argv)
{
  chibi_suite *suite = chibi_suite_new();
  chibi_suite_add_test(suite, TestMakeSuite);
  chibi_suite_add_test(suite, TestRunNothing);
  chibi_suite_add_test(suite, TestFail);
  chibi_suite_run(suite);
  chibi_suite_summary(suite);
  chibi_suite_delete(suite);
  return 1;
}
