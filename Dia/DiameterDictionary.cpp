//
//  DiameterDictionary.cpp
//  Dia
//
//  Created by Oleg Serdyukov on 12.12.11.
//  Copyright (c) 2011 The Apple Geek. All rights reserved.
//

#include <iostream>

#include "DiameterDictionary.hpp"

/***********************************
 * Class members definition
 */
std::ostream& operator<< (std::ostream &out, const DiameterDictionary &aDiameterDictionary) {
	
	using namespace std;
	
	std::map<long, AVPItem>::const_iterator map_it = aDiameterDictionary.avpDictionary.begin();
	while (map_it != aDiameterDictionary.avpDictionary.end()) {
		out << setw(10) << left << (map_it->second).Code
		<< setw(50) << left << (map_it->second).Name 
		<< setw(20) << left << (map_it->second).Type << endl;
		
		avp_enum_type enumMap = (map_it->second).Enum;
		avp_enum_type::const_iterator map_enum = enumMap.begin();
		while (map_enum != enumMap.end()) {
			
			out << setw(10) << left << "" 
			<< setw(10) << left << map_enum->first 
			<< setw(40) << left << "" 
			<< setw(30) << left << map_enum->second << endl;
			++map_enum;
		}
		++map_it;
	}
	return out;
}

int DiameterDictionary::load_xml_text(const std::string& input_xml_text) {
    using namespace rapidxml;
    using namespace std;
	
    vector<char> xml_copy(input_xml_text.begin(), input_xml_text.end());
    xml_copy.push_back('\0');
    
    xml_document<> doc;
	try
	{
		doc.parse<parse_declaration_node | parse_no_data_nodes>(&xml_copy[0]);
	}
	catch (...)
	{
		cerr << "An exception occurred while parsing dictionary file." <<  endl;
		return 0;
	}
    
    xml_node<>* topNode = doc.first_node("dictionary");
    xml_node<>* avpNode = topNode->first_node("avp");
	
    while (avpNode) {
		string avpName = avpNode->first_attribute("name")->value();
		string avpCode = avpNode->first_attribute("code")->value();
		string avpType = AVPTYPE_UNDEFINED;
		
		xml_node<>* typeNode = avpNode->first_node("type");
		if (typeNode) {
			if (typeNode->first_attribute("type-name")) {
				avpType = typeNode->first_attribute("type-name")->value();
			}
		} else {
			// Для типа grouped
			xml_node<>* groupNode = avpNode->first_node("grouped");
			if (groupNode) {
				avpType = AVPTYPE_GROUPED;
			}
		}
		
		// cout << avpCode << "\t" << avpName << "/" << avpType << endl;
		
		avp_enum_type enumMap;
		xml_node<>* enumNode = avpNode->first_node("enum");
		while (enumNode) {
			if (enumNode->first_attribute("name")) {
				string avpEnumName = enumNode->first_attribute("name")->value();
				string avpEnumCode = enumNode->first_attribute("code")->value();
				// cout << "\t" << avpEnumCode << "\t" << avpEnumName << endl;
				
				enumMap[dictatoi(avpEnumCode)] = avpEnumName;
				//enumVector.push_back(make_pair(dictatoi(avpEnumCode), avpEnumName));
			}
			enumNode = enumNode->next_sibling("enum");
		}
		
		// Записываю информацию для удобного поиска в других частях программы
		AVPItem avpItem;
		avpItem.set_code(avpCode);
		avpItem.set_name(avpName);
		avpItem.set_type(avpType);
		avpItem.set_enum(enumMap);
		avpDictionary[dictatoi(avpCode)]=avpItem;
		
		avpNode = avpNode->next_sibling("avp");
    }

	// Разбор Commands
	xml_node<>* commandNode = topNode->first_node("command");
	
    while (commandNode) {
		string cmdName = commandNode->first_attribute("name")->value();
		string cmdCode = commandNode->first_attribute("code")->value();

		std::stringstream ss;
		long lcode;
		ss << cmdCode;
		ss >> lcode;

		cmdDictionary[lcode] = cmdName;

		commandNode = commandNode->next_sibling("command");
    }
	
	return 1;
}

int DiameterDictionary::load_xml_file(const char* input_xml_file) {
	using namespace std;
	
    string input_dict;
    string line;
    
	std::ifstream dictfile(input_xml_file);
    if(dictfile.is_open())
    {
        while (dictfile.good()) {
            getline(dictfile, line);
            input_dict += line;
            input_dict += "\n";
        }
        dictfile.close();
		
		return load_xml_text(input_dict);
    } else {
		return 0;
    };
}

const std::string DiameterDictionary::findAVPName(long avpCode) {
	return avpDictionary[avpCode].Name;
}

const std::string DiameterDictionary::findAVPType(long avpCode) {
	return avpDictionary[avpCode].Type;
}

const std::string DiameterDictionary::findAVPEnumName(long avpCode, long avpEnumCode) {
	return avpDictionary[avpCode].Enum[avpEnumCode];
}

const std::string DiameterDictionary::findCommandName(long cmdCode) {
	return cmdDictionary[cmdCode];
}
