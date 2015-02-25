//
//  DiameterPacket.cpp
//  Dia
//
//  Created by Oleg Serdyukov on 12.12.11.
//  Copyright (c) 2011 The Apple Geek. All rights reserved.
//

#include <iostream>
#include "DiameterPacket.hpp"
#include "IPPacket.hpp"

using namespace std;

//
// Преобразовать тип к числу
// Если возврат 0, то тип неизвестен
int DiameterPacket::typeNameToNum(const std::string *atypeName) {
	if (atypeName->compare(AVPTYPE_OCTETSTRING) == 0) {
		return AVPTYPE_N_OCTETSTRING;
	} else if (atypeName->compare(AVPTYPE_INTEGER32) == 0) {
		return AVPTYPE_N_INTEGER32;
	} else if (atypeName->compare(AVPTYPE_INTEGER64) == 0) {
		return AVPTYPE_N_INTEGER64;
	} else if (atypeName->compare(AVPTYPE_UNSIGNED32) == 0) {
		return AVPTYPE_N_UNSIGNED32;
	} else if (atypeName->compare(AVPTYPE_UNSIGNED64) == 0) {
		return AVPTYPE_N_UNSIGNED64;
	} else if (atypeName->compare(AVPTYPE_TIME) == 0) {
		return AVPTYPE_N_TIME;
	}
	return 0;
}

int DiameterPacket::parse_avp(const u_char *avp_data, const int avp_max_length, const int level, const u_int packetNum) {
	const struct diameter_avp_header *diameterAVPHeader;
	diameterAVPHeader = (struct diameter_avp_header*)(avp_data);
	
	u_int32_t	avp_code 	 = ntohl(diameterAVPHeader->avp_code);
	u_int8_t	avp_vendor   = DAH_VENDOR(diameterAVPHeader);
	u_int16_t	avp_length   = ntohs(diameterAVPHeader->avp_length);
	//u_int32_t	avp_vendorid = ntohl(diameterAVPHeader->avp_vendorid);
	
	string avpName, avpType;
	
	avpName = diameterDictionary.findAVPName(avp_code);
	avpType = diameterDictionary.findAVPType(avp_code);

	if(avpName.length() > 0 && avpType.length() > 0) {
		// Проверка на неправильный разбор
		// TODO: Исправить допустимость кодов AVP
		if (avp_code == 0 || avp_code > 65530) {
			cerr << "AVP Code illegal (" << avp_code << ") in packet " << packetNum << " payload " << payloadNumber << endl;
			return -1;
		}
		if (avp_length <= 0 || avp_length > avp_max_length) {
			cerr << "Illegal AVP Length (" << avp_length << ") in packet " << packetNum << " payload " << payloadNumber << endl;
			return -2;
		}
		
		// Заполнение AVP
		AVP aAVP;
		aAVP.code = avp_code;
		aAVP.name = avpName;
		aAVP.typeName = avpType;

		int typeGrouped = aAVP.parse_value(avp_data + sizeof(diameter_avp_header) + (avp_vendor ? 4 : 0), 
										   avp_length - sizeof(diameter_avp_header) - (avp_vendor ? 4 : 0), level, packetNum, payloadNumber);
		if (typeGrouped) {
			if(parse_avp_array(avp_data + sizeof(diameter_avp_header) + (avp_vendor ? 4 : 0), 
							   avp_length - sizeof(diameter_avp_header) - (avp_vendor ? 4 : 0),
							   level + 1, packetNum) !=0) {
				cerr << "Grouped avp parse error in packet " << packetNum << " payload " << payloadNumber << endl;	
			}
		}

		aAVP.isOK = 1;
		// Не отображать тип Grouped
		if (aAVP.typeCode != AVPTYPE_N_GROUPED)
			avpVector.push_back(aAVP);
		
		//AVPTuple avpTuple = boost::make_tuple(avpName, avp_code, avpType);
		//avpVector.push_back(avpTuple);
		
	} else {
		cerr << "AVP Code didn't found in dictionary: " << avp_code << " in packet " << packetNum << " payload " << payloadNumber << endl;
		return -3;
	}
	
	return avp_length;
	
}

// Return
//	0 - ok
// Level - уровень вложенности AVP
int DiameterPacket::parse_avp_array(const u_char *avp_array_data, const int avp_array_max_length, const int level, const u_int packetNum) {
	
	int currentAVPPosition=0;
	int avpOffset=0;
	
	int avp_number=0;
	while (currentAVPPosition < avp_array_max_length) {
		avp_number++;
		if (avp_number > 100) {
			cerr << "Too many AVP Arrays in packet " << packetNum << endl;
			break;
			
		}
		
		avpOffset = currentAVPPosition;
		
		// Размер допустимого avp ограничиваю сверху размером diameterPacket.diameter_length
		// TODO: не уверен, какой вариант максимального размера пакета правильный
		int avp_length=parse_avp(avp_array_data+avpOffset, avp_array_max_length-avpOffset, level, packetNum);
		//int avp_length=parse_avp(avp_array_data+avpOffset, avp_array_max_length, level, packetNum);
		//cout << "LEN: avpOffset(" << avpOffset << "), avp_array_max_length(" << avp_array_max_length << ")" << endl;
		if (avp_length < 0){
			return -1;
		}
		
		// Важный момент - avp выравнивается по 4-х байтовым отрезкам и дополняет разницу нулями
		currentAVPPosition += avp_length;
		currentAVPPosition += (currentAVPPosition % 4) ? 4 - (currentAVPPosition % 4) : 0 ;
	};
	
	return 0;
}


int DiameterPacket::ParseDiameterPayload(pcap_pkthdr * header, const u_char *data, const uint packetLength, const uint aPacketNumber, const uint aPayloadNumber)
{
	payloadNumber=aPayloadNumber;
	
	// Diameter header
	const struct diameter_common_header *diameterHeader;
	diameterHeader = (struct diameter_common_header*)data;
	diameterVersion = diameterHeader->diameter_version;
	diameterLength  = ntohs(diameterHeader->diameter_length);
	diameterRequest  = DH_REQ(diameterHeader);
	diameterCmdCode  = ntohs(diameterHeader->diameter_cmd_code);
	diameterHopbyhopId  = ntohl(diameterHeader->diameter_hopbyhopid);
	diameterEndtoendId  = ntohl(diameterHeader->diameter_endtoendid);
	
	//PrintPacket();
	// Проверка на допустимость параметров
	if(diameterLength <= 0) {
		cerr << "Diameter payload length illegal diameterLength (" << (int)diameterLength << ") <= 0 in packet " << aPacketNumber << " payload " << aPayloadNumber << endl;
		return -1;
	} else if(diameterLength > packetLength) {
		cerr << "Diameter payload length illegal: diameterLength (" << diameterLength << ") > packetLength(" << packetLength << "), delta " << diameterLength - packetLength << " in packet " << aPacketNumber << " payload " << aPayloadNumber << endl;
		
		return -2;
	} else if(diameterVersion != 1)
	{
		cerr << "Diameter version illegal (" << (int)diameterVersion << ") != 1 in packet " << aPacketNumber << " payload " << aPayloadNumber << endl;
		return -3;
	} else {
		// Иначе проверяемые параметры в порядке и можно разбирать diameter
		// 0-й уровень рекурсии
		int ret=parse_avp_array(data + sizeof(diameter_common_header), diameterLength - sizeof(diameter_common_header), 0, aPacketNumber);
		if ( ret < 0){
			cerr << "AVP parse error in packet " << aPacketNumber << " payload " << aPayloadNumber << endl;
			return -4;
		}

		fillAuxVariablesUsingAVP();
		//cout << endl;
	}
	
	return diameterLength;
}


// Заполнить вспомогательные поля
void DiameterPacket::fillAuxVariablesUsingAVP() {
	for (vector<AVP>::iterator iter = avpVector.begin(); iter != avpVector.end(); ++iter) {
		switch (iter->code) {
			case 268:
				diameterResultCode = boost::get<u_int32_t>(iter->avpValue);
				break;
			case 416:
				diameterCCRequestType = boost::get<u_int32_t>(iter->avpValue);
				break;
			case 263:
				diameterSessionId = boost::get<std::string>(iter->avpValue);
				break;
			case 444:
				diameterSubscriptionId = boost::get<std::string>(iter->avpValue);
				break;
			case 8:
				in_addr addr; 
				addr.s_addr = boost::get<u_int32_t>(iter->avpValue);
				diameterFramedIPAddress = ipaddr2string(addr);
				break;
			case 412:
				diameterCCInputOctets = boost::get<u_int64_t>(iter->avpValue);
				break;
			case 414:
				diameterCCOutputOctets = boost::get<u_int64_t>(iter->avpValue);
				break;
			case 295:
				diameterTerminationCause = boost::get<u_int32_t>(iter->avpValue);
				break;
			default:
				break;
				
		}
	}

}

void DiameterPacket::PrintPacket() {
	cout << setw(FORMAT_FIELD_1) << left << "diameter.payload" << setw(FORMAT_FIELD_2) << left << payloadNumber << endl;
	cout << setw(FORMAT_FIELD_1) << left << "diameter.version" << setw(FORMAT_FIELD_2) << left << (int)diameterVersion << endl;
	cout << setw(FORMAT_FIELD_1) << left << "diameter.payloadlength" << setw(FORMAT_FIELD_2) << left << (int)diameterLength << endl;
	cout << setw(FORMAT_FIELD_1) << left << "diameter.cmd.code";
	cout << "[" << (int)diameterCmdCode << "] " << diameterDictionary.findCommandName(diameterCmdCode) << endl;
	cout << setw(FORMAT_FIELD_1) << left << "diameter.request";
	cout << "[" << (int)diameterRequest << "] " << ((int)diameterRequest ? "Request" : "Reply") << endl;
	cout << setw(FORMAT_FIELD_1) << left << "diameter.hopbyhopid" << setw(FORMAT_FIELD_2) << left << hex << setw(2) << setfill('0') << diameterHopbyhopId << setfill(' ') << dec << endl;
	cout << setw(FORMAT_FIELD_1) << left << "diameter.endtoendid" << setw(FORMAT_FIELD_2) << left << hex << setw(2) << setfill('0') << diameterEndtoendId << setfill(' ') << dec << endl;
	
	for (vector<AVP>::iterator iter = avpVector.begin(); iter != avpVector.end(); ++iter) {
		if (iter->isOK) {
			std::cout << "(" << setw(5) << right << (int)iter->code << ") " << setw(34) << left << iter->displayName;// << setw(20) << left << iter->typeName;
			std::cout << " [" << iter->avpValue << "]" << (iter->comment.length() > 0 ? string(" ").append(iter->comment) : "" ) << endl;
		}
	}

}