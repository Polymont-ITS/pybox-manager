///-------------------------------------------------------------------------------------------------
/// 
/// \file NewBoxPattern.h
/// \brief Class NewBoxPattern
/// \author Thibaut Monseigne (Inria) & Jimmy Leblanc (Polymont) & Yannis Bendi-Ouis (Polymont) 
/// \version 1.0.
/// \date 12/03/2020.
/// \copyright <a href="https://choosealicense.com/licenses/agpl-3.0/%22%3EGNU Affero General Public License v3.0</a>.
/// 
///-------------------------------------------------------------------------------------------------
#pragma once

#if defined TARGET_HAS_ThirdPartyPython

#include <Python.h>

#if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 2)

#include "../ovp_defines.h"
#include "CPolyBox.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <string>
#include <vector>
//#include <map>

namespace OpenViBEPlugins
{
	namespace Python
	{
		class CBoxAlgorithmProcessML final : public CPolyBox
		{
		public:
			CBoxAlgorithmProcessML() { m_sScriptFilename = "../../extras/contrib/applications/developer-tools/pybox-manager/ScriptBox/ProcessML.py";}
			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ProcessML);
		};
		
		class CBoxAlgorithmProcessMLListener final : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:
			bool onInputAdded(OpenViBE::Kernel::IBox& box, const uint32_t index) override
			{
				box.setInputType(index, OV_TypeId_StreamedMatrix);
				return true;
			};

			bool onOutputAdded(OpenViBE::Kernel::IBox& box, const uint32_t index) override
			{
				box.setOutputType(index, OV_TypeId_StreamedMatrix);
				return true;
			};
			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};
		
		class CBoxAlgorithmProcessMLDesc final : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("ProcessML"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Jimmy Leblanc & Yannis Bendi-Ouis"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("Polymont IT Services"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("This box aim to use a machine learning model previously trained to predict labels of input's data."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Scripting/PyBox/Classification"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("0.1"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-missing-image"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_ProcessML; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new CBoxAlgorithmProcessML; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmProcessMLListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const
			{
				prototype.addSetting("Clock frequency (Hz)", OV_TypeId_Integer, "64");
				// <tag> settings
                prototype.addSetting("Model filename", OV_TypeId_Filename, "");
                prototype.addSetting("Predictions filename", OV_TypeId_Filename, "");
				
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddOutput);
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddSetting);
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifySetting);

				prototype.addInputSupport(OV_TypeId_Signal);
				prototype.addInputSupport(OV_TypeId_Stimulations);
				prototype.addInputSupport(OV_TypeId_StreamedMatrix);

				prototype.addOutputSupport(OV_TypeId_Signal);
				prototype.addOutputSupport(OV_TypeId_Stimulations);
				prototype.addOutputSupport(OV_TypeId_StreamedMatrix);

				// <tag> input & output
                prototype.addOutput("stim_out", OV_TypeId_Stimulations);
                prototype.addInput("input_StreamMatrix", OV_TypeId_StreamedMatrix);
				
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ProcessMLDesc);
		};
	};
};

#endif // #if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 2)

#endif // TARGET_HAS_ThirdPartyPython
