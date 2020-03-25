#include "CPolyBox.hpp"
#if defined TARGET_HAS_ThirdPartyPython
//#if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 2)

#include <openvibe/ovITimeArithmetics.h>
#include <iostream>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;

using namespace OpenViBEPlugins;
using namespace /*OpenViBEPlugins::*/Python;

using namespace OpenViBEToolkit;

bool CPolyBox::m_isInitialized          = false;
PyObject* CPolyBox::m_mainModule        = nullptr;
PyObject* CPolyBox::m_mainDictionnary   = nullptr;
PyObject* CPolyBox::m_matrixHeader      = nullptr;
PyObject* CPolyBox::m_matrixBuffer      = nullptr;
PyObject* CPolyBox::m_matrixEnd         = nullptr;
PyObject* CPolyBox::m_signalHeader      = nullptr;
PyObject* CPolyBox::m_signalBuffer      = nullptr;
PyObject* CPolyBox::m_signalEnd         = nullptr;
PyObject* CPolyBox::m_stimulationHeader = nullptr;
PyObject* CPolyBox::m_stimulation       = nullptr;
PyObject* CPolyBox::m_stimulationSet    = nullptr;
PyObject* CPolyBox::m_stimulationEnd    = nullptr;
PyObject* CPolyBox::m_buffer            = nullptr;
PyObject* CPolyBox::m_execFileFunction  = nullptr;
PyObject* CPolyBox::m_sysStdout         = nullptr;
PyObject* CPolyBox::m_sysStderr         = nullptr;

bool CPolyBox::logSysStdout()
{
	//New reference
	PyObject* l_pPyStringToLog = PyObject_CallMethod(m_sysStdout, (char*)"getvalue", nullptr);
	if (l_pPyStringToLog == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to call sys.stdout.getvalue().\n";
		return false;
	}

	char* l_StringToLog = PyString_AsString(l_pPyStringToLog);
	if (l_StringToLog == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to convert l_pPyStringToLog to (char *).\n";
		Py_CLEAR(l_pPyStringToLog);
		return false;
	}
	Py_CLEAR(l_pPyStringToLog);

	if (strlen(const_cast<char*>(l_StringToLog)) > 0)
	{
		this->getLogManager() << LogLevel_Info << l_StringToLog;

		PyObject* l_pResult = PyObject_CallMethod(m_sysStdout, (char*)"flush", nullptr);
		if (l_pResult == nullptr)
		{
			this->getLogManager() << LogLevel_Error << "Failed to call sys.stdout.flush().\n";
			return false;
		}
		Py_CLEAR(l_pResult);
	}
	return true;
}

bool CPolyBox::logSysStderr()
{
	//New reference
	PyObject* l_pPyStringToLog = PyObject_CallMethod(m_sysStderr, (char*)"getvalue", nullptr);
	if (l_pPyStringToLog == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to call sys.stderr.getvalue().\n";
		return false;
	}

	char* l_StringToLog = PyString_AsString(l_pPyStringToLog);
	if (l_StringToLog == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to convert l_pPyStringToLog to (char *).\n";
		Py_CLEAR(l_pPyStringToLog);
		return false;
	}
	Py_CLEAR(l_pPyStringToLog);

	if (strlen(const_cast<char*>(l_StringToLog)) > 0)
	{
		this->getLogManager() << LogLevel_Error << l_StringToLog;

		PyObject* l_pResult = PyObject_CallMethod(m_sysStderr, (char*)"flush", nullptr);
		if (l_pResult == nullptr)
		{
			this->getLogManager() << LogLevel_Error << "Failed to call sys.stderr.flush().\n";
			return false;
		}
		Py_CLEAR(l_pResult);
	}
	return true;
}

void CPolyBox::buildPythonSettings()
{
	const IBox* boxCtx = getBoxAlgorithmContext()->getStaticBoxContext();
	for (uint32_t i = 2; i < boxCtx->getSettingCount(); i++)
	{
		CString name;
		boxCtx->getSettingName(i, name);

		const CString value = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), i);

		PyDict_SetItemString(m_boxSetting, name, PyString_FromString(value));
	}
}

bool CPolyBox::initializePythonSafely()
{
	// Only the first Python box does the initialization of the global parts
	if (m_isInitialized) { return true; }

	this->getLogManager() << LogLevel_Info << "Discovered Python is " << CString(Py_GetVersion()) << " (" << CString(Py_GetPlatform()) << ")\n";
	this->getLogManager() << LogLevel_Debug << "The Python path is [" << CString(Py_GetPath()) << "]\n";

	CString l_sCommand;
	l_sCommand = l_sCommand + "import sys\n";
	l_sCommand = l_sCommand + "sys.path.append('";
	l_sCommand = l_sCommand + Directories::getDataDir();
	l_sCommand = l_sCommand + "/plugins/python')\n";
	l_sCommand = l_sCommand + "sys.argv = [\"openvibe\"]\n";
	// l_sCommand = l_sCommand + "import openvibe\n";
	// l_sCommand = l_sCommand + "from StimulationsCodes import *\n";
	this->getLogManager() << LogLevel_Trace << "Running [" << l_sCommand << "].\n";

	PyRun_SimpleString(l_sCommand);

	//Borrowed reference
	m_mainModule = PyImport_AddModule("__main__");
	//Borrowed reference
	m_mainDictionnary = PyModule_GetDict(m_mainModule);

	//Execute the script which contains the different classes to interact with OpenViBE
	//New reference
	CString l_sFilePath     = Directories::getDataDir() + "/plugins/python/openvibe.py";
	PyObject* l_pScriptFile = PyFile_FromString((char*)l_sFilePath.toASCIIString(), (char*)"r");
	if (l_pScriptFile == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to open '" << l_sFilePath << "'.\n";
		return false;
	}

	if (PyRun_SimpleFile(PyFile_AsFile(l_pScriptFile), (char*)l_sFilePath.toASCIIString()) == -1)
	{
		this->getLogManager() << LogLevel_Error << "Failed to run " << l_sFilePath << ".\n";
		Py_CLEAR(l_pScriptFile);
		return false;
	}
	Py_CLEAR(l_pScriptFile);

	//Borrowed reference
	m_sysStdout = PySys_GetObject((char*)"stdout");
	if (m_sysStdout == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "sys.stdout does not exist.\n";
		return false;
	}
	//Borrowed reference
	m_sysStderr = PySys_GetObject((char*)"stderr");
	if (m_sysStderr == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "sys.stderr does not exist.\n";
		return false;
	}

	//Borrowed reference
	m_execFileFunction = PyDict_GetItemString(m_mainDictionnary, "execfileHandlingException");
	if (m_execFileFunction == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "openvibe.py doesn't have a execfileHandlingException function, quitting" << ".\n";
		return false;
	}

	if (!PyCallable_Check(m_execFileFunction))
	{
		this->getLogManager() << LogLevel_Error << "openvibe.py doesn't have a execfileHandlingException function callable, " << ".\n";
		return false;
	}

	//Borrowed reference
	m_matrixHeader = PyDict_GetItemString(m_mainDictionnary, "OVStreamedMatrixHeader");
	if (m_matrixHeader == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load class \"OVStreamedMatrixHeader\".\n";
		return false;
	}


	//Borrowed reference
	m_matrixBuffer = PyDict_GetItemString(m_mainDictionnary, "OVStreamedMatrixBuffer");
	if (m_matrixBuffer == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load class \"OVStreamedMatrixBuffer\".\n";
		return false;
	}

	//Borrowed reference
	m_matrixEnd = PyDict_GetItemString(m_mainDictionnary, "OVStreamedMatrixEnd");
	if (m_matrixEnd == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load class \"OVStreamedMatrixEnd\".\n";
		return false;
	}

	//Borrowed reference
	m_signalHeader = PyDict_GetItemString(m_mainDictionnary, "OVSignalHeader");
	if (m_signalHeader == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load class \"OVSignalHeader\".\n";
		return false;
	}

	//Borrowed reference
	m_signalBuffer = PyDict_GetItemString(m_mainDictionnary, "OVSignalBuffer");
	if (m_signalBuffer == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load class \"OVSignalBuffer\".\n";
		return false;
	}

	//Borrowed reference
	m_signalEnd = PyDict_GetItemString(m_mainDictionnary, "OVSignalEnd");
	if (m_signalEnd == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load class \"OVSignalEnd\".\n";
		return false;
	}

	//Borrowed reference
	m_stimulationHeader = PyDict_GetItemString(m_mainDictionnary, "OVStimulationHeader");
	if (m_stimulationHeader == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load class \"OVStimulationHeader\".\n";
		return false;
	}

	//Borrowed reference
	m_stimulation = PyDict_GetItemString(m_mainDictionnary, "OVStimulation");
	if (m_stimulation == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load class \"OVStimulation\".\n";
		return false;
	}

	//Borrowed reference
	m_stimulationSet = PyDict_GetItemString(m_mainDictionnary, "OVStimulationSet");
	if (m_stimulationSet == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load class \"OVStimulationSet\".\n";
		return false;
	}

	//Borrowed reference
	m_stimulationEnd = PyDict_GetItemString(m_mainDictionnary, "OVStimulationEnd");
	if (m_stimulationEnd == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load class \"OVStimulationEnd\".\n";
		return false;
	}

	//Borrowed reference
	m_buffer = PyDict_GetItemString(m_mainDictionnary, "OVBuffer");
	if (m_buffer == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load class \"OVBuffer\".\n";
		return false;
	}

	m_isInitialized = true;

	this->getLogManager() << LogLevel_Info << "Python Interpreter initialized\n";

	return true;
}

bool CPolyBox::initialize()
{
	m_box                 = nullptr;
	m_boxInput            = nullptr;
	m_boxOutput           = nullptr;
	m_boxCurrentTime      = nullptr;
	m_boxSetting          = nullptr;
	m_boxInitialize       = nullptr;
	m_boxProcess          = nullptr;
	m_boxUninitialize     = nullptr;
	m_initializeSucceeded = false;

	if (!initializePythonSafely()) { return false; }


	//Initialize the clock frequency of the box depending on the first setting of the box
	m_clockFrequency  = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_sScriptFilename = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	if (strlen(m_sScriptFilename.toASCIIString()) == 0)
	{
		this->getLogManager() << LogLevel_Error << "You have to choose a script.\n";
		return false;
	}

	//Create the decoders for the inputs
	const IBox& l_rStaticBoxContext = this->getStaticBoxContext();
	CIdentifier l_oTypeIdentifier;
	for (uint32_t input = 0; input < l_rStaticBoxContext.getInputCount(); input++)
	{
		l_rStaticBoxContext.getInputType(input, l_oTypeIdentifier);
		if (l_oTypeIdentifier == OV_TypeId_StreamedMatrix) { m_vDecoders.push_back(new TStreamedMatrixDecoder<CPolyBox>(*this, input)); }
		else if (l_oTypeIdentifier == OV_TypeId_Signal) { m_vDecoders.push_back(new TSignalDecoder<CPolyBox>(*this, input)); }
		else if (l_oTypeIdentifier == OV_TypeId_FeatureVector) { m_vDecoders.push_back(new TFeatureVectorDecoder<CPolyBox>(*this, input)); }
		else if (l_oTypeIdentifier == OV_TypeId_Spectrum) { m_vDecoders.push_back(new TSpectrumDecoder<CPolyBox>(*this, input)); }
		else if (l_oTypeIdentifier == OV_TypeId_ChannelLocalisation) { m_vDecoders.push_back(new TChannelLocalisationDecoder<CPolyBox>(*this, input)); }
		else if (l_oTypeIdentifier == OV_TypeId_Stimulations) { m_vDecoders.push_back(new TStimulationDecoder<CPolyBox>(*this, input)); }
		else if (l_oTypeIdentifier == OV_TypeId_ExperimentInformation) { m_vDecoders.push_back(new TExperimentInformationDecoder<CPolyBox>(*this, input)); }
		else
		{
			this->getLogManager() << LogLevel_Error << "Codec to decode " << l_oTypeIdentifier.toString() << " is not implemented.\n";
			return false;
		}
	}

	//Create the encoders for the outputs
	for (uint32_t output = 0; output < l_rStaticBoxContext.getOutputCount(); output++)
	{
		l_rStaticBoxContext.getOutputType(output, l_oTypeIdentifier);
		if (l_oTypeIdentifier == OV_TypeId_StreamedMatrix) { m_vEncoders.push_back(new TStreamedMatrixEncoder<CPolyBox>(*this, output)); }
		else if (l_oTypeIdentifier == OV_TypeId_Signal) { m_vEncoders.push_back(new TSignalEncoder<CPolyBox>(*this, output)); }
		else if (l_oTypeIdentifier == OV_TypeId_FeatureVector) { m_vEncoders.push_back(new TFeatureVectorEncoder<CPolyBox>(*this, output)); }
		else if (l_oTypeIdentifier == OV_TypeId_Spectrum) { m_vEncoders.push_back(new TSpectrumEncoder<CPolyBox>(*this, output)); }
		else if (l_oTypeIdentifier == OV_TypeId_ChannelLocalisation) { m_vEncoders.push_back(new TChannelLocalisationEncoder<CPolyBox>(*this, output)); }
		else if (l_oTypeIdentifier == OV_TypeId_Stimulations) { m_vEncoders.push_back(new TStimulationEncoder<CPolyBox>(*this, output)); }
		else if (l_oTypeIdentifier == OV_TypeId_ExperimentInformation) { m_vEncoders.push_back(new TExperimentInformationEncoder<CPolyBox>(*this, output)); }
		else
		{
			this->getLogManager() << LogLevel_Error << "Codec to encode " << l_oTypeIdentifier.toString() << " is not implemented.\n";
			return false;
		}
	}


	//New reference
	PyObject* l_pTemporyPyObject = Py_BuildValue("s,O", m_sScriptFilename.toASCIIString(), m_mainDictionnary);
	//New reference
	PyObject* l_pResult = PyObject_CallObject(m_execFileFunction, l_pTemporyPyObject);
	if (l_pResult == nullptr || PyInt_AsLong(l_pResult) != 0)
	{
		this->getLogManager() << LogLevel_Error << "Failed to run [" << m_sScriptFilename << "]";
		if (l_pResult) { this->getLogManager() << ", result = " << PyInt_AsLong(l_pResult) << "\n"; }
		else { this->getLogManager() << ", result = NULL\n"; }
		logSysStdout();
		logSysStderr();
		Py_CLEAR(l_pTemporyPyObject);
		Py_CLEAR(l_pResult);
		return false;
	}

	Py_CLEAR(l_pTemporyPyObject);
	Py_CLEAR(l_pResult);

	/*
	PyObject *l_pScriptFile = PyFile_FromString((char *) m_sScriptFilename.toASCIIString(), (char *) "r");
	if (l_pScriptFile == NULL)
	{
		this->getLogManager() << LogLevel_Error << "Failed to open " << m_sScriptFilename.toASCIIString() << ".\n";
		Py_CLEAR(l_pScriptFile);
		return false;
	}

	if (PyRun_SimpleFile(PyFile_AsFile(l_pScriptFile), m_sScriptFilename.toASCIIString()) == -1)
	{
		this->getLogManager() << LogLevel_Error << "Failed to run " << m_sScriptFilename.toASCIIString() << ".\n";
		Py_CLEAR(l_pScriptFile);
		return false;
	}
	Py_CLEAR(l_pScriptFile);
	*/

	//New reference
	m_box = PyObject_GetAttrString(m_mainModule, "box"); // la box qui vient juste d'etre creee
	if (m_box == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load \"box\" object.\n";
		return false;
	}
	//New reference
	m_boxInput = PyObject_GetAttrString(m_box, "input");
	if (m_boxInput == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load \"box.input\".\n";
		return false;
	}
	//New reference
	m_boxOutput = PyObject_GetAttrString(m_box, "output");
	if (m_boxOutput == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load \"box.output\".\n";
		return false;
	}

	CString l_sInputOutputType;
	for (uint32_t input = 0; input < l_rStaticBoxContext.getInputCount(); input++)
	{
		CIdentifier l_oTypeIdentifier;
		l_rStaticBoxContext.getInputType(input, l_oTypeIdentifier);
		if (l_oTypeIdentifier == OV_TypeId_StreamedMatrix) { l_sInputOutputType = "StreamedMatrix"; }
		else if (l_oTypeIdentifier == OV_TypeId_Signal) { l_sInputOutputType = "Signal"; }
		else if (l_oTypeIdentifier == OV_TypeId_FeatureVector) { l_sInputOutputType = "FeatureVector"; }
		else if (l_oTypeIdentifier == OV_TypeId_Spectrum) { l_sInputOutputType = "Spectrum"; }
		else if (l_oTypeIdentifier == OV_TypeId_ChannelLocalisation) { l_sInputOutputType = "ChannelLocalisation"; }
		else if (l_oTypeIdentifier == OV_TypeId_Stimulations) { l_sInputOutputType = "Stimulations"; }
		else if (l_oTypeIdentifier == OV_TypeId_ExperimentInformation) { l_sInputOutputType = "ExperimentInformation"; }
		//New reference
		PyObject* l_pResult = PyObject_CallMethod(m_box, const_cast<char*>("addInput"), const_cast<char*>("s"),
												  const_cast<char*>(l_sInputOutputType.toASCIIString()));
		if (l_pResult == nullptr)
		{
			this->getLogManager() << LogLevel_Error << "Failed to call box.addInput().\n";
			return false;
		}
		Py_CLEAR(l_pResult);
	}

	for (uint32_t output = 0; output < l_rStaticBoxContext.getOutputCount(); output++)
	{
		CIdentifier l_oTypeIdentifier;
		l_rStaticBoxContext.getOutputType(output, l_oTypeIdentifier);
		if (l_oTypeIdentifier == OV_TypeId_StreamedMatrix) { l_sInputOutputType = "StreamedMatrix"; }
		else if (l_oTypeIdentifier == OV_TypeId_Signal) { l_sInputOutputType = "Signal"; }
		else if (l_oTypeIdentifier == OV_TypeId_FeatureVector) { l_sInputOutputType = "FeatureVector"; }
		else if (l_oTypeIdentifier == OV_TypeId_Spectrum) { l_sInputOutputType = "Spectrum"; }
		else if (l_oTypeIdentifier == OV_TypeId_ChannelLocalisation) { l_sInputOutputType = "ChannelLocalisation"; }
		else if (l_oTypeIdentifier == OV_TypeId_Stimulations) { l_sInputOutputType = "Stimulations"; }
		else if (l_oTypeIdentifier == OV_TypeId_ExperimentInformation) { l_sInputOutputType = "ExperimentInformation"; }
		//New reference
		PyObject* l_pResult = PyObject_CallMethod(m_box, const_cast<char*>("addOutput"), const_cast<char*>("s"),
												  const_cast<char*>(l_sInputOutputType.toASCIIString()));
		if (l_pResult == nullptr)
		{
			this->getLogManager() << LogLevel_Error << "Failed to call box.addOutput().\n";
			return false;
		}
		Py_CLEAR(l_pResult);
	}

	//New reference
	m_boxSetting = PyObject_GetAttrString(m_box, "setting");
	if (m_boxSetting == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to load \"box.setting\".\n";
		return false;
	}
	buildPythonSettings();

	if (!PyObject_HasAttrString(m_box, "_clock"))
	{
		this->getLogManager() << LogLevel_Error << "Failed to initialize \"box._clock\" attribute because it does not exist.\n";
		return false;
	}
	//New reference
	PyObject* l_pBoxClock = PyInt_FromLong((long)m_clockFrequency);
	if (l_pBoxClock == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to convert m_clockFrequency into PyInt.\n";
		return false;
	}
	if (PyObject_SetAttrString(m_box, "_clock", l_pBoxClock) == -1)
	{
		this->getLogManager() << LogLevel_Error << "Failed to initialize \"box._clock\" attribute.\n";
		return false;
	}
	Py_CLEAR(l_pBoxClock);

	if (!PyObject_HasAttrString(m_box, "_currentTime"))
	{
		this->getLogManager() << LogLevel_Error << "Failed to initialize \"box._currentTime\" attribute because it does not exist.\n";
		return false;
	}
	//New reference
	m_boxCurrentTime = PyFloat_FromDouble(ITimeArithmetics::timeToSeconds(this->getPlayerContext().getCurrentTime()));
	if (m_boxCurrentTime == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to convert the current time into a PyFloat.\n";
		return false;
	}
	if (PyObject_SetAttrString(m_box, "_currentTime", m_boxCurrentTime) == -1)
	{
		this->getLogManager() << LogLevel_Error << "Failed to initialize \"box._currentTime\" attribute.\n";
		return false;
	}

	if (!PyObject_HasAttrString(m_box, "realInitialize"))
	{
		this->getLogManager() << LogLevel_Error << "No realInitialize.\n";
		return false;
	}

	//New reference
	m_boxInitialize = PyObject_GetAttrString(m_box, "realInitialize");
	if (m_boxInitialize == nullptr) { this->getLogManager() << LogLevel_ImportantWarning << "Failed to load \"box.realInitialize\" function.\n"; }
	else if (!PyCallable_Check(m_boxInitialize)) { this->getLogManager() << LogLevel_ImportantWarning << "\"box.realInitialize\" is not callable.\n"; }

	//New reference
	m_boxProcess = PyObject_GetAttrString(m_box, "realProcess");
	if (m_boxProcess == nullptr) { this->getLogManager() << LogLevel_ImportantWarning << "Failed to load \"box.realProcess\" function.\n"; }
	else if (!PyCallable_Check(m_boxProcess)) { this->getLogManager() << LogLevel_ImportantWarning << "\"box.realProcess\" is not callable.\n"; }

	//New reference
	m_boxUninitialize = PyObject_GetAttrString(m_box, "realUninitialize");
	if (m_boxUninitialize == nullptr) { this->getLogManager() << LogLevel_ImportantWarning << "Failed to load \"box.realUninitialize\" function.\n"; }
	else if (!PyCallable_Check(m_boxUninitialize)) { this->getLogManager() << LogLevel_ImportantWarning << "\"box.realUninitialize\" is not callable.\n"; }

	//Execute the initialize function defined in the python user script
	if (m_boxInitialize && PyCallable_Check(m_boxInitialize))
	{
		//New reference
		PyObject* l_pResult       = PyObject_CallObject(m_boxInitialize, nullptr);
		bool l_bLogSysStdoutError = logSysStdout(); // souci car la si l'init plante pas de sortie au bon endroit
		bool l_bLogSysStderrError = logSysStderr();
		if ((l_pResult == nullptr) || (!l_bLogSysStdoutError) || (!l_bLogSysStderrError))
		{
			if (l_pResult == nullptr) { this->getLogManager() << LogLevel_Error << "Failed to call \"box.__initialize\" function.\n"; }
			if (!l_bLogSysStdoutError) { this->getLogManager() << LogLevel_Error << "logSysStdout() failed during box.__initialization.\n"; }
			if (!l_bLogSysStderrError) { this->getLogManager() << LogLevel_Error << "logSysStderr() failed during box.__initialization.\n"; }
			Py_CLEAR(l_pResult);
			return false;
		}
		Py_CLEAR(l_pResult);
	}

	m_initializeSucceeded = true;
	return true;
}

bool CPolyBox::uninitialize()
{
	for (uint32_t i = 0; i < m_vDecoders.size(); i++)
	{
		m_vDecoders[i]->uninitialize();
		delete m_vDecoders[i];
	}
	m_vDecoders.clear();

	for (uint32_t i = 0; i < m_vEncoders.size(); i++)
	{
		m_vEncoders[i]->uninitialize();
		delete m_vEncoders[i];
	}
	m_vEncoders.clear();

	if (m_initializeSucceeded)
	{ // we call this uninit only if init had succeeded
		//Execute the uninitialize function defined in the python script
		// il y a un souci ici si le script n'a pas ete charge ca ne passe pas
		if (m_boxUninitialize && PyCallable_Check(m_boxUninitialize))
		{
			//New reference
			PyObject* l_pResult       = PyObject_CallObject(m_boxUninitialize, nullptr);
			bool l_bLogSysStdoutError = logSysStdout();
			bool l_bLogSysStderrError = logSysStderr();
			if ((l_pResult == nullptr) || (!l_bLogSysStdoutError) || (!l_bLogSysStderrError))
			{
				if (l_pResult == nullptr) { this->getLogManager() << LogLevel_Error << "Failed to call \"box.__uninitialize\" function.\n"; }
				if (!l_bLogSysStdoutError) { this->getLogManager() << LogLevel_Error << "logSysStdout() failed during box.__uninitialization.\n"; }
				if (!l_bLogSysStderrError) { this->getLogManager() << LogLevel_Error << "logSysStderr() failed during box.__uninitialization.\n"; }
				Py_CLEAR(l_pResult);
				return false;
			}
			Py_CLEAR(l_pResult);
		}
	}

	// Note: Py_CLEAR is safe to use on NULL pointers, so we can clean everything here
	Py_CLEAR(m_box);
	Py_CLEAR(m_boxInput);
	Py_CLEAR(m_boxOutput);
	Py_CLEAR(m_boxCurrentTime);
	Py_CLEAR(m_boxSetting);
	Py_CLEAR(m_boxInitialize);
	Py_CLEAR(m_boxProcess);
	Py_CLEAR(m_boxUninitialize);

	// Py_Initialize() and Py_Finalize() are called in ovp_main.cpp, we never uninitialize Python here

	return true;
}

bool CPolyBox::processClock(CMessageClock& messageClock)
{
	this->getLogManager() << LogLevel_Trace << "Received clock message at time " << messageClock.getTime() << "\n";
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CPolyBox::processInput(uint32_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CPolyBox::transferStreamedMatrixInputChunksToPython(const uint32_t input_index)
{
	IBoxIO& l_rDynamicBoxContext = this->getDynamicBoxContext();

	if (!PyList_Check(m_boxInput))
	{
		this->getLogManager() << LogLevel_Error << "box.input must be a list.\n";
		return false;
	}

	//Borrowed reference
	PyObject* l_pBuffer = PyList_GetItem(m_boxInput, (Py_ssize_t)input_index);
	if (l_pBuffer == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to get box.input[" << input_index << "].\n";
		return false;
	}
	//Expose input streamed matrix chunks to python
	for (uint32_t chunk_index = 0; chunk_index < l_rDynamicBoxContext.getInputChunkCount(input_index); chunk_index++)
	{
		m_vDecoders[input_index]->decode(chunk_index);

		if (m_vDecoders[input_index]->isHeaderReceived())
		{
			uint32_t l_ui32DimensionCount, l_ui32DimensionSize;
			IMatrix* l_pMatrix   = ((TStreamedMatrixDecoder<CPolyBox>*)m_vDecoders[input_index])->getOutputMatrix();
			l_ui32DimensionCount = l_pMatrix->getDimensionCount();

			//New reference
			PyObject* l_pDimensionSize = PyList_New(l_ui32DimensionCount);
			if (l_pDimensionSize == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new list l_pDimensionSize.\n";
				return false;
			}

			//New reference
			PyObject* l_pDimensionLabel = PyList_New(0);
			if (l_pDimensionLabel == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new list l_pDimensionLabel.\n";
				Py_CLEAR(l_pDimensionSize);
				return false;
			}

			for (uint32_t i = 0; i < l_ui32DimensionCount; i++)
			{
				l_ui32DimensionSize = l_pMatrix->getDimensionSize(i);
				if (PyList_SetItem(l_pDimensionSize, i, PyInt_FromLong(l_ui32DimensionSize)) == -1)
				{
					this->getLogManager() << LogLevel_Error << "Failed to set item " << i << " in dimension size list.\n";
					Py_CLEAR(l_pDimensionSize);
					Py_CLEAR(l_pDimensionLabel);
					return false;
				}
				for (uint32_t j = 0; j < l_ui32DimensionSize; j++)
				{
					if (PyList_Append(l_pDimensionLabel, PyString_FromString(l_pMatrix->getDimensionLabel(i, j))) == -1)
					{
						this->getLogManager() << LogLevel_Error << "Failed to append \"" << l_pMatrix->getDimensionLabel(i, j) <<
								"\" in dimension label list.\n";
						Py_CLEAR(l_pDimensionSize);
						Py_CLEAR(l_pDimensionLabel);
						return false;
					}
				}
			}

			//New reference
			PyObject* l_pArg = PyTuple_New(4);
			if (l_pArg == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new tuple l_pArg.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				return false;
			}
			if (PyTuple_SetItem(
					l_pArg, 0, PyFloat_FromDouble(ITimeArithmetics::timeToSeconds(l_rDynamicBoxContext.getInputChunkStartTime(input_index, chunk_index)))) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 0 (start time) in tuple l_pArg.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(
					l_pArg, 1, PyFloat_FromDouble(ITimeArithmetics::timeToSeconds(l_rDynamicBoxContext.getInputChunkEndTime(input_index, chunk_index)))) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 1 (end time) in tuple l_pArg.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 2, l_pDimensionSize) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 2 (dimension size) in tuple l_pArg.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 3, l_pDimensionLabel) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 3 (dimension label) in tuple l_pArg.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				Py_CLEAR(l_pArg);
				return false;
			}
			//New reference
			PyObject* l_pOVStreamedMatrixHeader = PyObject_Call(m_matrixHeader, l_pArg, nullptr);
			if (l_pOVStreamedMatrixHeader == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new OVStreamedMatrixHeader l_pOVStreamedMatrixHeader.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				Py_CLEAR(l_pArg);
				return false;
			}
			Py_CLEAR(l_pDimensionSize);
			Py_CLEAR(l_pDimensionLabel);
			Py_CLEAR(l_pArg);

			//New reference
			PyObject* l_pMethodToCall = PyString_FromString("append");
			//New reference
			PyObject* l_pResult = PyObject_CallMethodObjArgs(l_pBuffer, l_pMethodToCall, l_pOVStreamedMatrixHeader, NULL);
			Py_CLEAR(l_pMethodToCall);
			if (l_pResult == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to append chunk to box.input[" << input_index << "].\n";
				Py_CLEAR(l_pOVStreamedMatrixHeader);
				return false;
			}
			Py_CLEAR(l_pResult);
			Py_CLEAR(l_pOVStreamedMatrixHeader);
		}

		if (m_vDecoders[input_index]->isBufferReceived())
		{
			//New reference
			PyObject* l_pArg = PyTuple_New(3);
			if (l_pArg == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new tuple l_pArg.\n";
				return false;
			}
			if (PyTuple_SetItem(
					l_pArg, 0, PyFloat_FromDouble(ITimeArithmetics::timeToSeconds(l_rDynamicBoxContext.getInputChunkStartTime(input_index, chunk_index)))) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 0 (start time) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(
					l_pArg, 1, PyFloat_FromDouble(ITimeArithmetics::timeToSeconds(l_rDynamicBoxContext.getInputChunkEndTime(input_index, chunk_index)))) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 1 (end time) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 2, PyList_New(0)) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 2 (bufferElements) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}

			//New reference
			PyObject* l_pOVStreamedMatrixBuffer = PyObject_Call(m_matrixBuffer, l_pArg, nullptr);
			if (l_pOVStreamedMatrixBuffer == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new OVStreamedMatrixBuffer l_pOVStreamedMatrixBuffer.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			Py_CLEAR(l_pArg);

			IMatrix* l_pMatrix    = ((TStreamedMatrixDecoder<CPolyBox>*)m_vDecoders[input_index])->getOutputMatrix();
			double* l_pBufferBase = l_pMatrix->getBuffer();
			for (uint32_t element_index = 0; element_index < l_pMatrix->getBufferElementCount(); element_index++)
			{
				if (PyList_Append(l_pOVStreamedMatrixBuffer, PyFloat_FromDouble(l_pBufferBase[element_index])) == -1)
				{
					this->getLogManager() << LogLevel_Error << "Failed to append element " << element_index << " to l_pOVStreamedMatrixBuffer.\n";
					Py_CLEAR(l_pOVStreamedMatrixBuffer);
					return false;
				}
			}

			//New reference
			PyObject* l_pMethodToCall = PyString_FromString("append");
			//New reference
			PyObject* l_pResult = PyObject_CallMethodObjArgs(l_pBuffer, l_pMethodToCall, l_pOVStreamedMatrixBuffer, NULL);
			Py_CLEAR(l_pMethodToCall);
			if (l_pResult == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to append an OVStreamedMatrixBuffer to box.input[" << input_index << "].\n";
				Py_CLEAR(l_pOVStreamedMatrixBuffer);
				return false;
			}
			Py_CLEAR(l_pResult);
			Py_CLEAR(l_pOVStreamedMatrixBuffer);
		}

		if (m_vDecoders[input_index]->isEndReceived())
		{
			//New reference
			PyObject* l_pArg = PyTuple_New(2);
			if (l_pArg == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new tuple l_pArg.\n";
				return false;
			}
			if (PyTuple_SetItem(
					l_pArg, 0, PyFloat_FromDouble(ITimeArithmetics::timeToSeconds(l_rDynamicBoxContext.getInputChunkStartTime(input_index, chunk_index)))) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 0 (start time) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(
					l_pArg, 1, PyFloat_FromDouble(ITimeArithmetics::timeToSeconds(l_rDynamicBoxContext.getInputChunkEndTime(input_index, chunk_index)))) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 1 (end time) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}

			//New reference
			PyObject* l_pOVStreamedMatrixEnd = PyObject_Call(m_matrixEnd, l_pArg, nullptr);
			if (l_pOVStreamedMatrixEnd == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new OVStreamedMatrixEnd l_pOVStreamedMatrixEnd.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			Py_CLEAR(l_pArg);

			//New reference
			PyObject* l_pMethodToCall = PyString_FromString("append");
			//New reference
			PyObject* l_pResult = PyObject_CallMethodObjArgs(l_pBuffer, l_pMethodToCall, l_pOVStreamedMatrixEnd, NULL);
			Py_CLEAR(l_pMethodToCall);
			if (l_pResult == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to append an OVStreamedMatrixEnd to box.input[" << input_index << "].\n";
				Py_CLEAR(l_pOVStreamedMatrixEnd);
				return false;
			}
			Py_CLEAR(l_pResult);
			Py_CLEAR(l_pOVStreamedMatrixEnd);
		}
	}

	return true;
}

bool CPolyBox::transferStreamedMatrixOutputChunksFromPython(const uint32_t output_index)
{
	IBoxIO& boxCtx = this->getDynamicBoxContext();

	IMatrix* matrix = ((TStreamedMatrixEncoder<CPolyBox>*)m_vEncoders[output_index])->getInputMatrix();

	if (!PyList_Check(m_boxOutput))
	{
		this->getLogManager() << LogLevel_Error << "box.output must be a list.\n";
		return false;
	}

	//Borrowed reference
	PyObject* l_pBuffer = PyList_GetItem(m_boxOutput, Py_ssize_t(output_index));
	if (l_pBuffer == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to get box.output[" << output_index << "].\n";
		return false;
	}

	//New reference
	PyObject* l_pBufferLen = PyObject_CallMethod(l_pBuffer, (char*)"__len__", nullptr);
	if (l_pBufferLen == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to get box.output[" << output_index << "].__len__().\n";
		return false;
	}

	uint32_t l_ui32OutputLen = PyInt_AsUnsignedLongMask(l_pBufferLen);
	Py_CLEAR(l_pBufferLen);
	for (uint32_t chunk_index = 0; chunk_index < l_ui32OutputLen; chunk_index++)
	{
		//New reference
		PyObject* l_pOVChunk = PyObject_CallMethod(l_pBuffer, (char*)"pop", nullptr);
		if (l_pOVChunk == nullptr)
		{
			this->getLogManager() << LogLevel_Error << "Failed to get item " << chunk_index << " of box.output[" << output_index << "].\n";
			return false;
		}

		if (PyObject_IsInstance(l_pOVChunk, m_matrixHeader) == 1)
		{
			uint32_t l_ui32DimensionCount, l_ui32DimensionSize, i, j;

			//New reference
			PyObject* l_pDimensionCount = PyObject_CallMethod(l_pOVChunk, (char*)"getDimensionCount", nullptr);
			if (l_pDimensionCount == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to call chunk's getDimensionCount method.\n";
				Py_CLEAR(l_pOVChunk);
				return false;
			}
			l_ui32DimensionCount = PyInt_AsUnsignedLongMask(l_pDimensionCount);
			matrix->setDimensionCount(l_ui32DimensionCount);
			Py_CLEAR(l_pDimensionCount);

			//New reference
			PyObject* l_pDimensionSize = PyObject_GetAttrString(l_pOVChunk, "dimensionSizes");

			//New reference
			PyObject* l_pDimensionLabel = PyObject_GetAttrString(l_pOVChunk, "dimensionLabels");

			uint32_t offset = 0;
			for (i = 0; i < l_ui32DimensionCount; i++)
			{
				l_ui32DimensionSize = PyInt_AsUnsignedLongMask(PyList_GetItem(l_pDimensionSize, Py_ssize_t(i)));
				matrix->setDimensionSize(i, l_ui32DimensionSize);
				for (j = 0; j < l_ui32DimensionSize; j++)
				{
					matrix->setDimensionLabel(i, j, PyString_AsString(PyList_GetItem(l_pDimensionLabel, offset + j)));
				}
				offset = offset + l_ui32DimensionSize;
			}
			Py_CLEAR(l_pDimensionSize);
			Py_CLEAR(l_pDimensionLabel);

			m_vEncoders[output_index]->encodeHeader();

			//New reference
			PyObject* l_pStartTime = PyObject_GetAttrString(l_pOVChunk, "startTime");
			uint64_t start         = ITimeArithmetics::secondsToTime(PyFloat_AsDouble(l_pStartTime));
			Py_CLEAR(l_pStartTime);

			//New reference
			PyObject* l_pEndTime = PyObject_GetAttrString(l_pOVChunk, "endTime");
			uint64_t end         = ITimeArithmetics::secondsToTime(PyFloat_AsDouble(l_pEndTime));
			Py_CLEAR(l_pEndTime);

			boxCtx.markOutputAsReadyToSend(output_index, start, end);
		}

		else if (PyObject_IsInstance(l_pOVChunk, m_matrixBuffer) == 1)
		{
			double* l_pBufferBase = matrix->getBuffer();
			for (uint32_t i = 0; i < matrix->getBufferElementCount(); i++) { l_pBufferBase[i] = PyFloat_AsDouble(PyList_GetItem(l_pOVChunk, i)); }

			//New reference
			PyObject* l_pStartTime = PyObject_GetAttrString(l_pOVChunk, "startTime");
			uint64_t start         = ITimeArithmetics::secondsToTime(PyFloat_AsDouble(l_pStartTime));
			Py_CLEAR(l_pStartTime);

			//New reference
			PyObject* l_pEndTime = PyObject_GetAttrString(l_pOVChunk, "endTime");
			uint64_t end         = ITimeArithmetics::secondsToTime(PyFloat_AsDouble(l_pEndTime));
			Py_CLEAR(l_pEndTime);

			m_vEncoders[output_index]->encodeBuffer();
			boxCtx.markOutputAsReadyToSend(output_index, start, end);
		}

		else if (PyObject_IsInstance(l_pOVChunk, m_matrixEnd) == 1)
		{
			//New reference
			PyObject* l_pStartTime = PyObject_GetAttrString(l_pOVChunk, "startTime");
			uint64_t start         = ITimeArithmetics::secondsToTime(PyFloat_AsDouble(l_pStartTime));
			Py_CLEAR(l_pStartTime);

			//New reference
			PyObject* l_pEndTime = PyObject_GetAttrString(l_pOVChunk, "endTime");
			uint64_t end         = ITimeArithmetics::secondsToTime(PyFloat_AsDouble(l_pEndTime));
			Py_CLEAR(l_pEndTime);

			m_vEncoders[output_index]->encodeEnd();
			boxCtx.markOutputAsReadyToSend(output_index, start, end);
		}

		else
		{
			this->getLogManager() << LogLevel_Error << "Unexpected object type for item " << chunk_index << " in box.output[" << output_index << "].\n";
			Py_CLEAR(l_pOVChunk);
			return false;
		}

		Py_CLEAR(l_pOVChunk);
	}
	return true;
}

bool CPolyBox::transferSignalInputChunksToPython(const uint32_t input_index)
{
	IBoxIO& l_rDynamicBoxContext = this->getDynamicBoxContext();

	if (!PyList_Check(m_boxInput))
	{
		this->getLogManager() << LogLevel_Error << "box.input must be a list.\n";
		return false;
	}

	//Borrowed reference
	PyObject* l_pBuffer = PyList_GetItem(m_boxInput, (Py_ssize_t)input_index);
	if (l_pBuffer == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to get box.input[" << input_index << "].\n";
		return false;
	}
	//Expose input signal chunks to python
	for (uint32_t chunk_index = 0; chunk_index < l_rDynamicBoxContext.getInputChunkCount(input_index); chunk_index++)
	{
		m_vDecoders[input_index]->decode(chunk_index);

		if (m_vDecoders[input_index]->isHeaderReceived())
		{
			uint32_t l_ui32DimensionCount, l_ui32DimensionSize;
			IMatrix* l_pMatrix   = ((TSignalDecoder<CPolyBox>*)m_vDecoders[input_index])->getOutputMatrix();
			l_ui32DimensionCount = l_pMatrix->getDimensionCount();

			//New reference
			PyObject* l_pDimensionSize = PyList_New(l_ui32DimensionCount);
			if (l_pDimensionSize == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new list l_pDimensionSize.\n";
				return false;
			}

			//New reference
			PyObject* l_pDimensionLabel = PyList_New(0);
			if (l_pDimensionLabel == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new list l_pDimensionLabel.\n";
				Py_CLEAR(l_pDimensionSize);
				return false;
			}

			for (uint32_t i = 0; i < l_ui32DimensionCount; i++)
			{
				l_ui32DimensionSize = l_pMatrix->getDimensionSize(i);
				if (PyList_SetItem(l_pDimensionSize, i, PyInt_FromLong(l_ui32DimensionSize)) == -1)
				{
					this->getLogManager() << LogLevel_Error << "Failed to set item " << i << " in dimension size list.\n";
					Py_CLEAR(l_pDimensionSize);
					Py_CLEAR(l_pDimensionLabel);
					return false;
				}
				for (uint32_t j = 0; j < l_ui32DimensionSize; j++)
				{
					if (PyList_Append(l_pDimensionLabel, PyString_FromString(l_pMatrix->getDimensionLabel(i, j))) == -1)
					{
						this->getLogManager() << LogLevel_Error << "Failed to append \"" << l_pMatrix->getDimensionLabel(i, j) <<
								"\" in dimension label list.\n";
						Py_CLEAR(l_pDimensionSize);
						Py_CLEAR(l_pDimensionLabel);
						return false;
					}
				}
			}

			//New reference
			PyObject* l_pArg = PyTuple_New(5);
			if (l_pArg == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new tuple l_pArg.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				return false;
			}
			if (PyTuple_SetItem(
					l_pArg, 0, PyFloat_FromDouble(ITimeArithmetics::timeToSeconds(l_rDynamicBoxContext.getInputChunkStartTime(input_index, chunk_index)))) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 0 (start time) in tuple l_pArg.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(
					l_pArg, 1, PyFloat_FromDouble(ITimeArithmetics::timeToSeconds(l_rDynamicBoxContext.getInputChunkEndTime(input_index, chunk_index)))) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 1 (end time) in tuple l_pArg.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 2, l_pDimensionSize) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 2 (dimension size) in tuple l_pArg.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 3, l_pDimensionLabel) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 3 (dimension label) in tuple l_pArg.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 4, PyInt_FromLong((long)((TSignalDecoder<CPolyBox>*)m_vDecoders[input_index])->getOutputSamplingRate())) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 4 (samplingRate) in tuple l_pArg.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				Py_CLEAR(l_pArg);
				return false;
			}
			//New reference
			PyObject* l_pOVSignalHeader = PyObject_Call(m_signalHeader, l_pArg, nullptr);
			if (l_pOVSignalHeader == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new OVSignalHeader l_pOVSignalHeader.\n";
				Py_CLEAR(l_pDimensionSize);
				Py_CLEAR(l_pDimensionLabel);
				Py_CLEAR(l_pArg);
				return false;
			}
			Py_CLEAR(l_pDimensionSize);
			Py_CLEAR(l_pDimensionLabel);
			Py_CLEAR(l_pArg);

			//New reference
			PyObject* l_pMethodToCall = PyString_FromString("append");
			//New reference
			PyObject* l_pResult = PyObject_CallMethodObjArgs(l_pBuffer, l_pMethodToCall, l_pOVSignalHeader, NULL);
			Py_CLEAR(l_pMethodToCall);
			if (l_pResult == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to append chunk to box.input[" << input_index << "].\n";
				Py_CLEAR(l_pOVSignalHeader);
				return false;
			}
			Py_CLEAR(l_pResult);
			Py_CLEAR(l_pOVSignalHeader);
		}

		if (m_vDecoders[input_index]->isBufferReceived())
		{
			//New reference
			PyObject* l_pArg = PyTuple_New(3);
			if (l_pArg == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new tuple l_pArg.\n";
				return false;
			}
			if (PyTuple_SetItem(
					l_pArg, 0, PyFloat_FromDouble(ITimeArithmetics::timeToSeconds(l_rDynamicBoxContext.getInputChunkStartTime(input_index, chunk_index)))) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 0 (startTime) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(
					l_pArg, 1, PyFloat_FromDouble(ITimeArithmetics::timeToSeconds(l_rDynamicBoxContext.getInputChunkEndTime(input_index, chunk_index)))) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 1 (endTime) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(l_pArg, 2, PyList_New(0)) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 2 (bufferElements) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}

			//New reference
			PyObject* l_pOVSignalBuffer = PyObject_Call(m_signalBuffer, l_pArg, nullptr);
			if (l_pOVSignalBuffer == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new OVSignalBuffer l_pOVSignalBuffer.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			Py_CLEAR(l_pArg);

			IMatrix* l_pMatrix    = ((TSignalDecoder<CPolyBox>*)m_vDecoders[input_index])->getOutputMatrix();
			double* l_pBufferBase = l_pMatrix->getBuffer();
			for (uint32_t element_index = 0; element_index < l_pMatrix->getBufferElementCount(); element_index++)
			{
				if (PyList_Append(l_pOVSignalBuffer, PyFloat_FromDouble(l_pBufferBase[element_index])) == -1)
				{
					this->getLogManager() << LogLevel_Error << "Failed to append element " << element_index << " to l_pOVSignalBuffer.\n";
					Py_CLEAR(l_pOVSignalBuffer);
					return false;
				}
			}

			//New reference
			PyObject* l_pMethodToCall = PyString_FromString("append");
			//New reference
			PyObject* l_pResult = PyObject_CallMethodObjArgs(l_pBuffer, l_pMethodToCall, l_pOVSignalBuffer, NULL);
			Py_CLEAR(l_pMethodToCall);
			if (l_pResult == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to append an OVSignalBuffer to box.input[" << input_index << "].\n";
				Py_CLEAR(l_pOVSignalBuffer);
				return false;
			}
			Py_CLEAR(l_pResult);
			Py_CLEAR(l_pOVSignalBuffer);
		}

		if (m_vDecoders[input_index]->isEndReceived())
		{
			//New reference
			PyObject* l_pArg = PyTuple_New(2);
			if (l_pArg == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new tuple l_pArg.\n";
				return false;
			}
			if (PyTuple_SetItem(
					l_pArg, 0, PyFloat_FromDouble(ITimeArithmetics::timeToSeconds(l_rDynamicBoxContext.getInputChunkStartTime(input_index, chunk_index)))) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 0 (start time) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(
					l_pArg, 1, PyFloat_FromDouble(ITimeArithmetics::timeToSeconds(l_rDynamicBoxContext.getInputChunkEndTime(input_index, chunk_index)))) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 1 (end time) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}

			//New reference
			PyObject* l_pOVSignalEnd = PyObject_Call(m_signalEnd, l_pArg, nullptr);
			if (l_pOVSignalEnd == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new m_signalEnd l_pOVSignalEnd.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			Py_CLEAR(l_pArg);

			//New reference
			PyObject* l_pMethodToCall = PyString_FromString("append");
			//New reference
			PyObject* l_pResult = PyObject_CallMethodObjArgs(l_pBuffer, l_pMethodToCall, l_pOVSignalEnd, NULL);
			Py_CLEAR(l_pMethodToCall);
			if (l_pResult == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to append an OVSignalEnd to box.input[" << input_index << "].\n";
				Py_CLEAR(l_pOVSignalEnd);
				return false;
			}
			Py_CLEAR(l_pResult);
			Py_CLEAR(l_pOVSignalEnd);
		}
	}

	return true;
}

bool CPolyBox::transferSignalOutputChunksFromPython(const uint32_t output_index)
{
	IBoxIO& l_rDynamicBoxContext = this->getDynamicBoxContext();

	IMatrix* l_pMatrix = ((TSignalEncoder<CPolyBox>*)m_vEncoders[output_index])->getInputMatrix();

	if (!PyList_Check(m_boxOutput))
	{
		this->getLogManager() << LogLevel_Error << "box.output must be a list.\n";
		return false;
	}

	//Borrowed reference
	PyObject* l_pBuffer = PyList_GetItem(m_boxOutput, (Py_ssize_t)output_index);
	if (l_pBuffer == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to get box.output[" << output_index << "].\n";
		return false;
	}

	//New reference
	PyObject* l_pBufferLen = PyObject_CallMethod(l_pBuffer, (char*)"__len__", nullptr);
	if (l_pBufferLen == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to get box.output[" << output_index << "].__len__().\n";
		return false;
	}

	uint32_t l_ui32OutputLen = PyInt_AsUnsignedLongMask(l_pBufferLen);
	Py_CLEAR(l_pBufferLen);
	for (uint32_t chunk_index = 0; chunk_index < l_ui32OutputLen; chunk_index++)
	{
		//New reference
		PyObject* l_pOVChunk = PyObject_CallMethod(l_pBuffer, (char*)"pop", nullptr);
		if (l_pOVChunk == nullptr)
		{
			this->getLogManager() << LogLevel_Error << "Failed to get item " << chunk_index << " of box.output[" << output_index << "].\n";
			return false;
		}

		if (PyObject_IsInstance(l_pOVChunk, m_signalHeader) == 1)
		{
			uint32_t l_ui32DimensionCount, l_ui32DimensionSize, l_ui32DimensionIndex, l_ui32DimensionEntryIndex;

			//New reference
			PyObject* l_pDimensionCount = PyObject_CallMethod(l_pOVChunk, (char*)"getDimensionCount", nullptr);
			if (l_pDimensionCount == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to call chunk's getDimensionCount method.\n";
				Py_CLEAR(l_pOVChunk);
				return false;
			}
			l_ui32DimensionCount = PyInt_AsUnsignedLongMask(l_pDimensionCount);
			l_pMatrix->setDimensionCount(l_ui32DimensionCount);
			Py_CLEAR(l_pDimensionCount);

			//New reference
			PyObject* l_pDimensionSize = PyObject_GetAttrString(l_pOVChunk, "dimensionSizes");

			//New reference
			PyObject* l_pDimensionLabel = PyObject_GetAttrString(l_pOVChunk, "dimensionLabels");

			uint32_t offset = 0;
			for (l_ui32DimensionIndex = 0; l_ui32DimensionIndex < l_ui32DimensionCount; l_ui32DimensionIndex++)
			{
				l_ui32DimensionSize = PyInt_AsUnsignedLongMask(PyList_GetItem(l_pDimensionSize, (Py_ssize_t)l_ui32DimensionIndex));
				l_pMatrix->setDimensionSize(l_ui32DimensionIndex, l_ui32DimensionSize);
				for (l_ui32DimensionEntryIndex = 0; l_ui32DimensionEntryIndex < l_ui32DimensionSize; l_ui32DimensionEntryIndex++)
				{
					l_pMatrix->setDimensionLabel(l_ui32DimensionIndex, l_ui32DimensionEntryIndex,
												 PyString_AsString(PyList_GetItem(l_pDimensionLabel, offset + l_ui32DimensionEntryIndex)));
				}
				offset = offset + l_ui32DimensionSize;
			}
			Py_CLEAR(l_pDimensionSize);
			Py_CLEAR(l_pDimensionLabel);

			//New reference
			PyObject* l_pChunkSamplingRate = PyObject_GetAttrString(l_pOVChunk, "samplingRate");
			if (l_pChunkSamplingRate == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to load signal header sampling rate.\n";
				return false;
			}
			TParameterHandler<uint64_t>& l_pSamplingRate = ((TSignalEncoder<CPolyBox>*)m_vEncoders[output_index])->
					getInputSamplingRate();
			l_pSamplingRate = (uint64_t)PyInt_AsLong(l_pChunkSamplingRate);
			m_vEncoders[output_index]->encodeHeader();
			Py_CLEAR(l_pChunkSamplingRate);

			//New reference
			PyObject* l_pStartTime = PyObject_GetAttrString(l_pOVChunk, "startTime");
			uint64_t start         = ITimeArithmetics::secondsToTime(PyFloat_AsDouble(l_pStartTime));
			Py_CLEAR(l_pStartTime);

			//New reference
			PyObject* l_pEndTime = PyObject_GetAttrString(l_pOVChunk, "endTime");
			uint64_t end         = ITimeArithmetics::secondsToTime(PyFloat_AsDouble(l_pEndTime));
			Py_CLEAR(l_pEndTime);

			l_rDynamicBoxContext.markOutputAsReadyToSend(output_index, start, end);
		}

		else if (PyObject_IsInstance(l_pOVChunk, m_signalBuffer) == 1)
		{
			double* l_pBufferBase = l_pMatrix->getBuffer();
			for (uint32_t i = 0; i < l_pMatrix->getBufferElementCount(); i++) { l_pBufferBase[i] = PyFloat_AsDouble(PyList_GetItem(l_pOVChunk, i)); }

			//New reference
			PyObject* l_pStartTime = PyObject_GetAttrString(l_pOVChunk, "startTime");
			uint64_t start         = ITimeArithmetics::secondsToTime(PyFloat_AsDouble(l_pStartTime));
			Py_CLEAR(l_pStartTime);

			//New reference
			PyObject* l_pEndTime = PyObject_GetAttrString(l_pOVChunk, "endTime");
			uint64_t end         = ITimeArithmetics::secondsToTime(PyFloat_AsDouble(l_pEndTime));
			Py_CLEAR(l_pEndTime);

			m_vEncoders[output_index]->encodeBuffer();
			l_rDynamicBoxContext.markOutputAsReadyToSend(output_index, start, end);
		}

		else if (PyObject_IsInstance(l_pOVChunk, m_signalEnd) == 1)
		{
			//New reference
			PyObject* l_pStartTime = PyObject_GetAttrString(l_pOVChunk, "startTime");
			uint64_t start         = ITimeArithmetics::secondsToTime(PyFloat_AsDouble(l_pStartTime));
			Py_CLEAR(l_pStartTime);

			//New reference
			PyObject* l_pEndTime = PyObject_GetAttrString(l_pOVChunk, "endTime");
			uint64_t end         = ITimeArithmetics::secondsToTime(PyFloat_AsDouble(l_pEndTime));
			Py_CLEAR(l_pEndTime);

			m_vEncoders[output_index]->encodeEnd();
			l_rDynamicBoxContext.markOutputAsReadyToSend(output_index, start, end);
		}

		else
		{
			this->getLogManager() << LogLevel_Error << "Unexpected object type for item " << chunk_index << " in box.output[" << output_index << "].\n";
			Py_CLEAR(l_pOVChunk);
			return false;
		}

		Py_CLEAR(l_pOVChunk);
	}
	return true;
}

bool CPolyBox::transferStimulationInputChunksToPython(const uint32_t index)
{
	IBoxIO& l_rDynamicBoxContext = this->getDynamicBoxContext();
	//Borrowed reference
	PyObject* l_pBuffer = PyList_GetItem(m_boxInput, (Py_ssize_t)index);
	if (l_pBuffer == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to get box.input[" << index << "].\n";
		return false;
	}
	for (uint32_t chunk_index = 0; chunk_index < l_rDynamicBoxContext.getInputChunkCount(index); chunk_index++)
	{
		m_vDecoders[index]->decode(chunk_index);

		if (m_vDecoders[index]->isHeaderReceived())
		{
			//New reference
			PyObject* l_pArg = PyTuple_New(2);
			if (l_pArg == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new tuple l_pArg.\n";
				return false;
			}
			if (PyTuple_SetItem(
					l_pArg, 0, PyFloat_FromDouble(ITimeArithmetics::timeToSeconds(l_rDynamicBoxContext.getInputChunkStartTime(index, chunk_index)))) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 0 (start time) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(
					l_pArg, 1, PyFloat_FromDouble(ITimeArithmetics::timeToSeconds(l_rDynamicBoxContext.getInputChunkEndTime(index, chunk_index)))) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 1 (end time) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			PyObject* l_pOVStimulationHeader = PyObject_Call(m_stimulationHeader, l_pArg, nullptr);
			if (l_pOVStimulationHeader == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new OVStimulationHeader l_pOVStimulationHeader.\n";
				PyErr_Print();
				Py_CLEAR(l_pArg);
				return false;
			}
			Py_CLEAR(l_pArg);

			//New reference
			PyObject* l_pMethodToCall = PyString_FromString("append");
			//New reference
			PyObject* l_pResult = PyObject_CallMethodObjArgs(l_pBuffer, l_pMethodToCall, l_pOVStimulationHeader, NULL);
			Py_CLEAR(l_pMethodToCall);
			if (l_pResult == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to append an OVStimulationHeader to box.input[" << index << "].\n";
				Py_CLEAR(l_pOVStimulationHeader);
				return false;
			}
			Py_CLEAR(l_pResult);
			Py_CLEAR(l_pOVStimulationHeader);
		}

		if (m_vDecoders[index]->isBufferReceived())
		{
			IStimulationSet* l_pStimulationSet = ((TStimulationDecoder<CPolyBox>*)m_vDecoders[index])->getOutputStimulationSet();

			//New reference
			PyObject* l_pArg = PyTuple_New(2);
			if (l_pArg == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new tuple l_pArg.\n";
				return false;
			}
			if (PyTuple_SetItem(
					l_pArg, 0, PyFloat_FromDouble(ITimeArithmetics::timeToSeconds(l_rDynamicBoxContext.getInputChunkStartTime(index, chunk_index)))) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 0 (start time) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(
					l_pArg, 1, PyFloat_FromDouble(ITimeArithmetics::timeToSeconds(l_rDynamicBoxContext.getInputChunkEndTime(index, chunk_index)))) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 1 (end time) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			//New reference
			PyObject* l_pOVStimulationSet = PyObject_Call(m_stimulationSet, l_pArg, nullptr);
			if (l_pOVStimulationSet == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new OVStimulationSet l_pOVStimulationSet.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			Py_CLEAR(l_pArg);

			for (uint32_t stimulation_index = 0; stimulation_index < l_pStimulationSet->getStimulationCount(); stimulation_index++)
			{
				uint64_t l_ui64StimulationIdentifier = l_pStimulationSet->getStimulationIdentifier(stimulation_index);
				uint64_t l_ui64StimulationDate       = l_pStimulationSet->getStimulationDate(stimulation_index);
				uint64_t l_ui64StimulationDuration   = l_pStimulationSet->getStimulationDuration(stimulation_index);

				//New reference
				PyObject* l_pArg = PyTuple_New(3);
				if (l_pArg == nullptr)
				{
					this->getLogManager() << LogLevel_Error << "Failed to create a new tuple l_pArg.\n";
					return false;
				}
				if (PyTuple_SetItem(l_pArg, 0, PyInt_FromLong((long)l_ui64StimulationIdentifier)) != 0)
				{
					this->getLogManager() << LogLevel_Error << "Failed to set item 0 (identifier) in tuple l_pArg.\n";
					Py_CLEAR(l_pArg);
					return false;
				}
				if (PyTuple_SetItem(l_pArg, 1, PyFloat_FromDouble(ITimeArithmetics::timeToSeconds(l_ui64StimulationDate))) != 0)
				{
					this->getLogManager() << LogLevel_Error << "Failed to set item 1 (date) in tuple l_pArg.\n";
					Py_CLEAR(l_pArg);
					return false;
				}
				if (PyTuple_SetItem(l_pArg, 2, PyFloat_FromDouble(ITimeArithmetics::timeToSeconds(l_ui64StimulationDuration))) != 0)
				{
					this->getLogManager() << LogLevel_Error << "Failed to set item 2 (duration) in tuple l_pArg.\n";
					Py_CLEAR(l_pArg);
					return false;
				}
				//New reference
				PyObject* l_pOVStimulation = PyObject_Call(m_stimulation, l_pArg, nullptr);
				if (l_pOVStimulation == nullptr)
				{
					this->getLogManager() << LogLevel_Error << "Failed to create a new OVStimulation l_pOVStimulation.\n";
					Py_CLEAR(l_pArg);
					return false;
				}
				Py_CLEAR(l_pArg);

				//New reference
				PyObject* l_pMethodToCall = PyString_FromString("append");
				//New reference
				PyObject* l_pResult = PyObject_CallMethodObjArgs(l_pOVStimulationSet, l_pMethodToCall, l_pOVStimulation, NULL);
				Py_CLEAR(l_pMethodToCall);
				if (l_pResult == nullptr)
				{
					this->getLogManager() << LogLevel_Error << "Failed to append stimulation to box.input[" << index << "].\n";
					Py_CLEAR(l_pOVStimulation);
					return false;
				}
				Py_CLEAR(l_pResult);
				Py_CLEAR(l_pOVStimulation);
			}

			//New reference
			PyObject* l_pMethodToCall = PyString_FromString("append");
			//New reference
			PyObject* l_pResult = PyObject_CallMethodObjArgs(l_pBuffer, l_pMethodToCall, l_pOVStimulationSet, NULL);
			Py_CLEAR(l_pMethodToCall);
			if (l_pResult == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to append stimulation set to box.input[" << index << "].\n";
				Py_CLEAR(l_pOVStimulationSet);
				return false;
			}
			Py_CLEAR(l_pResult);
			Py_CLEAR(l_pOVStimulationSet);
		}

		if (m_vDecoders[index]->isEndReceived())
		{
			//New reference
			PyObject* l_pArg = PyTuple_New(2);
			if (l_pArg == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new tuple l_pArg.\n";
				return false;
			}
			if (PyTuple_SetItem(
					l_pArg, 0, PyFloat_FromDouble(ITimeArithmetics::timeToSeconds(l_rDynamicBoxContext.getInputChunkStartTime(index, chunk_index)))) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 0 (start time) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			if (PyTuple_SetItem(
					l_pArg, 1, PyFloat_FromDouble(ITimeArithmetics::timeToSeconds(l_rDynamicBoxContext.getInputChunkEndTime(index, chunk_index)))) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Failed to set item 1 (end time) in tuple l_pArg.\n";
				Py_CLEAR(l_pArg);
				return false;
			}

			//New reference
			PyObject* l_pOVStimulationEnd = PyObject_Call(m_stimulationEnd, l_pArg, nullptr);
			if (l_pOVStimulationEnd == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to create a new OVStimulationEnd l_pOVStimulationEnd.\n";
				Py_CLEAR(l_pArg);
				return false;
			}
			Py_CLEAR(l_pArg);

			//New reference
			PyObject* l_pMethodToCall = PyString_FromString("append");
			//New reference
			PyObject* l_pResult = PyObject_CallMethodObjArgs(l_pBuffer, l_pMethodToCall, l_pOVStimulationEnd, NULL);
			Py_CLEAR(l_pMethodToCall);
			if (l_pResult == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to append an OVStimulationEnd to box.input[" << index << "].\n";
				Py_CLEAR(l_pOVStimulationEnd);
				return false;
			}
			Py_CLEAR(l_pResult);
			Py_CLEAR(l_pOVStimulationEnd);
		}
	}
	return true;
}

bool CPolyBox::transferStimulationOutputChunksFromPython(const uint32_t index)
{
	IBoxIO& boxCtx = this->getDynamicBoxContext();

	IStimulationSet* l_pStimulationSet = ((TStimulationEncoder<CPolyBox>*)m_vEncoders[index])->getInputStimulationSet();

	if (!PyList_Check(m_boxOutput))
	{
		this->getLogManager() << LogLevel_Error << "box.output must be a list.\n";
		return false;
	}
	//Borrowed reference
	PyObject* l_pBuffer = PyList_GetItem(m_boxOutput, Py_ssize_t(index));
	if (l_pBuffer == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to get box.output[" << index << "].\n";
		return false;
	}

	//New reference
	PyObject* l_pBufferLen = PyObject_CallMethod(l_pBuffer, (char*)"__len__", nullptr);
	if (l_pBufferLen == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to get box.output[" << index << "].__len__().\n";
		return false;
	}
	uint32_t l_ui32OutputLen = PyInt_AsUnsignedLongMask(l_pBufferLen);
	Py_CLEAR(l_pBufferLen);
	for (uint32_t chunk_index = 0; chunk_index < l_ui32OutputLen; chunk_index++)
	{
		//New reference
		PyObject* l_pOVChunk = PyObject_CallMethod(l_pBuffer, (char*)"pop", nullptr);
		if (l_pOVChunk == nullptr)
		{
			this->getLogManager() << LogLevel_Error << "Failed to get item " << chunk_index << " of box.output[" << index << "].\n";
			return false;
		}

		if (PyObject_IsInstance(l_pOVChunk, m_stimulationHeader) == 1)
		{
			//New reference
			PyObject* l_pStartTime = PyObject_GetAttrString(l_pOVChunk, "startTime");
			uint64_t start         = ITimeArithmetics::secondsToTime(PyFloat_AsDouble(l_pStartTime));
			Py_CLEAR(l_pStartTime);

			//New reference
			PyObject* l_pEndTime = PyObject_GetAttrString(l_pOVChunk, "endTime");
			uint64_t end         = ITimeArithmetics::secondsToTime(PyFloat_AsDouble(l_pEndTime));
			Py_CLEAR(l_pEndTime);

			l_pStimulationSet->setStimulationCount(0);
			m_vEncoders[index]->encodeHeader();
			boxCtx.markOutputAsReadyToSend(index, start, end);
		}

		else if (PyObject_IsInstance(l_pOVChunk, m_stimulationSet) == 1)
		{
			//New reference
			PyObject* l_pOVChunkLen = PyObject_CallMethod(l_pOVChunk, (char*)"__len__", nullptr);
			if (l_pOVChunkLen == nullptr)
			{
				this->getLogManager() << LogLevel_Error << "Failed to get stimulations set length.\n";
				return false;
			}
			uint32_t l_ui32OVChunkLen = PyInt_AsUnsignedLongMask(l_pOVChunkLen);
			Py_CLEAR(l_pOVChunkLen);

			l_pStimulationSet->setStimulationCount(0);
			for (uint32_t stim_index = 0; stim_index < l_ui32OVChunkLen; stim_index++)
			{
				//Borrowed reference
				PyObject* l_pOVStimulation = PyList_GetItem(l_pOVChunk, (Py_ssize_t)stim_index);
				if (l_pOVStimulation == nullptr)
				{
					this->getLogManager() << LogLevel_Error << "Failed to get item " << stim_index << " of chunk " << chunk_index << ".\n";
					return false;
				}
				if (PyObject_IsInstance(l_pOVStimulation, m_stimulation) < 1)
				{
					this->getLogManager() << LogLevel_Error << "Item " << stim_index << " is not an OVStimulation.\n";
					return false;
				}
				//New reference
				PyObject* l_pIdentifier   = PyObject_GetAttrString(l_pOVStimulation, "identifier");
				uint64_t l_ui64Identifier = (uint64_t)PyFloat_AsDouble(l_pIdentifier);
				Py_CLEAR(l_pIdentifier);

				//New reference
				PyObject* l_pDate   = PyObject_GetAttrString(l_pOVStimulation, "date");
				uint64_t l_ui64Date = ITimeArithmetics::secondsToTime(PyFloat_AsDouble(l_pDate));
				Py_CLEAR(l_pDate);

				//New reference
				PyObject* l_pDuration   = PyObject_GetAttrString(l_pOVStimulation, "duration");
				uint64_t l_ui64Duration = ITimeArithmetics::secondsToTime(PyFloat_AsDouble(l_pDuration));
				Py_CLEAR(l_pDuration);

				l_pStimulationSet->appendStimulation(l_ui64Identifier, l_ui64Date, l_ui64Duration);
			}

			//New reference
			PyObject* l_pStartTime = PyObject_GetAttrString(l_pOVChunk, "startTime");
			uint64_t start         = ITimeArithmetics::secondsToTime(PyFloat_AsDouble(l_pStartTime));
			Py_CLEAR(l_pStartTime);

			//New reference
			PyObject* l_pEndTime = PyObject_GetAttrString(l_pOVChunk, "endTime");
			uint64_t end         = ITimeArithmetics::secondsToTime(PyFloat_AsDouble(l_pEndTime));
			Py_CLEAR(l_pEndTime);

			m_vEncoders[index]->encodeBuffer();
			boxCtx.markOutputAsReadyToSend(index, start, end);
		}

		else if (PyObject_IsInstance(l_pOVChunk, m_stimulationEnd) == 1)
		{
			//New reference
			PyObject* l_pStartTime = PyObject_GetAttrString(l_pOVChunk, "startTime");
			uint64_t start         = ITimeArithmetics::secondsToTime(PyFloat_AsDouble(l_pStartTime));
			Py_CLEAR(l_pStartTime);

			//New reference
			PyObject* l_pEndTime = PyObject_GetAttrString(l_pOVChunk, "endTime");
			uint64_t end         = ITimeArithmetics::secondsToTime(PyFloat_AsDouble(l_pEndTime));
			Py_CLEAR(l_pEndTime);

			m_vEncoders[index]->encodeEnd();
			boxCtx.markOutputAsReadyToSend(index, start, end);
		}

		else
		{
			this->getLogManager() << LogLevel_Error << "Unexpected object type for item " << chunk_index << " in box.output[" << index << "].\n";
			Py_CLEAR(l_pOVChunk);
			return false;
		}

		Py_CLEAR(l_pOVChunk);
	}

	return true;
}

bool CPolyBox::process()
{
	const IBox& boxCtx = this->getStaticBoxContext();
	CIdentifier typeID;

	for (uint32_t input = 0; input < boxCtx.getInputCount(); input++)
	{
		boxCtx.getInputType(input, typeID);
		if (typeID == OV_TypeId_StreamedMatrix) { if (!transferStreamedMatrixInputChunksToPython(input)) { return false; } }
		else if (typeID == OV_TypeId_Signal) { if (!transferSignalInputChunksToPython(input)) { return false; } }
		else if (typeID == OV_TypeId_Stimulations) { if (!transferStimulationInputChunksToPython(input)) { return false; } }
		else
		{
			this->getLogManager() << LogLevel_Error << "Codec to decode " << typeID.toString() << " is not implemented.\n";
			return false;
		}
	}

	//update the python current time
	m_boxCurrentTime = PyFloat_FromDouble(ITimeArithmetics::timeToSeconds(this->getPlayerContext().getCurrentTime()));
	if (m_boxCurrentTime == nullptr)
	{
		this->getLogManager() << LogLevel_Error << "Failed to convert the current time into a PyFloat during update.\n";
		return false;
	}
	if (PyObject_SetAttrString(m_box, "_currentTime", m_boxCurrentTime) == -1)
	{
		this->getLogManager() << LogLevel_Error << "Failed to update \"box._currentTime\" attribute.\n";
		return false;
	}

	//call the python process function
	if (m_boxProcess && PyCallable_Check(m_boxProcess))
	{
		//New reference
		PyObject* l_pResult       = PyObject_CallObject(m_boxProcess, nullptr);
		bool l_bLogSysStdoutError = logSysStdout();
		bool l_bLogSysStderrError = logSysStderr();
		if ((l_pResult == nullptr) || (!l_bLogSysStdoutError) || (!l_bLogSysStderrError))
		{
			if (l_pResult == nullptr) { this->getLogManager() << LogLevel_Error << "Failed to call \"box.__process\" function.\n"; }
			if (!l_bLogSysStdoutError) { this->getLogManager() << LogLevel_Error << "logSysStdout() failed during box.__process.\n"; }
			if (!l_bLogSysStderrError) { this->getLogManager() << LogLevel_Error << "logSysStderr() failed during box.__process.\n"; }
			Py_CLEAR(l_pResult);
			return false;
		}
		Py_CLEAR(l_pResult);
	}

	for (uint32_t output = 0; output < boxCtx.getOutputCount(); output++)
	{
		boxCtx.getOutputType(output, typeID);
		if (typeID == OV_TypeId_StreamedMatrix) { if (!transferStreamedMatrixOutputChunksFromPython(output)) { return false; } }
		else if (typeID == OV_TypeId_Signal) { if (!transferSignalOutputChunksFromPython(output)) { return false; } }
		else if (typeID == OV_TypeId_Stimulations) { if (!transferStimulationOutputChunksFromPython(output)) { return false; } }
		else
		{
			this->getLogManager() << LogLevel_Error << "Codec to encode " << typeID.toString() << " is not implemented.\n";
			return false;
		}
	}

	return true;
}

//#endif // #if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 2)
#endif // TARGET_HAS_ThirdPartyPython
