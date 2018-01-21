/**
    @file main.cpp
 
    @author Manfred Morgner
    @date 21.01.2018
 */

#include <iostream>

#include "odb.h"
#include "thing.h"

/**
    @brief Demo main program
 */
int main()
    {
    auto oOdb    = odb::COdb();
    auto pThing1 = oOdb.MakeThing("Ulrich");
    auto pThing2 = oOdb.MakeThing("Fred");
    auto pReason = oOdb.MakeReason("pays");
    pThing1->Link(pThing2, pReason);
    std::cout << "thing: " << *pThing1 << '\n';
    std::cout << "thing: " << *pThing2 << '\n';
    oOdb.print();
    oOdb.print_json(std::cout);
    }
