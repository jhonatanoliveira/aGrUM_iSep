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
 
#include <agrum/BN/inference/iSeparation.h>

#ifdef GUM_NO_INLINE
#include <agrum/BN/inference/iSeparation.inl>
#endif // GUM_NO_INLINE


namespace gum {

  
   bool iSeparation::test(const NodeSet& X,
                         const NodeSet& Y,
                         const NodeSet& Z) {
    NodeSet reachable_nodes;
    this->__reachable_nodes(X, Y, Z, reachable_nodes);

    NodeSet intersection = reachable_nodes * Z;

    //### DEBUG
    // std::cout << "*** DEBUG *** Reachable Variables" << std::endl;
    // for (auto var : reachable_nodes) {
    //   std::cout << var << std::endl;
    // }
    // std::cout << "*** DEBUG *** Intersection with Z" << std::endl;
    // for (auto var : intersection) {
    //   std::cout << var << std::endl;
    // }
    //--- DEBUG

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

    //### DEBUG
    // std::cout << "---> Ancestors of Y" << std::endl;
    // for (auto var : anY) {
    //   std::cout << var << std::endl;
    // }
    //--- DEBUG

    // Phase II: ancestors of XYZ, including XYZ
    NodeSet anXYZ;
    this->__ancestors(X + Y + Z, anXYZ);

    //### DEBUG
    // std::cout << "---> Ancestors of XYZ" << std::endl;
    // for (auto var : anXYZ) {
    //   std::cout << var << std::endl;
    // }
    //--- DEBUG

    // Phase III: traverse *active* paths starting from X
    List<std::pair<NodeId,bool>> nodes_to_visit;
    List<std::pair<NodeId,bool>> nodes_visited;

    for (auto node : X) {
      nodes_to_visit.insert( std::pair<NodeId,bool> (node,true) );
    }

    while ( !nodes_to_visit.empty() ) {

      std::pair<NodeId,bool> selected = nodes_to_visit.front();
      NodeId selectedNode = selected.first;
      bool direction = selected.second;

      bool selected_node_not_visited = false;
      if ( !nodes_visited.exists(selected) ) {
        nodes_visited.insert(selected);
        selected_node_not_visited = true;
      }
      nodes_to_visit.popFront();

      //### DEBUG
      // std::cout << "==> Selected Node: " << selectedNode << std::endl;
      // std::cout << "==> Nodes to visit: " << std::endl;
      // for (auto var : nodes_to_visit) {
      //   std::cout << var << std::endl;
      // }
      //--- DEBUG

      if ( selected_node_not_visited ) {

        if ( !Y.exists(selectedNode) ) {
          reachable_nodes.insert(selectedNode);

          if ( direction == true ) {
            this->__add_parents(selectedNode, nodes_to_visit, anXYZ);
            this->__add_children(selectedNode, nodes_to_visit, anXYZ);
          } else {
            this->__add_children(selectedNode, nodes_to_visit, anXYZ);
          }

        } else if ( (direction == false) && anXYZ.exists(selectedNode) ) {
          for (auto parent : this->__dag->parents(selectedNode)) {
            nodes_to_visit.insert( std::pair<NodeId,bool> (parent,true) );
          }
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
      //### DEBUG
      // std::cout << "==> For child: " << child << std::endl;
      // std::cout << "==> Is inaugural: " << this->__is_inaugural(child, anXYZ) << std::endl;
      //--- DEBUG
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
