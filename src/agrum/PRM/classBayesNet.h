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
 * @brief Headers of ClassBayesNet<GUM_SCALAR>.
 *
 * @author Lionel TORTI and Pierre-Henri WUILLEMIN
 */
#ifndef GUM_CLASS_BAYESNET_H
#define GUM_CLASS_BAYESNET_H

#include <list>

#include <agrum/BN/IBayesNet.h>

#include <agrum/PRM/PRM.h>
namespace gum {
  namespace prm {

    /**
     * @class ClassBayesNet<GUM_SCALAR> classBayesNet.h <agrum/PRM/classBayesNet.h>
     * @brief This class decorates a gum::prm::Class<GUM_SCALAR> has an
     *IBaseBayesNet.
     *
     * This class filters Attribute and Aggregate in a way it can be interpreted as
     * a IBayesNet. SlotChains and ReferenceSlot are not represented.
     *
     * Remember that a ClassBayesNet<GUM_SCALAR> does not contain input nodes parents
     *and
     * output nodes children. Thus you should be careful when using one of the
     * BayesNetInference over a ClassBayesNet<GUM_SCALAR> since some variables are
     *missing in
     * the DAG but not in the nodes CPT.
     */
    template <typename GUM_SCALAR>
    class ClassBayesNet : public IBayesNet<GUM_SCALAR> {
      public:
      // ========================================================================
      /// @name Constructors & destructor.
      // ========================================================================
      /// @{

      /// Default constructor.
      /// @param c The Class<GUM_SCALAR> decorated by this ClassBayesNet<GUM_SCALAR>.
      ClassBayesNet<GUM_SCALAR>(const Class<GUM_SCALAR> &c);

      /// Copy constructor.
      ClassBayesNet<GUM_SCALAR>(const ClassBayesNet<GUM_SCALAR> &from);

      /// Copy operator.
      ClassBayesNet<GUM_SCALAR> &operator=(const ClassBayesNet<GUM_SCALAR> &from);

      /// Destructor.
      virtual ~ClassBayesNet<GUM_SCALAR>();

      /// @}
      // ===========================================================================
      /// @name Variable manipulation methods.
      // ===========================================================================
      /// @{

      /**
       * @brief Returns the CPT of a node in this ClassBayesNet<GUM_SCALAR>.
       *
       * If the node is an Aggregate, this will raise an OperationNotAllowed
       * since Aggregate's CPT are define at instantiation only.
       *
       * @param varId a variable id.
       * @return the Potential of varId.
       *
       * @throw NotFound raised if varId does not match any variable in this
       *                 IBayesNet.
       * @throw OperationNotAllowed raised if varId is an Aggregate.
       */
      virtual const Potential<GUM_SCALAR> &cpt(NodeId varId) const;

      /// See gum::IBaseBayesNet::variableNodeMap().
      virtual const VariableNodeMap &variableNodeMap() const;

      /// See gum::IBaseBayesNet::variable().
      virtual const DiscreteVariable &variable(NodeId id) const;

      /// See gum::IBaseBayesNet::nodeId().
      virtual NodeId nodeId(const DiscreteVariable &var) const;

      /// See gum::IBaseBayesNet::idFromName().
      virtual NodeId idFromName(const std::string &name) const;

      /// See gum::IBaseBayesNet::variableFromName().
      virtual const DiscreteVariable &
      variableFromName(const std::string &name) const;

      /// See gum::IBaseBayesNet::modalities().
      const NodeProperty<Size> &modalities() const;

      /// @}
      // ===========================================================================
      /// @name Graphical methods
      // ===========================================================================
      /// @{
      /// @return Returns a dot representation of this IBayesNet.
      virtual std::string toDot() const;

      /// @}
      private:
      /// Mapping between DiscreteVariable and their NodeId
      HashTable<const DiscreteVariable *, const ClassElement<GUM_SCALAR> *>
          __varNodeMap;

      /// Private getter with type checking in case the id is not a formal Attribute.
      /// @throw NotFound Raised if id is not a formal attribute.
      const ClassElement<GUM_SCALAR> &__get(NodeId id) const;

      /// Private getter with type checking in case the id is not a formal Attribute.
      /// @throw NotFound Raised if id is not a formal attribute.
      const ClassElement<GUM_SCALAR> &__get(const std::string &name) const;

      /// The ClassElementContainer decorated by this.
      const Class<GUM_SCALAR> *__class;

      mutable NodeProperty<Size> __modalities;

      void __init(const Class<GUM_SCALAR> &c);
    };

    extern template class ClassBayesNet<double>;

  } /* namespace prm */
} /* namespace gum */

#include <agrum/PRM/classBayesNet.tcc>

#endif /* GUM_CLASS_BAYESNET_H */
