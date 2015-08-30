//
//  AVP.cpp
//  Dia
//
//  Created by Oleg Serdyukov on 12.12.11.
//  Copyright (c) 2011 The Apple Geek. All rights reserved.
//

#include "AVP.hpp"

using namespace std;
using namespace AVP{

long AVPParser ::parseType() {
	if (typeName.compare(AVPTYPE_UTF8STRING) == 0) {
		typeCode = AVPTYPE_N_UTF8STRING;
	} else if (typeName.compare(AVPTYPE_UNDEFINED) == 0) {
		typeCode = AVPTYPE_N_UNDEFINED;
	} else if (typeName.compare(AVPTYPE_OCTETSTRING) == 0) {
		typeCode = AVPTYPE_N_OCTETSTRING;
	} else if (typeName.compare(AVPTYPE_INTEGER32) == 0) {
		typeCode = AVPTYPE_N_INTEGER32;
	} else if (typeName.compare(AVPTYPE_INTEGER64) == 0) {
		typeCode = AVPTYPE_N_INTEGER64;
	} else if (typeName.compare(AVPTYPE_UNSIGNED32) == 0) {
		typeCode = AVPTYPE_N_UNSIGNED32;
	} else if (typeName.compare(AVPTYPE_UNSIGNED64) == 0) {
		typeCode = AVPTYPE_N_UNSIGNED64;
	} else if (typeName.compare(AVPTYPE_TIME) == 0) {
		typeCode = AVPTYPE_N_TIME;
	} else if (typeName.compare(AVPTYPE_GROUPED) == 0) {
		typeCode = AVPTYPE_N_GROUPED;
	} else if (typeName.compare(AVPTYPE_DIAMETERIDENTITY) == 0) {
		typeCode = AVPTYPE_N_DIAMETERIDENTITY;
	} else if (typeName.compare(AVPTYPE_IPADDRESS) == 0) {
		typeCode = AVPTYPE_N_IPADDRESS;
	} else if (typeName.compare(AVPTYPE_APPID) == 0) {
		typeCode = AVPTYPE_N_APPID;
	} else if (typeName.compare(AVPTYPE_ENUMERATED) == 0) {
		typeCode = AVPTYPE_N_ENUMERATED;
	} else {
		typeCode = AVPTYPE_N_UNDEFINED;
	}
	
	return typeCode;
}


int AVPParser::parse_value(const unsigned char *data, const unsigned int len, const int level, const u_int packetNum, const u_int payloadNum) {
	AVPValue v;
	
	// Функция Const, и я не могу поменять Name - это нужно для модификации Subscription-Id-Data в IMSI/MSISDN
	displayName=name;
	
	// Для проверки на ошибки
	int parseError = 0;
	
	// Если тип avp групповой, то возвращаем код необходимости рекурсии
	int typeGrouped = 0;
	
	if (typeName.compare(AVPTYPE_UTF8STRING) == 0 || typeName.compare(AVPTYPE_DIAMETERIDENTITY) == 0) { //Type.compare(AVPTYPE_OCTETSTRING) == 0
		string value;
		stringstream ss;
		for (unsigned int p=0; p < len; p++) {
			ss << data[p];
		}
		ss >> value;
		/* Please check the assignment with Variant type to string */
		v = value;
		//cout << value << endl;
		
		// Хак для записи IMSI и MSISDN из Subscription-Id-Type
		// Если в Subscription-Id-Data длина данных 12, то это MSISDN, а если 15 - то IMSI
		if (int code==444) {
			if (value.length()==12) {
				displayName = "Subscription-Id-MSISDN";
			} else if (value.length()==15) {
				displayName = "Subscription-Id-IMSI";
			}
		}
		
		
		
	} else if (typeName.compare(AVPTYPE_INTEGER32) == 0 || typeName.compare(AVPTYPE_APPID) == 0) {
		int32_t	*ptrValue = NULL;
		int32_t	value;
		if (len == sizeof(int32_t)) {
			ptrValue = (int32_t *)data;
			value = ntohl(*ptrValue);
			v = value;
		} else {
			parseError = 1;
		}
	} else if (typeName.compare(AVPTYPE_INTEGER64) == 0) {
		int64_t	*ptrValue = NULL;
		int64_t	value;
		if (len == sizeof(int64_t)) {
			ptrValue = (int64_t *)data;
			value = ntohll(*ptrValue);
			v = value;
		} else {
			parseError = 1;
		}
		
	} else if (typeName.compare(AVPTYPE_UNSIGNED32) == 0 || typeName.compare(AVPTYPE_ENUMERATED) == 0) {
		u_int32_t	*ptrValue = NULL;
		u_int32_t	value;
		if (len == sizeof(u_int32_t)) {
			ptrValue = (u_int32_t *)data;
			value = ntohl(*ptrValue);
			
			v = value;
			if (typeName.compare(AVPTYPE_ENUMERATED) == 0) {
				enumName = diameterDictionary.findAVPEnumName(code, value);
				if (enumName.length()>0) {
					comment = enumName;
				} else {
					parseError = 2;	
				}
			}
		} else {
			parseError = 1;
		}
	} else if (typeName.compare(AVPTYPE_UNSIGNED64) == 0) {
		uint64_t	*ptrValue = NULL;
		uint64_t	value;
		if (len == sizeof(uint64_t)) {
			ptrValue = (uint64_t *)data;
			value = ntohll(*ptrValue);
			v = value;
		} else {
			parseError = 1;
		}
	} else if (typeName.compare(AVPTYPE_TIME) == 0) {
		// The Time format is derived from the Unsigned32 AVP Base Format
		uint32_t	*ptrValue = NULL;
		uint32_t	value;
		if (len == sizeof(uint32_t)) {
			ptrValue = (uint32_t *)data;
			value = ntohl(*ptrValue);
#define NTP_BASETIME 2208988800ul
			value-=NTP_BASETIME;
			v = value;
			
			// Если поставить asctime(gmtime((time_t *)&value)), то начинаются креши
			// std::cout << value << endl; 
		} else {
			parseError = 1;
		}
	} else if (typeName.compare(AVPTYPE_IPADDRESS) == 0) {
		// Может быть обычный IP-адрес, а может быть c 2 байтами версии (Host-IP-Address)
		if (len == sizeof(in_addr) || len == sizeof(in_addr)+2) {
			in_addr *ptrValue = NULL;
			in_addr value;
			ptrValue = (in_addr *)(data + (len == sizeof(in_addr)?0:2) );

			value = *ptrValue ;
			
			string str = ipaddr2string(value);
			comment = str;
			v = value.s_addr;
		} else {
			parseError = 1;
		}
	} else if (typeName.compare(AVPTYPE_GROUPED) == 0) {
		typeGrouped = 1;
	} else {
		string str;
		for (unsigned int p=0; p < len; p++) {
			stringstream ss;
			string tempstr;
			ss << hex << setw(2) << setfill('0') << (int) data[p];
			ss >> tempstr;
			if (p < len-1) {
				str.append(tempstr.append(" "));
			} else {
				str.append(tempstr);
			}
		}
		v = str;
	}
	
	avpValue = v;
	try{
	long ReturnValueTypeCode = parseType();
	//is there any logic using return value of parsertype
	
	
	
	}
	catch(AVPException& e)
        {
           cout << "ParserException caught" << endl;
           cout << e.what() << endl;
           }
       catch(std::exception& e)
       {
        //Other errors
       }
	switch (parseError) {
			// Нет ошибок
		case 0:
			break;
			// Несоответствие размера переменной его реальной длине
		case 1:
			cerr << "Parse error, length mismatch " << len << " in packet " << packetNum << " payload " << payloadNum << endl;
			break;
			// Нет соответствующего Enumerated-значения
		case 2:
		        cerr << "Parse error, problem with enumeration (" << name << "/" << avpValue << ") in packet " << packetNum << " payload " << payloadNum << endl;
			break;
			
		default:
			break;
	}
	
	return typeGrouped;
}
}// namespace AVP 

