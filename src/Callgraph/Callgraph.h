// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#ifndef CALLGRAPH_CALLGRAPH_H
#define CALLGRAPH_CALLGRAPH_H

#include <boost/tr1/tuple.hpp>
#include <map>
#include <algorithm>
#include <iterator>
#include <utility>

#include "../AnalysisProps.h"
#include "../Languages.h"

namespace llvm { namespace callgraph {

    template<typename Language>
    struct ProgramFunction;

    template<typename LanguageType>
    struct BasicCallgraph {
        typedef LanguageType Language;
        typedef typename ProgramFunction<Language>::Type Function;
        typedef std::multimap<Function,Function> Container;
        typedef typename Container::key_type key_type;
        typedef typename Container::mapped_type mapped_type;
        typedef typename Container::value_type value_type;
        typedef typename Container::iterator iterator;
        typedef typename Container::const_iterator const_iterator;
        typedef std::pair<const_iterator,const_iterator> range_iterator;

        virtual ~BasicCallgraph() {}

        iterator insertDirectCall(value_type const& val)
        { return directCallsMap.insert(val); }

        range_iterator directCalls(key_type const& key) const
        { return directCallsMap.equal_range(key); }

        range_iterator directCallees(key_type const& key) const
        { return directCalleesMap.equal_range(key); }

        range_iterator calls(key_type const& key) const
        { return callsMap.equal_range(key); }

        range_iterator callees(key_type const& key) const
        { return calleesMap.equal_range(key); }

        const_iterator begin() const { return directCallsMap.begin(); }
        iterator begin() { return directCallsMap.begin(); }
        const_iterator end() const { return directCallsMap.end(); }
        iterator end() { return directCallsMap.end(); }
        const_iterator begin_closure() const { return callsMap.begin(); }
        const_iterator end_closure() const { return callsMap.end(); }
        Container const& getContainer() const { return directCallsMap; }
        Container& getContainer() { return directCallsMap; }
    protected:
        void computeRemainingDictionaries();
    private:
        Container directCallsMap;
        Container directCalleesMap;
        Container callsMap;
        Container calleesMap;
    };

    template<typename Language>
    struct Callgraph {
        typedef BasicCallgraph<Language> Type;
    };

}}

namespace llvm { namespace callgraph { namespace detail {

  template<typename Relation>
  void computeTransitiveClosure(Relation const& R, Relation& TCR) {
    typedef std::set<typename Relation::value_type> Set;
    typedef std::multimap<typename Relation::value_type::first_type,
                          typename Relation::value_type::second_type> Dict;

    Set S;
    std::copy(R.begin(),R.end(),std::inserter(S,S.end()));

    Dict D;
    std::copy(R.begin(),R.end(),std::inserter(D,D.end()));

    while (true)
    {
      std::size_t const old_size = S.size();

      for (typename Set::const_iterator it = S.begin(); it != S.end(); ++it) {
        typename Dict::const_iterator b,e;
        std::tr1::tie(b,e) = D.equal_range(it->second);
        for ( ; b != e; ++b)
          S.insert(typename Set::value_type(it->first,b->second));
      }

      if (old_size == S.size())
          break;
    }

    std::copy(S.begin(),S.end(),std::inserter(TCR,TCR.end()));
  }

}}}

namespace llvm { namespace callgraph {

    template<typename Language>
    typename Callgraph<Language>::Type::range_iterator
    getDirectCalls(
        typename Callgraph<Language>::Type::key_type const& key,
        typename Callgraph<Language>::Type const& CG) {
        return CG.directCalls(key);
    }

    template<typename Language>
    typename Callgraph<Language>::Type::range_iterator
    getDirectCallees(
        typename Callgraph<Language>::Type::key_type const& key,
        typename Callgraph<Language>::Type const& CG)
    {
        return CG.directCallees(key);
    }

    template<typename Language>
    typename Callgraph<Language>::Type::range_iterator
    getCalls(
        typename Callgraph<Language>::Type::key_type const& key,
        typename Callgraph<Language>::Type const& CG)
    {
        return CG.calls(key);
    }

    template<typename Language>
    typename Callgraph<Language>::Type::range_iterator
    getCallees(
        typename Callgraph<Language>::Type::key_type const& key,
        typename Callgraph<Language>::Type const& CG)
    {
        return CG.callees(key);
    }

    template<typename Language>
    void BasicCallgraph<Language>::computeRemainingDictionaries()
    {
        detail::computeTransitiveClosure(directCallsMap,callsMap);
        for (const_iterator it = begin(); it != end(); ++it)
            directCalleesMap.insert(value_type(it->second,it->first));
        for (const_iterator it = callsMap.begin(); it != callsMap.end(); ++it)
            calleesMap.insert(value_type(it->second,it->first));
    }
#if 0
    template<typename Language>
    std::ostream& dump(std::ostream& ostr, BasicCallgraph<Language> const& CG)
    {
        if (CG.begin() != CG.end())
            ostr << "--- Direct calls ---\n";
        for (typename BasicCallgraph<Language>::const_iterator
                it = CG.begin(), old_it = CG.end(); it != CG.end(); ++it)
        {
            using monty::codespy::dump;
            if (old_it == CG.end() || it->first != old_it->first)
            {
                ostr << "  ";
                dump(ostr,it->first);
                ostr << '\n';

                old_it = it;
            }
            ostr << "    ";
            dump(ostr,it->second);
            ostr << '\n';
        }
        if (CG.begin_closure() != CG.end_closure())
            ostr << "--- Transitive closure of calls ---\n";
        for (typename BasicCallgraph<Language>::const_iterator
                it = CG.begin_closure(), old_it = CG.end_closure();
                it != CG.end_closure(); ++it)
        {
            using monty::codespy::dump;
            if (old_it == CG.end_closure() || it->first != old_it->first)
            {
                ostr << "  ";
                dump(ostr,it->first);
                ostr << '\n';

                old_it = it;
            }
            ostr << "    ";
            dump(ostr,it->second);
            ostr << '\n';
        }
        return ostr;
    }
#endif
}}

#endif