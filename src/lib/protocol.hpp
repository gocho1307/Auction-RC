#ifndef __PROTOCOL_HPP__
#define __PROTOCOL_HPP__

#include "utils.hpp"

#include <string>
#include <vector>

class UDPPacket {
  public:
    virtual std::string serialize() = 0;
    virtual int deserialize(std::string &buffer) = 0;
    virtual ~UDPPacket() = default;

  protected:
    std::string readString(std::string &buffer);
    int readSpace(std::string &buffer);
    int readNewLine(std::string &buffer);
    int readAuctions(std::string &buffer, std::vector<Auction> &auctions);
};

class TCPPacket {
  public:
    virtual int serialize(const int fd) = 0;
    virtual int deserialize(const int fd) = 0;
    virtual ~TCPPacket() = default;

  protected:
    std::string readString(const int fd, const size_t lim);
    int readSpace(const int fd);
    int readNewLine(const int fd);
    int sendFile(const int fd, std::string fPath);
    int receiveFile(const int fd);

  private:
    char delim = 0;
};

// Send login packet (LIN)
#define LIN_LEN 21
class LINPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "LIN";
    std::string UID;
    std::string password;

    std::string serialize();
    int deserialize(std::string &buffer);
};

// Receive login packet (RLI)
#define RLI_LEN 9
class RLIPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "RLI";
    std::string status;

    std::string serialize();
    int deserialize(std::string &buffer);
};

// Send logout packet (LOU)
#define LOU_LEN 21
class LOUPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "LOU";
    std::string UID;
    std::string password;

    std::string serialize();
    int deserialize(std::string &buffer);
};

// Receive logout packet (RLO)
#define RLO_LEN 9
class RLOPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "RLO";
    std::string status;

    std::string serialize();
    int deserialize(std::string &buffer);
};

// Send register packet (UNR)
#define UNR_LEN 21
class UNRPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "UNR";
    std::string UID;
    std::string password;

    std::string serialize();
    int deserialize(std::string &buffer);
};

// Receive register packet (RUR)
#define RUR_LEN 9
class RURPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "RUR";
    std::string status;

    std::string serialize();
    int deserialize(std::string &buffer);
};

// Send open packet (OPA)
class OPAPacket : public TCPPacket {
  public:
    static constexpr const char *ID = "OPA";
    std::string UID;
    std::string password;
    std::string auctionName;
    int startValue;
    int timeActive;
    std::string assetfPath;

    int serialize(const int fd);
    int deserialize(const int fd);
};

// Receive open packet (ROA)
class ROAPacket : public TCPPacket {
  public:
    static constexpr const char *ID = "AID";
    std::string status;
    std::string AID;

    int serialize(const int fd);
    int deserialize(const int fd);
};

// Send close packet (CLS)
class CLSPacket : public TCPPacket {
  public:
    static constexpr const char *ID = "CLS";
    std::string UID;
    std::string password;
    std::string AID;

    int serialize(const int fd);
    int deserialize(const int fd);
};

// Receive close packet (RCL)
class RCLPacket : public TCPPacket {
  public:
    static constexpr const char *ID = "RCL";
    std::string status;

    int serialize(const int fd);
    int deserialize(const int fd);
};

// Send myAuctions packet (LMA)
#define LMA_LEN 12
class LMAPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "LMA";
    std::string UID;

    std::string serialize();
    int deserialize(std::string &buffer);
};

// Receive myAuctions packet (RMA)
#define RMA_LEN 6003
class RMAPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "RMA";
    std::string status;
    std::vector<Auction> auctions;

    std::string serialize();
    int deserialize(std::string &buffer);
};

// Send myBids packet (LMB)
#define LMB_LEN 12
class LMBPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "LMB";
    std::string UID;

    std::string serialize();
    int deserialize(std::string &buffer);
};

// Receive myBids packet (RMB)
#define RMB_LEN 6003
class RMBPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "RMB";
    std::string status;
    std::vector<Auction> auctions;

    std::string serialize();
    int deserialize(std::string &buffer);
};

// Send list packet (LST)
#define LST_LEN 5
class LSTPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "LST";

    std::string serialize();
    int deserialize(std::string &buffer);
};

// Receive list packet (RLS)
#define RLS_LEN 6003
class RLSPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "RLS";
    std::string status;
    std::vector<Auction> auctions;

    std::string serialize();
    int deserialize(std::string &buffer);
};

// Send bid packet (BID)
class BIDPacket : public TCPPacket {
  public:
    static constexpr const char *ID = "BID";
    std::string UID;
    std::string password;
    std::string AID;
    int value;

    int serialize(const int fd);
    int deserialize(const int fd);
};

// Receive bid packet (RBD)
class RBDPacket : public TCPPacket {
  public:
    static constexpr const char *ID = "RBD";
    std::string status;

    int serialize(const int fd);
    int deserialize(const int fd);
};

// Send showAsset packet (SAS)
class SASPacket : public TCPPacket {
  public:
    static constexpr const char *ID = "SAS";
    std::string AID;

    int serialize(const int fd);
    int deserialize(const int fd);
};

// Receive showAsset packet (RSA)
class RSAPacket : public TCPPacket {
  public:
    static constexpr const char *ID = "RSA";
    std::string status;
    std::string assetfName;
    uint32_t assetfSize;

    int serialize(const int fd);
    int deserialize(const int fd);
};

// Send showRecord packet (SRC)
#define SRC_LEN 9
class SRCPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "SRC";
    std::string AID;

    std::string serialize();
    int deserialize(std::string &buffer);
};

// Receive showRecord packet (RRC)
#define RRC_LEN 2163
class RRCPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "RRC";
    std::string status;
    std::string hostUID;
    std::string auctionName;
    std::string assetfName;
    int startValue;
    std::string startDateTime;
    int timeActive;
    std::vector<Bid> bids;
    std::string endDateTime;
    int endSecTime;

    std::string serialize();
    int deserialize(std::string &buffer);
};

// Error UDP packet (ERR)
class ERRUDPPacket : public UDPPacket {
  public:
    static constexpr const char *ID = "ERR";
    std::string serialize();
    int deserialize(std::string &buffer) {
        (void)buffer; // unimplemented
        return 0;
    }
};

// Error TCP packet (ERR)
class ERRTCPPacket : public TCPPacket {
  public:
    static constexpr const char *ID = "ERR";
    int serialize(const int fd);
    int deserialize(const int fd) {
        (void)fd; // unimplemented
        return 0;
    }
};

int sendUDPPacket(UDPPacket &packet, struct addrinfo *res, int fd);

int receiveUDPPacket(std::string &response, struct addrinfo *res, int fd,
                     const size_t lim);

int sendTCPPacket(const char *msg, const size_t len, int fd);

#endif // __PROTOCOL_HPP__
