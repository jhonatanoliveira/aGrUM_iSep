/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}@lip6.fr                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it wil be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/** @file
 * @brief The basic class for computing the next graph changes possible in a
 * (directed) structure learning algorithm
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace gum {

  namespace learning {

    /// default constructor
    template <typename STRUCT_CONSTRAINT>
    GraphChangesGenerator4DiGraph<STRUCT_CONSTRAINT>::GraphChangesGenerator4DiGraph(
        STRUCT_CONSTRAINT &constraint)
        : _constraint(&constraint) {
      GUM_CONSTRUCTOR(GraphChangesGenerator4DiGraph);
    }

    /// copy constructor
    template <typename STRUCT_CONSTRAINT>
    GraphChangesGenerator4DiGraph<STRUCT_CONSTRAINT>::GraphChangesGenerator4DiGraph(
        const GraphChangesGenerator4DiGraph &from)
        : _graph(from._graph), _constraint(from._constraint),
          _legal_changes(from._legal_changes),
          __max_threads_number(from.__max_threads_number) {
      GUM_CONS_CPY(GraphChangesGenerator4DiGraph);
    }

    /// move operator
    template <typename STRUCT_CONSTRAINT>
    GraphChangesGenerator4DiGraph<STRUCT_CONSTRAINT>::GraphChangesGenerator4DiGraph(
        GraphChangesGenerator4DiGraph &&from)
        : _graph(std::move(from._graph)), _constraint(from._constraint),
          _legal_changes(std::move(from._legal_changes)),
          __max_threads_number(from.__max_threads_number) {
      GUM_CONS_MOV(GraphChangesGenerator4DiGraph);
    }

    /// destructor
    template <typename STRUCT_CONSTRAINT>
    GraphChangesGenerator4DiGraph<
        STRUCT_CONSTRAINT>::~GraphChangesGenerator4DiGraph() {
      GUM_DESTRUCTOR(GraphChangesGenerator4DiGraph);
    }

    /// copy operator
    template <typename STRUCT_CONSTRAINT>
    GraphChangesGenerator4DiGraph<STRUCT_CONSTRAINT> &
        GraphChangesGenerator4DiGraph<STRUCT_CONSTRAINT>::
        operator=(const GraphChangesGenerator4DiGraph<STRUCT_CONSTRAINT> &from) {
      if (this != &from) {
        _graph = from._graph;
        _constraint = from._constraint;
        _legal_changes = from._legal_changes;
        __max_threads_number = from.__max_threads_number;
      }
      return *this;
    }

    /// move operator
    template <typename STRUCT_CONSTRAINT>
    GraphChangesGenerator4DiGraph<STRUCT_CONSTRAINT> &
        GraphChangesGenerator4DiGraph<STRUCT_CONSTRAINT>::
        operator=(GraphChangesGenerator4DiGraph<STRUCT_CONSTRAINT> &&from) {
      if (this != &from) {
        _graph = std::move(from._graph);
        _constraint = std::move(from._constraint);
        _legal_changes = std::move(from._legal_changes);
        __max_threads_number = from.__max_threads_number;
      }
      return *this;
    }

    /// create the set of legal and illegal changes from a given graph
    template <typename STRUCT_CONSTRAINT>
    void GraphChangesGenerator4DiGraph<STRUCT_CONSTRAINT>::_createChanges() {
      _legal_changes.clear();

      // for all the pairs of nodes, consider adding, reverse and removing arcs
      std::vector<Set<GraphChange>> legal_changes;
#pragma omp parallel num_threads(__max_threads_number)
      {
        int num_threads = getNumberOfRunningThreads();

#pragma omp single
        {
          // resize the change vectors so that each thread can write to its
          // own vector
          legal_changes.resize(num_threads);
        }

        const unsigned int this_thread = getThreadNumber();

        unsigned int i = 0;
        for (const auto node1 : _graph) {
          if (i == this_thread) {
            for (const auto node2 : _graph) {
              if (node1 != node2) {
                // try arc additions
                ArcAddition arc_add(node1, node2);
                if (!_constraint->isAlwaysInvalid(arc_add)) {
                  legal_changes[this_thread].insert(std::move(arc_add));
                }

                // try arc deletion
                ArcDeletion arc_del(node1, node2);
                if (!_constraint->isAlwaysInvalid(arc_del)) {
                  legal_changes[this_thread].insert(std::move(arc_del));
                }

                // try arc reversal
                ArcReversal arc_rev(node1, node2);
                if (!_constraint->isAlwaysInvalid(arc_rev)) {
                  legal_changes[this_thread].insert(std::move(arc_rev));
                }
              }
            }
          }
          ++i;
          i %= num_threads;
        }
      }

      // now store the changes into the protected vectors of the
      // GraphChangesGenerator4DiGraph
      for (const auto &changes : legal_changes) {
        for (const auto &change : changes) {
          _legal_changes.insert(std::move(change));
        }
      }
    }

    /// sets a new graph from which the operator will compute possible changes
    template <typename STRUCT_CONSTRAINT>
    INLINE void GraphChangesGenerator4DiGraph<STRUCT_CONSTRAINT>::setGraph(
        const DiGraph &graph) {
      // sets the current graph
      _graph = graph;

      // generate the set of all changes
      _createChanges();
    }

    /// empty the set of possible change operators that can be applied
    template <typename STRUCT_CONSTRAINT>
    INLINE void
    GraphChangesGenerator4DiGraph<STRUCT_CONSTRAINT>::clearChanges() noexcept {
      _legal_changes.clear();
    }

    /// returns an (unsafe) iterator on the beginning of the list of operators
    template <typename STRUCT_CONSTRAINT>
    INLINE typename GraphChangesGenerator4DiGraph<STRUCT_CONSTRAINT>::iterator
    GraphChangesGenerator4DiGraph<STRUCT_CONSTRAINT>::begin() const {
      return _legal_changes.cbegin();
    }

    /// returns an (unsafe) iterator on the end of the list of operators
    template <typename STRUCT_CONSTRAINT>
    INLINE const typename GraphChangesGenerator4DiGraph<
        STRUCT_CONSTRAINT>::iterator &
    GraphChangesGenerator4DiGraph<STRUCT_CONSTRAINT>::end() const {
      return _legal_changes.cend();
    }

    /// notify the operator set of a change applied to the graph
    template <typename STRUCT_CONSTRAINT>
    INLINE void GraphChangesGenerator4DiGraph<STRUCT_CONSTRAINT>::modifyGraph(
        const ArcAddition &change) {}

    /// notify the operator set of a change applied to the graph
    template <typename STRUCT_CONSTRAINT>
    INLINE void GraphChangesGenerator4DiGraph<STRUCT_CONSTRAINT>::modifyGraph(
        const ArcDeletion &change) {}

    /// notify the operator set of a change applied to the graph
    template <typename STRUCT_CONSTRAINT>
    INLINE void GraphChangesGenerator4DiGraph<STRUCT_CONSTRAINT>::modifyGraph(
        const ArcReversal &change) {}

    /// notify the operator set of a change applied to the graph
    template <typename STRUCT_CONSTRAINT>
    INLINE void GraphChangesGenerator4DiGraph<STRUCT_CONSTRAINT>::modifyGraph(
        const GraphChange &change) {}

    /// notifies the generator that we have parsed all its legal changes
    template <typename STRUCT_CONSTRAINT>
    INLINE void
    GraphChangesGenerator4DiGraph<STRUCT_CONSTRAINT>::notifyGetCompleted() {
      if (_legal_changes.size())
        _legal_changes.clear();
    }

    /// sets the maximum number of threads used to perform countings
    template <typename STRUCT_CONSTRAINT>
    INLINE void GraphChangesGenerator4DiGraph<STRUCT_CONSTRAINT>::setMaxNbThreads(
        unsigned int nb) noexcept {
#if defined(_OPENMP) && defined(NDEBUG)
      if (nb == 0)
        nb = getMaxNumberOfThreads();
      __max_threads_number = nb;
#else
      __max_threads_number = 1;
#endif /* _OPENMP && NDEBUG */
    }

    /// returns the constraint that is used by the generator
    template <typename STRUCT_CONSTRAINT>
    INLINE STRUCT_CONSTRAINT &
    GraphChangesGenerator4DiGraph<STRUCT_CONSTRAINT>::constraint() const noexcept {
      return *_constraint;
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
