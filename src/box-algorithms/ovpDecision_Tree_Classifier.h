///-------------------------------------------------------------------------------------------------
/// 
/// \file ovpDecision_Tree_Classifier.h
/// \brief Class NewBoxPattern
/// \author Thibaut Monseigne (Inria) & Jimmy Leblanc (Polymont) & Yannis Bendi-Ouis (Polymont) 
/// \version 1.0.
/// \date 12/03/2020.
/// \copyright <a href="https://choosealicense.com/licenses/agpl-3.0/%22%3EGNU Affero General Public License v3.0</a>.
/// 
///-------------------------------------------------------------------------------------------------
#pragma once

#include "CPolyBox.h"

#if defined TARGET_HAS_ThirdPartyPython3 && !(defined(WIN32) && defined(TARGET_BUILDTYPE_Debug))
#if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 3)

namespace OpenViBE
{
	namespace Plugins
	{
		namespace PyBox
		{
			class CBoxAlgorithmDecision_Tree_Classifier final : public CPolyBox
			{
			public:
				CBoxAlgorithmDecision_Tree_Classifier() { m_script = "../../extras/contrib/applications/developer-tools/pybox-manager/ScriptBox/TrainerML.py"; }
				_IsDerivedFromClass_Final_(OpenViBE::Toolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >,
										   OVP_ClassId_BoxAlgorithm_Decision_Tree_Classifier)
			};

			class CBoxAlgorithmDecision_Tree_ClassifierListener final : public Toolkit::TBoxListener<IBoxListener>
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
				_IsDerivedFromClass_Final_(OpenViBE::Toolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
			};

			class CBoxAlgorithmDecision_Tree_ClassifierDesc final : virtual public IBoxAlgorithmDesc
			{
			public:

				void release() override { }

				CString getName() const override { return CString("Decision Tree Classifier"); }
				CString getAuthorName() const override { return CString("Jimmy Leblanc & Yannis Bendi-Ouis"); }
				CString getAuthorCompanyName() const override { return CString("Polymont IT Services"); }
				CString getShortDescription() const override { return CString("Train a Decision Tree Classifier from Sklearn."); }
				CString getDetailedDescription() const override { return CString(""); }
				CString getCategory() const override { return CString("Scripting/PyBox/Classification"); }
				CString getVersion() const override { return CString("0.1"); }
				CString getStockItemName() const override { return CString("gtk-missing-image"); }

				CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_Decision_Tree_Classifier; }
				IPluginObject* create() override { return new CBoxAlgorithmDecision_Tree_Classifier; }
				IBoxListener* createBoxListener() const override { return new CBoxAlgorithmDecision_Tree_ClassifierListener; }
				void releaseBoxListener(IBoxListener* pBoxListener) const override { delete pBoxListener; }

				bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
				{
					prototype.addSetting("Clock frequency (Hz)", OV_TypeId_Integer, "64");
					// <tag> settings
					prototype.addSetting("Filename to save model to", OV_TypeId_Filename, "");
					prototype.addSetting("Filename to load model from", OV_TypeId_Filename, "");
					prototype.addSetting("Classifier", OVPoly_ClassId_Classifier_Algorithm, "Decision Tree Classifier");
					prototype.addSetting("Test set share", OV_TypeId_Float, "0.2");
					prototype.addSetting("Labels", OV_TypeId_String, "");
					prototype.addSetting("criterion", OVPoly_ClassId_Criterion, "gini");
					prototype.addSetting("splitter", OVPoly_ClassId_DecisionTree_splitter, "best");
					prototype.addSetting("max_depth", OV_TypeId_String, "None");
					prototype.addSetting("min_samples_split", OV_TypeId_Float, "2");
					prototype.addSetting("min_samples_leaf", OV_TypeId_Float, "1");
					prototype.addSetting("min_weight_fraction_leaf", OV_TypeId_Float, "0");
					prototype.addSetting("max_features", OV_TypeId_String, "None");
					prototype.addSetting("random_state", OV_TypeId_String, "None");
					prototype.addSetting("max_leaf_nodes", OV_TypeId_String, "None");
					prototype.addSetting("min_impurity_decrease", OV_TypeId_Float, "0");
					prototype.addSetting("min_impurity_split", OV_TypeId_Float, "1e-7");

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
					prototype.addOutput("stim_out", OV_TypeId_Stimulations);
					prototype.addInput("input_StreamMatrix", OV_TypeId_StreamedMatrix);
					prototype.addInput("input_Stimulations", OV_TypeId_Stimulations);

					return true;
				}

				_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_Decision_Tree_ClassifierDesc)
			};
		}
	}
}

#endif // #if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 3)

#endif // TARGET_HAS_ThirdPartyPython3
