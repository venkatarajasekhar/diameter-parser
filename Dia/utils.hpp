//
//  utils.hpp
//  Dia
//
//  Created by Oleg Serdyukov on 02.12.11.
//  Copyright (c) 2011 The Apple Geek. All rights reserved.
//

#ifndef Dia_utils_hpp
#define Dia_utils_hpp

#include <iostream>
#include <iomanip>
#include <map>
#include <sstream>
#include <vector>
#include <string>
#include <netinet/in.h>

long dictatoi(const std::string &str);

unsigned long long htonll(unsigned long long v);
u_int64_t ntohll(u_int64_t v);

typedef	unsigned int		u_int;
std::string ipaddr2string(in_addr &inaddr);

typedef struct {
	uint8_t octet1;
	uint8_t octet2;
	uint8_t octet3;
	uint8_t octet4;
} parsedIPAddress;
#endif
