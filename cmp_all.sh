#!/bin/sh

for id in {1..111}
do
    echo "*** Comparing $id ***";
    if [[ $(cat test/open_$id.txt | ./highway | diff test/open_$id.output.txt - | wc -c) -ne 0 ]]
    then
        echo "Error: $id";
        break;
    fi
done