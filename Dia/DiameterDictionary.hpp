//
//  DiameterDictionary.hpp
//  Dia
//
//  Created by Oleg Serdyukov on 12.12.11.
//  Copyright (c) 2011 The Apple Geek. All rights reserved.
//

#ifndef Dia_DiameterDictionary_hpp
#define Dia_DiameterDictionary_hpp

#include <iostream>
#include <iomanip>
#include <map>
#include <sstream>
#include <vector>
#include <time.h>

#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"

#include <boost/format.hpp> 
#include <boost/variant.hpp> 
#include <boost/any.hpp> 

#include "utils.hpp"
#include "AVPItem.hpp"


//
// Functions
void traverse_dictionary_recursion_main(const std::string& input_xml);


class DiameterDictionary {
public:
    DiameterDictionary(): avpDictionary() { }
	std::map<long, AVPItem> avpDictionary;
	
	std::map<long, std::string> cmdDictionary;
	
	friend std::ostream& operator<< (std::ostream &out, const DiameterDictionary &aDiameterDictionary);
	
	int load_xml_text(const std::string& input_xml_text);
	int load_xml_file(const char * input_xml_file);
	
	const std::string findAVPName(long avpCode);
	const std::string findAVPType(long avpCode);
	const std::string findAVPEnumName(long avpCode, long avpEnumCode);
	const std::string findCommandName(long cmdCode);
	
private:
};


#endif
