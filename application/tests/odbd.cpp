/**
    @file main.cpp
 
    @author Manfred Morgner
    @date 22.03.2018

    to use this server:

    Save a odb to file (JSON) and name it wodb.json. Put this file in the
    same directory as you run the server in and start the server. E.g. by:

        ./odbd 1025 &

    After loading the database a statistic will be print to the console and
    the tcp server starts listening for client connections.

 */

#include <iostream>
#include <memory>
#include <array>

#include <vector>
#include <algorithm>
#include <iterator>

#include <fstream>
#include <json/json.h>

#include "generic.h"
#include "odb.h"
#include "atom.h"
#include "thing.h"

using namespace std::string_literals;

auto poOdb = std::make_unique<odb::COdb>();

template<typename ...Args> void mkthings    (Args&&... args) { (poOdb->MakeThing   (args), ...); } 
template<typename... Args> void mkproperties(Args&&... args) { (poOdb->MakeProperty(args), ...); }
template<typename... Args> void mkreasons   (Args&&... args) { (poOdb->MakeReason  (args), ...); }
template<typename... Args> void mkatoms     (Args&&... args) { (poOdb->MakeAtom    (args, "fold" ), ...); }

// append a property to a group of things, if property does not exists and
// 'cbForce' is 'true', the property will be added to the DB
template<typename... Args>
void ap2ts(std::string const & crsProperty, // name of the property
                  bool const   cbForce,     // create it if not existent?
                     Args&&... args)        // pack of names of 'things'
    {
    (poOdb->AppendProperty2Thing(crsProperty, cbForce, args), ...);
    }
        
template<typename... Args>
void lt2t(std::string const & crsNameTo, // name of the property
          std::string const & crsReason, // create it if not existent?
                    Args&&... args)      // pack of names of 'things'
    {
    (poOdb->LinkThing2Thing(args, crsNameTo, crsReason), ...);
    }

void FillInSomeData()
    {
    // filling in some data
    // ================================================================================================
    mkthings    ("Ulli", "Nora", "Peter", "Paula", "Rudi", "Marta", "Arnold", "Bertha", "Elise", "Jack");
    mkthings    ("Emerald woods", "Madix", "Skoda", "Trombone", "Lecho", "SilentOS", "Insurance");
    mkproperties("person", "male", "female", "driver", "consumer", "contractor");
    mkreasons   ("wrote", "read", "bought", "left", "foundet", "loves", "sells", "works at", "uses", "plays");
    mkatoms     ( 2.5, "done", 7, std::array{2,1,3}, "go", 89, "sold", "percent");
    // ================================================================================================

    // ================================================================================================
    lt2t("Emerald woods", "wrote",    "Ulli"                                                         );
    lt2t("Emerald woods", "read",     "Nora",   "Peter", "Paula", "Rudi", "Marta", "Arnold", "Bertha");
    lt2t("Trombone",      "bought",   "Peter"                                                        );
    lt2t("Rudi",          "left",     "Paula",  "Elise", "Marta"                                     );
    lt2t("Lecho",         "foundet",  "Rudi"                                                         );
    lt2t("Peter",         "loves",    "Marta",  "Jack"                                               );
    lt2t("Insurance",     "sells",    "Arnold"                                                       );
    lt2t("Skoda",         "works at", "Bertha", "Ulli", "Nora", "Arnold"                             );
    lt2t("SilentOS",      "uses",     "Elise"                                                        );
    lt2t("Trombone",      "plays",    "Jack",   "Peter"                                              );
    // ================================================================================================


    // give all 'things' the property 'person'
    for ( size_t n = 0; n < poOdb->Things().size(); ++n )
        {
        poOdb->AppendProperty2Thing( 0, n );
        }

    // assign 'properties' to groups of 'things' (supported by fold expressions)
    // ========================================================================================================
    ap2ts( "person",     false, "Ulli", "Nora", "Peter", "Paula", "Rudi", "Marta", "Arnold", "Bertha", "Elise");
    ap2ts( "male",       false, "Ulli",         "Peter",          "Rudi",          "Arnold"                   );
    ap2ts( "female",     false,         "Nora",          "Paula",         "Marta",           "Bertha", "Elise");
    ap2ts( "driver",     false, "Ulli",                  "Paula", "Rudi", "Marta",           "Bertha"         );
    ap2ts( "consumer",   false, "Ulli", "Nora", "Peter",                                               "Elise");
    ap2ts( "contractor", false,                 "Peter", "Paula", "Rudi",                              "Elise");
    ap2ts( "artist",     true,  "Ulli",                                            "Arnold", "Bertha", "Elise");
    ap2ts( "builder",    true,                  "Peter", "Paula",                  "Arnold"                   );
    // ========================================================================================================

    auto px = poOdb->MakeThing("Ulli");
    poOdb->AppendProperty2Thing( 0, px->m_nId );
    poOdb->AppendProperty2Thing( 6, px->m_nId );
    poOdb->AppendProperty2Thing( 7, px->m_nId );

    } // void FillInSomeData()


#include <asio/ip/tcp.hpp>
#include <asio/io_service.hpp>

using asio::ip::tcp;


bool Insert(std::string const & crsQuery, tcp::iostream & ros)
    {
    ros << "+ " + crsQuery;

    if ( crsQuery.length() < 2 ) return false;

    char c = crsQuery[0];
    std::string sInput = crsQuery.substr(2);

    odb::PThing    t;
    odb::PReason   r;
    odb::PProperty p;
    odb::PAtom     a;

    switch (c)
        {
        case 't': t = poOdb->MakeThing(sInput);    ros << " \n" << ":" << t->m_nId << ":" << *t << " \n"; break;
        case 'r': r = poOdb->MakeReason(sInput);   ros << " \n" << ":" << r->m_nId << ":" << *r << " \n"; break;
        case 'p': p = poOdb->MakeProperty(sInput); ros << " \n" << ":" << p->m_nId << ":" << *p << " \n"; break;
        case 'a': a = poOdb->MakeAtom(sInput);     ros << " \n" << ":" << a->m_nId << ":" << *a << " \n"; break;

        default : ros << ": no insert";
        }

    return true;
    }

template<typename T>
void SendResult(T const & croData, std::iostream & ros, char const ccSwitch)
    {
    if (ccSwitch == 'j') 
        {
        ros << " \n { \n";
        poOdb->print_json(croData, ros);
        ros << " \n } \n";
        return;
        }

    if (ccSwitch == 'c') 
        {
        ros << " \n";
        }
    else
        {
        for (auto const & a:croData) 
            {
            ros << " \n" << ":" << a->m_nId << ":";
            if ( (ccSwitch == ':') || ((ccSwitch >= 'A')&&(ccSwitch <= 'Z')) )
                {
                ros << *a << " \n";
                }
            else
                {
                ros << (*a).m_sName;
                }
            }

        }

    if (ccSwitch == '.')
        {
        ros << " \n";
        }

    ros << " \n  total: " << croData.size() << " \n";
    }

bool Answer(std::string const & crsQuery, tcp::iostream & ros)
    {
    ros << "? " + crsQuery;

    if ( crsQuery.length() < 2 ) return false;

    char c = crsQuery[0];
    char d = crsQuery[1];
    std::string sInput = crsQuery.substr(2);

    odb::CThings     ts;
    odb::CReasons    rs;
    odb::CProperties ps;
    odb::CAtoms      as;

    switch (c)
        {
        case 't': ts = poOdb->Find(poOdb->Things(),std::string( sInput )); if (ts.size() == 0) ts = poOdb->Find(poOdb->Things(),std::regex( sInput ));
                  SendResult(ts, ros, d);
                  break;
        case 'r': rs = poOdb->Find(poOdb->Reasons(),std::string( sInput )); if (rs.size() == 0) rs = poOdb->Find(poOdb->Reasons(),std::regex( sInput ));
                  SendResult(rs, ros, d);
                  break;
        case 'p': ps = poOdb->Find(poOdb->Properties(),std::string( sInput )); if (ps.size() == 0) ps = poOdb->Find(poOdb->Properties(),std::regex( sInput ));
                  SendResult(ps, ros, d);
                  break;
        case 'a': as = poOdb->Find(poOdb->Atoms(),std::string( sInput )); if (as.size() == 0) as = poOdb->Find(poOdb->Atoms(),std::regex( sInput ));
                  SendResult(as, ros, d);
                  break;
        default : ros << ": no result";
                  return false;
        }

    return true;
    } // bool Answer(std::string const & crsQuery, tcp::iostream & ros)

void SendStatistics(std::ostream & ros)
    {
    ros << "---------------- " <<  poOdb->Things().size()     << " things" << " \n";
    ros << "---------------- " <<  poOdb->Properties().size() << " properties" << " \n";
    ros << "---------------- " <<  poOdb->Reasons().size()    << " reasons" << " \n";
    ros << "---------------- " <<  poOdb->Atoms().size()      << " atoms" << " \n";
    }

int main(int argc, char* argv[])
    {
    if (argc != 2)
        {
        std::cout << "Usage: " << argv[0] << " <ip> <port>\n";
        std::cout << "Example:\n";
        std::cout << "  " << argv[0] << " 1025 &\n";
        return 1;
        }

    auto const sFilename = "wdb.json";

    poOdb->LoadDB(sFilename);

    std::cout << '\n';
    SendStatistics(std::cout);

    try
        {
        asio::io_service io_service;

        tcp::endpoint endpoint(tcp::v4(), std::stoul(argv[1]));
        tcp::acceptor acceptor(io_service, endpoint);

        for (;;)
            {
            tcp::iostream stream;
            asio::error_code ec;
            acceptor.accept(*stream.rdbuf(), ec);
            std::string sQuery;
            std::getline(stream, sQuery);
            std::cout << "> " << sQuery << '\n';
            if ( sQuery == "stat" )
                {
                SendStatistics(stream);
                SendStatistics(std::cout);
                }
            else if ( sQuery == "clean" )
                {
                stream << "Emptying DB, old statistics:\n";
                SendStatistics(stream);
                stream << "Cleaning, please wait\n";
                poOdb = std::make_unique<odb::COdb>();
                SendStatistics(stream);
                SendStatistics(std::cout);
                stream << "Done\n";
                }
            else if ( sQuery == "fillr" )
                {
                stream << "Fill DB with small set of data, old statistics:\n";
                SendStatistics(stream);
                stream << "Cleaning, please wait\n";
                poOdb = std::make_unique<odb::COdb>();
                stream << "Filling, please wait\n";
                FillInSomeData();
                SendStatistics(stream);
                SendStatistics(std::cout);
                stream << "Done\n";
                }
            else if ( sQuery == "load" )
                {
                stream << "Replacing DB, old statistics:\n";
                SendStatistics(stream);
                poOdb = std::make_unique<odb::COdb>();
                stream << "Loading, please wait\n";
                poOdb->LoadDB(sFilename);
                SendStatistics(stream);
                SendStatistics(std::cout);
                stream << "Done\n";
                }
            else if ( sQuery == "save" )
                {
                stream << "Saving DB, statistics:\n";
                SendStatistics(stream);
                poOdb->SaveDB(sFilename);
                stream << "Done\n";
                }
            else if ( sQuery == "help" )
                {
                stream << "help    - shows this help page\n";
                stream << "stat    - shows db statistics\n";
                stream << "clean   . deletes all entries from DB\n";
                stream << "fillr   . fills the DB with test data\n";
                stream << "load    . loads the DB from disk\n";
                stream << "save    . saves the DB to disk\n";
                stream << " \n";
                stream << "t:regex - search for a thing\n";
                stream << "p:regex - search for a property\n";
                stream << "r:regex - search for a reason\n";
                stream << "a:regex - search for an atom\n";
                stream << " \n";
                stream << "t.regex - search for a thing (short result)\n";
                stream << "p.regex - search for a property (short result)\n";
                stream << "r.regex - search for a reason (short result)\n";
                stream << "a.regex - search for an atom (short result)\n";
                stream << "tcregex - search for things, returns only the result count\n";
                stream << "tjregex - search for things, returns result in JSON format\n";
                stream << " \n";
                stream << "Example\n";
                stream << " \n";
                stream << "t:Star (Trek|Wars).*\n";
                stream << "t.Star (Trek|Wars).*\n";
                stream << "tcStar (Trek|Wars).*\n";
                stream << " \n";
                stream << "Searches for all 'things' named \"Star Trek\" or \"Star Wars\"\n";
                stream << " \n";
                stream << "t+name  - insert a thing\n";
                stream << "p+name  - insert a property\n";
                stream << "r+name  - insert a reason\n";
                stream << "a+name  - insert an atom\n";
                }
            else
                {
                if ( sQuery.length() < 2 )
                        {
                        stream << "try: 'help' to get help\n";
                        }
                else if ( (sQuery[1] == ':') || (sQuery[1] == '.') || (sQuery[1] == 'c') || (sQuery[1] == 'j') || (sQuery.substr(0,2) == "tP") || (sQuery.substr(0,2) == "tp") )
                        {
                        if ( not Answer(sQuery, stream) ) stream << ": no result\n";
                        }
                else if ( sQuery[1] == '+' )
                        {
                        if ( not Insert(sQuery, stream) ) stream << ": not inserted\n";
                        }
                else
                    {
                    stream << ": did't understand";
                    }
                }
            stream << ".\n";
            }
        }
    catch (std::exception& e)
        {
        std::cerr << e.what() << std::endl;
        }

    return 0;
    }

