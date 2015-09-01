#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chibi.h"

chibi_suite *chibi_suite_new()
{
  chibi_suite *result = malloc(sizeof(chibi_suite));
  result->head = NULL;
  return result;
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

void _chibi_suite_run(chibi_suite *suite, int verbose)
{
  chibi_testcase *testcase = suite->head;
  while (testcase) {
    testcase->fun(testcase);
    if (verbose) {
      if (testcase->success) fprintf(stderr, ".");
      else fprintf(stderr, "F");
    }
    testcase = testcase->next;
  }
  if (verbose) fprintf(stderr, "\n");
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
char *assemble_message(const char *msg, const char *srcfile, const char *funname, int line)
{
  char *msgbuffer = malloc(strlen(msg) + strlen(srcfile) + strlen(funname) + 12 + 8);
  sprintf(msgbuffer, "%s:%d - %s() - %s", srcfile, line, funname, msg);
  return msgbuffer;
}

char *assemble_message2(const char *msg1, const char *msg2, const char *srcfile, const char *funname, int line)
{
  char *msgbuffer = malloc(strlen(msg1) + strlen(msg2) + strlen(srcfile) + strlen(funname) + 12 + 10);
  sprintf(msgbuffer, "%s:%d - %s() - %s %s", srcfile, line, funname, msg1, msg2);
  return msgbuffer;
}


void _chibi_assert_not_null(chibi_testcase *tc, void *ptr, const char *msg, const char *srcfile, int line)
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

void _chibi_assert(chibi_testcase *tc, int cond, const char *cond_str, const char *msg, const char *srcfile, int line)
{
  if (!cond) {
    tc->error_msg = assemble_message2(msg, cond_str, srcfile, tc->fname, line);
    tc->success = 0;
  }
}
