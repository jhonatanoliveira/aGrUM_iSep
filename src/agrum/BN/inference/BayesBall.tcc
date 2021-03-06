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
 * @brief Implementation of the BayesBall class.
 */
#include <agrum/core/list.h>
#include <agrum/core/hashTable.h>


namespace gum {


  // update a set of potentials, keeping only those d-connected with
  // query variables
  template <typename GUM_SCALAR, template <typename> class TABLE>
  void BayesBall::relevantPotentials(
      const IBayesNet<GUM_SCALAR>& bn,
      const NodeSet& query,
      const NodeSet& hardEvidence,
      const NodeSet& softEvidence,
      Set<const TABLE<GUM_SCALAR>*>& potentials ) {
    const DAG& dag = bn.dag (); 
    
    // create the marks (top = first and bottom = second)
    NodeProperty<std::pair<bool, bool>> marks;
    marks.resize( dag.size() );
    const std::pair<bool, bool> empty_mark( false, false );

    /// for relevant potentials: indicate which tables contain a variable
    /// (nodeId)
    HashTable<NodeId, Set<const TABLE<GUM_SCALAR>*>> node2potentials;
    for ( const auto pot : potentials ) {
      const Sequence<const DiscreteVariable*>& vars = pot->variablesSequence();
      for ( const auto var : vars ) {
        const NodeId id = bn.nodeId( *var );
        if ( ! node2potentials.exists( id ) ) {
          node2potentials.insert( id, Set<const TABLE<GUM_SCALAR>*>() );
        }
        node2potentials[id].insert( pot );
      }
    }

    // indicate that we will send the ball to all the query nodes (as children):
    // in list nodes_to_visit, the first element is the next node to send the
    // ball to and the Boolean indicates whether we shall reach it from one of
    // its children (true) or from one parent (false)
    List<std::pair<NodeId, bool>> nodes_to_visit;
    for ( const auto node : query ) {
      nodes_to_visit.insert( std::pair<NodeId, bool>( node, true ) );
    }

    // perform the bouncing ball until __node2potentials becomes empty (which
    // means that we have reached all the potentials and, therefore, those
    // are d-connected to query) or until there is no node in the graph to send
    // the ball to
    while ( ! nodes_to_visit.empty() && ! node2potentials.empty() ) {
      // get the next node to visit
      NodeId node = nodes_to_visit.front().first;

      // if the marks of the node do not exist, create them
      if ( ! marks.exists( node ) )
        marks.insert( node, empty_mark );

      // if the node belongs to the query, update __node2potentials: remove all
      // the potentials containing the node
      if ( node2potentials.exists( node ) ) {
        auto& pot_set = node2potentials[node];
        for ( const auto pot : pot_set ) {
          const auto& vars = pot->variablesSequence();
          for ( const auto var : vars ) {
            const NodeId id = bn.nodeId( *var );
            if ( id != node ) {
              node2potentials[id].erase( pot );
              if ( node2potentials[id].empty() ) {
                node2potentials.erase( id );
              }
            }
          }
        }
        node2potentials.erase( node );

        // if __node2potentials is empty, no need to go on: all the potentials
        // are d-connected to the query
        if ( node2potentials.empty() )
          return;
      }


      // bounce the ball toward the neighbors
      if ( nodes_to_visit.front().second ) {  // visit from a child
        nodes_to_visit.popFront();

        if ( hardEvidence.exists( node ) ) {
          continue;
        }

        if ( not marks[node].first ) {
          marks[node].first = true;  // top marked
          for ( const auto par : dag.parents( node ) ) {
            nodes_to_visit.insert( std::pair<NodeId, bool>( par, true ) );
          }
        }

        if ( not marks[node].second ) {
          marks[node].second = true;  // bottom marked
          for ( const auto chi : dag.children( node ) ) {
            nodes_to_visit.insert( std::pair<NodeId, bool>( chi, false ) );
          }
        }
      }
      else {  // visit from a parent
        nodes_to_visit.popFront();

        const bool is_hard_evidence = hardEvidence.exists( node );
        const bool is_evidence = is_hard_evidence or softEvidence.exists( node );

        if ( is_evidence && ! marks[node].first ) {
          marks[node].first = true;

          for ( const auto par : dag.parents( node ) ) {
            nodes_to_visit.insert( std::pair<NodeId, bool>( par, true ) );
          }
        }

        if ( ! is_hard_evidence && ! marks[node].second ) {
          marks[node].second = true;

          for ( const auto chi : dag.children( node ) ) {
            nodes_to_visit.insert( std::pair<NodeId, bool>( chi, false ) );
          }
        }
      }
    }


    // here, all the potentials that belong to __node2potentials are d-separated
    // from the query
    for ( const auto elt : node2potentials ) {
      for ( const auto pot : elt.second ) {
        potentials.erase( pot );
      }
    }
  }


} /* namespace gum */
