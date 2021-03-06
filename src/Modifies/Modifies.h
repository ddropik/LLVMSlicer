// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#ifndef MODIFIES_MODIFIES_H
#define MODIFIES_MODIFIES_H

#include "PredefContainers.h"

namespace llvm { namespace mods {

    template<typename Algorithm>
    struct Modifies {
        typedef ModifiesAsMap<Algorithm> Type;
    };

    template<typename ProgramStructureType, typename Callgraph,
             typename PointsToSets, typename ModifiesType>
    void computeModifies(ProgramStructureType const& P, Callgraph const& CG,
                         PointsToSets const& PS, ModifiesType& M)
    {
        computeModifies(P, CG, PS, M, typename ModifiesType::Algorithm());
    }

}}

#endif
