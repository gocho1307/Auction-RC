# Auction Manager (Project RC 2023/2024)

Group 75:

- Gonçalo Bárias (ist1103124)
- Raquel Braunschweig (ist1102624)

## Compiling

To compile the project, navigate to the main directory and run `make`.
The project uses C++17.

Once compiled, two binaries, `user` and `AS` will be placed in the directory.

## Running the user

The options available for the `user` executable can be seen by running:

```
./user -h
```

The user program prompts for commands, displaying the list on startup and by typing `help` at any time. All commands adhere to the specifications.

The primary code responsible for user handling is located in the 'user' directory.

## User Directory

Here's a brief breakdown of the user directory:

1. **`user.cpp`**: Contains the main entry point for the user executable.

2. **`user_state.cpp`**: Has the socket operations, and packet communication
   for the system's user component.

3. **`commands.cpp`**: Implements the core functionality for handling user commands.

## Running the server

The options available for the `AS` executable can be seen by running:

```
./AS -h
```

The server organizes data in USERS and AUCTIONS directories that are very close to what was suggested by the teachers.

Some of the differences are that files that do not contain any information have no ".txt" extension, and the names
of sub directories are uppercase, while the names of files are lowercase.

Here we have an example of the USERS directory, the data inside the files have the same structure as the project guide:

```
USERS
├── 111111
│   ├── BIDDED
│   │   └── 020
│   ├── HOSTED
│   │   ├── 001
│   │   ...
│   │   └── 019
│   └── password.txt     <--- password of user, "aaaaaaaa"
...

```

Here is an example of the AUCTIONS directory:

```
AUCTIONS
├── 001
│   ├── ASSET
│   │   ├── A.txt        <--- asset file for the auction
│   │   └── name.txt     <--- name of asset file, "A.txt"
│   ├── BIDS
│   │   ├── highest.txt  <--- value of highest bid, "1"
│   │   └── list.txt     <--- each line contains "UID bid_value bid_datetime bid_sec_time"
│   ├── end.txt          <--- "2023-12-14 23:00:22 7200"
│   ├── start.txt        <--- "111111 One A.txt 1 2023-12-14 21:00:22 7200"
│   └── time.txt         <--- first line contains the full start time in seconds and the second line contains the duration in seconds
...

```

The server is designed to handle mistakes and errors effectively. It tries to fix issues when it can and shuts down gracefully if it can't.

The server responds to the SIGINT signal (CTRL + C) by waiting for ongoing TCP connections to complete. If the user presses CTRL + C again, it forcefully exits the server.

We opted for the select() system call to deal with multiple user connections at the same time.

The primary code responsible for server handling is located in the 'server' directory.

## Server Directory

Here's a brief breakdown of the server directory:

1. **`server.cpp`**: Contains the main entry point for the server executable.

2. **`server_state.cpp`**: Has the socket operations, and packet communication for the system's server component.

3. **`persistance.cpp`**: Handles file operations, such as adding a user to the USERS directory.

4. **`packets.cpp`**: Implements the core functionality for handling packets coming from users.

## Lib Directory

This directory has the files both the user and the server access.
Here's a brief breakdown of the lib directory:

1. **`constants.hpp`**: Contains all the constants used throughout the code.

2. **`messages.hpp`**: Contains all the messages displayed either to the user or the server.

3. **`protocol.cpp`**: Contains all the serialize and deserialize functions for the packets.

4. **`utils.cpp`**: Includes conversion, date formatting, and input validation, along with signal handler setup.

### Modifiable constants

Adjustable constants in `src/lib/constants.hpp` for testing include:

- `MAX_TCP_QUEUE`: Maximum number of TCP queued requests.
- `MAX_TCP_CONNS`: The number of maximum concurrent connections.
- `READ_TIMEOUT_SECONDS`: The read timeout (in seconds) for TCP connections and for UDP.
- `WRITE_TIMEOUT_SECONDS`: The write timeout (in seconds) for TCP connections.
