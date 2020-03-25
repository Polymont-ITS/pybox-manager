#ifndef __OpenViBEPlugins_BoxAlgorithm_Random_Forest_H__
#define __OpenViBEPlugins_BoxAlgorithm_Random_Forest_H__

#if defined TARGET_HAS_ThirdPartyPython

#include <Python.h>

#if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 2)

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <string.h>
#include <string>
#include <vector>
//#include <map>

namespace OpenViBEPlugins
{
	namespace Python
	{
		

		class CBoxAlgorithmRandom_Forest : virtual public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:
			virtual void release(void) { delete this; }

			virtual OpenViBE::uint64 getClockFrequency(void);
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_Random_Forest);

		protected:

			OpenViBE::uint64 m_ui64ClockFrequency;
			OpenViBE::CString m_sScriptFilename;
			
			std::vector < OpenViBEToolkit::TDecoder < CBoxAlgorithmRandom_Forest >* > m_vDecoders;
			std::vector < OpenViBEToolkit::TEncoder < CBoxAlgorithmRandom_Forest >* > m_vEncoders;

			//std::map<char,PyObject *> m_PyObjectMap;
			
			PyObject *m_pBox, *m_pBoxInput, *m_pBoxOutput, *m_pBoxSetting, *m_pBoxCurrentTime;
			PyObject *m_pBoxInitialize, *m_pBoxProcess, *m_pBoxUninitialize;
            OpenViBE::boolean m_bInitializeSucceeded ;
            
			
			OpenViBE::boolean logSysStdout(void);
			OpenViBE::boolean logSysStderr(void);
			void buildPythonSettings(void);

			OpenViBE::boolean initializePythonSafely();
			OpenViBE::boolean clearPyObjectMap();
			OpenViBE::boolean transferStreamedMatrixInputChunksToPython(const OpenViBE::uint32 input_index);
			OpenViBE::boolean transferStreamedMatrixOutputChunksFromPython(const OpenViBE::uint32 output_index);
			OpenViBE::boolean transferSignalInputChunksToPython(const OpenViBE::uint32 input_index);
			OpenViBE::boolean transferSignalOutputChunksFromPython(const OpenViBE::uint32 output_index);
			OpenViBE::boolean transferStimulationInputChunksToPython(const OpenViBE::uint32 input_index);
			OpenViBE::boolean transferStimulationOutputChunksFromPython(const OpenViBE::uint32 output_index);


			
		};
		
		class CBoxAlgorithmRandom_ForestListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:
			virtual OpenViBE::boolean onInputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) 
			{
				rBox.setInputType(ui32Index, OV_TypeId_StreamedMatrix);
				return true;
			};

			virtual OpenViBE::boolean onOutputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) 
			{ 
				rBox.setOutputType(ui32Index, OV_TypeId_StreamedMatrix);
				return true; 
			};

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};
		
		class CBoxAlgorithmRandom_ForestDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Random Forest"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Jimmy Leblanc and Yannis Bendi-Ouis"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("CICIT Garches, Inria"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Train a Random Forest classifier from Sklearn."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Classification"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("0.1"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-missing-image"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_Random_Forest; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Python::CBoxAlgorithmRandom_Forest; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmRandom_ForestListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				//rBoxAlgorithmPrototype.addInput  ("Input",  OV_TypeId_StreamedMatrix);
				//rBoxAlgorithmPrototype.addInput  ("Input stimulations", OV_TypeId_Stimulations);
                rBoxAlgorithmPrototype.addInput  ("input_StreamMatrix", OV_TypeId_StreamedMatrix);
                rBoxAlgorithmPrototype.addInput  ("input_Stimulations", OV_TypeId_Stimulations);
				//rBoxAlgorithmPrototype.addOutput ("Output", OV_TypeId_StreamedMatrix);
				//rBoxAlgorithmPrototype.addOutput ("Output stimulations", OV_TypeId_Stimulations);
                rBoxAlgorithmPrototype.addOutput ("stim_out", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addSetting("Clock frequency (Hz)", OV_TypeId_Integer, "64");
				////rBoxAlgorithmPrototype.addSetting("Script", OV_TypeId_Script, "");
                rBoxAlgorithmPrototype.addSetting("Filename to save model to", OV_TypeId_Filename, "");
                rBoxAlgorithmPrototype.addSetting("Filename to load model from", OV_TypeId_Filename, "");
                rBoxAlgorithmPrototype.addSetting("Classifier", OVPoly_ClassId_Classifier_Algorithm, "Random Forest");
                rBoxAlgorithmPrototype.addSetting("Test set share", OV_TypeId_Float, "0.2");
                rBoxAlgorithmPrototype.addSetting("Labels", OV_TypeId_String, "");
                rBoxAlgorithmPrototype.addSetting("criterion", OVPoly_ClassId_Criterion, "gini");
                rBoxAlgorithmPrototype.addSetting("max_depth", OV_TypeId_String, "None");
                rBoxAlgorithmPrototype.addSetting("min_samples_split", OV_TypeId_Float, "2");
                rBoxAlgorithmPrototype.addSetting("min_samples_leaf", OV_TypeId_Float, "1");
                rBoxAlgorithmPrototype.addSetting("min_weight_fraction_leaf", OV_TypeId_Float, "0");
                rBoxAlgorithmPrototype.addSetting("max_features", OV_TypeId_String, "auto");
                rBoxAlgorithmPrototype.addSetting("random_state", OV_TypeId_String, "None");
                rBoxAlgorithmPrototype.addSetting("max_leaf_nodes", OV_TypeId_String, "None");
                rBoxAlgorithmPrototype.addSetting("min_impurity_decrease", OV_TypeId_Float, "0");
                rBoxAlgorithmPrototype.addSetting("min_impurity_split", OV_TypeId_Float, "1e-7");
                rBoxAlgorithmPrototype.addSetting("bootstrap", OV_TypeId_Boolean, "false");
                rBoxAlgorithmPrototype.addSetting("oob_score", OV_TypeId_Boolean, "false");
                rBoxAlgorithmPrototype.addSetting("verbose", OV_TypeId_Integer, "0");
                rBoxAlgorithmPrototype.addSetting("warm_start", OV_TypeId_Boolean, "false");
                rBoxAlgorithmPrototype.addSetting("max_samples", OV_TypeId_String, "None");
                rBoxAlgorithmPrototype.addSetting("n_estimators", OV_TypeId_Integer, "100");
				//rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_IsUnstable);
				
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddOutput);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddSetting);
				//rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifySetting);

				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_StreamedMatrix);

				rBoxAlgorithmPrototype.addOutputSupport(OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutputSupport(OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addOutputSupport(OV_TypeId_StreamedMatrix);
				
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_Random_ForestDesc);
		};
	};
};

#endif // #if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 2)

#endif // TARGET_HAS_ThirdPartyPython

#endif // __OpenViBEPlugins_BoxAlgorithm_Random_Forest_H__

