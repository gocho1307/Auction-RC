#ifndef __CONSTANTS_HPP__
#define __CONSTANTS_HPP__

#define PROMPT "> "

#define DEFAULT_AS_HOST "tejo.tecnico.ulisboa.pt" // localhost
#define DEFAULT_AS_PORT "58011"                   // 58075

#define FILE_BUFFER_SIZE (1024)
#define MAX_FILE_SIZE (10000000)

#define PACKET_ID_LEN (3)
#define MAX_STATUS_LEN (3)
#define UID_LEN (6)
#define PASSWORD_LEN (8)
#define AID_LEN (3)
#define MAX_NAME_LEN (10)
#define MAX_START_VAL (999999)
#define MAX_DURATION (99999)
#define CAL_DATE_LEN (10)
#define TIME_DATE_LEN (8)
#define MAX_BID_TIME (999999)
#define MAX_FILE_NAME_LEN (24)

#define TCP_READ_TIMEOUT_SECS (10)
#define TCP_WRITE_TIMEOUT_SECS (10 * 60) // 10 minutes

// Same as in-class exercise, don't know if we should change
#define MAX_TCP_QUEUE (5)

#endif // __CONSTANTS_HPP__
