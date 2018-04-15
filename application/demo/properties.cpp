/**
    @file properties.cpp
 
    @author Manfred Morgner
    @date 15.04.2018

    Demonstrates how Properties are added to the mix. Here the properties are
    autogenerated to simplify the program. Even if they are generated 
    automatically, they are only generated ones each. Automatic helper
    functions prevent the odb from becoming filled up with duplicates
 */

#include <iostream>

#include "generic.h"
#include "odb.h"
#include "atom.h"
#include "thing.h"

auto oOdb = odb::COdb();


// Demo main program
int main()
    {
    // 3 people
    oOdb.MakeThing("Udo");
    oOdb.MakeThing("Ina");
    oOdb.MakeThing("Rob");

    // 3 kind of relation
    oOdb.MakeReason("is father of");
    oOdb.MakeReason("knows");
    oOdb.MakeReason("loves");
    
    // 3 bindings
    oOdb.LinkThing2Thing("Udo", "is father of", "Ina");
    oOdb.LinkThing2Thing("Udo", "knows"       , "Rob");
    oOdb.LinkThing2Thing("Ina", "loves"       , "Rob");


    oOdb.AppendProperty2Thing("sex:male"  , true, "Udo");
                                                       
    oOdb.AppendProperty2Thing("born:1999" , true, "Ina");
    oOdb.AppendProperty2Thing("sex:female", true, "Ina");
                                                       
    oOdb.AppendProperty2Thing("sex:male"  , true, "Rob");

    // show us
    std::cout << "---------------- all things" << '\n';
    for ( auto const & a:oOdb.Things() )
        {
        std::cout << *a << '\n';
        }
    }

