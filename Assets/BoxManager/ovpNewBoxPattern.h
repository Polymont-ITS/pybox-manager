#pragma once

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
		

		class CBoxAlgorithmNewBoxPattern : virtual public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:
			virtual void release(void) { delete this; }

			virtual uint64_t getClockFrequency(void);
			virtual bool initialize(void);
			virtual bool uninitialize(void);
			virtual bool processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual bool processInput(uint32_t ui32InputIndex);
			virtual bool process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_NewBoxPattern);

		protected:

			uint64_t m_ui64ClockFrequency;
			OpenViBE::CString m_sScriptFilename;
			
			std::vector < OpenViBEToolkit::TDecoder < CBoxAlgorithmNewBoxPattern >* > m_vDecoders;
			std::vector < OpenViBEToolkit::TEncoder < CBoxAlgorithmNewBoxPattern >* > m_vEncoders;

			//std::map<char,PyObject *> m_PyObjectMap;
			
			PyObject *m_pBox, *m_pBoxInput, *m_pBoxOutput, *m_pBoxSetting, *m_pBoxCurrentTime;
			PyObject *m_pBoxInitialize, *m_pBoxProcess, *m_pBoxUninitialize;
            bool m_bInitializeSucceeded ;
            
			
			bool logSysStdout(void);
			bool logSysStderr(void);
			void buildPythonSettings(void);

			bool initializePythonSafely();
			bool clearPyObjectMap();
			bool transferStreamedMatrixInputChunksToPython(const uint32_t index);
			bool transferStreamedMatrixOutputChunksFromPython(const uint32_t index);
			bool transferSignalInputChunksToPython(const uint32_t index);
			bool transferSignalOutputChunksFromPython(const uint32_t index);
			bool transferStimulationInputChunksToPython(const uint32_t index);
			bool transferStimulationOutputChunksFromPython(const uint32_t index);


			
		};
		
		class CBoxAlgorithmNewBoxPatternListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:
			virtual bool onInputAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) 
			{
				rBox.setInputType(ui32Index, OV_TypeId_StreamedMatrix);
				return true;
			};

			virtual bool onOutputAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) 
			{ 
				rBox.setOutputType(ui32Index, OV_TypeId_StreamedMatrix);
				return true; 
			};

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};
		
		class CBoxAlgorithmNewBoxPatternDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("NewBoxPattern"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Aurelien Van Langhenhove and Laurent George"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("CICIT Garches, Inria"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Default Python Description"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Scripting"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("0.1"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-missing-image"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_NewBoxPattern; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Python::CBoxAlgorithmNewBoxPattern; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmNewBoxPatternListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const
			{
				//prototype.addInput  ("Input",  OV_TypeId_StreamedMatrix);
				//prototype.addInput  ("Input stimulations", OV_TypeId_Stimulations);
				//prototype.addOutput ("Output", OV_TypeId_StreamedMatrix);
				//prototype.addOutput ("Output stimulations", OV_TypeId_Stimulations);
				prototype.addSetting("Clock frequency (Hz)", OV_TypeId_Integer, "64");
				//prototype.addSetting("Script", OV_TypeId_Script, "");
				//prototype.addFlag(OpenViBE::Kernel::BoxFlag_IsUnstable);
				
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
				
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_NewBoxPatternDesc);
		};
	};
};

#endif // #if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 2)

#endif // TARGET_HAS_ThirdPartyPython
