#ifndef __CONSTANTS_HPP__
#define __CONSTANTS_HPP__

#define PROMPT "> "

#define DEFAULT_AS_HOST "tejo.tecnico.ulisboa.pt" // localhost
#define DEFAULT_AS_PORT "58011"                   // 58075

#define BUFFER_SIZE (256)
#define BUFFER_FILE_SIZE (1024)
#define MAX_FILE_SIZE (10000000)

#define MIN_UID (100000)
#define MAX_UID (999999)
#define PASSWORD_LEN (8)
#define MAX_NAME_LEN (10)
#define MAX_START_VAL (999999)
#define MAX_DURATION (99999)
#define MAX_AID (999)
#define DATE_TIME_LEN (19)
#define MAX_BID_TIME (999999)

#define TCP_READ_TIMEOUT_SECS (10)
#define TCP_WRITE_TIMEOUT_SECS (10 * 60) // 10 minutes

#endif // __CONSTANTS_HPP__
