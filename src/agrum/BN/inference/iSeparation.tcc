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
 * @brief Implementation of the iSeparation class.
 */
#include <agrum/core/list.h>
#include <agrum/core/hashTable.h>


namespace gum {
  
  // update a set of potentials, keeping only those d-connected with
  // query variables
  template <typename GUM_SCALAR, template <typename> class TABLE>
  void
  iSeparation::relevantPotentials ( const IBayesNet<GUM_SCALAR>& bn,
                                    const NodeSet& query,
                                    const NodeSet& hardEvidence,
                                    Set<const TABLE<GUM_SCALAR> *>& potentials ) {

    Set<const TABLE<GUM_SCALAR>*> to_remove;
    // ### DEBUG
    // std::cout << "*** DEBUG *** Before removing irrelevant." << std::endl;
    // --- DEBUG

    for (auto potential : potentials) {

      // ### DEBUG
      // std::cout << *potential << std::endl;
      // --- DEBUG

      NodeSet X;
      for ( const auto var : potential->variablesSequence() ) {
        X.insert( bn.nodeId ( *var ) );
      }

      NodeSet intersection = X * hardEvidence;

      if (intersection.size() < X.size()) {
        if ( this->test(X, hardEvidence, query) ) {
          to_remove.insert(potential);
          //### DEBUG
          // std::cout << ">>> Vars in X" << std::endl;
          // for (auto var : X) {
          //   std::cout << bn.variable(var).name() << std::endl;
          // }
          // std::cout << ">>> Vars in Y" << std::endl;
          // for (auto var : hardEvidence) {
          //   std::cout << bn.variable(var).name() << std::endl;
          // }
          // std::cout << ">>> Vars in Z" << std::endl;
          // for (auto var : query) {
          //   std::cout << bn.variable(var).name() << std::endl;
          // }
          // std::cout << ">>> Test: " << this->test(X, hardEvidence, query) << std::endl;
          //--- DEBUG
        }
      } else {
        //### DEBUG
        // std::cout << ">>> Intersection is equal or greater!" << std::endl;
        //--- DEBUG
      }
    }

    for (auto potential : to_remove) {
      potentials.erase(potential);
    }

    // ### DEBUG
    // std::cout << ">>> After removing irrelevant." << std::endl;
    // for (auto potential : potentials) {
    //   std::cout << *potential << std::endl;
    // }
    // --- DEBUG

  }



  
} /* namespace gum */
