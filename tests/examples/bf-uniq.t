  $ yes uuidgen | head -500 | xargs -L1 bash -c | sort > uuids
  $ uniq uuids > uuids.uniq
  $ cat uuids | bf-uniq > uuids.bf-uniq
  $ diff uuids.uniq uuids.bf-uniq
  $ (echo "a"; sleep 2; echo "a") | bf-uniq -d 1s
  a
  a
