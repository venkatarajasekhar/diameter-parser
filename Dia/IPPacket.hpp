//
//  IPPacket.hpp
//  Dia
//
//  Created by Oleg Serdyukov on 12.12.11.
//  Copyright (c) 2011 The Apple Geek. All rights reserved.
//

#ifndef Dia_IPPacket_hpp
#define Dia_IPPacket_hpp

#include <iostream>
#include <iomanip>
#include <string>
#include <iostream>
#include <pcap.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <boost/format.hpp> 

#include <boost/archive/text_oarchive.hpp> 
#include <boost/archive/text_iarchive.hpp> 


// From tcpdump
#include "ip.h"
#include "ethertype.h"
#include "ether.h"
#include "tcp.h"
#include "diameter.h"

#include "DiameterPacket.hpp"

extern DiameterDictionary diameterDictionary;
//
// Variables
// extern std::vector<std::string> filterFields;

//
// Functions
int parsePcapFile(const std::string& file);

struct packet {
    long		time_epoch_sec;		// Epoch time, seconds
	long		time_epoch_usec;	// Epoch time, microseconds
	u_int		len;				// Packet length
	u_int		ether_hdr_len;
    u_int	    ip_hdr_len;
    u_int		ip_len;
    u_int16_t   ip_id;
    u_int8_t    ip_proto;
    struct in_addr ip_src, ip_dst;
	
	u_int16_t	tcp_sport;			// source port
	u_int16_t	tcp_dport;			// destination port
	u_int32_t	tcp_seq;			// sequence number
	bool		tcp_flags_reset;
    u_int		tcp_hdr_len;
	u_int		tcp_pdu_size;
	
	
	u_int8_t	diameter_version;
	u_int16_t	diameter_length;
	u_int8_t	diameter_request;
	u_int16_t	diameter_cmd_code;
	u_int32_t	diameter_hopbyhopid;
	u_int32_t	diameter_endtoendid;
	
};

class IPPacket {
public:
	IPPacket() {
		packetNumber = 0;
		timeEpochSec = 0;
		timeEpochUsec = 0;
		packetLength = 0;
		etherHeaderLength = 0;
		ipHeaderLength = 0;
		ipLength = 0;
		ipId = 0;
		ipProto = 0;
		tcpSPort = 0;
		tcpDPort = 0;
		tcpSeq = 0;
		tcpFlagsReset = 0;
		tcpHeaderLength = 0;
		tcpPduSize = 0;
	}
	
	u_int		packetNumber;
	long		timeEpochSec;		// Epoch time, seconds
	long		timeEpochUsec;	// Epoch time, microseconds
	u_int		packetLength;			// Packet length
	u_int		etherHeaderLength;
    u_int	    ipHeaderLength;
    u_int		ipLength;
    u_int16_t   ipId;
    u_int8_t    ipProto;
    struct in_addr ipSrc, ipDst;
	
	u_int16_t	tcpSPort;			// source port
	u_int16_t	tcpDPort;			// destination port
	u_int32_t	tcpSeq;			// sequence number
	bool		tcpFlagsReset;
    u_int		tcpHeaderLength;
	u_int		tcpPduSize;
	
	std::vector<DiameterPacket>	diameterArray;
	
	void ParsePacket(pcap_pkthdr * header, const u_char *data, const uint aPacketNumber);
	void ParseDiameterPacket(pcap_pkthdr * header, const u_char *data, const uint packetLength, const uint aPacketNumber);

	void PrintPacket();
	// Каждый diameter payload сопровождается заголовком IP-пакета
	void PrintPacketSeparated();
	
	// Печать diameter как на 3g-cap
	void PrintLegacyDiameter();


	
private:
	friend class boost::serialization::access; 
	
	template <typename Archive> 
	void serialize(Archive &ar, const unsigned int version) 
	{ 
		ar & packetNumber & timeEpochSec & timeEpochUsec & ipSrc.s_addr & ipDst.s_addr; 
	} 

	
};




#endif
