///-------------------------------------------------------------------------------------------------
/// 
/// \file ovpMLP.h
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
			class CBoxAlgorithmMLP final : public CPolyBox
			{
			public:
				CBoxAlgorithmMLP() { m_script = "../../extras/contrib/applications/developer-tools/pybox-manager/ScriptBox/TrainerML.py"; }
				_IsDerivedFromClass_Final_(OpenViBE::Toolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_MLP)
			};

			class CBoxAlgorithmMLPListener final : public Toolkit::TBoxListener<IBoxListener>
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

			class CBoxAlgorithmMLPDesc final : virtual public IBoxAlgorithmDesc
			{
			public:

				void release() override { }

				CString getName() const override { return CString("MLP"); }
				CString getAuthorName() const override { return CString("Jimmy Leblanc & Yannis Bendi-Ouis"); }
				CString getAuthorCompanyName() const override { return CString("Polymont IT Services"); }
				CString getShortDescription() const override { return CString("Train a Multi-Layers Perceptron Classifier from Sklearn."); }
				CString getDetailedDescription() const override { return CString(""); }
				CString getCategory() const override { return CString("Scripting/PyBox/Classification"); }
				CString getVersion() const override { return CString("0.1"); }
				CString getStockItemName() const override { return CString("gtk-missing-image"); }

				CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_MLP; }
				IPluginObject* create() override { return new CBoxAlgorithmMLP; }
				IBoxListener* createBoxListener() const override { return new CBoxAlgorithmMLPListener; }
				void releaseBoxListener(IBoxListener* pBoxListener) const override { delete pBoxListener; }

				bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
				{
					prototype.addSetting("Clock frequency (Hz)", OV_TypeId_Integer, "64");
					// <tag> settings
					prototype.addSetting("Filename to save model to", OV_TypeId_Filename, "");
					prototype.addSetting("Filename to load model from", OV_TypeId_Filename, "");
					prototype.addSetting("Classifier", OVPoly_ClassId_Classifier_Algorithm, "Multi Layer Perceptron");
					prototype.addSetting("Test set share", OV_TypeId_Float, "0.2");
					prototype.addSetting("Labels", OV_TypeId_String, "");
					prototype.addSetting("hidden_layer_sizes", OV_TypeId_String, "(100,)");
					prototype.addSetting("activation", OVPoly_ClassId_MLP_activation, "relu");
					prototype.addSetting("learning_rate", OVPoly_ClassId_MLP_learning_rate, "constant");
					prototype.addSetting("solver", OVPoly_ClassId_MLP_solver, "adam");
					prototype.addSetting("alpha", OV_TypeId_Float, "0.0001");
					prototype.addSetting("batch_size", OV_TypeId_String, "auto");
					prototype.addSetting("learning_rate_init", OV_TypeId_Float, "0.001");
					prototype.addSetting("power_t", OV_TypeId_Float, "0.5");
					prototype.addSetting("max_iter", OV_TypeId_Integer, "200");
					prototype.addSetting("shuffle", OV_TypeId_Boolean, "true");
					prototype.addSetting("random_state", OV_TypeId_String, "None");
					prototype.addSetting("tol", OV_TypeId_Float, "1e-4");
					prototype.addSetting("verbose", OV_TypeId_Boolean, "true");
					prototype.addSetting("warm_start", OV_TypeId_Boolean, "false");
					prototype.addSetting("momentum", OV_TypeId_Float, "0.9");
					prototype.addSetting("nesterovs_momentum", OV_TypeId_Boolean, "true");
					prototype.addSetting("early_stopping", OV_TypeId_Boolean, "false");
					prototype.addSetting("validation_fraction", OV_TypeId_Float, "0.1");
					prototype.addSetting("beta_1", OV_TypeId_Float, "0.9");
					prototype.addSetting("beta_2", OV_TypeId_Float, "0.999");
					prototype.addSetting("epsilon", OV_TypeId_Float, "1e-8");
					prototype.addSetting("n_iter_no_change", OV_TypeId_Integer, "10");

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

				_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_MLPDesc)
			};
		}
	}
}

#endif // #if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 3)

#endif // TARGET_HAS_ThirdPartyPython3
