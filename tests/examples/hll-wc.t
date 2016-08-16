  $ yes uuidgen | head -500 | xargs -L1 bash -c | sort -u > uuids
  $ cat uuids | wc -l
  500
  $ echo $(((500 - $(cat uuids uuids uuids | hll-wc)) / 10))
  0
  $ (echo '1'; echo '2'; echo '3'; echo '1'; echo '2'; echo '3') | hll-wc -s
  1
  2
  3
  3
  3
  3
