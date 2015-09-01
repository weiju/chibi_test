#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chibi.h"

extern chibi_suite *chibi_suite_new_fixture(chibi_fixfunc setup,
                                            chibi_fixfunc teardown,
                                            void *userdata)
{
  chibi_suite *result = malloc(sizeof(chibi_suite));
  result->head = NULL;
  result->setup = setup;
  result->teardown = teardown;
  result->userdata = userdata;
  return result;
}

chibi_suite *chibi_suite_new()
{
  return chibi_suite_new_fixture(NULL, NULL, NULL);
}

void chibi_suite_delete(chibi_suite *suite)
{
  if (suite) {
    struct _chibi_testcase *tc = suite->head, *tmp;
    while (tc) {
      tmp = tc->next;
      if (tc->error_msg) free(tc->error_msg);
      free(tc);
      tc = tmp;
    }
    free(suite);
  }
}

void _chibi_suite_add_test(chibi_suite *suite, chibi_testfunc fun, const char *fname)
{
  struct _chibi_testcase *tc, *newtc;
  newtc = malloc(sizeof(struct _chibi_testcase));
  newtc->fun = fun;
  newtc->fname = fname;
  newtc->next = NULL;
  newtc->success = 1;
  newtc->error_msg = NULL;

  if (!suite->head) suite->head = newtc;
  else {
    tc = suite->head;
    while (tc->next) tc = tc->next;
    tc->next = newtc;
  }
}

void chibi_suite_summary(chibi_suite *suite)
{
  int num_tests = 0, num_failures = 0, i = 1;
  chibi_testcase *tc = suite->head;

  /* First pass: counting */
  while (tc) {
    num_tests++;
    if (!tc->success) {
      num_failures++;
    }
    tc = tc->next;
  }

  fprintf(stderr, "\nSummary\n");
  fprintf(stderr, "-------\n");

  /* Second pass: messages */
  if (num_failures > 0) {
    fprintf(stderr, "# of failures: %d\n\n", num_failures);
    tc = suite->head;
    while (tc) {
      if (!tc->success) {
        fprintf(stderr, "%d. %s\n", i, tc->error_msg);
        i++;
      }
      tc = tc->next;
    }
    fprintf(stderr, "\n");
  }
  fprintf(stderr, "Runs: %d Pass: %d Fail: %d\n\n", num_tests, num_tests - num_failures, num_failures);
}

void chibi_suite_summary_data(chibi_suite *suite, chibi_summary_data *summary)
{
  if (suite && summary) {
    chibi_testcase *tc = suite->head;

    summary->num_runs = 0;
    summary->num_failures = 0;

    while (tc) {
      summary->num_runs++;
      if (!tc->success) {
        summary->num_failures++;
      }
      tc = tc->next;
    }
    summary->num_pass = summary->num_runs - summary->num_failures;
  }
}

/*
 *  Reused by all assertions to generate a standard format error message.
 */
static char *assemble_message(const char *msg, const char *srcfile,
                              const char *funname, int line)
{
  char *msgbuffer = malloc(strlen(msg) + strlen(srcfile) + strlen(funname) + 12 + 8);
  sprintf(msgbuffer, "%s:%d - %s() - %s", srcfile, line, funname, msg);
  return msgbuffer;
}

static char *assemble_message2(const char *msg1, const char *msg2,
                               const char *srcfile, const char *funname,
                               int line)
{
  char *msgbuffer = malloc(strlen(msg1) + strlen(msg2) + strlen(srcfile) + strlen(funname) + 12 + 10);
  sprintf(msgbuffer, "%s:%d - %s() - %s %s", srcfile, line, funname, msg1, msg2);
  return msgbuffer;
}


void _chibi_assert_not_null(chibi_testcase *tc, void *ptr, const char *msg, const char *srcfile,
                            int line)
{
  if (ptr == NULL) {
    tc->error_msg = assemble_message(msg, srcfile, tc->fname, line);
    tc->success = 0;
  }
}

void _chibi_fail(chibi_testcase *tc, const char *msg, const char *srcfile, int line)
{
  tc->error_msg = assemble_message(msg, srcfile, tc->fname, line);
  tc->success = 0;
}

void _chibi_assert(chibi_testcase *tc, int cond, const char *cond_str, const char *msg,
                   const char *srcfile, int line)
{
  if (!cond) {
    tc->error_msg = assemble_message2(msg, cond_str, srcfile, tc->fname, line);
    tc->success = 0;
  }
}

/**********************************************************************
 *
 * TEST RUNNERS
 *
 **********************************************************************/

/*
 * Generic runner. Supports fixtures and report function customization.
 * If the suite was defined with setup and/or teardown functions, those
 * are run on the optional userdata object.
 * The report_num_tests(int), report_success(int, chibi_testcase *) and
 * report_fail(int, chibi_testcase *) functions are used to support
 * different output protocols (e.g. for reporting the success/failure of
 * tests while they are run).
 */
static void _chibi_suite_run(chibi_suite *suite, int verbose, void (*report_num_tests)(int),
                             void (*report_success)(int, chibi_testcase *),
                             void (*report_fail)(int, chibi_testcase *))
{
  if (suite) {
    chibi_testcase *testcase = suite->head;
    int num_tests = 0, i = 0;

    if (suite->setup) suite->setup(suite->userdata);

    /* count the tests */
    while (testcase) {
      num_tests++;
      testcase = testcase->next;
    }
    report_num_tests(num_tests);

    /* rewind and run */
    testcase = suite->head;    
    while (testcase) {
      testcase->fun(testcase);
      if (verbose) {
        if (testcase->success) report_success(i, testcase);
        else report_fail(i, testcase);
      }
      testcase = testcase->next;
      i++;
    }
    if (verbose) fprintf(stderr, "\n");
    if (suite->teardown) suite->teardown(suite->userdata);
  }
}

/*
 * Standard Runner
 */
static void report_num_tests_std(int num_tests) { }
static void report_success_std(int testnum, chibi_testcase *testcase) { fprintf(stderr, "."); }
static void report_fail_std(int testnum, chibi_testcase *testcase) { fprintf(stderr, "F"); }

void chibi_suite_run(chibi_suite *suite)
{
  _chibi_suite_run(suite, 1, report_num_tests_std, report_success_std, report_fail_std);
}

void chibi_suite_run_silently(chibi_suite *suite)
{
  _chibi_suite_run(suite, 0, report_num_tests_std, report_success_std, report_fail_std);
}


/*
 * TAP Runner
 */
static void report_num_tests_tap(int num_tests) { fprintf(stdout, "1..%d\n", num_tests); }
static void report_success_tap(int testnum, chibi_testcase *testcase)
{
  fprintf(stdout, "ok %d - %s\n", testnum + 1, testcase->fname);
}
static void report_fail_tap(int testnum, chibi_testcase *testcase)
{
  fprintf(stdout, "not ok %d - %s\n", testnum + 1, testcase->fname);
}

void chibi_suite_run_tap(chibi_suite *suite)
{
  _chibi_suite_run(suite, 1, report_num_tests_tap, report_success_tap, report_fail_tap);  
}

