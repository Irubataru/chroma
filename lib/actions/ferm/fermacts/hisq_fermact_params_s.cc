// $Id: hisq_fermact_params_s.cc,v 1.1 2007-05-09 12:43:20 mcneile Exp $
/*! \file
 *  \brief Hisq fermion action parameters
 */

#include "chromabase.h"
#include "actions/ferm/fermacts/hisq_fermact_params_s.h"

#include "io/param_io.h"

namespace Chroma
{
  //! Default constructor
  HisqFermActParams::HisqFermActParams()
  {
    Mass = 0.0;
    u0  = 1.0;
  }


  //! Read parameters
  HisqFermActParams::HisqFermActParams(XMLReader& xml, const string& path)
  {
    XMLReader paramtop(xml, path);

    // Read the stuff for the action
    read(paramtop, "Mass", Mass);
    read(paramtop, "u0", u0);
    
    //  Read optional anisoParam.
  }

  //! Read parameters
  void read(XMLReader& xml, const string& path, HisqFermActParams& param)
  {
    HisqFermActParams tmp(xml, path);
    param = tmp;
  }

  //! Writer parameters
  void write(XMLWriter& xml, const string& path, const HisqFermActParams& param)
  {
    push(xml, path);

    write(xml, "Mass", param.Mass);
    write(xml, "u0", param.u0);
    pop(xml);
  }
}
