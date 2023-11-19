#ifndef __PROTOCOL_HPP__
#define __PROTOCOL_HPP__

#include <string>

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
    enum status { OK, NOK, REG };
    status status;

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
    enum status { OK, NOK, UNR };
    status status;

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
    enum status { OK, NOK, UNR };
    status status;

    std::string serialize();
    void deserialize(std::string &buffer);
};

void sendUDPPacket(UDPPacket &packet);

std::string receiveUDPPacket();

void sendTCPPacket(TCPPacket &packet);

std::string receiveTCPPacket();

#endif // __PROTOCOL_HPP__
