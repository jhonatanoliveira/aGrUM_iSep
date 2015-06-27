/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
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
 * @brief Headers of the DFSTree class.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */

#ifndef GUM_DFS_TREE_H
#define GUM_DFS_TREE_H

#include <utility>
#include <cmath>
#include <list>
#include <vector>
#include <ostream>

#include <agrum/core/sequence.h>
#include <agrum/core/set.h>
#include <agrum/core/bijection.h>

#include <agrum/graphs/diGraph.h>

#include <agrum/graphs/triangulations/partialOrderedTriangulation.h>

#include <agrum/PRM/gspan/interfaceGraph.h>
#include <agrum/PRM/gspan/pattern.h>
#include <agrum/PRM/gspan/edgeGrowth.h>

#include <agrum/PRM/gspan/searchStrategy.h>

namespace gum {
  namespace prm {

    template <typename GUM_SCALAR> class GSpan;

    namespace gspan {
      template <typename GUM_SCALAR> class SearchStrategy;

      /**
       * @class DFSTree DFSTree.h <agrum/PRM/gspan/DFSTree.h>
       *
       * A DFSTree is used by gspan to sort lexicographically patterns discovered
       * in an interface graph.
       */
      template <typename GUM_SCALAR> class DFSTree : private DiGraph {

        public:
        // =========================================================================
        /// @name Constructor and destructor.
        // ==========================================================================
        /// @{

        /// Default constructor.
        // DFSTree(InterfaceGraph<GUM_SCALAR>* graph);
        DFSTree(const InterfaceGraph<GUM_SCALAR> &graph,
                SearchStrategy<GUM_SCALAR> *strategy = 0);

        /// Destructor.
        ~DFSTree();

        /// @}
        // =========================================================================
        /// @name DFSTree getters and setters.
        // ==========================================================================
        /// @{

        const InterfaceGraph<GUM_SCALAR> &graph() const;

        struct PatternData {
          /// Constructor.
          PatternData(Pattern *p);
          /// Copy constructor.
          PatternData(const PatternData &from);
          /// Destructor.
          ~PatternData();
          /// The pattern.
          Pattern *pattern;
          /// The list of the pattern's children, sorted lexicographically.
          std::list<NodeId> children;
          /// The isomorphism graph of the pattern.
          UndiGraph iso_graph;
          /// The instances matching p in the interface graph.
          NodeProperty<Sequence<Instance<GUM_SCALAR> *> *> iso_map;
          /// The maximal independent set of p.
          Set<NodeId> max_indep_set;
          /// The cost of this Pattern
          Size cost;
          /// The gain of this Pattern
          Size gain;
          // /// The different sub_patterns of p given the iso_map.
          // Sequence< HashTable<ClassElement<GUM_SCALAR>*, Size>* > sub_patterns;
          // /// The mapping between an iso_map and the given sub pattern.
          // HashTable<NodeId, Idx> sub_patterns_map;
          // /// The number of each sub pattern
          // std::vector<Size> sub_patterns_count;
        };

        /// Returns the list of root patterns in this DFSTree.
        std::list<NodeId> &roots();

        /// Returns the list of root patterns in this DFSTree.
        const std::list<NodeId> &roots() const;

        /// Returns the parent of p in this DFSTree.
        Pattern &parent(const Pattern &p);

        /// Returns the parent of p in this DFSTree.
        const Pattern &parent(const Pattern &p) const;

        /// Returns the list of p children in this DFSTree.
        std::list<NodeId> &children(const Pattern &p);

        /// Returns the list of p children in this DFSTree.
        const std::list<NodeId> &children(const Pattern &p) const;

        /// Returns the pattern represented by id in this DFSTree.
        Pattern &pattern(NodeId id);

        /// Returns the pattern represented by id in this DFSTree.
        const Pattern &pattern(NodeId id) const;

        /**
         * @brief Add a one edge Pattern in this DFSTree.
         *
         * @param data Data over the edge used to create a root of this DFSTree.
         * @return Returns the Pattern added as a root of this DFSTree.
         */
        void addRoot(LabelData &data);

        /**
         * @class EdgeGrowth DFSTree.h <agrum/PRM/DFSTree.h>
         * This class is used to define an edge growth of a pattern
         * in this DFSTree.
         */

        /**
         * @brief Add a one edge growth of p as one of its child.
         *
         * The child is inserted lexicographically among the children of p.
         * However if the child is found to be not minimal an
         * OperationNotAllowed is raised.
         *
         * @param p The Pattern from which a one edge growth is spawned.
         * @param edge_growth The data about the edge growth of p.
         * @param min_freq minimum number of occurrence to be used as a pattern
         *
         * @throw FatalError Raised if the grow is an illegal backedge growth.
         * @throw OperationNotAllowed Raised if the grow is found to be not minimal.
         */
        Pattern &growPattern(Pattern &p, EdgeGrowth<GUM_SCALAR> &edge_growth,
                             Size min_freq);

        /// @}
        // =========================================================================
        /// @name Isomorphisms for patterns in this DFSTree.
        // ==========================================================================
        /// @{

        /**
         * @brief Returns the isomorphism graph of p in the interface graph.
         *
         * The isomorphism graph is a undirected graph in which each node represents
         * a set of Instance<GUM_SCALAR> matching p in the interface graph.
         *
         * If there exists an edge between two nodes in the isomorphism graph, then
         * the two respective set of instances are not disjoint.
         *
         * @param p The pattern for which we want the isomorphism graph.
         * @return The isomorphism graph of p.
         *
         * @throw NotFound Raised if p is not a node in this DFSTree.
         */
        UndiGraph &iso_graph(const Pattern &p);

        /**
         * @brief Given a pattern and a node in its isomorphism graph, this methods
         *        returns the sequence of instance matching p in the interface graph.
         *
         * The sequence of instances respect DSF subscripting. Each node in the
         * pattern's graph have a DSF subscript from 1 to n, where n is the number
         * of nodes in the pattern's graph.
         *
         * If for a given match you want the k-th instance repecting p's DFS
         *subscripting,
         * then it will be the (k - 1)th element in the sequence.
         *
         * @param p The pattern for which we want a match in the interface graph.
         * @param node The node in p isomorphism graph for which we want the matching
         *             set if instances.
         * @return Returns the sequence of instances matching p and node.
         *
         * @throw NotFound Raised if p or node does not exists.
         */
        Sequence<Instance<GUM_SCALAR> *> &iso_map(const Pattern &p, NodeId node);

        /**
         * @brief Returns the maximal independent set of p isomorphism graph.
         *
         * @param p The pattern for which we want its maximal independent set.
         *
         * @throw NotFound Raised if p is not a node in this DFSTree.
         */
        Set<NodeId> &max_indep_set(const Pattern &p);

        /// Returns the frequency of p respecting it's maximal independent set.
        /// @param p The pattern
        double frequency(const Pattern &p) const;

        /// @param p The pattern
        PatternData &data(const Pattern &p);
        /// @param p The pattern
        const PatternData &data(const Pattern &p) const;

        /// strategy getter
        SearchStrategy<GUM_SCALAR> &strategy();

        /// strategy getter
        const SearchStrategy<GUM_SCALAR> &strategy() const;

        /// @class NeighborDegreeSort DFSTree.h <agrum/PRM/gspan/DFSTree.h>
        /// @brief This is used to generate the max_indep_set of a Pattern.

        struct NeighborDegreeSort {
          /// Constructor
          NeighborDegreeSort(UndiGraph &graph);
          /// Copy constructor.
          NeighborDegreeSort(const NeighborDegreeSort &source);
          /// Destructor.
          ~NeighborDegreeSort();
          /// The operator used to sort stuff.
          bool operator()(NodeId i, NodeId j);
          /// The isomorphism graph.
          UndiGraph &g;
        };

        /// @}

        private:
        /// The interface graph on which this DFSTree applies.
        const InterfaceGraph<GUM_SCALAR> *__graph;

        /// The list of root patterns in this DFSTree.
        std::list<NodeId> __roots;

        /// The mapping between nodes in this DFSTree and the patterns they
        /// represents.
        Bijection<NodeId, Pattern *> __node_map;

        /// Data about patterns in this DFSTree.
        HashTable<Pattern *, PatternData *> __data;

        /// The strategy used to prune the search tree.
        SearchStrategy<GUM_SCALAR> *__strategy;

        /// Raise different exceptions if child is invalid or illegal
        void __checkGrowth(Pattern &p, Pattern *child,
                           EdgeGrowth<GUM_SCALAR> &edge_growth);

        /// Add a child to this DFSTree.
        void __addChild(Pattern &p, Pattern *child,
                        EdgeGrowth<GUM_SCALAR> &edge_growth);

        /// Check if an instance match is redundant.
        bool __is_new_seq(Sequence<Instance<GUM_SCALAR> *> &seq,
                          NodeProperty<Sequence<Instance<GUM_SCALAR> *> *> &iso_map);

        /// This initialize the DSFTree with a new root.
        /// @param p A Pattern.
        /// @param seq A sequence of EdgeData<GUM_SCALAR>.
        void __initialiaze_root(Pattern *p, Sequence<EdgeData<GUM_SCALAR> *> &seq);

        /// This can be used to decompose a pattern in sub patter, which could be
        /// useful
        /// in some scenarios.
        void __find_sub_pattern(Pattern &p, NodeId iso_map);

        // Used by __find_sub_pattern.
        bool __test_equality(HashTable<ClassElement<GUM_SCALAR> *, Size> &x,
                             HashTable<ClassElement<GUM_SCALAR> *, Size> &y);
      };

      extern template class DFSTree<double>;
    } /* namespace gspan */
  }   /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/gspan/DFSTree.tcc>

#endif /* GUM_DFS_TREE_H */
