/****************************************************************************************[Dimacs.h]
Copyright (c) 2003-2006, Niklas Een, Niklas Sorensson
Copyright (c) 2007-2010, Niklas Sorensson

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**************************************************************************************************/

#ifndef Minisat_Dimacs_h
#define Minisat_Dimacs_h

#include <math.h>
#include <stdio.h>
#include <iostream>

#ifdef GLUCOSE
#include "glucose/core/SolverTypes.h"
#include "glucose/utils/ParseUtils.h"
#else
#include "minisat/core/SolverTypes.h"
#include "minisat/utils/ParseUtils.h"
#endif

#include "maxhs/core/MaxSolverTypes.h"
#include "maxhs/core/Wcnf.h"

using std::cerr;
using std::cout;

#ifdef GLUCOSE
namespace Minisat = Glucose;
#endif

template <class B>
static long double parseIntegerLongDouble(B& in) {
  long double val = 0;
  bool neg = false;
  skipWhitespace(in);
  if (*in == '-')
    neg = true, ++in;
  else if (*in == '+')
    ++in;
  if (*in < '0' || *in > '9')
    cerr << "PARSE ERROR! Unexpected char: " << *in << "\n", exit(3);
  while (*in >= '0' && *in <= '9') val = val * 10 + (*in - '0'), ++in;
  return neg ? -val : val;
}

// JD
template <class B>
static long double parseLongDouble(B& in) {
  long double val = 0;
  bool neg = false;
  bool frac = false;
  int exponent = 0;
  int n = -1;
  skipWhitespace(in);
  if (*in == '-')
    neg = true, ++in;
  else if (*in == '+')
    ++in;
  if ((*in < '0' || *in > '9') && *in != '.' && *in != 'e' && *in != '-' &&
      *in != '+')
    cerr << "PARSE ERROR 3! Unexpected char: " << *in << "\n", exit(3);
  while ((*in >= '0' && *in <= '9') || *in == '.' || *in == 'e' || *in == '-' ||
         *in == '+') {
    if (*in == '.')
      frac = true;
    else if (*in == 'e') {
      bool expNeg = false;
      ++in;
      while (*in == '-' || *in == '0' || *in == '+') {
        if (*in == '-') expNeg = true;
        ++in;
      }
      int intExp = parseInt(in);
      exponent = expNeg ? -intExp : intExp;
    } else
      frac ? val = val + (*in - '0') * pow(10.0, n),
             --n : val = val * 10 + (*in - '0');
    ++in;
  }
  val = val * pow(10, exponent);
  return neg ? -val : val;
}

//=================================================================================================
// DIMACS Parser:

template <class B>
static void readClause(B& in, vector<Lit>& lits) {
  int parsed_lit, var;
  lits.clear();
  for (;;) {
    parsed_lit = parseInt(in);
    if (parsed_lit == 0) break;
    var = abs(parsed_lit) - 1;
    lits.push_back((parsed_lit > 0) ? mkLit(var) : ~mkLit(var));
  }
}

// JD Read clauses with weights
template <class B>
static void readClause(B& in, vector<Lit>& lits, Weight& outW) {
  bool first_time = true;
  int parsed_lit, var;
  lits.clear();
  for (;;) {
    if (first_time) {
      first_time = false;
      outW = parseLongDouble(in);
      continue;
    }
    parsed_lit = parseInt(in);
    if (parsed_lit == 0) break;
    var = abs(parsed_lit) - 1;
    lits.push_back((parsed_lit > 0) ? mkLit(var) : ~mkLit(var));
  }
}

template <class B>
static void readClause_new(B& in, vector<Lit>& lits, Weight& outW) {
    bool first_time = true;
    int parsed_lit, var;
    lits.clear();
    for (;;) {
        if (first_time) {
            first_time = false;
            if(*in=='h'){
                outW=INT64_MAX;
                ++in;
            }
            else
                outW = parseLongDouble(in);
            continue;
        }
        parsed_lit = parseInt(in);
        if (parsed_lit == 0) break;
        var = abs(parsed_lit) - 1;
        lits.push_back((parsed_lit > 0) ? mkLit(var) : ~mkLit(var));
    }
}

/********************************************************/

template <class B>
static bool parse_DIMACS_old(B& in, Wcnf* F) {
  vector<Lit> lits;
  int clauses = 0;
  // JD Note that partial maxsat clauses either have weight 1 or partial_Top
  bool clausesHaveWeights = false;

  assert(*in == 'p') ;
    // JD it could be unweighted or weighted CNF
    if (eagerMatch(in, "p cnf")) {
        int nvars = parseInt(in);
        clauses = parseInt(in);
        F->set_dimacs_params(nvars, clauses);
        // JD
    } else if (eagerMatch(in, "wcnf")) {
        clausesHaveWeights = true;
        int nvars = parseInt(in);
        clauses = parseInt(in);
        if (!eagerMatch(in, "\n")) {
            Weight top = parseIntegerLongDouble(in);
            F->set_dimacs_params(nvars, clauses, top);
        } else {  // no top => no hard clauses => no upper bound on soft clause
            // weight.
            F->set_dimacs_params(nvars, clauses);
        }
    } else {
        // not 'p cnf' or 'p wcnf'
        return false;
    }

  for (;;) {
    skipWhitespace(in);
    if (*in == EOF)
      break;
     else if (*in == 'c') {
      skipLine(in);
    } else {
      // JD parse the weights of clauses as well (default weight 1)
      // printf("A| parse clause\n");
      Weight w = 1;
      clausesHaveWeights ? readClause(in, lits, w) : readClause(in, lits);
      F->addDimacsClause(lits, w);
    }
  }
  return true;
}

template <class B>
static bool parse_DIMACS_new(B& in, Wcnf* F) {
    vector<Lit> lits;
    // JD Note that partial maxsat clauses either have weight 1 or partial_Top
    F->set_dimacs_params(0, 0, INT64_MAX-1);

    for (;;) {
        skipWhitespace(in);
        if (*in == EOF)
            break;
        else if (*in == 'c') {
            skipLine(in);
        } else {
            // JD parse the weights of clauses as well (default weight 1)
            // printf("A| parse clause\n");
            Weight w = 1;
            readClause_new(in, lits, w);
            F->addDimacsClause(lits, w);
        }
    }
    return true;
}


// Inserts problem into solver.
//
static bool parse_DIMACS(gzFile input_stream, Wcnf* F) {
  Minisat::StreamBuffer in(input_stream);
    for (;;) {
        skipWhitespace(in);
        if (*in == EOF) break;
        else if (*in == 'p') {
            return parse_DIMACS_old(in, F);
            break;
        } else if (*in == 'h' || *in == '-' || (*in >= '0' && *in <= '9')) {
            return parse_DIMACS_new(in, F);
            break;
        }
        else if (*in == 'c')
            skipLine(in);

    }

  return false;
}

#endif
