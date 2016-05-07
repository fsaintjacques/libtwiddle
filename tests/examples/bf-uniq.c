#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "../../src/twiddle/macrology.h"
#include <twiddle/bloomfilter/bloomfilter.h>

static struct option long_options[] = {
    {"probability", required_argument, 0, 'p'},
    {"count", required_argument, 0, 'n'},
    {"duration", required_argument, 0, 'd'},
    {0, 0, 0, 0}};

static int parse_probability(float *p)
{
  const float parsed_p = strtof(optarg, NULL);
  if (!(0 < parsed_p && parsed_p <= 1)) {
    return false;
  }

  *p = parsed_p;

  return true;
}

static bool parse_count(int64_t *n)
{
  const int64_t parsed_n = strtoll(optarg, NULL, 10);
  if (parsed_n < 0) {
    return false;
  }

  *n = parsed_n;

  return true;
}

static bool apply_time_suffix(float *x, char suffix_char)
{
  int multiplier;

  switch (suffix_char) {
  case 0:
  case 's':
    multiplier = 1;
    break;
  case 'm':
    multiplier = 60;
    break;
  case 'h':
    multiplier = 60 * 60;
    break;
  case 'd':
    multiplier = 60 * 60 * 24;
    break;
  default:
    return false;
  }

  *x *= multiplier;

  return true;
}

static bool parse_duration(const char *str, float *d)
{
  char *ep;
  float duration = strtof(str, &ep);

  if (!(0 <= duration)
      /* No extra chars after the number and an optional s,m,h,d char.  */
      || (*ep && *(ep + 1))
      /* Check any suffix char and update timeout based on the suffix.  */
      || !apply_time_suffix(&duration, *ep)) {
    return false;
  }

  *d = duration;

  return true;
}

static int parse_arguments(int argc, char **argv, int64_t *n, float *p,
                           float *d)
{
  int c = 0;

  while (1) {
    int option_index = 0;

    c = getopt_long(argc, argv, "n:p:d:", long_options, &option_index);
    if (c == -1)
      break;

    switch (c) {
    case 'n':
      if (!parse_count(n)) {
        return -1;
      }
      break;
    case 'p':
      if (!parse_probability(p)) {
        return -1;
      }
      break;
    case 'd':
      if (!parse_duration(optarg, d)) {
        return -1;
      }
      break;
    default:
      printf("?? getopt returned character code 0%o ??\n", c);
      return -1;
    }
  }

  return 0;
}

#ifdef __APPLE__
#include <sys/time.h>
#define CLOCK_MONOTONIC 0
int clock_gettime(int __attribute__((unused)) clk_id, struct timespec *t)
{
  struct timeval now;
  int rv = gettimeofday(&now, NULL);
  if (rv)
    return rv;
  t->tv_sec = now.tv_sec;
  t->tv_nsec = now.tv_usec * 1000;
  return 0;
}
#endif

int main(int argc, char *argv[])
{
  int64_t n = 1000000;
  float p = 0.0001;
  float c = -1.0;

  if (parse_arguments(argc, argv, &n, &p, &c) != 0) {
    exit(-1);
  }

  const uint64_t m = tw_bloomfilter_optimal_m(n, p);
  const uint16_t k = tw_bloomfilter_optimal_k(n, m);

  /* parse options */

  struct tw_bloomfilter *bf = tw_bloomfilter_new(m, k);

  if (!bf) {
    exit(1);
  }

  char *line = NULL;
  size_t buf_len = 0;
  ssize_t line_len = 0;

  /* setup expire */
  struct timespec next_expire = {0, 0};
  clock_gettime(CLOCK_MONOTONIC, &next_expire);
  next_expire.tv_sec += c;

  while ((line_len = getline(&line, &buf_len, stdin)) != -1) {

    if (tw_unlikely(c > 0)) {
      struct timespec now = {0, 0};
      clock_gettime(CLOCK_MONOTONIC, &now);

      if (tw_unlikely(now.tv_sec >= next_expire.tv_sec)) {
        next_expire.tv_sec += c;
        tw_bloomfilter_zero(bf);
      }
    }

    if (!tw_bloomfilter_test(bf, line, line_len)) {
      fprintf(stdout, "%s", line);
      tw_bloomfilter_set(bf, line, line_len);
    }
  }

  free(line);

  tw_bloomfilter_free(bf);

  return 0;
}
