#include <agrum/BN/inference/iSeparation.h>

#ifdef GUM_NO_INLINE
#include <agrum/BN/inference/iSeparation.inl>
#endif // GUM_NO_INLINE


namespace gum {

  
   bool iSeparation::test(const NodeSet& X,
                         const NodeSet& Y,
                         const NodeSet& Z) {
    NodeSet reachable_nodes;
    __reachable_nodes(X, Y, Z, reachable_nodes);

    const NodeSet intersection = reachable_nodes * Z;
    return ( intersection.size() == 0 );
  }


  void iSeparation::__reachable_nodes(const NodeSet& X,
                                      const NodeSet& Y,
                                      const NodeSet& Z,
                                      NodeSet& reachable_nodes) {
    reachable_nodes.clear ();

    // Phase I: mark the ancestors of Y, including Y
    // ancestors[x] does not exist => x is an ancestor of neither Y nor Z
    // ancestors[x] = false => x is an ancestor of Y (and possibly of Z as well)
    // ancestors[x] = true  => x is an ancestor of Z but not Y
    __ancestors.resize ( __dag->size() );
    {
      List<NodeId> anc_to_visit;
      for ( const auto node : Y ) {
        anc_to_visit.insert ( node );
      }
      while ( ! anc_to_visit.empty () ) {
        const NodeId node = anc_to_visit.front ();
        anc_to_visit.popFront ();

        if ( ! __ancestors.exists ( node ) ) {
          __ancestors.insert ( node, false );
          for ( const auto par : __dag->parents ( node ) ) {
            anc_to_visit.insert ( par );
          }
        }
      }
    }
    
    // Phase II: ancestors of YZ, including YZ
    {
      List<NodeId> anc_to_visit;
      for ( const auto node : Z ) {
        anc_to_visit.insert ( node );
      }
      while ( ! anc_to_visit.empty () ) {
        const NodeId node = anc_to_visit.front ();
        anc_to_visit.popFront ();

        if ( ! __ancestors.exists ( node ) ) {
          __ancestors.insert ( node, true );
          for ( const auto par : __dag->parents ( node ) ) {
            anc_to_visit.insert ( par );
          }
        }
      }
    }

 
    // Phase III: traverse *active* paths starting from X
    // pair (x,b): b = true <=> from child, b = false <=> from parent
    List<std::pair<NodeId,bool>> nodes_to_visit;
    for ( const auto node : X ) {
      nodes_to_visit.insert( std::pair<NodeId,bool> (node, true) );
    }
    NodeSet visited_from_child;
    NodeSet visited_from_parent;

    // phase III.1: parse all the ancestors of X
    unsigned int nb_Xanc = X.size ();
    while ( nb_Xanc ) {
      // get the next node
      const NodeId node = nodes_to_visit.front().first;
      nodes_to_visit.popFront();
      --nb_Xanc;

      // check if the node has already been visited
      if ( ! visited_from_child.exists ( node ) ) {
        visited_from_child.insert ( node );

        if ( ! Y.exists ( node ) ) {
          // R = R U {v}
          reachable_nodes.insert(node);

          // as we are only coming from a child: add parents and children 
          // add parents
          for ( const auto parent : this->__dag->parents( node ) ) {
            nodes_to_visit.pushFront( std::pair<NodeId,bool> (parent, true) );
            ++nb_Xanc;
          }

          // add children
          for ( const auto child : this->__dag->children ( node ) ) {
            nodes_to_visit.pushBack( std::pair<NodeId,bool> (child,false) );
          }
        }
      }
    }


    // Phase III.2
    while ( ! nodes_to_visit.empty () ) {
      // get the next pair (node,direction)
      const NodeId node    = nodes_to_visit.front().first;
      const bool direction = nodes_to_visit.front().second;
      nodes_to_visit.popFront();

      // check if the pair has already been visited
      bool already_visited;
      if ( direction ) {
        already_visited = visited_from_child.exists ( node );
        if ( ! already_visited ) {
          visited_from_child.insert ( node );
        }
      }
      else {
        already_visited = visited_from_parent.exists ( node );
        if ( ! already_visited ) {
          visited_from_parent.insert ( node );
        }
      }

      if ( ! already_visited ) {
        if ( ! Y.exists(node) ) {
          // R = R U {v}
          reachable_nodes.insert(node);

          if ( direction == true ) { // up from child
            // add parents
            for ( const auto parent : this->__dag->parents( node ) ) {
              if ( ! this->__is_inaugural ( parent ) ) {
                nodes_to_visit.pushBack( std::pair<NodeId,bool> (parent, true) );
              }
            }

            // add children
            for ( const auto child : this->__dag->children ( node ) ) {
              if ( !this->__is_inaugural( child ) ) {
                nodes_to_visit.pushFront( std::pair<NodeId,bool> (child,false) );
              }
            }
          }
          else {
            // add children
            for ( const auto child : this->__dag->children ( node ) ) {
              if ( !this->__is_inaugural ( child ) ) {
                nodes_to_visit.pushFront( std::pair<NodeId,bool> (child,false) );
              }
            }
          }
        }
        if ( ( direction == false ) && __ancestors.exists( node )
             && ( __ancestors[node] == false ) ) {
          // add parents
          for ( const auto parent : this->__dag->parents( node ) ) {
            if ( ! this->__is_inaugural ( parent ) ) {
              nodes_to_visit.pushBack( std::pair<NodeId,bool> (parent, true) );
            }
          }
        }
      }
    }
  }

  
} /* namespace gum */
