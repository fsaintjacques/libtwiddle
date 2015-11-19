#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <twiddle/hyperloglog/hyperloglog.h>

static struct option long_options[] = {
  {"precision", required_argument, 0, 'p'},
  {"stream",    no_argument,       0, 's'},
  {0          , 0,                 0,  0 }
};


int parse_precision(const char *opt, uint8_t *p) {
  const int64_t parsed_p = strtol(optarg, NULL, 10);
  if (!(TW_HLL_MIN_PRECISION < parsed_p &&
        parsed_p <= TW_HLL_MAX_PRECISION)) {
    return -1;
  }

  *p = parsed_p;

  return 0;
}

static
int
parse_arguments(int argc, char **argv, uint8_t *p, bool *stream)
{
  int c = 0;
  int ret = 0;

  while (1) {
    int option_index = 0;

    c = getopt_long(argc, argv, "p:s", long_options, &option_index);
    if (c == -1)
        break;

    switch (c) {
    case 'p':
      if ((ret = parse_precision(optarg, p)) != 0) {
        return ret;
      }
      break;
    case 's':
      *stream = true;
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
  uint8_t p = 16;
  bool stream = false;

  if (parse_arguments(argc, argv, &p, &stream) != 0) {
    exit(-1);
  }

  struct tw_hyperloglog *hll = tw_hyperloglog_new(p);

  if (!hll) {
    exit(1);
  }

  char   *line     = NULL;
  size_t  buf_len  = 0;
  ssize_t line_len = 0;

  while ((line_len = getline(&line, &buf_len, stdin)) != -1) {
    tw_hyperloglog_add(hll, line_len, line);
    if (stream) {
      fprintf(stdout, "%" PRIu64 "\n", (uint64_t) tw_hyperloglog_count(hll));
    }
  }

  if (!stream) {
    fprintf(stdout, "%" PRIu64 "\n", (uint64_t) tw_hyperloglog_count(hll));
  }

  free(line);
  tw_hyperloglog_free(hll);

  return 0;
}
