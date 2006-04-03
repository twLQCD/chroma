// $Id: inline_erase_obj.cc,v 3.0 2006-04-03 04:59:03 edwards Exp $
/*! \file
 * \brief Inline task to erase an object from a named buffer
 *
 * Named object writing
 */

#include "meas/inline/abs_inline_measurement_factory.h"
#include "meas/inline/io/inline_erase_obj.h"
#include "meas/inline/io/named_objmap.h"

namespace Chroma 
{ 
  namespace InlineEraseNamedObjEnv 
  { 
    AbsInlineMeasurement* createMeasurement(XMLReader& xml_in, 
					    const std::string& path) 
    {
      return new InlineEraseNamedObj(InlineEraseNamedObjParams(xml_in, path));
    }

    const std::string name = "ERASE_NAMED_OBJECT";

    bool registerAll() 
    {
      bool success = true; 

      // Inline measurement
      success &= TheInlineMeasurementFactory::Instance().registerObject(name, createMeasurement);

      return success;
    }

    const bool registered = registerAll();
  };


  //! Object buffer
  void write(XMLWriter& xml, const string& path, const InlineEraseNamedObjParams::NamedObject_t& input)
  {
    push(xml, path);

    write(xml, "object_id", input.object_id);

    pop(xml);
  }


  //! Object buffer
  void read(XMLReader& xml, const string& path, InlineEraseNamedObjParams::NamedObject_t& input)
  {
    XMLReader inputtop(xml, path);

    read(inputtop, "object_id", input.object_id);
  }


  // Param stuff
  InlineEraseNamedObjParams::InlineEraseNamedObjParams() { frequency = 0; }

  InlineEraseNamedObjParams::InlineEraseNamedObjParams(XMLReader& xml_in, const std::string& path) 
  {
    try 
    {
      XMLReader paramtop(xml_in, path);

      if (paramtop.count("Frequency") == 1)
	read(paramtop, "Frequency", frequency);
      else
	frequency = 1;

      // Ids
      read(paramtop, "NamedObject", named_obj);
    }
    catch(const std::string& e) 
    {
      QDPIO::cerr << __func__ << ": caught Exception reading XML: " << e << endl;
      QDP_abort(1);
    }
  }


  void
  InlineEraseNamedObjParams::write(XMLWriter& xml_out, const std::string& path) 
  {
    push(xml_out, path);
    
    // Ids
    Chroma::write(xml_out, "NamedObject", named_obj);

    pop(xml_out);
  }


  void 
  InlineEraseNamedObj::operator()(unsigned long update_no,
				  XMLWriter& xml_out) 
  {
    START_CODE();

    push(xml_out, "erase_named_obj");
    write(xml_out, "update_no", update_no);

    QDPIO::cout << InlineEraseNamedObjEnv::name << ": object erase" << endl;

    // Erase the object
    QDPIO::cout << "Attempt to erase object name = " << params.named_obj.object_id << endl;
    write(xml_out, "object_id", params.named_obj.object_id);
    try
    {
      // Now erase the object
      TheNamedObjMap::Instance().erase(params.named_obj.object_id);

      QDPIO::cout << "Object erased" << endl;
    }
    catch( std::bad_cast ) 
    {
      QDPIO::cerr << InlineEraseNamedObjEnv::name << ": cast error" 
		  << endl;
      QDP_abort(1);
    }
    catch (const string& e) 
    {
      QDPIO::cerr << InlineEraseNamedObjEnv::name << ": error message: " << e 
		  << endl;
      QDP_abort(1);
    }
    
    QDPIO::cout << InlineEraseNamedObjEnv::name << ": ran successfully" << endl;

    pop(xml_out);  // erase_named_obj

    END_CODE();
  } 

};
