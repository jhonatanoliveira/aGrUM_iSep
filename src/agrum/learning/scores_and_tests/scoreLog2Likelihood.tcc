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
/** @file
 * @brief the class for computing Log2Likelihood scores
 *
 * @author Christophe GONZALES and Pierre-Henri WUILLEMIN
 */

#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <cmath>

namespace gum {

  namespace learning {

    /// default constructor
    template <typename IdSetAlloc, typename CountAlloc>
    template <typename RowFilter>
    INLINE ScoreLog2Likelihood<IdSetAlloc, CountAlloc>::ScoreLog2Likelihood(
        const RowFilter &filter, const std::vector<unsigned int> &var_modalities,
        Apriori<IdSetAlloc, CountAlloc> &apriori)
        : Score<IdSetAlloc, CountAlloc>(filter, var_modalities, apriori) {
      // for debugging purposes
      GUM_CONSTRUCTOR(ScoreLog2Likelihood);
    }

    /// copy constructor
    template <typename IdSetAlloc, typename CountAlloc>
    ScoreLog2Likelihood<IdSetAlloc, CountAlloc>::ScoreLog2Likelihood(
        const ScoreLog2Likelihood<IdSetAlloc, CountAlloc> &from)
        : Score<IdSetAlloc, CountAlloc>(from),
          __internal_apriori(from.__internal_apriori) {
      GUM_CONS_CPY(ScoreLog2Likelihood);
    }

    /// move constructor
    template <typename IdSetAlloc, typename CountAlloc>
    ScoreLog2Likelihood<IdSetAlloc, CountAlloc>::ScoreLog2Likelihood(
        ScoreLog2Likelihood<IdSetAlloc, CountAlloc> &&from)
        : Score<IdSetAlloc, CountAlloc>(std::move(from)),
          __internal_apriori(std::move(from.__internal_apriori)) {
      GUM_CONS_MOV(ScoreLog2Likelihood);
    }

    /// virtual copy factory
    template <typename IdSetAlloc, typename CountAlloc>
    ScoreLog2Likelihood<IdSetAlloc, CountAlloc> *
    ScoreLog2Likelihood<IdSetAlloc, CountAlloc>::copyFactory() const {
      return new ScoreLog2Likelihood<IdSetAlloc, CountAlloc>(*this);
    }

    /// destructor
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE ScoreLog2Likelihood<IdSetAlloc, CountAlloc>::~ScoreLog2Likelihood() {
      // for debugging purposes
      GUM_DESTRUCTOR(ScoreLog2Likelihood);
    }

    /// indicates whether the apriori is compatible (meaningful) with the score
    template <typename IdSetAlloc, typename CountAlloc>
    bool ScoreLog2Likelihood<IdSetAlloc, CountAlloc>::isAprioriCompatible(
        const std::string &apriori_type, float weight) {
      // check that the apriori is compatible with the score
      if ((apriori_type == AprioriDirichletType::type) ||
          (apriori_type == AprioriSmoothingType::type) ||
          (apriori_type == AprioriNoAprioriType::type)) {
        return true;
      }

      // apriori types unsupported by the type checker
      std::stringstream msg;
      msg << "The apriori '" << apriori_type
          << "' is not yet supported by method isAprioriCompatible";
      GUM_ERROR(InvalidArgument, msg.str());
    }

    /// indicates whether the apriori is compatible (meaningful) with the score
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE bool ScoreLog2Likelihood<IdSetAlloc, CountAlloc>::isAprioriCompatible(
        const Apriori<IdSetAlloc, CountAlloc> &apriori) {
      return isAprioriCompatible(apriori.getType(), apriori.weight());
    }

    /// indicates whether the apriori is compatible (meaningful) with the score
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE bool
    ScoreLog2Likelihood<IdSetAlloc, CountAlloc>::isAprioriCompatible() const {
      return isAprioriCompatible(*this->_apriori);
    }

    /// returns the internal apriori of the score
    template <typename IdSetAlloc, typename CountAlloc>
    INLINE const ScoreInternalApriori<IdSetAlloc, CountAlloc> &
    ScoreLog2Likelihood<IdSetAlloc, CountAlloc>::internalApriori() const noexcept {
      return __internal_apriori;
    }

    /// returns the score corresponding to a given nodeset
    template <typename IdSetAlloc, typename CountAlloc>
    float
    ScoreLog2Likelihood<IdSetAlloc, CountAlloc>::score(unsigned int nodeset_index) {
      // if the score has already been computed, get its value
      if (this->_isInCache(nodeset_index)) {
        return this->_cachedScore(nodeset_index);
      }

      // get the counts for all the targets and for the conditioning nodes
      const std::vector<float, CountAlloc> &N_ijk =
          this->_getAllCounts(nodeset_index);
      const unsigned int targets_modal = N_ijk.size();
      float score = 0;

      // get the nodes involved in the score
      const std::vector<unsigned int, IdSetAlloc> *conditioning_nodes =
          this->_getConditioningNodes(nodeset_index);

      // here, we distinguish nodesets with conditioning nodes from those
      // without conditioning nodes
      if (conditioning_nodes) {
        // get the counts for all the targets and for the conditioning nodes
        const std::vector<float, CountAlloc> &N_ij =
            this->_getConditioningCounts(nodeset_index);
        const unsigned int conditioning_modal = N_ij.size();

        if (this->_apriori->weight()) {
          const std::vector<float, CountAlloc> &N_prime_ijk =
              this->_getAllApriori(nodeset_index);
          const std::vector<float, CountAlloc> &N_prime_ij =
              this->_getConditioningApriori(nodeset_index);

          // compute the score: it remains to compute the log likelihood, i.e.,
          // sum_k=1^r_i sum_j=1^q_i N_ijk log (N_ijk / N_ij), which is also
          // equivalent to:
          // sum_j=1^q_i sum_k=1^r_i N_ijk log N_ijk - sum_j=1^q_i N_ij log N_ij
          for (unsigned int k = 0; k < targets_modal; ++k) {
            const float new_count = N_ijk[k] + N_prime_ijk[k];
            if (new_count) {
              score += new_count * logf(new_count);
            }
          }
          for (unsigned int j = 0; j < conditioning_modal; ++j) {
            const float new_count = N_ij[j] + N_prime_ij[j];
            if (new_count) {
              score -= new_count * logf(new_count);
            }
          }
        } else {
          // compute the score: it remains to compute the log likelihood, i.e.,
          // sum_k=1^r_i sum_j=1^q_i N_ijk log (N_ijk / N_ij), which is also
          // equivalent to:
          // sum_j=1^q_i sum_k=1^r_i N_ijk log N_ijk - sum_j=1^q_i N_ij log N_ij
          for (unsigned int k = 0; k < targets_modal; ++k) {
            if (N_ijk[k]) {
              score += N_ijk[k] * logf(N_ijk[k]);
            }
          }
          for (unsigned int j = 0; j < conditioning_modal; ++j) {
            if (N_ij[j]) {
              score -= N_ij[j] * logf(N_ij[j]);
            }
          }
        }

        // divide by log(2), since the log likelihood uses log_2
        score *= this->_1log2;

        // shall we put the score into the cache?
        if (this->_isUsingCache()) {
          this->_insertIntoCache(nodeset_index, score);
        }

        return score;
      } else {
        // here, there are no conditioning nodes

        // get the counts for all the targets and for the conditioning nodes
        const std::vector<float, CountAlloc> &N_ijk =
            this->_getAllCounts(nodeset_index);
        const unsigned int targets_modal = N_ijk.size();

        if (this->_apriori->weight()) {
          const std::vector<float, CountAlloc> &N_prime_ijk =
              this->_getAllApriori(nodeset_index);

          // compute the score: it remains to compute the log likelihood, i.e.,
          // sum_k=1^r_i N_ijk log (N_ijk / N), which is also
          // equivalent to:
          // sum_j=1^q_i sum_k=1^r_i N_ijk log N_ijk - N log N
          float N_plus_N_prime = 0;
          for (unsigned int k = 0; k < targets_modal; ++k) {
            const float new_count = N_ijk[k] + N_prime_ijk[k];
            if (new_count) {
              score += new_count * logf(new_count);
              N_plus_N_prime += new_count;
            }
          }
          score -= N_plus_N_prime * logf(N_plus_N_prime);
        } else {
          // compute the score: it remains to compute the log likelihood, i.e.,
          // sum_k=1^r_i N_ijk log (N_ijk / N), which is also
          // equivalent to:
          // sum_j=1^q_i sum_k=1^r_i N_ijk log N_ijk - N log N
          float N = 0;
          for (unsigned int k = 0; k < targets_modal; ++k) {
            if (N_ijk[k]) {
              score += N_ijk[k] * logf(N_ijk[k]);
              N += N_ijk[k];
            }
          }
          score -= N * logf(N);
        }

        // divide by log(2), since the log likelihood uses log_2
        score *= this->_1log2;

        // shall we put the score into the cache?
        if (this->_isUsingCache()) {
          this->_insertIntoCache(nodeset_index, score);
        }

        return score;
      }
    }

  } /* namespace learning */

} /* namespace gum */

#endif /* DOXYGEN_SHOULD_SKIP_THIS */