#!/bin/sh

echo "Cleaning Containers and config"
docker-compose down && rm -rf crypto-config/* artefacts/* *peer/data && docker rm $(docker ps -qa)
echo "Container and Config dealt with"

# Killing all chaincode then peers
function killport {
    if [ ! -n "$1" ] || [ $1 == '--help' ] || [ $1 == '-h' ]
    then
        echo '`killport <PORT>` finds the process listening to the specified port and kills it.'
    else
        process_line=`sudo lsof -i :$1 | tail -1`
        if [ "$process_line" == "" ]
        then
            echo "no processes listening on $1"
        else
            process_name=`echo "$process_line" | awk '{print $1}'`
            echo "killing $process_name"
            sudo kill `echo "$process_line" | awk '{print $2}'`
        fi
    fi
}

killport 11752 && \
killport 12752 && \
killport 13752 && \
killport 14752 &&
killport 11751 && \
killport 12751 && \
killport 13751 && \
killport 14751 &&
echo "Peers and Chaincodes murdered"
