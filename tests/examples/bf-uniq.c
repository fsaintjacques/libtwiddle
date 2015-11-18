#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <twiddle/bloomfilter/bloomfilter.h>

static struct option long_options[] = {
  {"probability", required_argument, 0, 'p'},
  {"count"      , required_argument, 0, 'n'},
  {0            , 0                , 0,  0 }
};


int parse_probability(const char *opt, float *p) {
  const float parsed_p = strtof(optarg, NULL);
  if (!(0 < parsed_p && parsed_p <= 1)) {
    return -1;
  }

  *p = parsed_p;

  return 0;
}


int parse_count(const char *opt, int64_t *n) {
  const int64_t parsed_n = strtoll(optarg, NULL, 10);
  if (!(0 <= parsed_n && parsed_n <= TW_BITMAP_MAX_BITS)) {
    return -1;
  }

  *n = parsed_n;

  return 0;
}

static
int
parse_arguments(int argc, char **argv, int64_t *n, float *p)
{
  int c = 0;
  int ret = 0;

  while (1) {
    int option_index = 0;

    c = getopt_long(argc, argv, "n:p:", long_options, &option_index);
    if (c == -1)
        break;

    switch (c) {
    case 'p':
      if ((ret = parse_probability(optarg, p)) != 0) {
        return ret;
      }
      break;
    case 'n':
      if ((ret = parse_count(optarg, n)) != 0) {
        return ret;
      }
      break;
    default:
      printf("?? getopt returned character code 0%o ??\n", c);
      return -1;
    }
  }

  return 0;
}

int
main(int argc, char *argv[])
{
  int64_t n = 1000000;
  float   p = 0.0001;

  if (parse_arguments(argc, argv, &n, &p) != 0) {
    exit(-1);
  }

  const uint64_t m = tw_bloomfilter_optimal_m(n, p);
  const uint16_t k = tw_bloomfilter_optimal_k(n, m);

  /* parse options */

  struct tw_bloomfilter *bf = tw_bloomfilter_new(m, k);

  if (!bf) {
    exit(1);
  }

  char   *line     = NULL;
  size_t  buf_len  = 0;
  ssize_t line_len = 0;

  while ((line_len = getline(&line, &buf_len, stdin)) != -1) {

    if (!tw_bloomfilter_test(bf, line_len, line)) {
      fprintf(stdout, "%s", line);
      tw_bloomfilter_set(bf, line_len, line);
    }

  }


  tw_bloomfilter_free(bf);

  return 0;
}
