  $ echo $'a\na\nb\nc\nb\naaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\na' | bf-uniq
  a
  b
  c
  aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

  $ (echo "a"; sleep 2; echo "a") | bf-uniq -d 1s
  a
  a
