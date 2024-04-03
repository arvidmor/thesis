#!/bin/bash

# This script compares two files in hexadecimal format and outputs the lines in the new file that differ from the first. 
# Example: 
# file1:    0000000 0000 0000 0000 0000 0000 0000 0000 0000
#           0000010 0000 0000 0000 0000 1111 0000 0000 0000

# file2:    0000000 0000 0000 0000 0000 0000 0000 0000 0000
#           0000010 0000 0000 0000 0000 2222 0000 0000 0000

# outfile:  0000010 0000 0000 0000 0000 2222 0000 0000 0000

# Usage: ./xdiff.sh oldFile newFile outfile
diff <(hexdump -x $1) <(hexdump -x $2) \
| awk '/---/{flag=1;next} flag {gsub(/    /, " "); print}' \
| cut -c 3- > $3