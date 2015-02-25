//
//  tests.cpp
//  Dia
//
//  Created by Oleg Serdyukov on 29.09.11.
//  Copyright 2011 The Apple Geek. All rights reserved.
//

#include "tests.hpp"

std::vector<std::string> filterFields;

void fieldsProcess(const int level, const std::string& fieldName, const std::string& fieldShowName, const std::string& fieldValue) {
    using namespace std;
    
    bool printField(false);
    for (vector<std::string>::iterator iter = filterFields.begin(); iter != filterFields.end(); ++iter) {
        if (*iter == fieldName) {
            //cout << "@";
            printField=true;
            break;
        }
    
    }
// Debug
//printField=true;
    
    if (printField) {
        for (int l=0; l<=level; l++) {
            cout << "\t";
        }
        cout << setw(50-(level)*4) << left << fieldName \
             << setw( 30 ) << (fieldValue.length() > 25 ? fieldValue.substr(0, 25).append("...") : fieldValue) << "\t" \
             << setw( 60 ) << left << fieldShowName << endl;
    }    
    return;
}

void initFilterFields(void) {
    filterFields.clear();
    
    // geninfo
    //filterFields.push_back("timestamp");
    
    // frame
    filterFields.push_back("frame.time");
    filterFields.push_back("frame.time_epoch");
    filterFields.push_back("frame.protocols");
    
    // ip
    filterFields.push_back("ip.id");
    filterFields.push_back("ip.src");
    filterFields.push_back("ip.dst");
    
    // tcp
    filterFields.push_back("tcp.srcport");
    filterFields.push_back("tcp.dstport");
    filterFields.push_back("tcp.seq");
    filterFields.push_back("tcp.flags.reset");
    filterFields.push_back("tcp.pdu.size");

    // diameter
    // Initial Request
    filterFields.push_back("diameter.hopbyhopid");
    filterFields.push_back("diameter.endtoendid");
    filterFields.push_back("diameter.Origin-Host");
    filterFields.push_back("diameter.User-Name");
    filterFields.push_back("diameter.3GPP-Charging-Id");
    filterFields.push_back("diameter.Rulebase-Id");
    
    filterFields.push_back("diameter.resp_time");
    filterFields.push_back("diameter.flags.request");
    filterFields.push_back("diameter.Session-Id");
    filterFields.push_back("diameter.flags.error");
    filterFields.push_back("diameter.CC-Request-Type");
    filterFields.push_back("diameter.CC-Request-Number");
    filterFields.push_back("diameter.Destination-Host");
    filterFields.push_back("diameter.Subscription-Id-Type");
    filterFields.push_back("diameter.Subscription-Id-Data");
    filterFields.push_back("diameter.Framed-IP-Address");
    filterFields.push_back("diameter.Called-Station-Id");
    filterFields.push_back("diameter.3GPP-GGSN-Address");
    //filterFields.push_back("diameter.3GPP-PDP-Type");
    filterFields.push_back("diameter.3GPP-CG-Address");
    filterFields.push_back("diameter.3GPP-GGSN-MCC-MNC");
    filterFields.push_back("diameter.3GPP-SGSN-MCC-MNC");
    filterFields.push_back("diameter.3GPP-SGSN-Address");
    //filterFields.push_back("gtp.qos_version");
    filterFields.push_back("gtp.qos_delay");
    filterFields.push_back("gtp.qos_peak");
    filterFields.push_back("gtp.qos_precedence");
    filterFields.push_back("gtp.qos_mean");
    
    filterFields.push_back("gtp.qos_traf_class");
    filterFields.push_back("gtp.qos_max_sdu_size");
    filterFields.push_back("gtp.qos_max_ul");
    filterFields.push_back("gtp.qos_max_dl");
    //filterFields.push_back("gtp.qos_guar_ul");
    //filterFields.push_back("gtp.qos_guar_dl");
    // Initial Reply
    filterFields.push_back("diameter.Result-Code");
    // Update Request
    //filterFields.push_back("diameter.avp.code");
    filterFields.push_back("diameter.Rating-Group");
    filterFields.push_back("diameter.Event-Timestamp");
    // Update Reply
    filterFields.push_back("diameter.Origin-Host");
    filterFields.push_back("diameter.Destination-Host");
    filterFields.push_back("diameter.CC-Total-Octets");
    filterFields.push_back("diameter.Validity-Time");
    filterFields.push_back("diameter.Result-Code");
    filterFields.push_back("diameter.Nokia-URI");
    // Termination Request
    filterFields.push_back("diameter.Termination-Cause");
    filterFields.push_back("diameter.CC-Input-Octets");
    filterFields.push_back("diameter.CC-Output-Octets");
    filterFields.push_back("diameter.Reporting-Reason");
    filterFields.push_back("diameter.3GPP-Session-Stop-Indicator");
    filterFields.push_back("diameter.Time-Of-First-Usage");
    filterFields.push_back("diameter.Time-Of-Last-Usage");
    // Termination Reply

}




void traverse_xml_recursion_main(const std::string& input_xml)
{
    using namespace rapidxml;
    using namespace std;
    
    initFilterFields();
    
    
    // make a safe-to-modify copy of input_xml
    // (you should never modify the contents of an std::string directly)
    vector<char> xml_copy(input_xml.begin(), input_xml.end());
    xml_copy.push_back('\0');
    
    // only use xml_copy from here on!
    xml_document<> doc;
    // we are choosing to parse the XML declaration
    // parse_no_data_nodes prevents RapidXML from using the somewhat surprising
    // behavior of having both values and data nodes, and having data nodes take
    // precedence over values when printing
    // >>> note that this will skip parsing of CDATA nodes <<<
    doc.parse<parse_declaration_node | parse_no_data_nodes>(&xml_copy[0]);
    
    // alternatively, use one of the two commented lines below to parse CDATA nodes, 
    // but please note the above caveat about surprising interactions between 
    // values and data nodes (also read http://www.ffuts.org/blog/a-rapidxml-gotcha/)
    // if you use one of these two declarations try to use data nodes exclusively and
    // avoid using value()
    //doc.parse<parse_declaration_node>(&xml_copy[0]); // just get the XML declaration
    //doc.parse<parse_full>(&xml_copy[0]); // parses everything (slowest)
    
    // since we have parsed the XML declaration, it is the first node
    // (otherwise the first node would be our root node)
    string version = doc.first_node()->first_attribute("version")->value();
    // encoding == "utf-8"
    
    // we didn't keep track of our previous traversal, so let's start again
    // we can match nodes by name, skipping the xml declaration entirely
    xml_node<>* topNode = doc.first_node("pdml");
    string pdmlCreator = topNode->first_attribute("creator")->value();
    string pdmlTime = topNode->first_attribute("time")->value();
    string pdmlCaptureFile = topNode->first_attribute("capture_file")->value();
    
    
    
    // go straight to the first evendeepernode
    xml_node<>* packetNode = topNode->first_node("packet");
    
    do {
        if (!packetNode) break;
        cout << "###########################################################################################################################" << endl;
        cout << "# Packet"<< endl;
        cout << "###########################################################################################################################" << endl;
        rapidxml::xml_node<>* protoNode = packetNode->first_node("proto");
        
        do {
            if (!protoNode) break;
            string attrName;
            string attrShowName;

            attrName = protoNode->first_attribute("name")->value();
            attrShowName = protoNode->first_attribute("showname")->value();
            cout << attrName << endl;
            if(attrName.compare("geninfo")==0 ||
               attrName.compare("frame")==0 ||
               attrName.compare("ip")==0 || 
               attrName.compare("tcp")==0 ||
               attrName.compare("diameter")==0) {
                traverse_xml_recursion_detail(protoNode, 0);
            }
        } while ((protoNode = protoNode->next_sibling("proto")));
    } while ((packetNode = packetNode->next_sibling("packet")));


}

// level: recursion level
void traverse_xml_recursion_detail(rapidxml::xml_node<> *xml_node, int level)
{
    using namespace rapidxml;
    using namespace std;
    
    rapidxml::xml_node<>* protoNode = xml_node->first_node("field");    
    do {
        if (!protoNode) break;
        string fieldShowName="No Data";
        string fieldName="No Data";
        string fieldShow="No Data";
        string fieldValue="No Data";

        if(protoNode->first_attribute("name")) {
            fieldName = protoNode->first_attribute("name")->value();
            if (fieldName.compare("") == 0)
                fieldName = "Empty";
        }
        if(protoNode->first_attribute("showname"))
            fieldShowName = protoNode->first_attribute("showname")->value();
        if(protoNode->first_attribute("show"))
            fieldShow = protoNode->first_attribute("show")->value();
        if(protoNode->first_attribute("value"))
            fieldValue=protoNode->first_attribute("value")->value();
        
        fieldsProcess(level, fieldName, fieldShowName, (fieldValue.compare("No Data")==0?fieldShow:fieldValue));
        
        traverse_xml_recursion_detail(protoNode, level+1);
    } while ((protoNode = protoNode->next_sibling("field")));
}