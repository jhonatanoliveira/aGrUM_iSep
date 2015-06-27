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
 * @brief exists aggregator
 *
* @author Pierre-Henri WUILLEMIN et Christophe GONZALES <{prenom.nom}_at_lip6.fr>
 */
#include <sstream>
// to ease parser in IDEs
#include <agrum/multidim/aggregators/exists.h>

namespace gum {

  namespace aggregator {
    template <typename GUM_SCALAR>
    INLINE Exists<GUM_SCALAR>::Exists(Idx value)
        : MultiDimAggregator<GUM_SCALAR>(), __value(value) {
      GUM_CONSTRUCTOR(Exists)
    }

    template <typename GUM_SCALAR>
    INLINE Exists<GUM_SCALAR>::Exists(const Exists<GUM_SCALAR> &from)
        : MultiDimAggregator<GUM_SCALAR>(from) {
      __value = from.__value;
      GUM_CONS_CPY(Exists);
    }

    template <typename GUM_SCALAR> INLINE Exists<GUM_SCALAR>::~Exists() {
      GUM_DESTRUCTOR(Exists);
    }

    template <typename GUM_SCALAR>
    INLINE Idx Exists<GUM_SCALAR>::_neutralElt() const {
      return (Idx)0;
    }

    template <typename GUM_SCALAR>
    INLINE Idx Exists<GUM_SCALAR>::_folder(const DiscreteVariable &v, Idx i1, Idx i2,
                                           bool &stop_iteration) const {
      if (i1 != __value) {
        return (Idx)0;
      } else {
        stop_iteration = true;
        return (Idx)1;
      }
    }

    template <typename GUM_SCALAR>
    INLINE

        std::string
        Exists<GUM_SCALAR>::aggregatorName(void) const {
      std::stringstream ss;
      ss << "exists[" << __value << "]";
      return ss.str();
    }

    template <typename GUM_SCALAR>
    INLINE MultiDimContainer<GUM_SCALAR> *Exists<GUM_SCALAR>::newFactory() const {
      return new Exists<GUM_SCALAR>(__value);
    }

  } // namespace aggregator
} // namespace gum
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; ;
