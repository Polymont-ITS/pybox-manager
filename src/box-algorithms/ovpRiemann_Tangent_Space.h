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

#if defined TARGET_HAS_ThirdPartyPython && !(defined(WIN32) && defined(TARGET_BUILDTYPE_Debug))

#include <Python.h>

#if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 2)

#include "../defines.h"
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
		class CBoxAlgorithmRiemann_Tangent_Space final : public CPolyBox
		{
		public:
			CBoxAlgorithmRiemann_Tangent_Space()
			{
				m_sScriptFilename = "../../extras/contrib/applications/developer-tools/pybox-manager/ScriptBox/TrainerML.py";
			}
			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_Riemann_Tangent_Space)
		};

		class CBoxAlgorithmRiemann_Tangent_SpaceListener final : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
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
			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmRiemann_Tangent_SpaceDesc final : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Riemann Tangent Space"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Jimmy Leblanc & Yannis Bendi-Ouis"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("Polymont IT Services"); }
			virtual OpenViBE::CString getShortDescription() const
			{
				return OpenViBE::CString("Tangent space projection map a set of covariance matrices to their tangent space.");
			}
			virtual OpenViBE::CString getDetailedDescription() const
			{
				return OpenViBE::CString(
					"Tangent space projection map a set of covariance matrices to their tangent space\n\nTangent space projection is useful to convert covariance matrices in euclidean vectors while conserving the inner structure of the manifold. After projection, standard processing and vector-based classification can be applied.(By default, LDA would be applied)\n\\\"");
			}
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Scripting/PyBox/Classification"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("0.1"); }
			virtual OpenViBE::CString getStockItemName() const { return OpenViBE::CString("gtk-missing-image"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_Riemann_Tangent_Space; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmRiemann_Tangent_Space; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener() const { return new CBoxAlgorithmRiemann_Tangent_SpaceListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const
			{
				prototype.addSetting("Clock frequency (Hz)", OV_TypeId_Integer, "64");
				// <tag> settings
				prototype.addSetting("Filename to save model to", OV_TypeId_Filename, "");
				prototype.addSetting("Filename to load model from", OV_TypeId_Filename, "");
				prototype.addSetting("Classifier", OVPoly_ClassId_Classifier_Algorithm, "Riemann Tangent Space");
				prototype.addSetting("Test set share", OV_TypeId_Float, "0.2");
				prototype.addSetting("Labels", OV_TypeId_String, "");
				prototype.addSetting("Discriminator", OVPoly_ClassId_Classifier_Algorithm, "Linear Discriminant Analysis");

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
				prototype.addInput("input_Stimulations", OV_TypeId_Stimulations);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_Riemann_Tangent_SpaceDesc)
		};
	};
};

#endif // #if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 2)

#endif // TARGET_HAS_ThirdPartyPython
