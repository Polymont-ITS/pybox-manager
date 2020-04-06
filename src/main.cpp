#if defined TARGET_HAS_ThirdPartyPython && !(defined(WIN32) && defined(TARGET_BUILDTYPE_Debug))

#include "box-algorithms/CPolyBox.h"
#include "box-algorithms/DataViz.hpp"
#include "box-algorithms/DatasetCreator.hpp"
#include "box-algorithms/ProcessML.hpp"
#include "box-algorithms/ovpADA.h"
#include "box-algorithms/ovpSVM.h"
#include "box-algorithms/ovpSGD.h"
#include "box-algorithms/ovpRMDM.h"
#include "box-algorithms/ovpRiemann_Tangent_Space.h"
#include "box-algorithms/ovpRandom_Forest.h"
#include "box-algorithms/ovpNearestCentroid.h"
#include "box-algorithms/ovpMLP.h"
#include "box-algorithms/ovpLogistic_Regression.h"
#include "box-algorithms/ovpLDA.h"
#include "box-algorithms/ovpKNearestNeighbors.h"
#include "box-algorithms/ovpGaussianNB.h"
#include "box-algorithms/ovpExtra_Trees.h"
#include "box-algorithms/ovpDecision_Tree_Classifier.h"
#include "box-algorithms/ovpBagging.h"

#if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 2)

#ifdef TARGET_OS_Windows
#include "windows.h"
#endif

#include <string>
#include <iostream>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace OpenViBEPlugins;

class CPythonInitializer
{
public:
	CPythonInitializer();
	~CPythonInitializer();
	bool IsPythonAvailable();
private:
#ifdef TARGET_OS_Windows
	bool checkPythonPath();
#endif
	//		PyThreadState *m_pMainPyThreadState;
	bool m_bPythonAvailable;
};

#ifdef TARGET_OS_Windows
bool CPythonInitializer::checkPythonPath()
{
#ifdef HAVE_WORKING_PYCHECK
	const CString testCmd = "\"" + Directories::getBinDir() + "\\openvibe-py2-check.exe\"";
	if (std::system(testCmd.toASCIIString()))
	{
		std::cout << "Warning: The Python version found does not seem to be compatible and using it would cause Designer to crash.";
		std::cout << "Check that Python 2.7 is installed and/or your PYTHONPATH/PYTHONHOME is set correctly." << std::endl;
		std::cout << "Disabling the Python scripting box for now." << std::endl;
		return false;
	}
#endif
	std::string path = Py_GetPath();

	int found = path.find_first_of(";");
	while (found != std::string::npos)
	{
		if (found > 0)
		{
			std::string filename = path.substr(0, found);
			const bool exists    = (_access(filename.c_str(), 0) == 0);
			if (exists) { return true; }
		}
		path  = path.substr(found + 1);
		found = path.find_first_of(";");
	}

	std::cout << "Python directory not found. You probably have a corrupted python installation!" << std::endl;
	std::cout << "The tried path from Py_GetPath() was [" << Py_GetPath() << "]\n";

	return false;
}
#endif

CPythonInitializer::CPythonInitializer() : m_bPythonAvailable(false)
{
	//m_pMainPyThreadState = nullptr;

#ifdef TARGET_OS_Windows
	__try
	{
		if (!Py_IsInitialized())
		{
			// We do not care about the last file, since it is the OpenViBE runtime path
			if (checkPythonPath())
			{
				Py_Initialize();
				m_bPythonAvailable = true;
			}
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER) { }
#else
		if (!Py_IsInitialized())
		{
			Py_Initialize();
			m_bPythonAvailable = true;
		}
#endif
}

CPythonInitializer::~CPythonInitializer()
{
	if (m_bPythonAvailable)
	{
		m_bPythonAvailable = false;
		Py_Finalize();
	}
}

bool CPythonInitializer::IsPythonAvailable() { return m_bPythonAvailable; }


OVP_Declare_Begin()
	static CPythonInitializer pythonInitializer;
	if (pythonInitializer.IsPythonAvailable())
	{
		// <tag> OVP_Declare_New
		OVP_Declare_New(Python::CBoxAlgorithmDataVizDesc);
		OVP_Declare_New(Python::CBoxAlgorithmADADesc);
		OVP_Declare_New(Python::CBoxAlgorithmDatasetCreatorDesc);
		OVP_Declare_New(Python::CBoxAlgorithmSVMDesc);
		OVP_Declare_New(Python::CBoxAlgorithmSGDDesc);
		OVP_Declare_New(Python::CBoxAlgorithmRMDMDesc);
		OVP_Declare_New(Python::CBoxAlgorithmRiemann_Tangent_SpaceDesc);
		OVP_Declare_New(Python::CBoxAlgorithmRandom_ForestDesc);
		OVP_Declare_New(Python::CBoxAlgorithmProcessMLDesc);
		OVP_Declare_New(Python::CBoxAlgorithmNearestCentroidDesc);
		OVP_Declare_New(Python::CBoxAlgorithmMLPDesc);
		OVP_Declare_New(Python::CBoxAlgorithmLogistic_RegressionDesc);
		OVP_Declare_New(Python::CBoxAlgorithmLDADesc);
		OVP_Declare_New(Python::CBoxAlgorithmKNearestNeighborsDesc);
		OVP_Declare_New(Python::CBoxAlgorithmGaussianNBDesc);
		OVP_Declare_New(Python::CBoxAlgorithmExtra_TreesDesc);
		OVP_Declare_New(Python::CBoxAlgorithmDecision_Tree_ClassifierDesc);
		OVP_Declare_New(Python::CBoxAlgorithmBaggingDesc);


		// <tag> Custom Type Settings

		rPluginModuleContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_Classifier_Algorithm, "Classifier_Algorithm");
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Nearest Centroid", 0);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Nearest Neighbors Classifier", 1);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Gaussian Naive Bayes", 2);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Stochastic Gradient Descent", 3);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Logistic Regression", 4);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Decision Tree Classifier", 5);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Extra Trees", 6);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Bagging", 7);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Random Forest", 8);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Support Vector Machine", 9);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Linear Discriminant Analysis", 10);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "AdaBoost", 11);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Multi Layer Perceptron", 12);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Riemann Minimum Distance to Mean", 13);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Riemann Tangent Space", 14);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "None", 15);

		rPluginModuleContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_Knn_Algorithm, "Knn_Algorithm");
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Knn_Algorithm, "auto", 0);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Knn_Algorithm, "ball_tree", 1);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Knn_Algorithm, "kd_tree", 2);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Knn_Algorithm, "brute", 3);
		rPluginModuleContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_Knn_Weights, "Knn_Weights");
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Knn_Weights, "uniform", 0);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Knn_Weights, "distance", 1);

		rPluginModuleContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_Metric, "Metric");
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "cityblock", 0);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "cosine", 1);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "euclidean", 2);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "l1", 3);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "l2", 4);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "manhattan", 5);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "braycurtis", 6);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "canberra", 7);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "chebyshev", 8);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "correlation", 9);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "dice", 10);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "hamming", 11);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "jaccard", 12);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "kulsinski", 13);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "minkowski", 14);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "mahalanobis", 15);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "rogerstanimoto", 16);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "russellrao", 17);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "seuclidean", 18);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "sokalmichener", 19);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "sokalsneath", 20);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "sqeuclidean", 21);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "yule", 22);

		rPluginModuleContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_Penalty, "Penalty");
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Penalty, "l1", 0);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Penalty, "l2", 1);

		rPluginModuleContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_SVM_Loss, "SVM_Loss");
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SVM_Loss, "hinge", 0);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SVM_Loss, "squared_hinge", 1);

		rPluginModuleContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_SVM_MultiClass, "SVM_MultiClass");
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SVM_MultiClass, "ovr", 0);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SVM_MultiClass, "crammer_singer", 1);

		rPluginModuleContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_Criterion, "Criterion");
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Criterion, "gini", 0);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Criterion, "entropy", 1);


		// Logistic regression
		rPluginModuleContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_Log_reg_solver, "Solver");
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Log_reg_solver, "newton-cg", 0);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Log_reg_solver, "lbfgs", 1);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Log_reg_solver, "liblinear", 2);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Log_reg_solver, "sag", 3);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Log_reg_solver, "saga", 4);

		rPluginModuleContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_Log_reg_multi_class, "Multi_class");
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Log_reg_multi_class, "auto", 0);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Log_reg_multi_class, "ovr", 1);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Log_reg_multi_class, "multinominal", 2);


		// Decision Tree Classifier
		rPluginModuleContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_DecisionTree_splitter, "Splitter");
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_DecisionTree_splitter, "best", 0);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_DecisionTree_splitter, "random", 1);

		// MLP 
		rPluginModuleContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_MLP_activation, "Activation");
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_activation, "identity", 0);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_activation, "logistic", 1);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_activation, "tanh", 2);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_activation, "relu", 3);

		rPluginModuleContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_MLP_solver, "Solver");
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_solver, "lbfgs", 0);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_solver, "sgd", 1);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_solver, "adam", 2);

		rPluginModuleContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_MLP_learning_rate, "Learning rate");
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_learning_rate, "constant", 0);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_learning_rate, "invscaling", 1);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_learning_rate, "adaptive", 2);

		// SGD
		rPluginModuleContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_SGD_loss, "Loss");
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_loss, "hinge", 0);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_loss, "log", 1);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_loss, "modified_huber", 2);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_loss, "squared_hinge", 3);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_loss, "perceptron", 4);

		rPluginModuleContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_SGD_learning_rate, "Learning rate");
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_learning_rate, "optimal", 0);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_learning_rate, "constant", 1);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_learning_rate, "invscaling", 2);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_learning_rate, "adaptive", 3);

		// LDA
		rPluginModuleContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_LDA_solver, "Loss");
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_LDA_solver, "svd", 0);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_LDA_solver, "lsqr", 1);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_LDA_solver, "eigen", 2);

		// ADA
		rPluginModuleContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_ADA_algorithm, "Algorithm");
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_ADA_algorithm, "SAMME", 0);
		rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_ADA_algorithm, "SAMME.R", 1);
	}

OVP_Declare_End()

#else
#pragma message ("WARNING: Python 2.x headers are required to build the Python plugin, different includes found, skipped")
#endif // #if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 2)

#endif // TARGET_HAS_ThirdPartyPython
