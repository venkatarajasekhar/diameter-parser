//
//  main.cpp
//  Dia
//
//  Created by Oleg Serdyukov on 29.09.11.
//  Copyright 2011 The Apple Geek. All rights reserved.
//
#include <string>

using namespace std;

#include "DiameterDictionary.hpp"
#include "DiameterPacket.hpp"
#include "IPPacket.hpp"
#include "AVP.hpp"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

DiameterDictionary diameterDictionary;

int main (int ac, char* av[])
{
	// Разбор командной строки
	string pcapFile, dictionaryFile;
	try {
		po::options_description desc("Allowed options");
		desc.add_options()
		("help", "help message")
		("pcap,p", po::value<string>(), 
		 "pcap file to proceed")
		("dictionary,d", po::value<string>(), "diameter dictionary file")
		;
		
		po::positional_options_description p;
		p.add("dictionary", -1);
		
		po::variables_map vm;
		po::store(po::command_line_parser(ac, av).
				  options(desc).positional(p).run(), vm);
		po::notify(vm);

		if (vm.count("help"))
		{
			cout << desc << "\n";
		}

		if (vm.count("pcap"))
		{
			pcapFile=vm["pcap"].as<string>();
		}
		
		if (vm.count("dictionary"))
		{
			dictionaryFile=vm["dictionary"].as<string>();
		}
		
    }
    catch(exception& e) {
        cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        cerr << "Exception of unknown type!\n";
    }
	
	// Загружаю dictionary по Diameter
	if (dictionaryFile.size()==0)
		dictionaryFile = "/Users/ctrld/Documents/Development/Dia/Data/dictionary.xml";

	if(diameterDictionary.load_xml_file(dictionaryFile.c_str()) != 1) {
		cerr << "Could not load diameter dictionary " << dictionaryFile << endl;
		return 2;
	}

    string file = "/Users/ctrld/Documents/Development/Dia/Data/mul04.pcap";
    //string file = "/Users/ctrld/Documents/Development/Dia/Data/diameter_00037_20111130105028";
    //string file = "/Users/ctrld/Documents/Development/Dia/Data/1.pcap";
	//string file = "/Users/ctrld/Documents/Development/Dia/Data/n.pcap";
	//string file = "/Users/ctrld/Documents/Development/Dia/Data/o.pcap";
	//string file = "/Users/ctrld/Documents/Development/Dia/Data/pac05.pcap";
	
	if (pcapFile.size() == 0)
		pcapFile = file;
	if (parsePcapFile(pcapFile) == -1)
		return 1;
	
    return 0;
}

