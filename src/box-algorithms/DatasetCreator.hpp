///-------------------------------------------------------------------------------------------------
/// 
/// \file DatasetCreator.hpp
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

namespace OpenViBE {
namespace Plugins {
namespace PyBox {

class CBoxAlgorithmDatasetCreator final : public CPolyBox
{
public:
	CBoxAlgorithmDatasetCreator() { m_script = Directories::getDataDir() + "/plugins/python3/pybox/DatasetCreator.py"; }
	_IsDerivedFromClass_Final_(OpenViBE::Toolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_DatasetCreator)
};

class CBoxAlgorithmDatasetCreatorListener final : public Toolkit::TBoxListener<IBoxListener>
{
public:
	bool onInputAdded(Kernel::IBox& box, const size_t index) override
	{
		box.setInputType(index, OV_TypeId_StreamedMatrix);
		return true;
	}

	_IsDerivedFromClass_Final_(OpenViBE::Toolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
};

class CBoxAlgorithmDatasetCreatorDesc final : virtual public IBoxAlgorithmDesc
{
public:

	void release() override { }

	CString getName() const override { return "DatasetCreator"; }
	CString getAuthorName() const override { return "Yannis Bendi-Ouis & Jimmy LeBlanc"; }
	CString getAuthorCompanyName() const override { return "Polymont IT Services"; }
	CString getShortDescription() const override { return "Monitor the user to create a dataset."; }
	CString getDetailedDescription() const override { return ""; }
	CString getCategory() const override { return "Scripting/PyBox"; }
	CString getVersion() const override { return "0.1"; }
	CString getStockItemName() const override { return "gtk-convert"; }

	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_DatasetCreator; }
	IPluginObject* create() override { return new CBoxAlgorithmDatasetCreator; }
	IBoxListener* createBoxListener() const override { return new CBoxAlgorithmDatasetCreatorListener; }
	void releaseBoxListener(IBoxListener* pBoxListener) const override { delete pBoxListener; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addSetting("Clock frequency (Hz)", OV_TypeId_Integer, "64");
		// <tag> settings
		prototype.addSetting("Path directory", OV_TypeId_Filename, "${Player_ScenarioDirectory}/datas/");
		prototype.addSetting("Label_1", OV_TypeId_String, "right");
		prototype.addSetting("Label_2", OV_TypeId_String, "left");
		prototype.addSetting("Label_3", OV_TypeId_String, "up");
		prototype.addSetting("Label_4", OV_TypeId_String, "down");
		prototype.addSetting("Several CSV", OV_TypeId_Boolean, "false");
		prototype.addSetting("Number of folds", OV_TypeId_Integer, "1");
		prototype.addSetting("Number of actions", OV_TypeId_Integer, "4");

		prototype.addFlag(Kernel::BoxFlag_CanAddInput);
		prototype.addFlag(Kernel::BoxFlag_CanModifyInput);
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

		return true;
	}

	_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_DatasetCreatorDesc)
};

}
}
}

#endif // #if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 3)
#endif // TARGET_HAS_ThirdPartyPython3
