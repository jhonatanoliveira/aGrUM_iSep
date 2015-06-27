/***************************************************************************
 *  aGrUM modified frames and atg files for cocoR
 *   Copyright (c) 2005 by Christophe GONZALES and Pierre-Henri WUILLEMIN  *
 *   {prenom.nom}_at_lip6.fr
***************************************************************************/
/*----------------------------------------------------------------------
Compiler Generator Coco/R,
Copyright (c) 1990, 2004 Hanspeter Moessenboeck, University of Linz
extended by M. Loeberbauer & A. Woess, Univ. of Linz
ported to C++ by Csaba Balazs, University of Szeged
with improvements by Pat Terry, Rhodes University

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

As an exception, it is allowed to write an extension of Coco/R that is
used as a plugin in non-free software.

If not otherwise stated, any source code generated by Coco/R (other than
Coco/R itself) does not fall under the GNU General Public License.
-----------------------------------------------------------------------*/

#if !defined(gum_BIF_COCO_PARSER_H__)
#define gum_BIF_COCO_PARSER_H__

#include <agrum/core/cast_unicode.h>

#include <agrum/BN/IBayesNet.h>
#include <agrum/BN/BayesNetFactory.h>

#undef TRY
#define TRY(inst)                                                                   \
  try {                                                                             \
    inst;                                                                           \
  } catch (gum::Exception & e) {                                                    \
    SemErr(e.errorType());                                                          \
  }

#include <iostream>
#include <string>
#include <fstream>
#include "Scanner.h"

namespace gum {
  namespace BIF {

    class Parser {
      private:
      enum {
        _EOF = 0,
        _ident = 1,
        _integer = 2,
        _number = 3,
        _string = 4,
        _largestring = 5
      };
      int maxT;

      Token *dummyToken;
      int errDist;
      int minErrDist;

      void SynErr(int n);
      void Get();
      void Expect(int n);
      bool StartOf(int s);
      void ExpectWeak(int n, int follow);
      bool WeakSeparator(int n, int syFol, int repFol);

      ErrorsContainer __errors;

      public:
      Scanner *scanner;

      Token *t;  // last recognized token
      Token *la; // lookahead token

      gum::IBayesNetFactory *__factory;

      void setFactory(gum::IBayesNetFactory *f) { __factory = f; }

      gum::IBayesNetFactory &factory(void) {
        if (__factory)
          return *__factory;
        GUM_ERROR(gum::OperationNotAllowed,
                  "Please set a factory for scanning BIF file...");
      }

      void SemErr(std::string s) { SemErr(widen(s).c_str()); }

      void Warning(std::string s) { Warning(widen("Warning : " + s).c_str()); }

      void __checkSizeOfProbabilityAssignation(const std::vector<float> &v,
                                               const std::string &var) {
        gum::Size s = (gum::Size)0;
        TRY(s = factory().varInBN(factory().variableId(var)).domainSize());
        if (v.size() < s)
          Warning("Not enough data in probability assignation for node " + var);
        if (v.size() > s)
          Warning("Too many data in probability assignation for node " + var);
      }

      //=====================

      Parser(Scanner *scanner);
      ~Parser();
      void SemErr(const wchar_t *msg);
      void SynErr(const std::wstring &filename, int line, int col, int n);
      void Warning(const wchar_t *msg);
      const ErrorsContainer &errors() const;

      void BIF();
      void NETWORK();
      void VARIABLE();
      void PROBA();
      void IDENT(std::string &name);
      void STRING(std::string &str);
      void PROPERTY();
      void LABELIZE_VAR(int &nbrMod);
      void NBR(int &val);
      void MODALITY_LIST();
      void IDENT_OR_INTEGER(std::string &name);
      void LISTE_PARENTS(std::vector<std::string> &parents);
      void RAW_PROBA(std::vector<float> &v);
      void FACTORIZED_PROBA(std::string &var,
                            const std::vector<std::string> &parents);
      void LISTE_FLOAT(std::vector<float> &v);
      void ASSIGNATION(const std::string &var,
                       const std::vector<std::string> &parents, bool is_first);
      void LISTE_LABELS(const std::vector<std::string> &parents,
                        std::vector<std::string> &labels, unsigned int num_label);
      void FLOAT(float &val);

      void Parse();

    }; // end Parser

  } // namespace
} // namespace

#endif // !defined(COCO_PARSER_H__)
