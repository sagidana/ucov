# UCOV
This is a userspace version of the kcov implementation of the linux kernel

This is the command to create a file for my vim JumpList and Mark plugins

```shell
cat /tmp/coverage |
uniq |
sed -n 1,10000p |
addr2line -f -p -i -e lib.so |
awk '{print $3 ":" $1}' > /tmp/quickfix.cov
```
- the `sed -n 1,10000p` command is there to limit the PCs for the vim quickfix
  list which for some reason cannot handle more entries..


