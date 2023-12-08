#!/bin/sh
# This test script assumes port forwarding on port "MY_PORT"

MY_IP=$(curl -s http://ipecho.net/plain)
MY_PORT=58075
TEJO="nc tejo.tecnico.ulisboa.pt 59000"

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # stands for No Color

# Create the tmp folder if it doesn't exist, and clear it
mkdir -p tmp
rm -rf tmp/*

if [ ! -s ../AS ]; then
    echo "Make sure to compile your program first!"
    exit 1
fi

# Actual Script Core ---------------------------------------------------------------

echo "You can find the result of the test in the tmp folder."
echo ""

../AS -v -p $MY_PORT > tmp/server.log &

echo "Testing $1..."
echo "$MY_IP $MY_PORT $1" | $TEJO > tmp/report-"$1".html
if [ -s tmp/report-"$1".html ] || grep -q "color=\"red\"" tmp/report-"$1".html; then
    echo -e "${RED}Failed Test${NC}"
else
    echo -e "${GREEN}Passed Test${NC}"
fi

killall AS
