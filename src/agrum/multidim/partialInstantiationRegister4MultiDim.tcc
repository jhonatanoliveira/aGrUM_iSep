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
/** @file
 * @brief A container for registering partial instantiations on
 * multiDimImplementations
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <agrum/config.h>

#include <agrum/multidim/partialInstantiationRegister4MultiDim.h>

namespace gum {

  /// adds a new entry into the register
  template <typename GUM_SCALAR>
  void PartialInstantiationRegister4MultiDim<GUM_SCALAR>::insert(
      const std::string &instantiation_func_name, const std::string &type_multidim,
      typename PartialInstantiationRegister4MultiDim<
          GUM_SCALAR>::PartialInstantiationPtr newFunction) {
    // insert the new entry
    PartialInstantiationSet *theset;

    if (!__set.exists(instantiation_func_name)) {
      theset =
          __set.insert(instantiation_func_name, new PartialInstantiationSet).second;
#ifndef NDEBUG
      // for debugging purposes, we should inform the aGrUM's debugger that
      // the hashtable contained within the PartialInstantiationRegister4MultiDim
      // will be removed at the end of the program's execution.
      __debug__::__inc_deletion("HashTable", __FILE__, __LINE__, "destructor of",
                                (void *)theset);
#endif /* NDEBUG */
    } else {
      theset = __set[instantiation_func_name];
    }

    theset->insert(type_multidim, newFunction);
  }

  /// removes a given entry from the register
  template <typename GUM_SCALAR>
  void PartialInstantiationRegister4MultiDim<GUM_SCALAR>::erase(
      const std::string &instantiation_func_name, const std::string &type_multidim) {
    if (!__set.exists(instantiation_func_name))
      return;

    PartialInstantiationSet *theset = __set[instantiation_func_name];

    theset->erase(type_multidim);
  }

  /// indicates whether a given entry exists in the register
  template <typename GUM_SCALAR>
  INLINE bool PartialInstantiationRegister4MultiDim<GUM_SCALAR>::exists(
      const std::string &instantiation_func_name,
      const std::string &type_multidim) const {
    if (!__set.exists(instantiation_func_name))
      return false;

    return __set[instantiation_func_name].exists(type_multidim);
  }

  /** @brief returns the specialized operator assigned to a given subtype of
   * MultiDimImplementation */
  template <typename GUM_SCALAR>
  INLINE typename PartialInstantiationRegister4MultiDim<
      GUM_SCALAR>::PartialInstantiationPtr
  PartialInstantiationRegister4MultiDim<GUM_SCALAR>::get(
      const std::string &instantiation_func_name,
      const std::string &type_multidim) const {
    PartialInstantiationSet *theset = __set[instantiation_func_name];
    return theset->operator[](type_multidim);
  }

  /// a named constructor that constructs one and only one Register per data type
  template <typename GUM_SCALAR>
  PartialInstantiationRegister4MultiDim<GUM_SCALAR> &
  PartialInstantiationRegister4MultiDim<GUM_SCALAR>::Register() {
    static PartialInstantiationRegister4MultiDim container;

#ifndef NDEBUG
    static bool first = true;

    if (first) {
      first = false;
      // for debugging purposes, we should inform the aGrUM's debugger that
      // the hashtable contained within the PartialInstantiationRegister4MultiDim
      // will be removed at the end of the program's execution.
      __debug__::__inc_deletion("HashTable", __FILE__, __LINE__, "destructor of",
                                (void *)&container.__set);
    }

#endif /* NDEBUG */

    return container;
  }

  /// Default constructor: creates an empty register
  template <typename GUM_SCALAR>
  PartialInstantiationRegister4MultiDim<
      GUM_SCALAR>::PartialInstantiationRegister4MultiDim() {}

  /// destructor
  template <typename GUM_SCALAR>
  PartialInstantiationRegister4MultiDim<
      GUM_SCALAR>::~PartialInstantiationRegister4MultiDim() {
    // remove all the sets
    for (typename HashTable<std::string, PartialInstantiationSet *>::iterator_safe
             iter = __set.beginSafe();
         iter != __set.endSafe(); ++iter)
      delete iter.val();
  }

  /// a function to more easily register new instantiation functions in MultiDims
  template <typename GUM_SCALAR>
  void
  registerPartialInstantiation(const std::string &instantiation_func_name,
                               const std::string &type_multidim,
                               typename PartialInstantiationRegister4MultiDim<
                                   GUM_SCALAR>::PartialInstantiationPtr function) {
    PartialInstantiationRegister4MultiDim<GUM_SCALAR>::Register().insert(
        instantiation_func_name, type_multidim, function);
  }

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */