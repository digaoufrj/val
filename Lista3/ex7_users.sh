#!/bin/sh

cut -d: -f1,5 /etc/passwd | tr ":" "\t"

exit 0
