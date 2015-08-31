//
//  DiameterPacket.hpp
//  Dia
//
//  Created by Oleg Serdyukov on 12.12.11.
//  Copyright (c) 2011 The Apple Geek. All rights reserved.
//

#ifndef Dia_DiameterPacket_hpp
#define Dia_DiameterPacket_hpp

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

// From tcpdump
#include "ip.h"
#include "ethertype.h"
#include "ether.h"
#include "tcp.h"
#include "diameter.h"
#include "utils.hpp"

#include "DiameterDictionary.hpp"
#include "AVP.hpp"

#include <boost/tuple/tuple.hpp> 
#include <boost/tuple/tuple_io.hpp> 

#define FORMAT_HEADER	100
#define FORMAT_FIELD_1	43
#define FORMAT_FIELD_2	20

extern DiameterDictionary diameterDictionary;

class DiameterPacket {
        std::vector<AVP> avpVector;
	u_int		payloadNumber;
	u_int8_t	diameterVersion;
	u_int16_t	diameterLength;
	u_int8_t	diameterRequest;
	u_int16_t	diameterCmdCode;
	u_int32_t	diameterHopbyhopId;
	u_int32_t	diameterEndtoendId;
		// Auxilary variables
	long diameterResultCode;			// 268
	long diameterCCRequestType;		// 416
	std::string diameterSessionId;			// 263
	std::string diameterSubscriptionId;		// 444/Subscription-Id-IMSI
	std::string diameterFramedIPAddress;	// 8
	long diameterCCInputOctets;		// 412
	long diameterCCOutputOctets;		// 414
	long diameterTerminationCause;		// 295
public:
	        DiameterPacket() {
		payloadNumber = 0;
		diameterVersion = 0;
		diameterLength = 0;
		diameterRequest = 0;
		diameterCmdCode = 0;
		diameterHopbyhopId = 0;
		diameterEndtoendId = 0;
		
		diameterResultCode = -1;
		diameterCCRequestType = -1;
		diameterCCInputOctets = -1;
		diameterCCOutputOctets = -1;
		diameterTerminationCause = -1;
	}
	//typedef boost::variant<std::string, int32_t, int64_t, uint32_t, uint64_t> AVPValue;
	//typedef boost::tuple<std::string, long, std::string> AVPTuple;
	int ParseDiameterPayload(pcap_pkthdr * header, const u_char *data, const uint packetLength, const uint aPacketNumber, const uint aPayloadNumber);
	void PrintPacket();
	int typeNameToNum(const std::string *atypeName);
	int parse_avp(const u_char *avp_data, const int avp_max_length, const int level, const u_int packetNum);
	int parse_avp_array(const u_char *avp_array_data, const int avp_array_max_length, const int level, const u_int packetNum);
	void fillAuxVariablesUsingAVP();
};


#endif
