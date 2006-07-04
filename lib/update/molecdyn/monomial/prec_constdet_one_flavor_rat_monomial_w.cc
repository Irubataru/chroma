// $Id: prec_constdet_one_flavor_rat_monomial_w.cc,v 3.1 2006-07-04 02:55:52 edwards Exp $
/*! @file
 * @brief One-flavor collection of even-odd preconditioned 4D ferm monomials
 */

#include "update/molecdyn/monomial/prec_constdet_one_flavor_rat_monomial_w.h"
#include "update/molecdyn/monomial/monomial_factory.h"
#include "update/molecdyn/monomial/genapprox.h"

#include "actions/ferm/fermacts/fermacts_aggregate_w.h"
#include "actions/ferm/fermacts/fermact_factory_w.h"
#include "fermact.h"

namespace Chroma 
{ 
 
  namespace EvenOddPrecConstDetOneFlavorWilsonTypeFermRatMonomialEnv 
  {
    //! Callback
    Monomial< multi1d<LatticeColorMatrix>,
	      multi1d<LatticeColorMatrix> >* createMonomial(XMLReader& xml, const string& path)
    {
      QDPIO::cout << "Create Monomial: " << name << endl;

      return new EvenOddPrecConstDetOneFlavorWilsonTypeFermRatMonomial(
	OneFlavorWilsonTypeFermRatMonomialParams(xml, path));
    }

    const std::string name("ONE_FLAVOR_EOPREC_CONSTDET_FERM_RAT_MONOMIAL");

    bool registerAll()
    {
      bool foo = true;

      foo &= WilsonTypeFermActs4DEnv::registered;
      foo &= TheMonomialFactory::Instance().registerObject(name, createMonomial);

      return foo;
    }

    //! Register the fermact
    const bool registered = registerAll();

  } //end namespace EvenOddPrec OneFlavorWilsonFermRatMonomialEnv



  // Constructor
  EvenOddPrecConstDetOneFlavorWilsonTypeFermRatMonomial::EvenOddPrecConstDetOneFlavorWilsonTypeFermRatMonomial(
    const OneFlavorWilsonTypeFermRatMonomialParams& param) 
  {
    inv_param = param.inv_param;
    nthRoot   = param.nthRoot;

    cout << "Param.fermact is : "<< param.fermact.id << endl;

    std::istringstream is(param.fermact.xml);
    XMLReader fermact_reader(is);
    QDPIO::cout << "Fermact reader holds: " << param.fermact.xml << endl;

    const WilsonTypeFermAct<T,P,Q>* tmp_act = TheWilsonTypeFermActFactory::Instance().createObject(
      param.fermact.id, fermact_reader, param.fermact.path);

    const EvenOddPrecWilsonTypeFermAct<T,P,Q>* downcast =
      dynamic_cast<const EvenOddPrecWilsonTypeFermAct<T,P,Q>*>(tmp_act);

    // Check success of the downcast 
    if( downcast == 0x0 ) {
      QDPIO::cerr << "Unable to downcast FermAct to EvenOddPrecWilsonTypeFermAct in EvenOddPrecOneFlavorWilsonTypeFermRatMonomial()" << endl;
      QDP_abort(1);
    }

    fermact = downcast;    

    //*********************************************************************
    // Remez approx
    // M term
    QDPIO::cout << "Normal operator PFE" << endl;
    generateApprox(fpfe, spfe, sipfe,
		   param.remez.lowerMin, param.remez.upperMax, 
		   -param.expNumPower, 2*param.expDenPower*nthRoot, 
		   param.remez.forceDegree, param.remez.actionDegree,
		   param.remez.digitPrecision);
    //*********************************************************************

    QDPIO::cout << "DONECONSTRUCTIN " << endl;
  }

} //end namespace Chroma


