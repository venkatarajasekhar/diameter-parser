//
//  IPPacket.cpp
//  Dia
//
//  Created by Oleg Serdyukov on 12.12.11.
//  Copyright (c) 2011 The Apple Geek. All rights reserved.
//

#include <iostream>
#include "IPPacket.hpp"


using namespace std;

int parsePcapFile(const std::string& file) {
    char errbuff[PCAP_ERRBUF_SIZE];
	
    pcap_t * pcap = pcap_open_offline(file.c_str(), errbuff);
	if (pcap == NULL) {
		cerr << "Could not open pcap file " << file << endl;
		return -1;
	}
	
    assert(pcap != NULL);
	
    struct pcap_pkthdr *header;
    const u_char *data;
	u_int packetCount = 0;
	
	vector<IPPacket>packets;
    
    while (pcap_next_ex(pcap, &header, &data) >= 0)
    {	
		++packetCount;
		
		// Show a warning if the length captured is different
		if (header->len != header->caplen)
			cerr << "Capture size different than packet size " << header->len << endl;
		
		IPPacket ipPacket;
        ipPacket.ParsePacket(header, data, packetCount);
		//ipPacket.PrintPacketSeparated();
		packets.push_back(ipPacket);
    }

	//std::ofstream filearchive("/Users/ctrld/Documents/Development/Dia/Data/1seria01.txt"); 
	//boost::archive::text_oarchive oa(filearchive);
	for (vector<IPPacket>::iterator iter = packets.begin(); iter != packets.end(); ++iter) {
		//iter->PrintPacketSeparated();
		iter->PrintLegacyDiameter();
		//oa << *iter;
	}
	return 0;
}



void IPPacket::ParsePacket(pcap_pkthdr * header, const u_char *data, const uint aPacketNumber)
{
	packetNumber = aPacketNumber;
	
	// Prepare  ether_header
	const struct ether_header *ethernet;
	ethernet = (struct ether_header*)(data);
	
	// Frame data
	packetLength = header->len;
	timeEpochSec = header->ts.tv_sec;
	timeEpochUsec = header->ts.tv_usec;
	etherHeaderLength = ETHER_HDRLEN;
	
	// IP data
	const struct ip *ipHeader;
	ipHeader = (struct ip*)(data + etherHeaderLength);
	// Лёгкое откровение: header length in bytes = value set in ip_hl x 4 [each # counts for 4 octets]
	ipHeaderLength = IP_HL(ipHeader)*4;
	ipLength = ntohs(ipHeader->ip_len);
	ipId = ntohs(ipHeader->ip_id);
	ipProto = ipHeader->ip_p;
	ipSrc = ipHeader->ip_src;
	ipDst = ipHeader->ip_dst;
	
	// TCP data
	const struct tcphdr *tcpHeader;
	tcpHeader = (struct tcphdr*)(data + etherHeaderLength + ipHeaderLength);
	
	tcpSPort = ntohs(tcpHeader->th_sport);
	tcpDPort = ntohs(tcpHeader->th_dport);
	// TODO: проверить TCP Seq, возможен неправильный порядок байт, но это несущественно для моего случая
	tcpSeq = tcpHeader->th_seq;
	// TODO: проверить TCP Reset Flag
	tcpFlagsReset = tcpHeader->th_flags & TH_RST;
	tcpHeaderLength = TH_OFF(tcpHeader)*4;
	
	// Размер payload в пакете. Равен размеру пакета без заголовков протоколов
	tcpPduSize =  packetLength - (etherHeaderLength + ipHeaderLength + tcpHeaderLength);
	
	// Diameter
	if (tcpDPort == 3868 || tcpSPort == 3868) { 
		
		uint dOffset = etherHeaderLength + ipHeaderLength + tcpHeaderLength;
		ParseDiameterPacket(header, data + dOffset, packetLength - dOffset, packetNumber);
	} else {
		//cout << endl;
		cout << "Other#" << timeEpochSec << "#" << (tcpFlagsReset ? "RST" : "..." ) << endl;
		
	}
	//cout << endl;
	delete header;
}


void IPPacket::ParseDiameterPacket(pcap_pkthdr * header, const u_char *data, const uint packetLength, const uint aPacketNumber)
{
	// Позиция в пакете текущего обрабатываемого PDU
	int currentPDUPosition=0;
	int pduOffset=0;
	
	// Номер PDU в пакете
	int aPayloadNumber=0;
	
	// До исполнения цикла в первый раз это значение ещё неизвестно, поэтому делаю так, чтобы цикл гарантированно выполнился
	int diameterLength = 0;
	while (pduOffset < packetLength - diameterLength) {
		aPayloadNumber++;
		
		if (aPayloadNumber > 30) {
			cerr << "Too many PDU in packet " << aPacketNumber << ", breaking" << endl;
			break;
		}
		// Ссылка на очередной PDU относительно начала пакета
		pduOffset = currentPDUPosition;
		if (pduOffset + sizeof(diameter_common_header) > packetLength) {
			cerr << "PDU size is not enough: pduOffset (" << pduOffset << ") + header (" << sizeof(diameter_common_header) << ") > packetLength (" << packetLength << "), delta " << pduOffset + sizeof(diameter_common_header) - packetLength << " in packet " << aPacketNumber << ", payload " << aPayloadNumber << endl;
			break;
		}
		
		DiameterPacket diameterPacket;
		diameterLength = diameterPacket.ParseDiameterPayload(header, data + pduOffset, packetLength - pduOffset, aPacketNumber, aPayloadNumber);
		
		// Добавлять payload в массив только тогда, когда не было ошибки при его разборе
		if (diameterLength >= 0)
			diameterArray.push_back(diameterPacket);
		
		if (diameterLength == -1) {
			break;
		} else if (diameterLength == -2) {
			break;
		} else if (diameterLength == -3) {
			break;
		} else if (diameterLength == -4) {
			break;
		}
		
		currentPDUPosition += diameterLength;
	} 
	delete header;
}


void IPPacket::PrintPacket() {
	cout << string(FORMAT_HEADER, '#') << endl;
    cout << "# Packet No " << packetNumber << " (" << packetLength << " bytes)" << endl;
    
	cout << string(FORMAT_HEADER, '#') << endl;
	cout << setw(FORMAT_FIELD_1) << left << "frame.number" << setw(FORMAT_FIELD_2) << left << packetNumber << endl;
	cout << setw(FORMAT_FIELD_1) << left << "frame.length" << setw(FORMAT_FIELD_2) << left << packetLength << endl;

	cout << setw(FORMAT_FIELD_1) << left << "frame.time_epoch.sec" << setw(FORMAT_FIELD_2) << left << timeEpochSec << endl;
	cout << setw(FORMAT_FIELD_1) << left << "frame.time_epoch.usec" << setw(FORMAT_FIELD_2) << left << timeEpochUsec << endl;

	cout << setw(FORMAT_FIELD_1) << left << "ip.src" << setw(FORMAT_FIELD_2) << left << ipaddr2string(ipSrc) << endl;
	cout << setw(FORMAT_FIELD_1) << left << "ip.dst" << setw(FORMAT_FIELD_2) << left << ipaddr2string(ipDst) << endl;

	cout << setw(FORMAT_FIELD_1) << left << "tcp.flags.reset" << setw(FORMAT_FIELD_2) << left << (tcpFlagsReset ? "True" : "False" ) << endl;

	for (vector<DiameterPacket>::iterator iter = diameterArray.begin(); iter != diameterArray.end(); ++iter) {
		cout << string(FORMAT_HEADER, '#') << endl;
		iter->PrintPacket();
	}
	
	std::cout << std::endl;
}

void IPPacket::PrintPacketSeparated() {
	for (vector<DiameterPacket>::iterator iter = diameterArray.begin(); iter != diameterArray.end(); ++iter) {
		cout << string(FORMAT_HEADER, '#') << endl;
		cout << setw(FORMAT_FIELD_1) << left << "frame.number" << setw(FORMAT_FIELD_2) << left << packetNumber << endl;
		cout << setw(FORMAT_FIELD_1) << left << "frame.length" << setw(FORMAT_FIELD_2) << left << packetLength << endl;
		
		cout << setw(FORMAT_FIELD_1) << left << "frame.time_epoch.sec" << setw(FORMAT_FIELD_2) << left << timeEpochSec << endl;
		cout << setw(FORMAT_FIELD_1) << left << "frame.time_epoch.usec" << setw(FORMAT_FIELD_2) << left << timeEpochUsec << endl;
		
		cout << setw(FORMAT_FIELD_1) << left << "ip.src" << setw(FORMAT_FIELD_2) << left << ipaddr2string(ipSrc) << endl;
		cout << setw(FORMAT_FIELD_1) << left << "ip.dst" << setw(FORMAT_FIELD_2) << left << ipaddr2string(ipDst) << endl;
		
		cout << setw(FORMAT_FIELD_1) << left << "tcp.flags.reset" << setw(FORMAT_FIELD_2) << left << (tcpFlagsReset ? "True" : "False" ) << endl;
		iter->PrintPacket();
		std::cout << std::endl;
	}	
}


void IPPacket::PrintLegacyDiameter() {
	for (vector<DiameterPacket>::iterator iter = diameterArray.begin(); iter != diameterArray.end(); ++iter) {
		if (iter->diameterCmdCode != 272) {
			continue;
		}
		
		// Dec 13, 2011 16:24:04.
		char buffer [80] = {0};
		strftime (buffer, 80, "%b %d, %Y %X", localtime(&timeEpochSec));
		string dStr(buffer); //asctime(gmtime((time_t *)&timeEpochSec))
		//if(dStr.size() > 0) dStr.resize(dStr.size()-1);

		cout << dStr << "." << timeEpochUsec << "#";
		cout << "diameter" << "#";
		cout << tcpFlagsReset << "#";
		
		cout << ipaddr2string(ipSrc) << "#";
		cout << ipaddr2string(ipDst) << "#";
		
		if(iter->diameterResultCode != -1)
			cout << iter->diameterResultCode;
		cout << "#";
		
		if (iter->diameterCCRequestType != -1)
			cout << iter->diameterCCRequestType;
		cout << "#";
		
		cout << iter->diameterSessionId << "#";
		cout << iter->diameterSubscriptionId << "#";
		cout << iter->diameterFramedIPAddress << "#";

		if (iter->diameterCCInputOctets != -1)
			cout << iter->diameterCCInputOctets;
		cout << "#";

		if (iter->diameterCCOutputOctets != -1)
			cout << iter->diameterCCOutputOctets;
		cout << "#";

		if (iter->diameterTerminationCause != -1)
			cout << iter->diameterTerminationCause;
		cout << endl;
	}	
}
