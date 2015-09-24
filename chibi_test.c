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
    chibi_suite_run_silently(suite, &summary);
    chibi_assert(summary.num_runs == 0);
    chibi_assert(summary.num_pass == 0);
    chibi_assert(summary.num_failures == 0);
    chibi_suite_delete(suite);
}

/* This test is meant to be run from within TestFail() */
CHIBI_TEST(DummyFail) { chibi_fail("just fail"); }
CHIBI_TEST(DummySuccess) { }

struct FixtureData {
    int setupCalled;
    int teardownCalled;
    int reset_me;
};
void setup1(struct FixtureData *data) {
    data->setupCalled++;
    data->reset_me++;
}
void teardown1(struct FixtureData *data) {
    data->teardownCalled++;
    data->reset_me--;
}

CHIBI_TEST(DependsOnSetup)
{
    struct FixtureData *data = (struct FixtureData *) TC_USERDATA;
    chibi_assert(data->setupCalled);
}

CHIBI_TEST(TestFixture)
{
    chibi_suite *suite;
    struct FixtureData data;
    data.setupCalled = 0;
    data.teardownCalled = 0;
    data.reset_me = 0;

    suite = chibi_suite_new_fixture((chibi_fixfunc) setup1, (chibi_fixfunc) teardown1,
                                    &data);
    /* add twice to check that */
    chibi_suite_add_test(suite, DependsOnSetup);
    chibi_suite_add_test(suite, DependsOnSetup);
    chibi_suite_run_silently(suite, NULL);
    chibi_assert_eq_int(2, data.setupCalled);
    chibi_assert_eq_int(2, data.teardownCalled);
    chibi_assert_eq_int(0, data.reset_me);
    chibi_suite_delete(suite);
}

CHIBI_TEST(Test_fail)
{
    chibi_summary_data summary;
    chibi_suite *suite = chibi_suite_new();
    chibi_suite_add_test(suite, DummyFail);
    chibi_suite_run_silently(suite, &summary);
    chibi_assert(summary.num_failures == 1);
    chibi_suite_delete(suite);
}

/*
 * Tests chibi_assert_eq_int()
 */
CHIBI_TEST(eq_int_fail) { chibi_assert_eq_int(2, 1); }
CHIBI_TEST(eq_int_success) { chibi_assert_eq_int(2, 2); }
CHIBI_TEST(Test_assert_eq_int)
{
    chibi_suite *suite = chibi_suite_new();
    chibi_suite_add_test(suite, eq_int_success);
    chibi_suite_add_test(suite, eq_int_fail);
    chibi_suite_run_silently(suite, NULL);
    chibi_assert(suite->head->success);
    chibi_assert(!suite->head->next->success);
    chibi_suite_delete(suite);
}

/*
 * Tests chibi_assert_eq_cstr()
 */
CHIBI_TEST(eq_cstr_fail) { chibi_assert_eq_cstr("hello", "bye"); }
CHIBI_TEST(eq_cstr_success) { chibi_assert_eq_cstr("hello", "hello"); }
CHIBI_TEST(Test_assert_eq_cstr)
{
    chibi_suite *suite = chibi_suite_new();
    chibi_suite_add_test(suite, eq_cstr_success);
    chibi_suite_add_test(suite, eq_cstr_fail);
    chibi_suite_run_silently(suite, NULL);
    chibi_assert(suite->head->success);
    chibi_assert(!suite->head->next->success);
    chibi_suite_delete(suite);
}

CHIBI_TEST(Test_nested_suites)
{
    chibi_suite *toplevel, *child1, *child2;
    struct FixtureData data1, data2;
    data1.setupCalled = 0;
    data1.teardownCalled = 0;
    data2.setupCalled = 0;
    data2.teardownCalled = 0;

    toplevel = chibi_suite_new();
    child1 = chibi_suite_new_fixture((chibi_fixfunc) setup1, (chibi_fixfunc) teardown1, &data1);
    child2 = chibi_suite_new_fixture((chibi_fixfunc) setup1, (chibi_fixfunc) teardown1, &data2);
    chibi_suite_add_test(child1, DependsOnSetup);
    chibi_suite_add_test(child2, DependsOnSetup);

    chibi_suite_add_suite(toplevel, child1);
    chibi_suite_add_suite(toplevel, child2);

    chibi_suite_run_silently(toplevel, NULL);

    chibi_assert(data1.setupCalled);
    chibi_assert(data1.teardownCalled);
    chibi_assert(data2.setupCalled);
    chibi_assert(data2.teardownCalled);

    chibi_suite_delete(toplevel);
}

int main(int argc, char **argv)
{
    chibi_summary_data summary;
    chibi_suite *suite = chibi_suite_new();

    chibi_suite_add_test(suite, TestMakeSuite);
    chibi_suite_add_test(suite, TestRunNothing);
    chibi_suite_add_test(suite, TestFixture);
    chibi_suite_add_test(suite, Test_fail);
    chibi_suite_add_test(suite, Test_assert_eq_int);
    chibi_suite_add_test(suite, Test_assert_eq_cstr);
    chibi_suite_add_test(suite, Test_nested_suites);

    chibi_suite_run(suite, &summary);

    chibi_suite_delete(suite);
    return summary.num_failures;
}
