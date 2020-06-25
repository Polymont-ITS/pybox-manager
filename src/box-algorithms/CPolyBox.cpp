#if defined TARGET_HAS_ThirdPartyPython3 && !(defined(WIN32) && defined(TARGET_BUILDTYPE_Debug))

#include "CPolyBox.hpp"

#if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 3)

#include <fstream>
#include <iostream>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace /*OpenViBE::*/Toolkit;
using namespace /*OpenViBE::Plugins::*/PyBox;

using namespace std;

#define DICTIONNARY_REFERENCE(obj, ref, error)\
	obj = PyDict_GetItemString(m_mainDictionnary, ref);\
	if (obj == nullptr) { getLogManager() << LogLevel_Error << error; return false; }

//****************************
//***** Static Functions *****
//****************************
///-------------------------------------------------------------------------------------------------
static bool appendToPyObject(PyObject* obj, PyObject* buffer)
{
	PyObject* methodToCall = PyUnicode_FromString("append");
	PyObject* result       = PyObject_CallMethodObjArgs(buffer, methodToCall, obj, NULL);
	Py_CLEAR(methodToCall);
	if (result == nullptr) { return false; }
	Py_CLEAR(result);
	return true;
}

static bool getLenFromPyObject(PyObject* obj, size_t& len)
{
	PyObject* pyLen = PyObject_CallMethod(obj, "__len__", nullptr);
	if (pyLen == nullptr) { return false; }
	len = size_t(PyLong_AsUnsignedLongMask(pyLen));
	Py_CLEAR(pyLen);
	return true;
}

static void getTimeFromPyObject(PyObject* obj, const char* attr, CTime& time)
{
	PyObject* pyTime = PyObject_GetAttrString(obj, attr);
	time             = CTime(PyFloat_AsDouble(pyTime));
	Py_CLEAR(pyTime);
}

static void getTimesFromPyObject(PyObject* obj, CTime& start, CTime& end)
{
	getTimeFromPyObject(obj, "startTime", start);
	getTimeFromPyObject(obj, "endTime", end);
}

static bool setMatrixInfosFromPyObject(PyObject* obj, CMatrix* matrix)
{
	PyObject* pyNDim = PyObject_CallMethod(obj, "getDimensionCount", nullptr);
	if (pyNDim == nullptr) { return false; }

	const size_t nDim = PyLong_AsUnsignedLongMask(pyNDim);
	matrix->setDimensionCount(nDim);
	Py_CLEAR(pyNDim);

	PyObject* pySizeDim  = PyObject_GetAttrString(obj, "dimensionSizes");
	PyObject* pyLabelDim = PyObject_GetAttrString(obj, "dimensionLabels");

	size_t offset = 0;
	for (size_t i = 0; i < nDim; ++i)
	{
		const size_t size = PyLong_AsUnsignedLongMask(PyList_GetItem(pySizeDim, Py_ssize_t(i)));
		matrix->setDimensionSize(i, size);
		for (size_t j = 0; j < size; ++j) { matrix->setDimensionLabel(i, j, PyBytes_AS_STRING(PyList_GetItem(pyLabelDim, offset + j))); }
		offset = offset + size;
	}
	Py_CLEAR(pySizeDim);
	Py_CLEAR(pyLabelDim);
	return true;
}
///-------------------------------------------------------------------------------------------------

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
PyObject* CPolyBox::m_stdout            = nullptr;
PyObject* CPolyBox::m_stderr            = nullptr;

bool CPolyBox::logSysStd(const bool out)
{
	PyObject* pyLog = PyObject_CallMethod((out ? m_stdout : m_stderr), "getvalue", nullptr);
	if (pyLog == nullptr)
	{
		getLogManager() << LogLevel_Error << "Failed to call sys.std" << (out ? "out" : "err") << ".getvalue().\n";
		return false;
	}

	char* log = PyBytes_AS_STRING(PyUnicode_AsEncodedString(pyLog, "utf-8", "strict"));
	if (log == nullptr)
	{
		getLogManager() << LogLevel_Error << "Failed to convert pyLog to (char *).\n";
		Py_CLEAR(pyLog);
		return false;
	}
	Py_CLEAR(pyLog);

	if (strlen(log) > 0)
	{
		getLogManager() << LogLevel_Info << log;
		PyObject* result = PyObject_CallMethod((out ? m_stdout : m_stderr), "flush", nullptr);
		if (result == nullptr)
		{
			getLogManager() << LogLevel_Error << "Failed to call sys.std" << (out ? "out" : "err") << ".flush().\n";
			return false;
		}
		Py_CLEAR(result);
	}
	return true;
}

void CPolyBox::buildPythonSettings()
{
	const IBox* boxCtx = getBoxAlgorithmContext()->getStaticBoxContext();
	for (uint32_t i = 1; i < boxCtx->getSettingCount(); ++i)
	{
		CString name;
		boxCtx->getSettingName(i, name);
		const CString value = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), i);
		PyDict_SetItemString(m_boxSetting, name.toASCIIString(), PyUnicode_FromString(value.toASCIIString()));
	}
}

bool CPolyBox::initializePythonSafely()
{
	// Only the first Python box does the initialization of the global parts
	if (m_isInitialized) { return true; }

	getLogManager() << LogLevel_Info << "Discovered Python is " << Py_GetVersion() << " (" << Py_GetPlatform() << ")\n";
	getLogManager() << LogLevel_Debug << "The Python path is [" << Py_GetPath() << "]\n";

	const string cmd = string("import sys\nsys.path.append('") + Directories::getDataDir().toASCIIString() + "/plugins/python3')\nsys.argv = [\"openvibe\"]\n";
	getLogManager() << LogLevel_Trace << "Running [\n" << cmd << "\n].\n";

	PyRun_SimpleString(cmd.c_str());

	//Borrowed reference
	m_mainModule      = PyImport_AddModule("__main__");
	m_mainDictionnary = PyModule_GetDict(m_mainModule);

	// Open & Run Base
	//****************************************
	const string path = string(Directories::getDataDir().toASCIIString()) + "/plugins/python3/PolyBox.py";
	ifstream file;
	file.open(path);
	if (!file.is_open())
	{
		getLogManager() << LogLevel_Error << "Failed to open '" << path << "'.\n";
		return false;
	}
	const std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	if (PyRun_SimpleString(str.c_str()) == -1)	// Yolo but PyRun_File crash
	{
		getLogManager() << LogLevel_Error << "Failed to run '" << path << "'.\n";
		return false;
	}
	file.close();
	//****************************************

	// Apply References
	//****************************************
	m_stdout = PySys_GetObject("stdout");
	if (m_stdout == nullptr)
	{
		getLogManager() << LogLevel_Error << "sys.stdout does not exist.\n";
		return false;
	}
	m_stderr = PySys_GetObject("stderr");
	if (m_stderr == nullptr)
	{
		getLogManager() << LogLevel_Error << "sys.stderr does not exist.\n";
		return false;
	}

	DICTIONNARY_REFERENCE(m_execFileFunction, "execfileHandlingException", "PolyBox.py doesn't have a execfileHandlingException function, quitting.\n")

	if (!PyCallable_Check(m_execFileFunction))
	{
		getLogManager() << LogLevel_Error << "PolyBox.py doesn't have a execfileHandlingException function callable.\n";
		return false;
	}

	//Streams
	DICTIONNARY_REFERENCE(m_matrixHeader, "OVStreamedMatrixHeader", "Failed to load class \"OVStreamedMatrixHeader\".\n")
	DICTIONNARY_REFERENCE(m_matrixBuffer, "OVStreamedMatrixBuffer", "Failed to load class \"OVStreamedMatrixBuffer\".\n")
	DICTIONNARY_REFERENCE(m_matrixEnd, "OVStreamedMatrixEnd", "Failed to load class \"OVStreamedMatrixEnd\".\n")

	DICTIONNARY_REFERENCE(m_signalHeader, "OVSignalHeader", "Failed to load class \"OVSignalHeader\".\n")
	DICTIONNARY_REFERENCE(m_signalBuffer, "OVSignalBuffer", "Failed to load class \"OVSignalBuffer\".\n")
	DICTIONNARY_REFERENCE(m_signalEnd, "OVSignalEnd", "Failed to load class \"OVSignalEnd\".\n")

	DICTIONNARY_REFERENCE(m_stimulationHeader, "OVStimulationHeader", "Failed to load class \"OVStimulationHeader\".\n")
	DICTIONNARY_REFERENCE(m_stimulation, "OVStimulation", "Failed to load class \"OVStimulation\".\n")
	DICTIONNARY_REFERENCE(m_stimulationSet, "OVStimulationSet", "Failed to load class \"OVStimulationSet\".\n")
	DICTIONNARY_REFERENCE(m_stimulationEnd, "OVStimulationEnd", "Failed to load class \"OVStimulationEnd\".\n")

	DICTIONNARY_REFERENCE(m_buffer, "OVBuffer", "Failed to load class \"OVBuffer\".\n")

	m_isInitialized = true;
	getLogManager() << LogLevel_Info << "Python Interpreter initialized\n";
	return true;
}

bool CPolyBox::initialize()
{
	m_box                 = nullptr;
	m_boxInput            = nullptr;
	m_boxOutput           = nullptr;
	m_boxTime             = nullptr;
	m_boxSetting          = nullptr;
	m_boxInitialize       = nullptr;
	m_boxProcess          = nullptr;
	m_boxUninitialize     = nullptr;
	m_initializeSucceeded = false;

	if (!initializePythonSafely()) { return false; }

	//Initialize the clock frequency of the box depending on the first setting of the box
	m_clockFrequency = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);

	if (strlen(m_script.toASCIIString()) == 0)
	{
		getLogManager() << LogLevel_Error << "You have to choose a script.\n";
		return false;
	}

	//Create the decoders for the inputs
	const IBox& boxCtx = this->getStaticBoxContext();
	CIdentifier typeID;
	for (size_t i = 0; i < boxCtx.getInputCount(); ++i)
	{
		boxCtx.getInputType(i, typeID);
		if (typeID == OV_TypeId_StreamedMatrix) { m_decoders.push_back(new TStreamedMatrixDecoder<CPolyBox>(*this, i)); }
		else if (typeID == OV_TypeId_Signal) { m_decoders.push_back(new TSignalDecoder<CPolyBox>(*this, i)); }
		else if (typeID == OV_TypeId_FeatureVector) { m_decoders.push_back(new TFeatureVectorDecoder<CPolyBox>(*this, i)); }
		else if (typeID == OV_TypeId_Spectrum) { m_decoders.push_back(new TSpectrumDecoder<CPolyBox>(*this, i)); }
		else if (typeID == OV_TypeId_ChannelLocalisation) { m_decoders.push_back(new TChannelLocalisationDecoder<CPolyBox>(*this, i)); }
		else if (typeID == OV_TypeId_Stimulations) { m_decoders.push_back(new TStimulationDecoder<CPolyBox>(*this, i)); }
		else if (typeID == OV_TypeId_ExperimentInfo) { m_decoders.push_back(new TExperimentInfoDecoder<CPolyBox>(*this, i)); }
		else
		{
			getLogManager() << LogLevel_Error << "Codec to decode " << typeID.str() << " is not implemented.\n";
			return false;
		}
	}

	//Create the encoders for the outputs
	for (size_t i = 0; i < boxCtx.getOutputCount(); ++i)
	{
		boxCtx.getOutputType(i, typeID);
		if (typeID == OV_TypeId_StreamedMatrix) { m_encoders.push_back(new TStreamedMatrixEncoder<CPolyBox>(*this, i)); }
		else if (typeID == OV_TypeId_Signal) { m_encoders.push_back(new TSignalEncoder<CPolyBox>(*this, i)); }
		else if (typeID == OV_TypeId_FeatureVector) { m_encoders.push_back(new TFeatureVectorEncoder<CPolyBox>(*this, i)); }
		else if (typeID == OV_TypeId_Spectrum) { m_encoders.push_back(new TSpectrumEncoder<CPolyBox>(*this, i)); }
		else if (typeID == OV_TypeId_ChannelLocalisation) { m_encoders.push_back(new TChannelLocalisationEncoder<CPolyBox>(*this, i)); }
		else if (typeID == OV_TypeId_Stimulations) { m_encoders.push_back(new TStimulationEncoder<CPolyBox>(*this, i)); }
		else if (typeID == OV_TypeId_ExperimentInfo) { m_encoders.push_back(new TExperimentInfoEncoder<CPolyBox>(*this, i)); }
		else
		{
			getLogManager() << LogLevel_Error << "Codec to encode " << typeID.str() << " is not implemented.\n";
			return false;
		}
	}

	PyObject* pyTmp = Py_BuildValue("s,O", m_script.toASCIIString(), m_mainDictionnary);
	if (pyTmp == nullptr) { getLogManager() << LogLevel_Error << "Failed to load [" << m_script << "]"; }

	PyObject* result = PyObject_CallObject(m_execFileFunction, pyTmp);
	if (result == nullptr || PyLong_AsLong(result) != 0)
	{
		getLogManager() << LogLevel_Error << "Failed to run [" << m_script << "], result = "
				<< (result ? std::to_string(PyLong_AsLong(result)) : "NULL") << "\n";
		logSysStdout();
		logSysStderr();
		Py_CLEAR(pyTmp);
		Py_CLEAR(result);
		return false;
	}

	Py_CLEAR(pyTmp);
	Py_CLEAR(result);

	m_box = PyObject_GetAttrString(m_mainModule, "box"); // la box qui vient juste d'etre creee
	if (m_box == nullptr)
	{
		getLogManager() << LogLevel_Error << "Failed to load \"box\" object.\n";
		return false;
	}

	m_boxInput = PyObject_GetAttrString(m_box, "input");
	if (m_boxInput == nullptr)
	{
		getLogManager() << LogLevel_Error << "Failed to load \"box.input\".\n";
		return false;
	}

	m_boxOutput = PyObject_GetAttrString(m_box, "output");
	if (m_boxOutput == nullptr)
	{
		getLogManager() << LogLevel_Error << "Failed to load \"box.output\".\n";
		return false;
	}

	std::string type;
	for (size_t i = 0; i < boxCtx.getInputCount(); ++i)
	{
		boxCtx.getInputType(i, typeID);
		if (typeID == OV_TypeId_StreamedMatrix) { type = "StreamedMatrix"; }
		else if (typeID == OV_TypeId_Signal) { type = "Signal"; }
		else if (typeID == OV_TypeId_FeatureVector) { type = "FeatureVector"; }
		else if (typeID == OV_TypeId_Spectrum) { type = "Spectrum"; }
		else if (typeID == OV_TypeId_ChannelLocalisation) { type = "ChannelLocalisation"; }
		else if (typeID == OV_TypeId_Stimulations) { type = "Stimulations"; }
		else if (typeID == OV_TypeId_ExperimentInfo) { type = "ExperimentInfo"; }
		PyObject* res = PyObject_CallMethod(m_box, "addInput", "s", type.c_str());
		if (res == nullptr)
		{
			getLogManager() << LogLevel_Error << "Failed to call box.addInput().\n";
			return false;
		}
		Py_CLEAR(res);
	}

	for (size_t i = 0; i < boxCtx.getOutputCount(); ++i)
	{
		boxCtx.getOutputType(i, typeID);
		if (typeID == OV_TypeId_StreamedMatrix) { type = "StreamedMatrix"; }
		else if (typeID == OV_TypeId_Signal) { type = "Signal"; }
		else if (typeID == OV_TypeId_FeatureVector) { type = "FeatureVector"; }
		else if (typeID == OV_TypeId_Spectrum) { type = "Spectrum"; }
		else if (typeID == OV_TypeId_ChannelLocalisation) { type = "ChannelLocalisation"; }
		else if (typeID == OV_TypeId_Stimulations) { type = "Stimulations"; }
		else if (typeID == OV_TypeId_ExperimentInfo) { type = "ExperimentInfo"; }
		PyObject* res = PyObject_CallMethod(m_box, "addOutput", "s", type.c_str());
		if (res == nullptr)
		{
			getLogManager() << LogLevel_Error << "Failed to call box.addOutput().\n";
			return false;
		}
		Py_CLEAR(res);
	}

	m_boxSetting = PyObject_GetAttrString(m_box, "setting");
	if (m_boxSetting == nullptr)
	{
		getLogManager() << LogLevel_Error << "Failed to load \"box.setting\".\n";
		return false;
	}
	buildPythonSettings();

	if (!PyObject_HasAttrString(m_box, "_clock"))
	{
		getLogManager() << LogLevel_Error << "Failed to initialize \"box._clock\" attribute because it does not exist.\n";
		return false;
	}

	PyObject* pyBoxClock = PyLong_FromLong(long(m_clockFrequency));
	if (pyBoxClock == nullptr)
	{
		getLogManager() << LogLevel_Error << "Failed to convert m_ClockFrequency into PyInt.\n";
		return false;
	}
	if (PyObject_SetAttrString(m_box, "_clock", pyBoxClock) == -1)
	{
		getLogManager() << LogLevel_Error << "Failed to initialize \"box._clock\" attribute.\n";
		return false;
	}
	Py_CLEAR(pyBoxClock);

	if (!PyObject_HasAttrString(m_box, "_currentTime"))
	{
		getLogManager() << LogLevel_Error << "Failed to initialize \"box._currentTime\" attribute because it does not exist.\n";
		return false;
	}

	m_boxTime = PyFloat_FromDouble(getPlayerContext().getCurrentTime().toSeconds());
	if (m_boxTime == nullptr)
	{
		getLogManager() << LogLevel_Error << "Failed to convert the current time into a PyFloat.\n";
		return false;
	}
	if (PyObject_SetAttrString(m_box, "_currentTime", m_boxTime) == -1)
	{
		getLogManager() << LogLevel_Error << "Failed to initialize \"box._currentTime\" attribute.\n";
		return false;
	}

	if (!PyObject_HasAttrString(m_box, "realInitialize"))
	{
		getLogManager() << LogLevel_Error << "No realInitialize.\n";
		return false;
	}

	m_boxInitialize = PyObject_GetAttrString(m_box, "realInitialize");
	if (m_boxInitialize == nullptr) { getLogManager() << LogLevel_ImportantWarning << "Failed to load \"box.realInitialize\" function.\n"; }
	else if (!PyCallable_Check(m_boxInitialize)) { getLogManager() << LogLevel_ImportantWarning << "\"box.realInitialize\" is not callable.\n"; }

	m_boxProcess = PyObject_GetAttrString(m_box, "realProcess");
	if (m_boxProcess == nullptr) { getLogManager() << LogLevel_ImportantWarning << "Failed to load \"box.realProcess\" function.\n"; }
	else if (!PyCallable_Check(m_boxProcess)) { getLogManager() << LogLevel_ImportantWarning << "\"box.realProcess\" is not callable.\n"; }

	m_boxUninitialize = PyObject_GetAttrString(m_box, "realUninitialize");
	if (m_boxUninitialize == nullptr) { getLogManager() << LogLevel_ImportantWarning << "Failed to load \"box.realUninitialize\" function.\n"; }
	else if (!PyCallable_Check(m_boxUninitialize)) { getLogManager() << LogLevel_ImportantWarning << "\"box.realUninitialize\" is not callable.\n"; }

	//Execute the initialize function defined in the python user script
	if (m_boxInitialize && PyCallable_Check(m_boxInitialize))
	{
		PyObject* res          = PyObject_CallObject(m_boxInitialize, nullptr);
		const bool stdoutError = logSysStdout(); // souci car la si l'init plante pas de sortie au bon endroit
		const bool stderrError = logSysStderr();
		if ((res == nullptr) || (!stdoutError) || (!stderrError))
		{
			if (res == nullptr) { getLogManager() << LogLevel_Error << "Failed to call \"box.__initialize\" function.\n"; }
			if (!stdoutError) { getLogManager() << LogLevel_Error << "logSysStdout() failed during box.__initialization.\n"; }
			if (!stderrError) { getLogManager() << LogLevel_Error << "logSysStderr() failed during box.__initialization.\n"; }
			Py_CLEAR(res);
			return false;
		}
		Py_CLEAR(res);
	}

	m_initializeSucceeded = true;
	return true;
}

bool CPolyBox::uninitialize()
{
	for (size_t i = 0; i < m_decoders.size(); ++i)
	{
		m_decoders[i]->uninitialize();
		delete m_decoders[i];
	}
	m_decoders.clear();

	for (size_t i = 0; i < m_encoders.size(); ++i)
	{
		m_encoders[i]->uninitialize();
		delete m_encoders[i];
	}
	m_encoders.clear();

	if (m_initializeSucceeded)	// we call this uninit only if init had succeeded Execute the uninitialize function defined in the python script
	{	// il y a un souci ici si le script n'a pas ete charge ca ne passe pas
		if (m_boxUninitialize && PyCallable_Check(m_boxUninitialize))
		{
			PyObject* result       = PyObject_CallObject(m_boxUninitialize, nullptr);
			const bool stdoutError = logSysStdout();
			const bool stderrError = logSysStderr();
			if ((result == nullptr) || (!stdoutError) || (!stderrError))
			{
				if (result == nullptr) { getLogManager() << LogLevel_Error << "Failed to call \"box.__uninitialize\" function.\n"; }
				if (!stdoutError) { getLogManager() << LogLevel_Error << "logSysStdout() failed during box.__uninitialization.\n"; }
				if (!stderrError) { getLogManager() << LogLevel_Error << "logSysStderr() failed during box.__uninitialization.\n"; }
				Py_CLEAR(result);
				return false;
			}
			Py_CLEAR(result);
		}
	}

	// Note: Py_CLEAR is safe to use on NULL pointers, so we can clean everything here
	Py_CLEAR(m_box);
	Py_CLEAR(m_boxInput);
	Py_CLEAR(m_boxOutput);
	Py_CLEAR(m_boxTime);
	Py_CLEAR(m_boxSetting);
	Py_CLEAR(m_boxInitialize);
	Py_CLEAR(m_boxProcess);
	Py_CLEAR(m_boxUninitialize);

	// Py_Initialize() and Py_Finalize() are called in ovp_main.cpp, we never uninitialize Python here

	return true;
}

bool CPolyBox::processClock(CMessage& /*msg*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CPolyBox::processInput(const size_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CPolyBox::transferStreamedMatrixInputChunksToPython(const size_t index)
{
	IBoxIO& boxCtx = this->getDynamicBoxContext();

	if (!PyList_Check(m_boxInput))
	{
		getLogManager() << LogLevel_Error << "box.input must be a list.\n";
		return false;
	}

	PyObject* pyBuffer = PyList_GetItem(m_boxInput, Py_ssize_t(index));
	if (pyBuffer == nullptr)
	{
		getLogManager() << LogLevel_Error << "Failed to get box.input[" << index << "].\n";
		return false;
	}
	//Expose input streamed matrix chunks to python
	for (size_t idx = 0; idx < boxCtx.getInputChunkCount(index); ++idx)
	{
		m_decoders[index]->decode(idx);

		if (m_decoders[index]->isHeaderReceived())
		{
			CMatrix* matrix = dynamic_cast<TStreamedMatrixDecoder<CPolyBox>*>(m_decoders[index])->getOutputMatrix();
			size_t nDim     = matrix->getDimensionCount();

			PyObject* pySizeDim = PyList_New(nDim);
			if (pySizeDim == nullptr)
			{
				getLogManager() << LogLevel_Error << "Failed to create a new list pySizeDim.\n";
				return false;
			}

			PyObject* pyLabelDim = PyList_New(0);
			if (pyLabelDim == nullptr)
			{
				getLogManager() << LogLevel_Error << "Failed to create a new list pyLabelDim.\n";
				Py_CLEAR(pySizeDim);
				return false;
			}

			for (size_t i = 0; i < nDim; ++i)
			{
				size_t dimSize = matrix->getDimensionSize(i);
				if (PyList_SetItem(pySizeDim, i, PyLong_FromLong(dimSize)) == -1)
				{
					getLogManager() << LogLevel_Error << "Failed to set item " << i << " in dimension size list.\n";
					Py_CLEAR(pySizeDim);
					Py_CLEAR(pyLabelDim);
					return false;
				}
				for (size_t j = 0; j < dimSize; ++j)
				{
					if (PyList_Append(pyLabelDim, PyUnicode_FromString(matrix->getDimensionLabel(i, j).c_str())) == -1)
					{
						getLogManager() << LogLevel_Error << "Failed to append \"" << matrix->getDimensionLabel(i, j) << "\" in dimension label list.\n";
						Py_CLEAR(pySizeDim);
						Py_CLEAR(pyLabelDim);
						return false;
					}
				}
			}

			PyObject* pyArg = PyTuple_New(4);
			if (pyArg == nullptr)
			{
				getLogManager() << LogLevel_Error << "Failed to create a new tuple pyArg.\n";
				Py_CLEAR(pySizeDim);
				Py_CLEAR(pyLabelDim);
				return false;
			}
			if (PyTuple_SetItem(pyArg, 0, PyFloat_FromDouble(boxCtx.getInputChunkStartTime(index, idx).toSeconds())) != 0)
			{
				getLogManager() << LogLevel_Error << "Failed to set item 0 (start time) in tuple pyArg.\n";
				Py_CLEAR(pySizeDim);
				Py_CLEAR(pyLabelDim);
				Py_CLEAR(pyArg);
				return false;
			}
			if (PyTuple_SetItem(pyArg, 1, PyFloat_FromDouble(boxCtx.getInputChunkEndTime(index, idx).toSeconds())) != 0)
			{
				getLogManager() << LogLevel_Error << "Failed to set item 1 (end time) in tuple pyArg.\n";
				Py_CLEAR(pySizeDim);
				Py_CLEAR(pyLabelDim);
				Py_CLEAR(pyArg);
				return false;
			}
			if (PyTuple_SetItem(pyArg, 2, pySizeDim) != 0)
			{
				getLogManager() << LogLevel_Error << "Failed to set item 2 (dimension size) in tuple pyArg.\n";
				Py_CLEAR(pySizeDim);
				Py_CLEAR(pyLabelDim);
				Py_CLEAR(pyArg);
				return false;
			}
			if (PyTuple_SetItem(pyArg, 3, pyLabelDim) != 0)
			{
				getLogManager() << LogLevel_Error << "Failed to set item 3 (dimension label) in tuple pyArg.\n";
				Py_CLEAR(pySizeDim);
				Py_CLEAR(pyLabelDim);
				Py_CLEAR(pyArg);
				return false;
			}

			PyObject* pyMatrixHeader = PyObject_Call(m_matrixHeader, pyArg, nullptr);
			if (pyMatrixHeader == nullptr)
			{
				getLogManager() << LogLevel_Error << "Failed to create a new OVStreamedMatrixHeader pyMatrixHeader.\n";
				Py_CLEAR(pySizeDim);
				Py_CLEAR(pyLabelDim);
				Py_CLEAR(pyArg);
				return false;
			}
			Py_CLEAR(pySizeDim);
			Py_CLEAR(pyLabelDim);
			Py_CLEAR(pyArg);

			if (!appendToPyObject(pyMatrixHeader, pyBuffer))
			{
				getLogManager() << LogLevel_Error << "Failed to append an OVStreamedMatrixHeader to box.input[" << index << "].\n";
				Py_CLEAR(pyMatrixHeader);
				return false;
			}
			Py_CLEAR(pyMatrixHeader);
		}

		if (m_decoders[index]->isBufferReceived())
		{
			PyObject* pyArg = PyTuple_New(3);
			if (pyArg == nullptr)
			{
				getLogManager() << LogLevel_Error << "Failed to create a new tuple pyArg.\n";
				return false;
			}
			if (PyTuple_SetItem(pyArg, 0, PyFloat_FromDouble(boxCtx.getInputChunkStartTime(index, idx).toSeconds())) != 0)
			{
				getLogManager() << LogLevel_Error << "Failed to set item 0 (start time) in tuple pyArg.\n";
				Py_CLEAR(pyArg);
				return false;
			}
			if (PyTuple_SetItem(pyArg, 1, PyFloat_FromDouble(boxCtx.getInputChunkEndTime(index, idx).toSeconds())) != 0)
			{
				getLogManager() << LogLevel_Error << "Failed to set item 1 (end time) in tuple pyArg.\n";
				Py_CLEAR(pyArg);
				return false;
			}
			if (PyTuple_SetItem(pyArg, 2, PyList_New(0)) != 0)
			{
				getLogManager() << LogLevel_Error << "Failed to set item 2 (bufferElements) in tuple pyArg.\n";
				Py_CLEAR(pyArg);
				return false;
			}

			PyObject* pyMatrixBuffer = PyObject_Call(m_matrixBuffer, pyArg, nullptr);
			if (pyMatrixBuffer == nullptr)
			{
				getLogManager() << LogLevel_Error << "Failed to create a new OVStreamedMatrixBuffer pyMatrixBuffer.\n";
				Py_CLEAR(pyArg);
				return false;
			}
			Py_CLEAR(pyArg);

			CMatrix* matrix    = dynamic_cast<TStreamedMatrixDecoder<CPolyBox>*>(m_decoders[index])->getOutputMatrix();
			double* bufferBase = matrix->getBuffer();
			for (size_t i = 0; i < matrix->getSize(); ++i)
			{
				if (PyList_Append(pyMatrixBuffer, PyFloat_FromDouble(bufferBase[i])) == -1)
				{
					getLogManager() << LogLevel_Error << "Failed to append element " << i << " to pyMatrixBuffer.\n";
					Py_CLEAR(pyMatrixBuffer);
					return false;
				}
			}

			if (!appendToPyObject(pyMatrixBuffer, pyBuffer))
			{
				getLogManager() << LogLevel_Error << "Failed to append an OVStreamedMatrixBuffer to box.input[" << index << "].\n";
				Py_CLEAR(pyMatrixBuffer);
				return false;
			}
			Py_CLEAR(pyMatrixBuffer);
		}

		if (m_decoders[index]->isEndReceived())
		{
			PyObject* pyArg = PyTuple_New(2);
			if (pyArg == nullptr)
			{
				getLogManager() << LogLevel_Error << "Failed to create a new tuple pyArg.\n";
				return false;
			}
			if (PyTuple_SetItem(pyArg, 0, PyFloat_FromDouble(boxCtx.getInputChunkStartTime(index, idx).toSeconds())) != 0)
			{
				getLogManager() << LogLevel_Error << "Failed to set item 0 (start time) in tuple pyArg.\n";
				Py_CLEAR(pyArg);
				return false;
			}
			if (PyTuple_SetItem(pyArg, 1, PyFloat_FromDouble(boxCtx.getInputChunkEndTime(index, idx).toSeconds())) != 0)
			{
				getLogManager() << LogLevel_Error << "Failed to set item 1 (end time) in tuple pyArg.\n";
				Py_CLEAR(pyArg);
				return false;
			}

			PyObject* pyMatrixEnd = PyObject_Call(m_matrixEnd, pyArg, nullptr);
			if (pyMatrixEnd == nullptr)
			{
				getLogManager() << LogLevel_Error << "Failed to create a new OVStreamedMatrixEnd pyMatrixEnd.\n";
				Py_CLEAR(pyArg);
				return false;
			}
			Py_CLEAR(pyArg);

			if (!appendToPyObject(pyMatrixEnd, pyBuffer))
			{
				getLogManager() << LogLevel_Error << "Failed to append an OVStreamedMatrixEnd to box.input[" << index << "].\n";
				Py_CLEAR(pyMatrixEnd);
				return false;
			}
			Py_CLEAR(pyMatrixEnd);
		}
	}

	return true;
}

bool CPolyBox::transferStreamedMatrixOutputChunksFromPython(const size_t index)
{
	IBoxIO& boxCtx  = this->getDynamicBoxContext();
	CMatrix* matrix = dynamic_cast<TStreamedMatrixEncoder<CPolyBox>*>(m_encoders[index])->getInputMatrix();

	if (!PyList_Check(m_boxOutput))
	{
		getLogManager() << LogLevel_Error << "box.output must be a list.\n";
		return false;
	}

	PyObject* pyBuffer = PyList_GetItem(m_boxOutput, Py_ssize_t(index));
	if (pyBuffer == nullptr)
	{
		getLogManager() << LogLevel_Error << "Failed to get box.output[" << index << "].\n";
		return false;
	}

	size_t len;
	if (!getLenFromPyObject(pyBuffer, len))
	{
		getLogManager() << LogLevel_Error << "Failed to get box.output[" << index << "].__len__().\n";
		return false;
	}

	for (size_t idx = 0; idx < len; ++idx)
	{
		PyObject* pyChunk = PyObject_CallMethod(pyBuffer, "pop", nullptr);
		if (pyChunk == nullptr)
		{
			getLogManager() << LogLevel_Error << "Failed to get item " << idx << " of box.output[" << index << "].\n";
			return false;
		}

		if (PyObject_IsInstance(pyChunk, m_matrixHeader) == 1)
		{
			if (!setMatrixInfosFromPyObject(pyChunk, matrix))
			{
				getLogManager() << LogLevel_Error << "Failed to call chunk's getDimensionCount method.\n";
				Py_CLEAR(pyChunk);
				return false;
			}

			m_encoders[index]->encodeHeader();

			CTime startTime, endTime;
			getTimesFromPyObject(pyChunk, startTime, endTime);

			boxCtx.markOutputAsReadyToSend(index, startTime, endTime);
		}

		else if (PyObject_IsInstance(pyChunk, m_matrixBuffer) == 1)
		{
			double* bufferBase = matrix->getBuffer();
			for (size_t i = 0; i < matrix->getSize(); ++i) { bufferBase[i] = PyFloat_AsDouble(PyList_GetItem(pyChunk, i)); }

			CTime startTime, endTime;
			getTimesFromPyObject(pyChunk, startTime, endTime);

			m_encoders[index]->encodeBuffer();
			boxCtx.markOutputAsReadyToSend(index, startTime, endTime);
		}

		else if (PyObject_IsInstance(pyChunk, m_matrixEnd) == 1)
		{
			CTime startTime, endTime;
			getTimesFromPyObject(pyChunk, startTime, endTime);

			m_encoders[index]->encodeEnd();
			boxCtx.markOutputAsReadyToSend(index, startTime, endTime);
		}

		else
		{
			getLogManager() << LogLevel_Error << "Unexpected object type for item " << idx << " in box.output[" << index << "].\n";
			Py_CLEAR(pyChunk);
			return false;
		}

		Py_CLEAR(pyChunk);
	}
	return true;
}

bool CPolyBox::transferSignalInputChunksToPython(const size_t index)
{
	IBoxIO& boxCtx = this->getDynamicBoxContext();

	if (!PyList_Check(m_boxInput))
	{
		getLogManager() << LogLevel_Error << "box.input must be a list.\n";
		return false;
	}

	PyObject* pyBuffer = PyList_GetItem(m_boxInput, Py_ssize_t(index));
	if (pyBuffer == nullptr)
	{
		getLogManager() << LogLevel_Error << "Failed to get box.input[" << index << "].\n";
		return false;
	}
	//Expose input signal chunks to python
	for (size_t idx = 0; idx < boxCtx.getInputChunkCount(index); ++idx)
	{
		m_decoders[index]->decode(idx);

		if (m_decoders[index]->isHeaderReceived())
		{
			CMatrix* matrix = dynamic_cast<TSignalDecoder<CPolyBox>*>(m_decoders[index])->getOutputMatrix();
			size_t nDim     = matrix->getDimensionCount();

			PyObject* pySizeDim = PyList_New(nDim);
			if (pySizeDim == nullptr)
			{
				getLogManager() << LogLevel_Error << "Failed to create a new list pySizeDim.\n";
				return false;
			}

			PyObject* pyLabelDim = PyList_New(0);
			if (pyLabelDim == nullptr)
			{
				getLogManager() << LogLevel_Error << "Failed to create a new list pyLabelDim.\n";
				Py_CLEAR(pySizeDim);
				return false;
			}

			for (size_t i = 0; i < nDim; ++i)
			{
				size_t dimSize = matrix->getDimensionSize(i);
				if (PyList_SetItem(pySizeDim, i, PyLong_FromLong(dimSize)) == -1)
				{
					getLogManager() << LogLevel_Error << "Failed to set item " << i << " in dimension size list.\n";
					Py_CLEAR(pySizeDim);
					Py_CLEAR(pyLabelDim);
					return false;
				}
				for (size_t j = 0; j < dimSize; ++j)
				{
					if (PyList_Append(pyLabelDim, PyUnicode_FromString(matrix->getDimensionLabel(i, j).c_str())) == -1)
					{
						getLogManager() << LogLevel_Error << "Failed to append \"" << matrix->getDimensionLabel(i, j) << "\" in dimension label list.\n";
						Py_CLEAR(pySizeDim);
						Py_CLEAR(pyLabelDim);
						return false;
					}
				}
			}

			PyObject* pyArg = PyTuple_New(5);
			if (pyArg == nullptr)
			{
				getLogManager() << LogLevel_Error << "Failed to create a new tuple pyArg.\n";
				Py_CLEAR(pySizeDim);
				Py_CLEAR(pyLabelDim);
				return false;
			}
			if (PyTuple_SetItem(pyArg, 0, PyFloat_FromDouble(boxCtx.getInputChunkStartTime(index, idx).toSeconds())) != 0)
			{
				getLogManager() << LogLevel_Error << "Failed to set item 0 (start time) in tuple pyArg.\n";
				Py_CLEAR(pySizeDim);
				Py_CLEAR(pyLabelDim);
				Py_CLEAR(pyArg);
				return false;
			}
			if (PyTuple_SetItem(pyArg, 1, PyFloat_FromDouble(boxCtx.getInputChunkEndTime(index, idx).toSeconds())) != 0)
			{
				getLogManager() << LogLevel_Error << "Failed to set item 1 (end time) in tuple pyArg.\n";
				Py_CLEAR(pySizeDim);
				Py_CLEAR(pyLabelDim);
				Py_CLEAR(pyArg);
				return false;
			}
			if (PyTuple_SetItem(pyArg, 2, pySizeDim) != 0)
			{
				getLogManager() << LogLevel_Error << "Failed to set item 2 (dimension size) in tuple pyArg.\n";
				Py_CLEAR(pySizeDim);
				Py_CLEAR(pyLabelDim);
				Py_CLEAR(pyArg);
				return false;
			}
			if (PyTuple_SetItem(pyArg, 3, pyLabelDim) != 0)
			{
				getLogManager() << LogLevel_Error << "Failed to set item 3 (dimension label) in tuple pyArg.\n";
				Py_CLEAR(pySizeDim);
				Py_CLEAR(pyLabelDim);
				Py_CLEAR(pyArg);
				return false;
			}
			if (PyTuple_SetItem(
					pyArg, 4, PyLong_FromLong(long(dynamic_cast<TSignalDecoder<CPolyBox>*>(m_decoders[index])->getOutputSamplingRate())))
				!= 0)
			{
				getLogManager() << LogLevel_Error << "Failed to set item 4 (samplingRate) in tuple pyArg.\n";
				Py_CLEAR(pySizeDim);
				Py_CLEAR(pyLabelDim);
				Py_CLEAR(pyArg);
				return false;
			}

			PyObject* pySignalHeader = PyObject_Call(m_signalHeader, pyArg, nullptr);
			if (pySignalHeader == nullptr)
			{
				getLogManager() << LogLevel_Error << "Failed to create a new OVSignalHeader pySignalHeader.\n";
				Py_CLEAR(pySizeDim);
				Py_CLEAR(pyLabelDim);
				Py_CLEAR(pyArg);
				return false;
			}
			Py_CLEAR(pySizeDim);
			Py_CLEAR(pyLabelDim);
			Py_CLEAR(pyArg);

			if (!appendToPyObject(pySignalHeader, pyBuffer))
			{
				getLogManager() << LogLevel_Error << "Failed to append an OVSignalHeader to box.input[" << index << "].\n";
				Py_CLEAR(pySignalHeader);
				return false;
			}
			Py_CLEAR(pySignalHeader);
		}

		if (m_decoders[index]->isBufferReceived())
		{
			PyObject* pyArg = PyTuple_New(3);
			if (pyArg == nullptr)
			{
				getLogManager() << LogLevel_Error << "Failed to create a new tuple pyArg.\n";
				return false;
			}
			if (PyTuple_SetItem(pyArg, 0, PyFloat_FromDouble(boxCtx.getInputChunkStartTime(index, idx).toSeconds())) != 0)
			{
				getLogManager() << LogLevel_Error << "Failed to set item 0 (startTime) in tuple pyArg.\n";
				Py_CLEAR(pyArg);
				return false;
			}
			if (PyTuple_SetItem(pyArg, 1, PyFloat_FromDouble(boxCtx.getInputChunkEndTime(index, idx).toSeconds())) != 0)
			{
				getLogManager() << LogLevel_Error << "Failed to set item 1 (endTime) in tuple pyArg.\n";
				Py_CLEAR(pyArg);
				return false;
			}
			if (PyTuple_SetItem(pyArg, 2, PyList_New(0)) != 0)
			{
				getLogManager() << LogLevel_Error << "Failed to set item 2 (bufferElements) in tuple pyArg.\n";
				Py_CLEAR(pyArg);
				return false;
			}

			PyObject* pySignalBuffer = PyObject_Call(m_signalBuffer, pyArg, nullptr);
			if (pySignalBuffer == nullptr)
			{
				getLogManager() << LogLevel_Error << "Failed to create a new OVSignalBuffer pySignalBuffer.\n";
				Py_CLEAR(pyArg);
				return false;
			}
			Py_CLEAR(pyArg);

			CMatrix* matrix    = dynamic_cast<TSignalDecoder<CPolyBox>*>(m_decoders[index])->getOutputMatrix();
			double* bufferBase = matrix->getBuffer();
			for (size_t i = 0; i < matrix->getSize(); ++i)
			{
				if (PyList_Append(pySignalBuffer, PyFloat_FromDouble(bufferBase[i])) == -1)
				{
					getLogManager() << LogLevel_Error << "Failed to append element " << i << " to pySignalBuffer.\n";
					Py_CLEAR(pySignalBuffer);
					return false;
				}
			}

			if (!appendToPyObject(pySignalBuffer, pyBuffer))
			{
				getLogManager() << LogLevel_Error << "Failed to append an OVSignalBuffer to box.input[" << index << "].\n";
				Py_CLEAR(pySignalBuffer);
				return false;
			}
			Py_CLEAR(pySignalBuffer);
		}

		if (m_decoders[index]->isEndReceived())
		{
			PyObject* pyArg = PyTuple_New(2);
			if (pyArg == nullptr)
			{
				getLogManager() << LogLevel_Error << "Failed to create a new tuple pyArg.\n";
				return false;
			}
			if (PyTuple_SetItem(pyArg, 0, PyFloat_FromDouble(boxCtx.getInputChunkStartTime(index, idx).toSeconds())) != 0)
			{
				getLogManager() << LogLevel_Error << "Failed to set item 0 (start time) in tuple pyArg.\n";
				Py_CLEAR(pyArg);
				return false;
			}
			if (PyTuple_SetItem(pyArg, 1, PyFloat_FromDouble(boxCtx.getInputChunkEndTime(index, idx).toSeconds())) != 0)
			{
				getLogManager() << LogLevel_Error << "Failed to set item 1 (end time) in tuple pyArg.\n";
				Py_CLEAR(pyArg);
				return false;
			}

			PyObject* pySignalEnd = PyObject_Call(m_signalEnd, pyArg, nullptr);
			if (pySignalEnd == nullptr)
			{
				getLogManager() << LogLevel_Error << "Failed to create a new m_signalEnd pySignalEnd.\n";
				Py_CLEAR(pyArg);
				return false;
			}
			Py_CLEAR(pyArg);

			if (!appendToPyObject(pySignalEnd, pyBuffer))
			{
				getLogManager() << LogLevel_Error << "Failed to append an OVSignalEnd to box.input[" << index << "].\n";
				Py_CLEAR(pySignalEnd);
				return false;
			}
			Py_CLEAR(pySignalEnd);
		}
	}
	return true;
}

bool CPolyBox::transferSignalOutputChunksFromPython(const size_t index)
{
	IBoxIO& boxCtx  = this->getDynamicBoxContext();
	CMatrix* matrix = dynamic_cast<TSignalEncoder<CPolyBox>*>(m_encoders[index])->getInputMatrix();

	if (!PyList_Check(m_boxOutput))
	{
		getLogManager() << LogLevel_Error << "box.output must be a list.\n";
		return false;
	}

	PyObject* pyBuffer = PyList_GetItem(m_boxOutput, Py_ssize_t(index));
	if (pyBuffer == nullptr)
	{
		getLogManager() << LogLevel_Error << "Failed to get box.output[" << index << "].\n";
		return false;
	}

	size_t len;
	if (!getLenFromPyObject(pyBuffer, len))
	{
		getLogManager() << LogLevel_Error << "Failed to get box.output[" << index << "].__len__().\n";
		return false;
	}

	for (size_t idx = 0; idx < len; ++idx)
	{
		PyObject* pyChunk = PyObject_CallMethod(pyBuffer, "pop", nullptr);
		if (pyChunk == nullptr)
		{
			getLogManager() << LogLevel_Error << "Failed to get item " << idx << " of box.output[" << index << "].\n";
			return false;
		}

		if (PyObject_IsInstance(pyChunk, m_signalHeader) == 1)
		{
			if (!setMatrixInfosFromPyObject(pyChunk, matrix))
			{
				getLogManager() << LogLevel_Error << "Failed to call chunk's getDimensionCount method.\n";
				Py_CLEAR(pyChunk);
				return false;
			}

			PyObject* pySampling = PyObject_GetAttrString(pyChunk, "samplingRate");
			if (pySampling == nullptr)
			{
				getLogManager() << LogLevel_Error << "Failed to load signal header sampling rate.\n";
				return false;
			}
			TParameterHandler<uint64_t>& sampling = dynamic_cast<TSignalEncoder<CPolyBox>*>(m_encoders[index])->getInputSamplingRate();
			sampling                              = uint64_t(PyLong_AsLong(pySampling));
			m_encoders[index]->encodeHeader();
			Py_CLEAR(pySampling);

			CTime startTime, endTime;
			getTimesFromPyObject(pyChunk, startTime, endTime);

			boxCtx.markOutputAsReadyToSend(index, startTime, endTime);
		}

		else if (PyObject_IsInstance(pyChunk, m_signalBuffer) == 1)
		{
			double* bufferBase = matrix->getBuffer();
			for (size_t i = 0; i < matrix->getSize(); ++i) { bufferBase[i] = PyFloat_AsDouble(PyList_GetItem(pyChunk, i)); }

			CTime startTime, endTime;
			getTimesFromPyObject(pyChunk, startTime, endTime);

			m_encoders[index]->encodeBuffer();
			boxCtx.markOutputAsReadyToSend(index, startTime, endTime);
		}

		else if (PyObject_IsInstance(pyChunk, m_signalEnd) == 1)
		{
			CTime startTime, endTime;
			getTimesFromPyObject(pyChunk, startTime, endTime);

			m_encoders[index]->encodeEnd();
			boxCtx.markOutputAsReadyToSend(index, startTime, endTime);
		}

		else
		{
			getLogManager() << LogLevel_Error << "Unexpected object type for item " << idx << " in box.output[" << index << "].\n";
			Py_CLEAR(pyChunk);
			return false;
		}

		Py_CLEAR(pyChunk);
	}
	return true;
}

bool CPolyBox::transferStimulationInputChunksToPython(const size_t index)
{
	IBoxIO& boxCtx     = this->getDynamicBoxContext();
	PyObject* pyBuffer = PyList_GetItem(m_boxInput, Py_ssize_t(index));
	if (pyBuffer == nullptr)
	{
		getLogManager() << LogLevel_Error << "Failed to get box.input[" << index << "].\n";
		return false;
	}
	for (size_t idx = 0; idx < boxCtx.getInputChunkCount(index); ++idx)
	{
		m_decoders[index]->decode(idx);

		if (m_decoders[index]->isHeaderReceived())
		{
			PyObject* pyArg = PyTuple_New(2);
			if (pyArg == nullptr)
			{
				getLogManager() << LogLevel_Error << "Failed to create a new tuple pyArg.\n";
				return false;
			}
			if (PyTuple_SetItem(pyArg, 0, PyFloat_FromDouble(boxCtx.getInputChunkStartTime(index, idx).toSeconds())) != 0)
			{
				getLogManager() << LogLevel_Error << "Failed to set item 0 (start time) in tuple pyArg.\n";
				Py_CLEAR(pyArg);
				return false;
			}
			if (PyTuple_SetItem(pyArg, 1, PyFloat_FromDouble(boxCtx.getInputChunkEndTime(index, idx).toSeconds())) != 0)
			{
				getLogManager() << LogLevel_Error << "Failed to set item 1 (end time) in tuple pyArg.\n";
				Py_CLEAR(pyArg);
				return false;
			}

			PyObject* pyStimHeader = PyObject_Call(m_stimulationHeader, pyArg, nullptr);
			if (pyStimHeader == nullptr)
			{
				getLogManager() << LogLevel_Error << "Failed to create a new OVStimulationHeader pyStimHeader.\n";
				PyErr_Print();
				Py_CLEAR(pyArg);
				return false;
			}
			Py_CLEAR(pyArg);

			if (!appendToPyObject(pyStimHeader, pyBuffer))
			{
				getLogManager() << LogLevel_Error << "Failed to append an OVStimulationHeader to box.input[" << index << "].\n";
				Py_CLEAR(pyStimHeader);
				return false;
			}
			Py_CLEAR(pyStimHeader);
		}

		if (m_decoders[index]->isBufferReceived())
		{
			CStimulationSet& stimSet = *dynamic_cast<TStimulationDecoder<CPolyBox>*>(m_decoders[index])->getOutputStimulationSet();

			PyObject* pyArg = PyTuple_New(2);
			if (pyArg == nullptr)
			{
				getLogManager() << LogLevel_Error << "Failed to create a new tuple pyArg.\n";
				return false;
			}
			if (PyTuple_SetItem(pyArg, 0, PyFloat_FromDouble(boxCtx.getInputChunkStartTime(index, idx).toSeconds())) != 0)
			{
				getLogManager() << LogLevel_Error << "Failed to set item 0 (start time) in tuple pyArg.\n";
				Py_CLEAR(pyArg);
				return false;
			}
			if (PyTuple_SetItem(pyArg, 1, PyFloat_FromDouble(boxCtx.getInputChunkEndTime(index, idx).toSeconds())) != 0)
			{
				getLogManager() << LogLevel_Error << "Failed to set item 1 (end time) in tuple pyArg.\n";
				Py_CLEAR(pyArg);
				return false;
			}

			PyObject* pyStimSet = PyObject_Call(m_stimulationSet, pyArg, nullptr);
			if (pyStimSet == nullptr)
			{
				getLogManager() << LogLevel_Error << "Failed to create a new OVStimulationSet pyStimSet.\n";
				Py_CLEAR(pyArg);
				return false;
			}
			Py_CLEAR(pyArg);

			for (const auto& stim : stimSet)
			{
				pyArg = PyTuple_New(3);
				if (pyArg == nullptr)
				{
					getLogManager() << LogLevel_Error << "Failed to create a new tuple pyArg.\n";
					return false;
				}
				if (PyTuple_SetItem(pyArg, 0, PyLong_FromLong(long(stim.m_ID))) != 0)
				{
					getLogManager() << LogLevel_Error << "Failed to set item 0 (identifier) in tuple pyArg.\n";
					Py_CLEAR(pyArg);
					return false;
				}
				if (PyTuple_SetItem(pyArg, 1, PyFloat_FromDouble(stim.m_Date.toSeconds())) != 0)
				{
					getLogManager() << LogLevel_Error << "Failed to set item 1 (date) in tuple pyArg.\n";
					Py_CLEAR(pyArg);
					return false;
				}
				if (PyTuple_SetItem(pyArg, 2, PyFloat_FromDouble(stim.m_Duration.toSeconds())) != 0)
				{
					getLogManager() << LogLevel_Error << "Failed to set item 2 (duration) in tuple pyArg.\n";
					Py_CLEAR(pyArg);
					return false;
				}

				PyObject* pyStim = PyObject_Call(m_stimulation, pyArg, nullptr);
				if (pyStim == nullptr)
				{
					getLogManager() << LogLevel_Error << "Failed to create a new OVStimulation pyStim.\n";
					Py_CLEAR(pyArg);
					return false;
				}
				Py_CLEAR(pyArg);

				if (!appendToPyObject(pyStim, pyStimSet))
				{
					getLogManager() << LogLevel_Error << "Failed to append stimulation to box.input[" << index << "].\n";
					Py_CLEAR(pyStim);
					return false;
				}
				Py_CLEAR(pyStim);
			}

			if (!appendToPyObject(pyStimSet, pyBuffer))
			{
				getLogManager() << LogLevel_Error << "Failed to append stimulation to box.input[" << index << "].\n";
				Py_CLEAR(pyStimSet);
				return false;
			}
			Py_CLEAR(pyStimSet);
		}

		if (m_decoders[index]->isEndReceived())
		{
			PyObject* pyArg = PyTuple_New(2);
			if (pyArg == nullptr)
			{
				getLogManager() << LogLevel_Error << "Failed to create a new tuple pyArg.\n";
				return false;
			}
			if (PyTuple_SetItem(pyArg, 0, PyFloat_FromDouble(boxCtx.getInputChunkStartTime(index, idx).toSeconds())) != 0)
			{
				getLogManager() << LogLevel_Error << "Failed to set item 0 (start time) in tuple pyArg.\n";
				Py_CLEAR(pyArg);
				return false;
			}
			if (PyTuple_SetItem(pyArg, 1, PyFloat_FromDouble(boxCtx.getInputChunkEndTime(index, idx).toSeconds())) != 0)
			{
				getLogManager() << LogLevel_Error << "Failed to set item 1 (end time) in tuple pyArg.\n";
				Py_CLEAR(pyArg);
				return false;
			}

			PyObject* pyStimEnd = PyObject_Call(m_stimulationEnd, pyArg, nullptr);
			if (pyStimEnd == nullptr)
			{
				getLogManager() << LogLevel_Error << "Failed to create a new OVStimulationEnd pyStimEnd.\n";
				Py_CLEAR(pyArg);
				return false;
			}
			Py_CLEAR(pyArg);

			if (!appendToPyObject(pyStimEnd, pyBuffer))
			{
				getLogManager() << LogLevel_Error << "Failed to append an OVStimulationEnd to box.input[" << index << "].\n";
				Py_CLEAR(pyStimEnd);
				return false;
			}
			Py_CLEAR(pyStimEnd);
		}
	}
	return true;
}

bool CPolyBox::transferStimulationOutputChunksFromPython(const size_t index)
{
	IBoxIO& boxCtx           = this->getDynamicBoxContext();
	CStimulationSet& stimSet = *dynamic_cast<TStimulationEncoder<CPolyBox>*>(m_encoders[index])->getInputStimulationSet();

	if (!PyList_Check(m_boxOutput))
	{
		getLogManager() << LogLevel_Error << "box.output must be a list.\n";
		return false;
	}

	PyObject* pyBuffer = PyList_GetItem(m_boxOutput, Py_ssize_t(index));
	if (pyBuffer == nullptr)
	{
		getLogManager() << LogLevel_Error << "Failed to get box.output[" << index << "].\n";
		return false;
	}

	size_t len;
	if (!getLenFromPyObject(pyBuffer, len))
	{
		getLogManager() << LogLevel_Error << "Failed to get box.output[" << index << "].__len__().\n";
		return false;
	}

	for (size_t idx = 0; idx < len; ++idx)
	{
		PyObject* pyChunk = PyObject_CallMethod(pyBuffer, "pop", nullptr);
		if (pyChunk == nullptr)
		{
			getLogManager() << LogLevel_Error << "Failed to get item " << idx << " of box.output[" << index << "].\n";
			return false;
		}

		if (PyObject_IsInstance(pyChunk, m_stimulationHeader) == 1)
		{
			CTime startTime, endTime;
			getTimesFromPyObject(pyChunk, startTime, endTime);

			stimSet.clear();
			m_encoders[index]->encodeHeader();
			boxCtx.markOutputAsReadyToSend(index, startTime, endTime);
		}

		else if (PyObject_IsInstance(pyChunk, m_stimulationSet) == 1)
		{
			size_t chunkLen;
			if (!getLenFromPyObject(pyChunk, chunkLen))
			{
				getLogManager() << LogLevel_Error << "Failed to get stimulations set length.\n";
				return false;
			}

			stimSet.clear();
			for (size_t i = 0; i < chunkLen; ++i)
			{
				PyObject* pyStim = PyList_GetItem(pyChunk, Py_ssize_t(i));
				if (pyStim == nullptr)
				{
					getLogManager() << LogLevel_Error << "Failed to get item " << i << " of chunk " << idx << ".\n";
					return false;
				}
				if (PyObject_IsInstance(pyStim, m_stimulation) < 1)
				{
					getLogManager() << LogLevel_Error << "Item " << i << " is not an OVStimulation.\n";
					return false;
				}

				PyObject* pyID    = PyObject_GetAttrString(pyStim, "identifier");
				const uint64_t id = uint64_t(PyFloat_AsDouble(pyID));
				Py_CLEAR(pyID);

				CTime date, duration;
				getTimeFromPyObject(pyStim, "date", date);
				getTimeFromPyObject(pyStim, "duration", duration);
				stimSet.append(id, date, duration);
			}

			CTime startTime, endTime;
			getTimesFromPyObject(pyChunk, startTime, endTime);

			m_encoders[index]->encodeBuffer();
			boxCtx.markOutputAsReadyToSend(index, startTime, endTime);
		}

		else if (PyObject_IsInstance(pyChunk, m_stimulationEnd) == 1)
		{
			CTime startTime, endTime;
			getTimesFromPyObject(pyChunk, startTime, endTime);

			m_encoders[index]->encodeEnd();
			boxCtx.markOutputAsReadyToSend(index, startTime, endTime);
		}

		else
		{
			getLogManager() << LogLevel_Error << "Unexpected object type for item " << idx << " in box.output[" << index << "].\n";
			Py_CLEAR(pyChunk);
			return false;
		}

		Py_CLEAR(pyChunk);
	}
	return true;
}

bool CPolyBox::process()
{
	const IBox& boxCtx = this->getStaticBoxContext();
	CIdentifier typeID;

	for (size_t i = 0; i < boxCtx.getInputCount(); ++i)
	{
		boxCtx.getInputType(i, typeID);
		if (typeID == OV_TypeId_StreamedMatrix) { if (!transferStreamedMatrixInputChunksToPython(i)) { return false; } }
		else if (typeID == OV_TypeId_Signal) { if (!transferSignalInputChunksToPython(i)) { return false; } }
		else if (typeID == OV_TypeId_Stimulations) { if (!transferStimulationInputChunksToPython(i)) { return false; } }
		else
		{
			getLogManager() << LogLevel_Error << "Codec to decode " << typeID.str() << " is not implemented.\n";
			return false;
		}
	}

	//update the python current time
	m_boxTime = PyFloat_FromDouble(getPlayerContext().getCurrentTime().toSeconds());
	if (m_boxTime == nullptr)
	{
		getLogManager() << LogLevel_Error << "Failed to convert the current time into a PyFloat during update.\n";
		return false;
	}
	if (PyObject_SetAttrString(m_box, "_currentTime", m_boxTime) == -1)
	{
		getLogManager() << LogLevel_Error << "Failed to update \"box._currentTime\" attribute.\n";
		return false;
	}

	//call the python process function
	if (m_boxProcess && PyCallable_Check(m_boxProcess))
	{
		PyObject* result       = PyObject_CallObject(m_boxProcess, nullptr);
		const bool stdoutError = logSysStdout();
		const bool stderrError = logSysStderr();
		if ((result == nullptr) || (!stdoutError) || (!stderrError))
		{
			if (result == nullptr) { getLogManager() << LogLevel_Error << "Failed to call \"box.__process\" function.\n"; }
			if (!stdoutError) { getLogManager() << LogLevel_Error << "logSysStdout() failed during box.__process.\n"; }
			if (!stderrError) { getLogManager() << LogLevel_Error << "logSysStderr() failed during box.__process.\n"; }
			Py_CLEAR(result);
			return false;
		}
		Py_CLEAR(result);
	}

	for (size_t i = 0; i < boxCtx.getOutputCount(); ++i)
	{
		boxCtx.getOutputType(i, typeID);
		if (typeID == OV_TypeId_StreamedMatrix) { if (!transferStreamedMatrixOutputChunksFromPython(i)) { return false; } }
		else if (typeID == OV_TypeId_Signal) { if (!transferSignalOutputChunksFromPython(i)) { return false; } }
		else if (typeID == OV_TypeId_Stimulations) { if (!transferStimulationOutputChunksFromPython(i)) { return false; } }
		else
		{
			getLogManager() << LogLevel_Error << "Codec to encode " << typeID.str() << " is not implemented.\n";
			return false;
		}
	}
	return true;
}

#endif // #if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 3)
#endif // TARGET_HAS_ThirdPartyPython3
