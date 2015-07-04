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

// to ease IDE parser
#include <agrum/BN/inference/iSeparation.h>

namespace gum {

  template <typename GUM_SCALAR>
  INLINE
  iSeparation::iSeparation(const IBayesNet<GUM_SCALAR>& bn) {
    GUM_CONSTRUCTOR(iSeparation);
    this->__dag = &( bn.dag () );
  }

  INLINE
  iSeparation::~iSeparation() {
    GUM_DESTRUCTOR(iSeparation); 
  }

} /* namespace gum */
