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
    iSeparation();

    /// Destructor.
    ~iSeparation();

    /// update a set of potentials, keeping only those d-connected with query variables
    template <typename GUM_SCALAR, template <typename> class TABLE>
    void relevantPotentials ( const IBayesNet<GUM_SCALAR>& bn,
                              const NodeSet& query,
                              const NodeSet& hardEvidence,
                              const NodeSet& softEvidence,
                              Set<const TABLE<GUM_SCALAR>*>& potentials );
    
    private:

    /// Top and bottom flags for each nodes.
    // HashTable< NodeId, std::pair<bool, bool> > __marks;
    // first element of the pair: top mark
    // second element of the pair: bottom mark
    NodeProperty<std::pair<bool, bool>> __marks;

    /// the dag on which we perform the ball bouncing
    const DAG *__dag;

    /// the set of hard evidence
    const NodeSet *__hardEvidence;

    /// the set of soft evidence
    const NodeSet *__softEvidence;    
  };
  
} /* namespace gum */

#include <agrum/BN/inference/iSeparation.tcc>

#endif /* GUM_ISEPARATION_H */
