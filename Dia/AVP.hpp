//
//  AVP.hpp
//  Dia
//
//  Created by Oleg Serdyukov on 12.12.11.
//  Copyright (c) 2011 The Apple Geek. All rights reserved.
//

#ifndef Dia_AVP_hpp
#define Dia_AVP_hpp

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
#include <boost/tuple/tuple.hpp> 
#include <boost/tuple/tuple_io.hpp> 
#include <boost/variant.hpp> 
#include <boost/any.hpp> 

#include "utils.hpp"
#include "DiameterDictionary.hpp"
extern DiameterDictionary diameterDictionary;

using namespace std;
using namespace boost;

// Типы данных
#define AVPTYPE_N_UNDEFINED				999
#define AVPTYPE_UNDEFINED				"UndefinedType"

#define AVPTYPE_N_OCTETSTRING			1
#define AVPTYPE_OCTETSTRING				"OctetString"

#define AVPTYPE_N_INTEGER32				2
#define AVPTYPE_INTEGER32				"Integer32"

#define AVPTYPE_N_INTEGER64				3
#define AVPTYPE_INTEGER64				"Integer64"

#define AVPTYPE_N_UNSIGNED32			4
#define AVPTYPE_UNSIGNED32				"Unsigned32"

#define AVPTYPE_N_UNSIGNED64			5
#define AVPTYPE_UNSIGNED64				"Unsigned64"

#define AVPTYPE_N_TIME					6
#define AVPTYPE_TIME					"Time"

#define AVPTYPE_N_GROUPED				7
#define AVPTYPE_GROUPED					"Grouped"

#define AVPTYPE_N_UTF8STRING			8
#define AVPTYPE_UTF8STRING				"UTF8String"

#define AVPTYPE_N_DIAMETERIDENTITY		9
#define AVPTYPE_DIAMETERIDENTITY		"DiameterIdentity"

#define AVPTYPE_N_IPADDRESS				10
#define AVPTYPE_IPADDRESS				"IPAddress"

#define AVPTYPE_N_APPID					11
#define AVPTYPE_APPID					"AppId"

#define AVPTYPE_N_ENUMERATED			12
#define AVPTYPE_ENUMERATED				"Enumerated"

using namespace AVP{
class AVP {
public:
        typedef variant<std::string, int32_t, int64_t, uint32_t, uint64_t> AVPValue;
	typedef tuple<std::string, long, std::string, AVPValue> AVPTuple;
	AVP(): code(0), typeCode(0), isOK(0) {
	};
	int parse_value(const unsigned char *data, const unsigned int len, const int level, const u_int packetNum, const u_int payloadNum);
	long parseType();
private:
        string name;
	long code;
	string typeName;
	long typeCode;
	// Содержит название типа для Enum
	string comment;
	string displayName;
	AVPValue avpValue;
	// Результат разбора AVP
	bool isOK;
	string enumName;
};
}


#endif
