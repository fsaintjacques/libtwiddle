  $ (echo 'a'; echo 'a'; echo 'b'; echo 'c'; echo 'b'; echo 'aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa'; echo 'a') | bf-uniq
  a
  b
  c
  aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

  $ (echo "a"; sleep 2; echo "a") | bf-uniq -d 1s
  a
  a
