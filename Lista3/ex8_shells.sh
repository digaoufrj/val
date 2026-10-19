#!/bin/sh

cut -d: -f7 /etc/passwd | sort | uniq

exit 0
