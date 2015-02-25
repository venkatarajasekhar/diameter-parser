//
//  utils.cpp
//  Dia
//
//  Created by Oleg Serdyukov on 02.12.11.
//  Copyright (c) 2011 The Apple Geek. All rights reserved.
//

#include "utils.hpp"

long dictatoi(const std::string &str){
	std::stringstream stream(str);
	long res;
	return !(stream >>res)?0:res;
}

// http://stackoverflow.com/questions/809902/64-bit-ntohl-in-c
u_int64_t htonll(u_int64_t v) {
    union { 
		u_int32_t lv[2]; 
		u_int64_t llv; 
	} u;
	
    u.lv[0] = (u_int32_t)htonl(v >> 32);
    u.lv[1] = (u_int32_t)htonl(v & 0xFFFFFFFFULL);
	
    return u.llv;
}

u_int64_t ntohll(u_int64_t v) {
    union { 
		u_int32_t lv[2]; 
		u_int64_t llv; 
	} u;
    
	u.llv = v;
	
    return ((u_int64_t)ntohl(u.lv[0]) << 32 | (u_int64_t)ntohl(u.lv[1]));
}


std::string ipaddr2string(in_addr &inaddr) {
	parsedIPAddress *ip;
	ip = (parsedIPAddress*)&inaddr.s_addr;
	
	std::stringstream ss;
	std::string str;
	
	ss << (unsigned int) ip->octet1 << "." << (unsigned int) ip->octet2 << "." << (unsigned int) ip->octet3 << "." << (unsigned int) ip->octet4;// << ":" << std::hex  << std::setfill('0') << inaddr.s_addr;
	ss >> str;

	return str;
}