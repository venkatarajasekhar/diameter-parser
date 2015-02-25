//
//  AVPItem.hpp
//  Dia
//
//  Created by Oleg Serdyukov on 12.12.11.
//  Copyright (c) 2011 The Apple Geek. All rights reserved.
//

#ifndef Dia_AVPItem_hpp
#define Dia_AVPItem_hpp

#include <iostream>
#include <iomanip>
#include <map>
#include <sstream>
#include <vector>
#include <time.h>

#include <boost/format.hpp> 
#include <boost/variant.hpp> 
#include <boost/any.hpp> 


#include "utils.hpp"

// Типы данных
#define AVPTYPE_UNDEFINED				"UndefinedType"
#define AVPTYPE_GROUPED					"Grouped"

typedef std::map<long, std::string> avp_enum_type;

class AVPItem {
public:
    AVPItem(): Code(0), Type(), Name(), Enum() { }
	
	void set_code(const long aCode) {
		Code = aCode;
	}
	
	void set_code(const std::string& aCode) {
		Code = dictatoi(aCode);
	}
	
	void set_name(const std::string& aName) {
		Name = aName;
	}
	
	void set_type(const std::string& aType) {
		Type = aType;
	}
	
	void set_enum(const avp_enum_type& aEnum) {
		Enum = aEnum;
	}
	
	long Code;
	std::string Name;
	std::string Type;
	avp_enum_type Enum;
	
private:
	
};



#endif
