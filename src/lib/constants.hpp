#ifndef __CONSTANTS_HPP__
#define __CONSTANTS_HPP__

#define PROMPT "> "

#define DEFAULT_AS_HOST "localhost"
#define DEFAULT_AS_PORT "58075"

#define FILE_BUFFER_SIZE (1024)
#define MAX_FILE_SIZE (10000000)
#define MAX_FILE_SIZE_DIGS (8)
#define MAX_FILE_NAME_LEN (24)
#define FILE_EXTENSION_LEN (3)

#define PACKET_ID_LEN (3)
#define MAX_STATUS_LEN (3)
#define UID_LEN (6)
#define PASSWORD_LEN (8)
#define MAX_AUCTIONS (999)
#define AID_LEN (3)
#define MAX_AUCTION_NAME_LEN (10)
#define MAX_VAL (999999)
#define MAX_VAL_DIGS (6)
#define MAX_DURATION (99999)
#define MAX_DURATION_DIGS (5)
#define CAL_DATE_LEN (10)
#define TIME_DATE_LEN (8)
#define MAX_BIDS_LISTINGS (50)

#define READ_TIMEOUT_SECS (15)
#define WRITE_TIMEOUT_SECS (10 * 60) // 10 minutes
#define MAX_TCP_QUEUE (5)
#define MAX_TCP_CONNS (50)

#endif // __CONSTANTS_HPP__
