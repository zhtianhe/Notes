./redis-cli  -a test123 --cluster create \
  10.10.10.76:6371 \
  10.10.10.76:6372 \
  10.10.10.76:6373 \
  10.10.10.76:6374 \
  10.10.10.76:6375 \
  10.10.10.76:6376 \
  10.10.10.76:6377 \
  10.10.10.76:6378 \
  10.10.10.76:6379 \
 --cluster-replicas  2
