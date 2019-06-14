#!/bin/sh

function killport {
    if [ ! -n "$1" ] || [ $1 == '--help' ] || [ $1 == '-h' ]
    then
        echo '`killport <PORT>` finds the process listening to the specified port and kills it.'
    else
        process_line=`lsof -i :$1 | tail -1`
        if [ "$process_line" == "" ]
        then
            echo "no processes listening on $1"
        else
            process_name=`echo "$process_line" | awk '{print $1}'`
            echo "killing $process_name"
            kill `echo "$process_line" | awk '{print $2}'`
        fi
    fi
}

killport 11752 && \
killport 12752 && \
killport 13752 && \
killport 14752
