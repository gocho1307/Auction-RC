#ifndef __CONSTANTS_HPP__
#define __CONSTANTS_HPP__

#define PROMPT "> "

#define DEFAULT_AS_HOST "tejo.tecnico.ulisboa.pt"
#define DEFAULT_AS_PORT "58011" // 58075

#define BUFFER_SIZE (512 + 1)

#define MIN_UID (100000)
#define MAX_UID (999999)
#define PASSWORD_LEN (8)

#define TCP_READ_TIMEOUT_SECS (10)
#define TCP_WRITE_TIMEOUT_SECS (10 * 60) // 10 minutes

#endif // __CONSTANTS_HPP__
