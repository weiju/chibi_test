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
CHIBI_TEST(DummySuccess) { }

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

struct FixtureData {
  int setupCalled;
  int teardownCalled;
};
void setup1(struct FixtureData *data) { data->setupCalled = 1; }
void teardown1(struct FixtureData *data) { data->teardownCalled = 1; }

CHIBI_TEST(TestFixture)
{
  chibi_suite *suite;
  struct FixtureData data;
  data.setupCalled = 0;
  data.teardownCalled = 0;

  suite = chibi_suite_new_fixture((chibi_fixfunc) setup1, (chibi_fixfunc) teardown1,
                                  &data);
  chibi_suite_add_test(suite, DummySuccess);
  chibi_suite_run_silently(suite);
  chibi_assert(data.setupCalled);
  chibi_assert(data.teardownCalled);
  chibi_suite_delete(suite);
}

/*
 * Tests chibi_assert_eq_int()
 */
CHIBI_TEST(eq_int_fail) { chibi_assert_eq_int(2, 1); }
CHIBI_TEST(eq_int_success) { chibi_assert_eq_int(2, 2); }
CHIBI_TEST(Test_assert_eq_int)
{
  chibi_summary_data summary;
  chibi_suite *suite = chibi_suite_new();
  chibi_suite_add_test(suite, eq_int_success);
  chibi_suite_add_test(suite, eq_int_fail);
  chibi_suite_run_silently(suite);
  chibi_assert(suite->head->success);
  chibi_assert(!suite->head->next->success);
  chibi_suite_delete(suite);
}

int main(int argc, char **argv)
{
  chibi_suite *suite = chibi_suite_new();
  chibi_suite_add_test(suite, TestMakeSuite);
  chibi_suite_add_test(suite, TestRunNothing);
  chibi_suite_add_test(suite, TestFail);
  chibi_suite_add_test(suite, TestFixture);
  chibi_suite_add_test(suite, Test_assert_eq_int);

  chibi_suite_run(suite);
  chibi_suite_summary(suite);
  chibi_suite_delete(suite);
  return 1;
}
