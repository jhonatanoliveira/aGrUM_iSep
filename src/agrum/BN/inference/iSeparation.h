/***************************************************************************
 *   Copyright (C) 2005 by Pierre-Henri WUILLEMIN et Christophe GONZALES   *
 *   {prenom.nom}_at_lip6.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
 * @file
 * @brief Header of the iSeparation class.
 *
 * @author Jhonatan Oliveira
 */

#ifndef GUM_ISEPARATION_H
#define GUM_ISEPARATION_H

#include <utility>

#include <agrum/config.h>

#include <agrum/core/hashTable.h>
#include <agrum/core/sequence.h>
#include <agrum/core/list.h>

#include <agrum/BN/IBayesNet.h>

namespace gum {
  /**
   * @class iSeparation iSeparation.h <agrum/BN/inference/iSeparation.h>
   * @brief Implementation of iSeparation algorithm.
   * @ingroup bn_group
   *
   */
  class iSeparation {

    public:

    /// Default constructor.
    template <typename GUM_SCALAR>
    iSeparation(const IBayesNet<GUM_SCALAR>& bn);

    /// Destructor.
    ~iSeparation();

    /// update a set of potentials, keeping only those d-connected with query variables
    template <typename GUM_SCALAR, template <typename> class TABLE>
    void relevantPotentials ( const IBayesNet<GUM_SCALAR>& bn,
                              const NodeSet& query,
                              const NodeSet& hardEvidence,
                              Set<const TABLE<GUM_SCALAR>*>& potentials );

    // Test if the independence I(X,Y,Z) holds
    bool test(const NodeSet& X,
              const NodeSet& Y,
              const NodeSet& Z);

    
    private:
    
    // Return a set of reachable nodes, from X, when traversing the DAG
    // through *active* paths.
    // Here, an inaugural variable is considered when testing active paths.
    void __reachable_nodes(const NodeSet& X,
                           const NodeSet& Y,
                           const NodeSet& Z,
                           NodeSet& reachable_nodes);

    // Test if given variable is inaugural
    bool __is_inaugural(const NodeId& variable, const NodeSet& ancestors);

    // Add parents to the set of nodes to be visited
    void __add_parents(const NodeId& node, List<std::pair<NodeId,bool>>& nodes_to_visit, const NodeSet& anXYZ);

    // Add parents to the set of nodes to be visited
    void __add_children(const NodeId& node, List<std::pair<NodeId,bool>>& nodes_to_visit, const NodeSet& anXYZ);

    // Ancestors of nodes, including nodes itself
    void __ancestors(const NodeSet& nodes, NodeSet& ancestors);

    /// The DAG on which we perform i-Separation
    const DAG *__dag;

  };
  
} /* namespace gum */

#ifndef GUM_NO_INLINE
#include <agrum/BN/inference/iSeparation.inl>
#endif // GUM_NO_INLINE

#include <agrum/BN/inference/iSeparation.tcc>

#endif
