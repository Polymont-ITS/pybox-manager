#ifndef __OpenViBEPlugins_BoxAlgorithm_KNearestNeighbors_H__
#define __OpenViBEPlugins_BoxAlgorithm_KNearestNeighbors_H__

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
		

		class CBoxAlgorithmKNearestNeighbors : virtual public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:
			virtual void release(void) { delete this; }

			virtual OpenViBE::uint64 getClockFrequency(void);
			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_KNearestNeighbors);

		protected:

			OpenViBE::uint64 m_ui64ClockFrequency;
			OpenViBE::CString m_sScriptFilename;
			
			std::vector < OpenViBEToolkit::TDecoder < CBoxAlgorithmKNearestNeighbors >* > m_vDecoders;
			std::vector < OpenViBEToolkit::TEncoder < CBoxAlgorithmKNearestNeighbors >* > m_vEncoders;

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
		
		class CBoxAlgorithmKNearestNeighborsListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
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
		
		class CBoxAlgorithmKNearestNeighborsDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("KNearestNeighbors"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yannis Bendi-Ouis and Jimmy Leblanc"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("CICIT Garches, Inria"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Train a K Nearest Neighbors classifier from Sklearn."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Classification"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("0.1"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-missing-image"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_KNearestNeighbors; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Python::CBoxAlgorithmKNearestNeighbors; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmKNearestNeighborsListener; }
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
                rBoxAlgorithmPrototype.addSetting("Classifier", OVPoly_ClassId_Classifier_Algorithm, "Nearest Neighbors Classifier");
                rBoxAlgorithmPrototype.addSetting("Discriminator", OV_TypeId_String, "");
                rBoxAlgorithmPrototype.addSetting("Test set share", OV_TypeId_Float, "0.2");
                rBoxAlgorithmPrototype.addSetting("Labels", OV_TypeId_String, "");
                rBoxAlgorithmPrototype.addSetting("n_neighbors", OV_TypeId_Integer, "5");
                rBoxAlgorithmPrototype.addSetting("weights", OVPoly_ClassId_Knn_Weights, "uniform");
                rBoxAlgorithmPrototype.addSetting("algorithm", OVPoly_ClassId_Knn_Algorithm, "auto");
                rBoxAlgorithmPrototype.addSetting("leaf_size", OV_TypeId_Integer, "30");
                rBoxAlgorithmPrototype.addSetting("p", OV_TypeId_Integer, "2");
                rBoxAlgorithmPrototype.addSetting("metric", OVPoly_ClassId_Metric, "minkowski");
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

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_KNearestNeighborsDesc);
		};
	};
};

#endif // #if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 2)

#endif // TARGET_HAS_ThirdPartyPython

#endif // __OpenViBEPlugins_BoxAlgorithm_KNearestNeighbors_H__

