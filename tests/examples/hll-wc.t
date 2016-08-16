  $ (echo '1'; echo '2'; echo '3'; echo '1'; echo '2'; echo '3') | hll-wc
  3

  $ (echo '1'; echo '2'; echo '3'; echo '1'; echo '2'; echo '3') | hll-wc -s
  1
  2
  3
  3
  3
  3
