/***************************************************************************
 *   Copyright (C) 2005 by Christophe GONZALES et Pierre-Henri WUILLEMIN   *
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
 * @brief Implementation of lazy propagation for inference in Bayesian Networks.
 */
#ifndef GUM_LAZY_PROPAGATION_OLD_H
#define GUM_LAZY_PROPAGATION_OLD_H

#include <math.h>

#include <agrum/BN/inference/BayesNetInference.h>
#include <agrum/graphs/triangulations/defaultTriangulation.h>

namespace gum {
  /**
   * @class LazyPropagationOld lazyPropagationOld.h <agrum/BN/inference/lazyPropagationOld.h>
   * @brief Implementation of lazy propagation for inference in Bayesian Networks
   * @ingroup bn_group
   */
  template <typename GUM_SCALAR>

  class LazyPropagationOld : public BayesNetInference<GUM_SCALAR> {
  public:
    // ############################################################################
    /// @name Constructors / Destructors
    // ############################################################################
    /// @{

    /// default constructor

    LazyPropagationOld(const IBayesNet<GUM_SCALAR> &BN);

    /// constructor with a given elimination sequence
    LazyPropagationOld(const IBayesNet<GUM_SCALAR> &BN,
                       const std::vector<NodeId> &elim_order);

    /// destructor
    ~LazyPropagationOld();

    /// @}

    // ############################################################################
    /// @name Accessors / Modifiers
    // ############################################################################
    /// @{

    /// insert new evidence in the graph
    /** @warning if an evidence already exists w.r.t. a given node and a new
     * evidence w.r.t. this node is inserted, the old evidence is removed. */
    virtual void insertEvidence(const List<const Potential<GUM_SCALAR> *> &);

    /// remove all evidence from the graph
    virtual void eraseAllEvidence();

    /// remove a given evidence from the graph
    virtual void eraseEvidence(const Potential<GUM_SCALAR> *);

    /// performs the collect phase of Lazy PropagationOld
    void collect(NodeId id, bool force_collect = false);

    /// performs the diffusion phase of Lazy PropagationOld
    void diffusion(NodeId id, bool force_diffusion = false);

    /// perfoms a whole inference (with force_inference flag at false)
    virtual void makeInference();

    /// performs a whole inference (collect + diffusion)
    void makeInference(bool force_inference);

    /// returns the probability P(e) of the evidence enterred into the BN
    GUM_SCALAR evidenceProbability();

    /// returns the joint a posteriori probability P(nodes|e)
    /** @warning right now, method joint cannot compute joint a posteriori
     * probabilities of every nodeset. In cases where it is not able to perform
     * properly this task, it will raise a OperationNotAllowed exception.
     * @warning : joint computes a new Potential<GUM_SCALAR> and returns a pointer :
     * do not forget to free it !
     * @return a pointer to a dynamically allocated Potential<GUM_SCALAR>
     * @throw OperationNotAllowed
     */
    Potential<GUM_SCALAR> *joint(const NodeSet &nodes);

    const JunctionTree *junctionTree() const;
    /// @}

    /// @name Information Theory related functions
    /// @{

    /** Entropy
     * Compute Shanon's entropy of a node given the observation
     * @see http://en.wikipedia.org/wiki/Information_entropy
     */
    GUM_SCALAR H(NodeId X);

    /** Mutual information between X and Y
     * @see http://en.wikipedia.org/wiki/Mutual_information
     *
     * @warning Due to limitation of @ref joint, may not be able to compute this value
     * @throw OperationNotAllowed in these cases
     */
    GUM_SCALAR I(NodeId X, NodeId Y);

    /** Variation of information between X and Y
     * @see http://en.wikipedia.org/wiki/Variation_of_information
     *
     * @warning Due to limitation of @ref joint, may not be able to compute this value
     * @throw OperationNotAllowed in these cases
     */
    GUM_SCALAR VI(NodeId X, NodeId Y);
    /// @}

  protected:
    /**
     * Returns the probability of the variable.
     *
     * @param id The variable's id.
     * @param posterior the potential to fill
     * @throw ElementNotFound Raised if no variable matches id.
     */
    virtual void _fillPosterior(Id id, Potential<GUM_SCALAR> &posterior);

  private:
    typedef Set<const Potential<GUM_SCALAR> *> __PotentialSet;
    typedef SetIteratorSafe<const Potential<GUM_SCALAR> *> __PotentialSetIterator;

    /// the triangulation class creating the junction tree used for inference
    DefaultTriangulation __triangulation;
    /// the junction tree associated to the bayes net
    JunctionTree *__JT;

    /// for each node of the bayes net, associate an ID in the JT
    HashTable<NodeId, NodeId> __node_to_clique;

    /// the list of all potentials stored in the cliques
    NodeProperty<List<const Potential<GUM_SCALAR> *>> __clique_potentials;

    /// the list of all the evidence stored in the cliques
    NodeProperty<List<const Potential<GUM_SCALAR> *>> __clique_evidence;

    /// the list of all potentials stored in the separators
    ArcProperty<__PotentialSet> __sep_potentials;

    /// a list of all the evidence stored into the graph
    __PotentialSet __evidences;

    /// the list of all potentials created during a propagation phase
    __PotentialSet __created_potentials;

    /// an indicator of whether the collect phase passed through a given clique
    NodeProperty<bool> __collected_cliques;

    /// an indicator of whether the diffusion phase passed through a given clique
    NodeProperty<bool> __diffused_cliques;

    /// the id of the last clique from which we performed a collect
    NodeId __last_collect_clique;

    NodeSet __roots;

    /// creates the message sent by clique from_id to clique to_id

    void __produceMessage(NodeId from_id, NodeId to_id);

    /// actually perform the collect phase

    void __collect(NodeId id, NodeId from);

    /// actually perform the diffusion phase

    void __diffusion(NodeId id, NodeId from);

    /// indicates that we need inference in a given Junction tree connected component
    /** This function indicates to the class that part of the junction tree needs
     * a new inference. As a result, it updates the messages sent on the separators
     * on this part of the junction tree. */

    void __setRequiredInference(NodeId id, NodeId from);

    /// remove variables del_vars from the list of potentials pot_list
    /** The function actually updates pot_list and, when it returns, pot_list
     * contains the list of potentials resulting from the marginalization of the
     * posterior*/

    void __marginalizeOut(__PotentialSet &pot_list,
                          Set<const DiscreteVariable *> &del_vars);

    void __aPosterioriMarginal(NodeId id, Potential<GUM_SCALAR> &posterior);

    void __aPosterioriJoint(const NodeSet &ids, Potential<GUM_SCALAR> &posterior);

    /// initialization function

    void __initialize(const IBayesNet<GUM_SCALAR> &BN,
                      StaticTriangulation &triangulation,
                      const NodeProperty<Size> &modalities);

    /// avoid copy constructors
    LazyPropagationOld(const LazyPropagationOld<GUM_SCALAR> &);

    /// avoid copy operators
    LazyPropagationOld<GUM_SCALAR> &operator=(const LazyPropagationOld<GUM_SCALAR> &);
  };

  extern template class LazyPropagationOld<float>;
  extern template class LazyPropagationOld<double>;

} /* namespace gum */

#include <agrum/BN/inference/lazyPropagationOld.tcc>

#endif /* GUM_LAZY_PROPAGATION_OLD_H */
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
