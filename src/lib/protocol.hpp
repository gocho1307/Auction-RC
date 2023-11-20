#ifndef __PROTOCOL_HPP__
#define __PROTOCOL_HPP__

#include <string>

enum status { OK, NOK, REG, UNR, NLG, EOW, ACC, ILG, END, ERR };

class UDPPacket {
  public:
    virtual std::string serialize() = 0;
    virtual void deserialize(std::string &buffer) = 0;

    virtual ~UDPPacket() = default;
};

class TCPPacket {
  public:
    virtual std::string serialize() = 0;
    virtual void deserialize(std::string &buffer) = 0;

    virtual ~TCPPacket() = default;
};

// Send login packet (LIN)
class LINPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "LIN";
    int UID;
    std::string password;

    std::string serialize();
    void deserialize(std::string &buffer);
};

// Receive login packet (RLI)
class RLIPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "RLI";
    status stat;

    std::string serialize();
    void deserialize(std::string &buffer);
};

// Send logout packet (LOU)
class LOUPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "LOU";
    int UID;
    std::string password;

    std::string serialize();
    void deserialize(std::string &buffer);
};

// Receive logout packet (RLO)
class RLOPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "RLO";
    status stat;

    std::string serialize();
    void deserialize(std::string &buffer);
};

// Send register packet (UNR)
class UNRPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "UNR";
    int UID;
    std::string password;

    std::string serialize();
    void deserialize(std::string &buffer);
};

// Receive register packet (RUR)
class RURPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "RUR";
    status stat;

    std::string serialize();
    void deserialize(std::string &buffer);
};

// Error UDP packet (ERR)
class ERRUDPPacket : public UDPPacket {
  public:
    std::string serialize();
    void deserialize(std::string &buffer);
};

// Error TCP packet (ERR)
class ERRTCPPacket : public TCPPacket {
  public:
    std::string serialize();
    void deserialize(std::string &buffer);
};

int sendUDPPacket(UDPPacket &packet, struct addrinfo *res, int fd);

int receiveUDPPacket(std::string &response, struct addrinfo *res, int fd);

int sendTCPPacket(TCPPacket &packet, int fd);

int receiveTCPPacket(std::string &response, int fd);

int readDelimiter(std::string &line);

int readFileData(std::string &fInfo);

int writeFileData(std::string &fName, std::string &str);

#endif // __PROTOCOL_HPP__
