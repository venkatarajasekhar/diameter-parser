//
//  tests.hpp
//  Dia
//
//  Created by Oleg Serdyukov on 29.09.11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef Dia_tests_hpp
#define Dia_tests_hpp

#include <iostream>
#include <iomanip>

#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"

//
// Variables
extern std::vector<std::string> filterFields;

//
// Functions
void fieldsProcess(const int level, const std::string& fieldName, const std::string& fieldShowName, const std::string& fieldValue);

void traverse_xml_recursion_main(const std::string& input_xml);
void traverse_xml_recursion_detail(rapidxml::xml_node<> *xml_node, int level);
void initFilterFields(void);

#endif