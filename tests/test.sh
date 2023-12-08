#!/bin/sh
# This test script assumes port forwarding on port "MY_PORT"

MY_IP=$(curl -s http://ipecho.net/plain)
MY_PORT=58075
NET_CAT="nc tejo.tecnico.ulisboa.pt 59000"

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # stands for No Color

# Create the tmp folder if it doesn't exist, and clear it
mkdir -p tmp
rm -rf tmp/*

# Actual Script Core -------------------------------------------------------------------

usage() {
    echo "Usage: $0 [-h] [<script_num>]"
    echo "  -h help - shows this message"
}

if [ ! -s ../AS ]; then
    echo "ERR: Make sure to run 'make' on the root project directory."
    usage
    exit 1
fi

while getopts ":h" OPTION; do
    case "$OPTION" in
        h)
            usage
            exit 0
            ;;
        *)
            echo "ERR: Unknown option."
            usage
            exit 1
            ;;
    esac
done
shift "$(( OPTIND - 1 ))"

if  [ "$#" -lt 1 ]; then
    echo "ERR: Wrong number of arguments. Expected at least one."
    usage
    exit 1
fi

echo "You can find the result of the test in the tmp folder."
echo ""

if [ "$(pidof -x AS | wc -l)" -eq 0 ]; then
    ../AS -v -p $MY_PORT > tmp/server.log &
fi

printf "$MY_IP $MY_PORT %s\n" "$1" | $NET_CAT > tmp/report-"$1".html
if [ -s tmp/report-"$1".html ] || grep -q "color=\"red\"" tmp/report-"$1".html; then
    echo -e "$1: ${RED}Failed Test${NC}"
    echo "Server log:"
    cat tmp/server.log
else
    echo -e "$1: ${GREEN}Passed Test${NC}"
fi

killall AS
