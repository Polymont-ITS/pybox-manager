///-------------------------------------------------------------------------------------------------
/// 
/// \file ovpDatasetCreator.h
/// \brief Class NewBoxPattern
/// \author Thibaut Monseigne (Inria) & Jimmy Leblanc (Polymont) & Yannis Bendi-Ouis (Polymont) 
/// \version 1.0.
/// \date 12/03/2020.
/// \copyright <a href="https://choosealicense.com/licenses/agpl-3.0/%22%3EGNU Affero General Public License v3.0</a>.
/// 
///-------------------------------------------------------------------------------------------------
#pragma once

#include "CPolyBox.h"

#if defined TARGET_HAS_ThirdPartyPython && !(defined(WIN32) && defined(TARGET_BUILDTYPE_Debug))
#if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 2)

namespace OpenViBEPlugins
{
	namespace Python
	{
		class CBoxAlgorithmDatasetCreator final : public CPolyBox
		{
		public:
			CBoxAlgorithmDatasetCreator() { m_script = "../../extras/contrib/applications/developer-tools/pybox-manager/ScriptBox/DatasetCreator.py"; }
			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_DatasetCreator)
		};

		class CBoxAlgorithmDatasetCreatorListener final : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:
			bool onInputAdded(OpenViBE::Kernel::IBox& box, const uint32_t index) override
			{
				box.setInputType(index, OV_TypeId_StreamedMatrix);
				return true;
			}

			bool onOutputAdded(OpenViBE::Kernel::IBox& box, const uint32_t index) override
			{
				box.setOutputType(index, OV_TypeId_StreamedMatrix);
				return true;
			}
			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmDatasetCreatorDesc final : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			void release() override { }

			OpenViBE::CString getName() const override { return OpenViBE::CString("DatasetCreator"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yannis Bendi-Ouis & Jimmy LeBlanc"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Polymont IT Services"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Monitor the user to create a dataset."); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Scripting/PyBox/Acquisition and network IO"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("0.1"); }
			OpenViBE::CString getStockItemName() const override { return OpenViBE::CString("gtk-missing-image"); }

			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_DatasetCreator; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmDatasetCreator; }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmDatasetCreatorListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const override { delete pBoxListener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addSetting("Clock frequency (Hz)", OV_TypeId_Integer, "64");
				// <tag> settings
				prototype.addSetting("Path directory", OV_TypeId_Filename, "");
				prototype.addSetting("Label_1", OV_TypeId_String, "");
				prototype.addSetting("Label_2", OV_TypeId_String, "");
				prototype.addSetting("Label_3", OV_TypeId_String, "");
				prototype.addSetting("Label_4", OV_TypeId_String, "");
				prototype.addSetting("Several CSV", OV_TypeId_Boolean, "");
				prototype.addSetting("Number of folds", OV_TypeId_Integer, "");
				prototype.addSetting("Number of actions", OV_TypeId_Integer, "");

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

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_DatasetCreatorDesc)
		};
	}
}

#endif // #if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 2)

#endif // TARGET_HAS_ThirdPartyPython
