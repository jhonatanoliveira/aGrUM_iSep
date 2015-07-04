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
                                    Set<const TABLE<GUM_SCALAR>*>& potentials ) {

    Set<const TABLE<GUM_SCALAR>*> to_remove;
    for (auto potential : potentials) {

      NodeSet X;
      for ( const auto var : potential->variablesSequence() ) {
        X.insert( bn.nodeId ( *var ) );
      }

      if ( this->test(X, hardEvidence, query) ) {
        to_remove.insert(potential);
      }
    }

    for (auto potential : to_remove) {
      potentials.erase(potential);
    }

  }


  bool iSeparation::test(const NodeSet& X,
                         const NodeSet& Y,
                         const NodeSet& Z) {
    NodeSet reachable_nodes;
    this->__reachable_nodes(X, Y, Z, reachable_nodes);

    NodeSet intersection = reachable_nodes * Z;

    bool independence_holds = true;
    if (intersection.size() > 0) {
      independence_holds = false;
    }

    return independence_holds;
  }


  void iSeparation::__reachable_nodes(const NodeSet& X,
                                      const NodeSet& Y,
                                      const NodeSet& Z,
                                      NodeSet& reachable_nodes) {

    // Phase I: ancestors of Y, including Y
    NodeSet anY;
    this->__ancestors(Y, anY);

    // Phase II: ancestors of XYZ, including XYZ
    NodeSet anXYZ;
    this->__ancestors(X + Y + Z, anXYZ);

    // Phase III: traverse *active* paths starting from X
    List<std::pair<NodeId,bool>> nodes_to_visit;
    List<std::pair<NodeId,bool>> nodes_visited;

    for (auto node : X) {
      nodes_to_visit.insert( std::pair<NodeId,bool> (node,true) );
    }

    while ( !nodes_to_visit.empty() ) {

      std::pair<NodeId,bool> selected = nodes_to_visit.front();
      NodeId node = selected.first;
      bool direction = selected.second;

      bool selected_node_not_visited = false;
      if ( !nodes_visited.exists(selected) ) {
        nodes_visited.insert(selected);
        selected_node_not_visited = true;
      }
      nodes_to_visit.popFront();

      if ( selected_node_not_visited ) {

        if ( !Y.exists(node) ) {
          reachable_nodes.insert(node);

          if ( direction == true ) {
            this->__add_parents(node, nodes_to_visit, anXYZ);
            this->__add_children(node, nodes_to_visit, anXYZ);
          } else {
            this->__add_children(node, nodes_to_visit, anXYZ);
          }

        } else if ( direction == false && anXYZ.exists(node) ) {

          this->__add_parents(node, nodes_to_visit, anXYZ);
        }

      }

    }
    
  }


  void iSeparation::__add_parents(const NodeId& node, List<std::pair<NodeId,bool>>& nodes_to_visit, const NodeSet& anXYZ) {
    for (auto parent : this->__dag->parents(node)) {
      if ( !this->__is_inaugural(parent, anXYZ) ) {
        nodes_to_visit.insert( std::pair<NodeId,bool> (parent,true) );
      }
    }
  }


  void iSeparation::__add_children(const NodeId& node, List<std::pair<NodeId,bool>>& nodes_to_visit, const NodeSet& anXYZ) {
    for (auto child : this->__dag->children(node)) {
      if ( !this->__is_inaugural(child, anXYZ) ) {
        nodes_to_visit.insert( std::pair<NodeId,bool> (child,false) );
      }
    }
  }


  bool iSeparation::__is_inaugural(const NodeId& variable, const NodeSet& ancestors) {
    bool is_inaugural = false;
    if ( !ancestors.exists(variable) ) {
      if ( this->__dag->parents(variable).size() > 1 ) {
        is_inaugural = true;
      }
    }
    return is_inaugural;
  }


  void iSeparation::__ancestors(const NodeSet& nodes,
                                NodeSet& ancestors) {
    for (auto node : nodes) {
      NodeSet parents;
      ancestors.insert(node);
      for (auto parent : this->__dag->parents(node)) {
        ancestors.insert(parent);
        parents.insert(parent);
      }
      this->__ancestors(parents, ancestors);
    }
  }

  
} /* namespace gum */
