///-------------------------------------------------------------------------------------------------
/// 
/// \file TrainerML.hpp
/// \brief Class TrainerML
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

namespace OpenViBE
{
	namespace Plugins
	{
		namespace PyBox
		{
			class CBoxAlgorithmTrainerML final : public CPolyBox
			{
			public:
				CBoxAlgorithmTrainerML() { m_script = Directories::getDataDir() + "/plugins/python3/pybox/TrainerML.py"; }
				_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_TrainerML)
			};

			class CBoxAlgorithmTrainerMLListener final : public Toolkit::TBoxListener<IBoxListener>
			{
			public:
				bool onInputAdded(Kernel::IBox& box, const size_t index) override
				{
					box.setInputType(index, OV_TypeId_StreamedMatrix);
					return true;
				}

				bool onSettingValueChanged(Kernel::IBox& box, const size_t index) override;

				_IsDerivedFromClass_Final_(Toolkit::TBoxListener<IBoxListener>, OV_UndefinedIdentifier)
			};

			class CBoxAlgorithmTrainerMLDesc final : virtual public IBoxAlgorithmDesc
			{
			public:

				void release() override { }

				CString getName() const override { return CString("Trainer Sklearn"); }
				CString getAuthorName() const override { return CString("Jimmy Leblanc & Yannis Bendi-Ouis"); }
				CString getAuthorCompanyName() const override { return CString("Polymont IT Services"); }
				CString getShortDescription() const override { return CString("Train an TrainerML Classifier from Sklearn."); }
				CString getDetailedDescription() const override { return CString(""); }
				CString getCategory() const override { return CString("Scripting/PyBox"); }
				CString getVersion() const override { return CString("0.1"); }
				CString getStockItemName() const override { return CString("gtk-convert"); }

				CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_TrainerML; }
				IPluginObject* create() override { return new CBoxAlgorithmTrainerML; }
				IBoxListener* createBoxListener() const override { return new CBoxAlgorithmTrainerMLListener; }
				void releaseBoxListener(IBoxListener* listener) const override { delete listener; }

				bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
				{
					prototype.addSetting("Clock frequency (Hz)", OV_TypeId_Integer, "64");
					// <tag> settings
					prototype.addSetting("Filename to save model to", OV_TypeId_Filename, "${Player_ScenarioDirectory}/model.clf");
					prototype.addSetting("Filename to load model from", OV_TypeId_Filename, "");
					prototype.addSetting("Classifier", OVPoly_ClassId_Classifier_Algorithm, "None");

					prototype.addInputSupport(OV_TypeId_Signal);
					prototype.addInputSupport(OV_TypeId_Stimulations);
					prototype.addInputSupport(OV_TypeId_StreamedMatrix);

					prototype.addOutputSupport(OV_TypeId_Signal);
					prototype.addOutputSupport(OV_TypeId_Stimulations);
					prototype.addOutputSupport(OV_TypeId_StreamedMatrix);

					// <tag> input & output
					prototype.addOutput("stim_out", OV_TypeId_Stimulations);
					prototype.addInput("input_StreamMatrix", OV_TypeId_StreamedMatrix);
					prototype.addInput("input_Stimulations", OV_TypeId_Stimulations);

					return true;
				}

				_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_TrainerMLDesc)
			};
		}
	}
}

#endif // #if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 3)

#endif // TARGET_HAS_ThirdPartyPython3
