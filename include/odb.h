#ifndef CODB_H
#define CODB_H

/**
 * @file odb.h
 *
 * @author Manfred Morgner
 * @date 26.12.2017
 */

#include <string>
#include <memory>    // shared_ptr
#include <iostream>  // cout

#include "generic.h"

#include "atom.h"
#include "thing.h"
#include "property.h"
#include "reason.h"
#include "strand.h"


namespace odb {


/**
 *  @brief The database
 *
 *  A Object Database itself is an Identifiable object. Enabeling multiple
 *  database instances in one application.
 *
 *  @author Manfred Morgner
 *  @since  0.1.17
 */
class COdb : public Identifiable<COdb>
    {
    public:
                /**
                 * @brief Creates a COdb, a database
                 *
                 * We only default construct the database, no copy construction
                 * no copy of the database at all. It is not known, what copying
                 * the database means.
                 */
                 COdb() = default;
                 /**
		  * @brief deleted: copy constructor
		  *  
		  * Copying a databse is not supported yet. We will support it
                  * as soon as we find out what it means.
		  */ 
                 COdb(COdb const & src) = delete;
                 /**
		  * @brief deleted: Assignment operator
		  *
		  * Here we are in the same situation as with copy construction
		  */
                 COdb & operator = (COdb const & src) = delete;
                 /**
                  * @brief Some cleanup
                  *  
                  * If we wish to convience valgrind we are doing it right,
                  * we have to free all links to other objects before leaving
                  * the show.
                  */
        virtual ~COdb()
                    {
                    clear();
                    }

        /**
         * @brief Frees all objects
         *
         * Frees all known objects at last so far that freeing the
         * object collections releases all memory, bound to them.
         * So valgrind will find nothing left on the table.
         */
        void clear();

        /**
         * @brief Creates a PThing
         *
         * Creates a shared_ptr with a new CThing named as given in the
         * call. If no name is given, the name will be the class default
         *
         * @param crsName The name for the CThing
         */
#ifdef __DOXYGEN__
        PThing MakeThing(std::string const & crsName = "")
#else
        auto MakeThing(std::string const & crsName = ""s)
#endif
            {
            auto p = std::make_shared<CThing>(crsName);
            m_oThings.push_back( p );
            return std::move( p );
            }

        /**
         * @brief Creates a PProperty
         *
         * Creates a shared_ptr with a new CProperty named as given in
         * the call. If no name is given, the name will be the class 
         * default
         *
         * @param crsName The name for the CProperty
         */
#ifdef __DOXYGEN__
        PProperty MakeProperty(std::string const & crsName = "")
#else
        auto MakeProperty(std::string const & crsName = ""s)
#endif
            {
            auto p = std::make_shared<CProperty>(crsName);
            m_oProperties.push_back( p );
            return std::move( p );
            }

        /**
            @brief Creates a PAtom

            Creates a shared_ptr with a new CAtom named as given in the
            call. If no name is given, the name will be the class default

            The data type of the data element in CAtom follows the input
            data type. It can be if any primitive type or most of the
            simple containers, like string or vector.

            CAtom further on manages the life time of the data element.
	    It's a unique_ptr

            @param data The data for the CAtom
            @param crsName The name for the CAtom
            @param crsPrefix The prefix for the CAtom in a GUI
            @param crsSuffix The suffix for the CAtom in a GUI
            @param crsFormat The format for the CAtom in a GUI
         */
#ifdef __DOXYGEN__
        PAtom const MakeAtom(
            int data,
            std::string const & crsName   = "",
            std::string const & crsPrefix = "",
            std::string const & crsSuffix = "",
            std::string const & crsFormat = "")
#else
        template<typename T>
        auto const MakeAtom(
            T data,
            std::string const & crsName   = ""s,
            std::string const & crsPrefix = ""s,
            std::string const & crsSuffix = ""s,
            std::string const & crsFormat = ""s)
#endif
            {
            auto p = std::make_shared<CAtom>(data, crsName, crsPrefix, crsSuffix, crsFormat);
            m_oAtoms.push_back( p );
            return std::move( p );
            }

        /**
         * @brief Creates a PReason
         *
         * Creates a shared_ptr with a new CReason named as given in the
         * call. If no name is given, the name will be the class default
         *
         * @param crsName The name for the CReason
         */
#ifdef __DOXYGEN__
        PReason MakeReason(std::string const & crsName = "")
#else
        auto MakeReason(std::string const & crsName = ""s)
#endif
            {
            auto p = std::make_shared<CReason>(crsName);
            m_oReasons.push_back( p );
            return std::move( p );
            }

        /**
         * @brief Creates a PStrand
         *
         * Creates a shared_ptr with a new CStrand named as given in the
         * call. If no name is given, the name will be the class default
         *
         * @param crsName The name for the CStrand
         */
#ifdef __DOXYGEN__
	PStrand MakeStrand(std::string const & crsName = "")
#else
	auto MakeStrand(std::string const & crsName = ""s)
#endif
            {
            auto p = std::make_shared<CStrand>(crsName);
            m_oStrands.push_back( p );
            return std::move( p );
            }

        /**
         * @brief Print out the database (Informative format)
         */
        void print()
            {
            print(m_oThings);
            print(m_oAtoms);
            print(m_oProperties);
            print(m_oReasons);
            } // void print()

        /**
         * @brief Print out container of CAtom objects
         *
         * @param crContainer The forward iterable container, containing
	 *        PAtom's
         */
        void print(CAtoms const & crContainer)
            {
            for (auto && e : crContainer)
                {
                std::cout << e->type << '\t' << " id: " << e->id << '\t'
                          << " name: " << e->m_sName << '\t'
                          << '(' << e.use_count() << ')' << '\t'
                          << " data: " << *e << '\n';
                }
            } // void print(CAtoms const & crContainer)

        /**
         * @brief Print out container of identifiable objects
         *
         * @tparam T A shared_ptr of an 'Identifiable' type, enriched with
	 *         a memberm_sName
         * @param  crContainer The forward iterable container, containing
	 *         all T instances
         */
        template<typename T>
        void print(std::deque<T> const & crContainer)
            {
            for (auto const & e:crContainer)
                {
                std::cout << e->type << '\t' << " id: " << e->id << '\t'
                          << " name: " << e->m_sName << '\t'
                          << '(' << e.use_count() << ')' << '\n';
                }
            } // void print(std::deque<T> const & crContainer)


        /**
         * @brief Dump all CThings in Sub-JSON format
         *
         * @param crContainer The forward iterable container, containing
	 *        all CThing instances
         * @param ros The output destination
         */
        void print_json(CThings const & crContainer, std::ostream & ros)
            {
            std::size_t cm{crContainer.size()};
            std::size_t cc{0};
            ros << spcr<2> << "\"Things\": " << '\n';
            ros << spcr<3> << '[' << '\n';
            for (auto const & e:crContainer)
                {
                ros << spcr<4> << "{ ";
//              ros            << "\"type\": \"" << e->type    << "\", ";
                ros            << "\"id\": \""   << e->id      << "\", ";
                ros            << "\"name\": \"" << e->m_sName << "\",\n";

                ros << spcr<5> << "\"properties\": [ ";
                long lc{0};     // Block counter
                bool lb{false}; // Start signal
                for (auto const & a:e->m_spoProperties)
                    {
                    if ( !lb ) { lb=true; ros << ""; } else { ros << ","; }
                    ++lc;
                    if ( lc % 5 == 0 & lc > 1 ) { ros << "\n" << spcr<6> ; }
                    ros << "{\"id\": \"" << a->id << "\"}";
                    }
                ros << " ],\n";

                ros << spcr<5> << "\"atoms\": [ ";
                lc = 0;     // Block counter
                lb = false; // Start signal
                for (auto const & a:e->m_spoAtoms)
                    {
                    if ( !lb ) { lb=true; ros << ""; } else { ros << ","; }
                    ++lc;
                    if ( lc % 5 == 0 & lc > 1 ) { ros << "\n" << spcr<6> ; }
                    ros << "{\"id\": \"" << a->id << "\"}";
                    }
                ros << " ],\n";

                ros << spcr<5> << "\"links\": [ ";
                lc = 0;     // Block counter
                lb = false; // Start signal
                for (auto const & b:e->m_mLink)
                    {
                    if ( !lb ) { lb=true; ros << ""; } else { ros << ","; }
                    ++lc;
                    if ( lc % 3 == 0 & lc > 1 ) { ros << "\n" << spcr<6>; }
                    ros << "{\"thing-id\": \"" << b.first->id << "\", \"reason-id\": \"" << b.second->id << "\"}";
                    }
                if ( ++cc < cm ) { ros << " ] },\n"; } else { ros << " ] }\n"; }
                }
            ros << spcr<3> << "]\n";
            } // void print_json(CThings const & crContainer, std::ostream & ros)

        /**
         * @brief Dump all CProperty's in Sub-JSON format
         *
         * @param crContainer The forward iterable container, containing
	 *        all CProperty instances
         * @param ros The output destination
         */
        void print_json(CProperties const & crContainer, std::ostream & ros)
            {
            std::size_t cm{crContainer.size()};
            std::size_t cc{0};
            ros << spcr<2> << "\"Properties\": " << '\n';
            ros << spcr<3> << '[' << '\n';
            for ( auto const & a:crContainer )
                {
                ros << spcr<4> << "{ ";
                ros            << "\"id\": \""     <<  a->id        << "\", ";
                ros            << "\"name\": \""   <<  a->m_sName   << "\" ";
                if ( ++cc < cm ) { ros << "},\n"; } else { ros << "}\n"; }
                }
            ros << spcr<3> << "],\n";
            } // void print_json(CProperties const & crContainer, std::ostream & ros)
            
        /**
         * @brief Dump all CAtoms in Sub-JSON format
         *
         * @param crContainer The forward iterable container, containing
	 *        all CAtom instances
         * @param ros The output destination
         */
        void print_json(CAtoms const & crContainer, std::ostream & ros)
            {
            std::size_t cm{crContainer.size()};
            std::size_t cc{0};
            ros << spcr<2> << "\"Atoms\": " << '\n';
            ros << spcr<3> << '[' << '\n';
            for ( auto const & a:crContainer )
                {
                                           ros << spcr<4> << "{ ";
//                                         ros << "\"type\": \""   << a->type      << "\", ";
                                           ros << "\"id\": \""     <<  a->id        << "\", ";
                if ( a->m_sName.size() )   ros << "\"name\": \""   <<  a->m_sName   << "\", ";
                if ( a->m_sPrefix.size() ) ros << "\"prefix\": \"" <<  a->m_sPrefix << "\", ";
                if ( a->m_sSuffix.size() ) ros << "\"suffix\": \"" <<  a->m_sSuffix << "\", ";
                if ( a->m_sFormat.size() ) ros << "\"format\": \"" <<  a->m_sFormat << "\", ";
                                           ros << "\"data\": \""   << *a            << "\" ";
                if ( ++cc < cm ) { ros << "},\n"; } else { ros << "}\n"; }
                }
            ros << spcr<3> << "],\n";
            } // void print_json(CAtoms const & crContainer, long const nRI, std::ostream & ros)

        /**
         * @brief Dump all CReasons in Sub-JSON format
         * @param crContainer The forward iterable container, containing
	 *        all CReason instances
         * @param ros The output destination
         */
        void print_json(CReasons const & crContainer, std::ostream & ros)
            {
            std::size_t cm{crContainer.size()};
            std::size_t cc{0};
            ros << spcr<2> << "\"Reasons\": " << '\n';
            ros << spcr<3> << '[' << '\n';
            for ( auto const & a:crContainer )
                {
                ros << spcr<4> << "{ ";
                ros            << "\"id\": \""     << a->id        << "\", ";
                ros            << "\"name\": \""   << a->m_sName   << "\" ";
                if ( ++cc < cm ) { ros << "},\n"; } else { ros << "}\n"; }
                }
            ros << spcr<3> << "],\n";
            }


        /**
         * @brief Dump the hole database in JSON format
         *
         * see also: JSON validator+converter: https://jsonformatter.org/
         *
         * @param ros The output destination
         */
        void print_json(std::ostream & ros)
            {
            ros << spcr<0> << '{' << '\n';
            ros << spcr<1> << "\"Object Database Dump\": " << '\n';
            ros << spcr<2> << '{' << '\n';

            print_json(m_oProperties, ros);
            print_json(m_oAtoms,      ros);
            print_json(m_oReasons,    ros);
            print_json(m_oThings,     ros);

            ros << spcr<2> << '}' << '\n';
            ros << spcr<0> << '}' << '\n';
          }

	/// Appends a Property to a Thing by given index value
        bool AppendProperty2Thing( size_t nThing, size_t nProperty )
            {
            if ( (nThing < m_oThings.size()) & (nProperty < m_oProperties.size()) )
		{
                m_oThings[nThing]->Append( m_oProperties[nProperty] );
		}
	    else
	        {
		return false;
		}
	    return true;
            }

	/// Appends a Property to a Thing by given names
	bool AppendProperty2Thing( std::string const & crsProperty, std::string const & crsThing )
            {
            auto itProperty = std::find(m_oProperties.begin(), m_oProperties.end(), crsProperty);
            if ( itProperty == m_oProperties.end() )
	        {
		return false;
		}

	    for ( auto & a:m_oThings )
	        {
		if ( a->m_sName == crsThing ) a->Append( *itProperty );
	        }

	    return false;
            }

        /// Access function to call then container of CThing's
        CThings      const & Things () const { return m_oThings;  }
        /// Access function to call then container of CProperties
        CProperties  const & Properties () const { return m_oProperties; }
        /// Access function to call then container of CAtom's
        CAtoms       const & Atoms  () const { return m_oAtoms;   }
        /// Access function to call then container of CReason's
        CReasons     const & Reasons() const { return m_oReasons; }
        /// Access function to call then container of CStrand's
        CStrands     const & Strands() const { return m_oStrands; }
    protected:
        /// A container instance of CThing's
        CThings      m_oThings;
        /// A container instance of CProperties
        CProperties  m_oProperties;
        /// A container instance of CAtom's
        CAtoms       m_oAtoms;
        /// A container instance of CReason's
        CReasons     m_oReasons;
        /// A container instance of CStrand's
        CStrands     m_oStrands;

    }; // class COdb

} // namespace odb

// CODB_H
#endif
