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

#include "CPolyBox.hpp"

#if defined TARGET_HAS_ThirdPartyPython3 && !(defined(WIN32) && defined(TARGET_BUILDTYPE_Debug))
#if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 3)

namespace OpenViBE { namespace Plugins
{
	namespace PyBox
	{
		class CBoxAlgorithmNewBoxPattern final : public CPolyBox
		{
		public:
			CBoxAlgorithmNewBoxPattern() { m_script = "NewScript.py";}
			_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm < IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_NewBoxPattern)
		};
		
		class CBoxAlgorithmNewBoxPatternListener final : public Toolkit::TBoxListener<IBoxListener>
		{
		public:
			bool onInputAdded(Kernel::IBox& box, const size_t index) override
			{
				box.setInputType(index, OV_TypeId_StreamedMatrix);
				return true;
			}

			bool onOutputAdded(Kernel::IBox& box, const size_t index) override
			{
				box.setOutputType(index, OV_TypeId_StreamedMatrix);
				return true;
			}
			_IsDerivedFromClass_Final_(Toolkit::TBoxListener < IBoxListener >, CIdentifier::undefined())
		};
		
		class CBoxAlgorithmNewBoxPatternDesc final : virtual public IBoxAlgorithmDesc
		{
		public:

			void release() override { }

			CString getName() const override                { return "NewBoxPattern"; }
			CString getAuthorName() const override          { return "NewAuthor"; }
			CString getAuthorCompanyName() const override   { return "NewCompany"; }
			CString getShortDescription() const override    { return "Default Python Description"; }
			CString getDetailedDescription() const override { return ""; }
			CString getCategory() const override            { return "Scripting/Pybox/"; }
			CString getVersion() const override             { return "0.1"; }
			CString getStockItemName() const override       { return "gtk-convert"; }

			CIdentifier getCreatedClass() const override    { return OVP_ClassId_BoxAlgorithm_NewBoxPattern; }
			IPluginObject* create() override       { return new CBoxAlgorithmNewBoxPattern; }
			IBoxListener* createBoxListener() const override              { return new CBoxAlgorithmNewBoxPatternListener; }
			void releaseBoxListener(IBoxListener* pBoxListener) const override{ delete pBoxListener; }

			bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
			{
				prototype.addSetting("Clock frequency (Hz)", OV_TypeId_Integer, "64");
				// <tag> settings
				
				prototype.addFlag(Kernel::BoxFlag_CanAddInput);
				prototype.addFlag(Kernel::BoxFlag_CanModifyInput);
				prototype.addFlag(Kernel::BoxFlag_CanAddOutput);
				prototype.addFlag(Kernel::BoxFlag_CanModifyOutput);
				prototype.addFlag(Kernel::BoxFlag_CanAddSetting);
				prototype.addFlag(Kernel::BoxFlag_CanModifySetting);

				prototype.addInputSupport(OV_TypeId_Signal);
				prototype.addInputSupport(OV_TypeId_Stimulations);
				prototype.addInputSupport(OV_TypeId_StreamedMatrix);

				prototype.addOutputSupport(OV_TypeId_Signal);
				prototype.addOutputSupport(OV_TypeId_Stimulations);
				prototype.addOutputSupport(OV_TypeId_StreamedMatrix);

				// <tag> input & output
				
				return true;
			}

			_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_NewBoxPatternDesc)
		};
	}
}
}
#endif // #if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 3)

#endif // TARGET_HAS_ThirdPartyPython3
