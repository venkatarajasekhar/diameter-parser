//
//  diameter.h
//  Dia
//
//  Created by Oleg Serdyukov on 01.12.11.
//  Copyright (c) 2011 The Apple Geek. All rights reserved.
//
//  Source: http://www.networksorcery.com/enp/protocol/diameter.htm

#ifndef Dia_diameter_hpp
#define Dia_diameter_hpp

struct diameter_common_header {
	u_int8_t	diameter_version;
	u_int8_t	diameter_res1;
	u_int16_t	diameter_length;
#define DH_REQ(dh)	(((dh)->diameter_flags & 0x80) >> 7)
	u_int8_t	diameter_flags;
	u_int8_t	diameter_res2;
	u_int16_t	diameter_cmd_code;
	u_int32_t	diameter_applicationid;
	u_int32_t	diameter_hopbyhopid;
	u_int32_t	diameter_endtoendid;
};
 

struct diameter_avp_header {
	u_int32_t	avp_code;
	u_int8_t	avp_flags;
#define DAH_VENDOR(dah)	(((dah)->avp_flags & 0x80) >> 7)
	u_int8_t	avp_res1; // Не хочу обрабатывать трёхбайтовые последовательности, поэтому игнорирую первый байт
	u_int16_t	avp_length;
	//u_int32_t	avp_vendorid; // Только в случае, если установлен Vendor Flag
};


#endif
