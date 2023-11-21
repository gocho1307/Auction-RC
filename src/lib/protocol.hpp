#ifndef __PROTOCOL_HPP__
#define __PROTOCOL_HPP__

#include <string>
#include <vector>

enum status { OK, NOK, REG, UNR, NLG, EOW, ACC, REF, ILG, END, EAU, ERR };

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

// Send open packet (OPA)
class OPAPacket : public TCPPacket {
  public:
    static constexpr const char *ID = "OPA";
    int UID;
    std::string name;
    std::string fname;
    std::size_t fsize;
    std::vector<char> fdata;
    int start_value;
    int timeactive;

    std::string serialize();
    void deserialize(std::string &buffer);
};

// Receive open packet (ROA)
class ROAPacket : public TCPPacket {
  public:
    static constexpr const char *ID = "AID";
    status stat;
    int AID;

    std::string serialize();
    void deserialize(std::string &buffer);
};

// Send close packet (CLS)
class CLSPacket : public TCPPacket {
  public:
    static constexpr const char *ID = "CLS";
    int UID;
    std::string password;
    int AID;

    std::string serialize();
    void deserialize(std::string &buffer);
};

// Receive close packet (RCL)
class RCLPacket : public TCPPacket {
  public:
    static constexpr const char *ID = "RCL";
    status stat;

    std::string serialize();
    void deserialize(std::string &buffer);
};

// Send myAuctions packet (LMA)
class LMAPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "LMA";
    int UID;

    std::string serialize();
    void deserialize(std::string &buffer);
};

// Receive myAuctions packet (RMA)
class RMAPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "RMA";
    status stat;
    std::string auctions;

    std::string serialize();
    void deserialize(std::string &buffer);
};

// Send myBids packet (LMB)
class LMBPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "LMB";
    int UID;

    std::string serialize();
    void deserialize(std::string &buffer);
};

// Receive myBids packet (RMB)
class RMBPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "RMB";
    status stat;
    std::string auctions;

    std::string serialize();
    void deserialize(std::string &buffer);
};

// Send list packet (LST)
class LSTPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "LST";

    std::string serialize();
    void deserialize(std::string &buffer);
};

// Receive list packet (RLS)
class RLSPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "RLS";
    status stat;
    std::string auctions;

    std::string serialize();
    void deserialize(std::string &buffer);
};


// Send bid packet (BID)
class BIDPacket : public TCPPacket {
  public:
    static constexpr const char *ID = "BID";
    int UID;
    std::string password;
    int AID;
    int value;

    std::string serialize();
    void deserialize(std::string &buffer);
};

// Receive bid packet (RBD)
class RBDPacket : public TCPPacket {
  public:
    static constexpr const char *ID = "RBD";
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
