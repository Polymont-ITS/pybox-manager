///-------------------------------------------------------------------------------------------------
/// 
/// \file ovpADA.h
/// \brief Class NewBoxPattern
/// \author Thibaut Monseigne (Inria) & Jimmy Leblanc (Polymont) & Yannis Bendi-Ouis (Polymont) 
/// \version 1.0.
/// \date 12/03/2020.
/// \copyright <a href="https://choosealicense.com/licenses/agpl-3.0/%22%3EGNU Affero General Public License v3.0</a>.
/// 
///-------------------------------------------------------------------------------------------------
#pragma once

// Windows debug build doesn't typically link as most people don't have the python debug library.
#if defined TARGET_HAS_ThirdPartyPython && !(defined(WIN32) && defined(TARGET_BUILDTYPE_Debug))

#include <Python.h>

#if defined(PY_MAJOR_VERSION)// && (PY_MAJOR_VERSION == 2)

#include "defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <vector>

namespace OpenViBEPlugins
{
	namespace Python
	{
		class CPolyBox : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }

			uint64_t getClockFrequency() override { return m_clockFrequency << 32; }
			bool initialize() override;
			bool uninitialize() override;
			bool processClock(OpenViBE::CMessageClock& messageClock) override;
			bool processInput(uint32_t index) override;
			bool process() override;

		protected:

			uint64_t m_clockFrequency = 0;
			OpenViBE::CString m_script;

			std::vector<OpenViBEToolkit::TDecoder<CPolyBox>*> m_vDecoders;
			std::vector<OpenViBEToolkit::TEncoder<CPolyBox>*> m_vEncoders;

			// These are all borrowed references in python v2.7. Do not free them.
			static bool m_isInitialized;

			static PyObject *m_mainModule, *m_mainDictionnary;
			static PyObject *m_matrixHeader, *m_matrixBuffer, *m_matrixEnd;
			static PyObject *m_signalHeader, *m_signalBuffer, *m_signalEnd;
			static PyObject *m_stimulationHeader, *m_stimulation, *m_stimulationSet, *m_stimulationEnd;
			static PyObject* m_buffer;
			static PyObject* m_execFileFunction;
			static PyObject *m_sysStdout, *m_sysStderr;


			//std::map<char,PyObject *> m_PyObjectMap;
			PyObject *m_box            = nullptr, *m_boxInput   = nullptr, *m_boxOutput       = nullptr, *m_boxSetting = nullptr, *m_boxCurrentTime = nullptr;
			PyObject *m_boxInitialize  = nullptr, *m_boxProcess = nullptr, *m_boxUninitialize = nullptr;
			bool m_initializeSucceeded = false;


			bool logSysStdout();
			bool logSysStderr();
			void buildPythonSettings();

			bool initializePythonSafely();
			bool transferStreamedMatrixInputChunksToPython(const uint32_t index);
			bool transferStreamedMatrixOutputChunksFromPython(const uint32_t index);
			bool transferSignalInputChunksToPython(const uint32_t index);
			bool transferSignalOutputChunksFromPython(const uint32_t index);
			bool transferStimulationInputChunksToPython(const uint32_t index);
			bool transferStimulationOutputChunksFromPython(const uint32_t index);
		};
	}
}

#endif // #if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 2)

#endif // TARGET_HAS_ThirdPartyPython
