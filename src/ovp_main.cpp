#if defined TARGET_HAS_ThirdPartyPython

#include "box-algorithms/CPolyBox.h"

#if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 2)

#ifdef TARGET_OS_Windows
#include "windows.h"
#endif

#include <string>
#include <iostream>

class CPythonInitializer
{
public:
	CPythonInitializer(void);
	~CPythonInitializer(void);
	OpenViBE::boolean IsPythonAvailable(void);
private:
#ifdef TARGET_OS_Windows
	bool checkPythonPath();
#endif
	//		PyThreadState *m_pMainPyThreadState;
	OpenViBE::boolean m_bPythonAvailable;
};

#ifdef TARGET_OS_Windows
bool CPythonInitializer::checkPythonPath()
{
#ifdef HAVE_WORKING_PYCHECK
	const OpenViBE::CString testCmd = "\"" + OpenViBE::Directories::getBinDir() + "\\openvibe-py2-check.exe\"";
	if (std::system(testCmd.toASCIIString()))
	{
		std::cout << "Warning: The Python version found does not seem to be compatible and using it would cause Designer to crash.";
		std::cout << "Check that Python 2.7 is installed and/or your PYTHONPATH/PYTHONHOME is set correctly." << std::endl;
		std::cout << "Disabling the Python scripting box for now." << std::endl;
		return false;
	}
#endif
	std::string l_sPath = Py_GetPath();
	
	int found;
	found = l_sPath.find_first_of(";");
	while(found != std::string::npos)
	{
		if(found > 0)
		{
			std::string l_sFilename = l_sPath.substr(0,found);
			bool l_bExists = (_access(l_sFilename.c_str(), 0) == 0);
			if (l_bExists)
			{
				// std::cout << "Found Python in : " << l_sPath.substr(0,found) << std::endl;
				return true;
			}
			else
			{
				//std::cout << "NOT found : " << l_sPath.substr(0,found) << std::endl;
			}
		}
		l_sPath = l_sPath.substr(found+1);
		found = l_sPath.find_first_of(";");
	}
	
	std::cout << "Python directory not found. You probably have a corrupted python installation!" << std::endl;
	std::cout << "The tried path from Py_GetPath() was [" << Py_GetPath() << "]\n";

	return false;
}
#endif

CPythonInitializer::CPythonInitializer(void) : 
	m_bPythonAvailable(false)
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
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
#else
		if (!Py_IsInitialized())
		{
			Py_Initialize();
			m_bPythonAvailable = true;
		}
#endif



}

CPythonInitializer::~CPythonInitializer(void)
{
	if (m_bPythonAvailable)
	{
		m_bPythonAvailable = false;
		Py_Finalize();
	}
}

OpenViBE::boolean CPythonInitializer::IsPythonAvailable(void)
{
	return m_bPythonAvailable;
}


OVP_Declare_Begin();

	static CPythonInitializer l_oPythonInitializer;
	if (l_oPythonInitializer.IsPythonAvailable())
	{
		// <tag> OVP_Declare_New


		// <tag> Custom Type Settings

		rKernelContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_Classifier_Algorithm, "Classifier_Algorithm");
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Nearest Centroid", 0);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Nearest Neighbors Classifier", 1);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Gaussian Naive Bayes", 2);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Stochastic Gradient Descent", 3);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Logistic Regression", 4);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Decision Tree Classifier", 5);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Extra Trees", 6);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Bagging", 7);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Random Forest", 8);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Support Vector Machine", 9);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Linear Discriminant Analysis", 10);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "AdaBoost", 11);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Multi Layer Perceptron", 12);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Riemann Minimum Distance to Mean", 13);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Riemann Tangent Space", 14);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "None", 15);

		rKernelContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_Knn_Algorithm, "Knn_Algorithm");
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Knn_Algorithm, "auto", 0);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Knn_Algorithm, "ball_tree", 1);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Knn_Algorithm, "kd_tree", 2);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Knn_Algorithm, "brute", 3);
		rKernelContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_Knn_Weights, "Knn_Weights");
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Knn_Weights, "uniform", 0);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Knn_Weights, "distance", 1);

		rKernelContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_Metric, "Metric");
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "cityblock", 0);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "cosine", 1);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "euclidean", 2);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "l1", 3);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "l2", 4);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "manhattan", 5);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "braycurtis", 6);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "canberra", 7);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "chebyshev", 8);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "correlation", 9);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "dice", 10);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "hamming", 11);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "jaccard", 12);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "kulsinski", 13);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "minkowski", 14);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "mahalanobis", 15);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "rogerstanimoto", 16);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "russellrao", 17);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "seuclidean", 18);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "sokalmichener", 19);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "sokalsneath", 20);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "sqeuclidean", 21);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "yule", 22);

		rKernelContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_Penalty, "Penalty");
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Penalty, "l1", 0);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Penalty, "l2", 1);

		rKernelContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_SVM_Loss, "SVM_Loss");
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SVM_Loss, "hinge", 0);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SVM_Loss, "squared_hinge", 1);

		rKernelContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_SVM_MultiClass, "SVM_MultiClass");
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SVM_MultiClass, "ovr", 0);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SVM_MultiClass, "crammer_singer", 1);

		rKernelContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_Criterion, "Criterion");
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Criterion, "gini", 0);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Criterion, "entropy", 1);


		// Logistic regression
		rKernelContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_Log_reg_solver, "Solver");
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Log_reg_solver, "newton-cg", 0);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Log_reg_solver, "lbfgs", 1);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Log_reg_solver, "liblinear", 2);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Log_reg_solver, "sag", 3);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Log_reg_solver, "saga", 4);

		rKernelContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_Log_reg_multi_class, "Multi_class");
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Log_reg_multi_class, "auto", 0);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Log_reg_multi_class, "ovr", 1);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Log_reg_multi_class, "multinominal", 2);


		// Decision Tree Classifier
		rKernelContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_DecisionTree_splitter, "Splitter");
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_DecisionTree_splitter, "best", 0);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_DecisionTree_splitter, "random", 1);

		// MLP 
		rKernelContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_MLP_activation, "Activation");
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_activation, "identity", 0);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_activation, "logistic", 1);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_activation, "tanh", 2);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_activation, "relu", 3);

		rKernelContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_MLP_solver, "Solver");
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_solver, "lbfgs", 0);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_solver, "sgd", 1);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_solver, "adam", 2);

		rKernelContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_MLP_learning_rate, "Learning rate");
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_learning_rate, "constant", 0);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_learning_rate, "invscaling", 1);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_learning_rate, "adaptive", 2);

		// SGD
		rKernelContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_SGD_loss, "Loss");
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_loss, "hinge", 0);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_loss, "log", 1);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_loss, "modified_huber", 2);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_loss, "squared_hinge", 3);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_loss, "perceptron", 4);

		rKernelContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_SGD_learning_rate, "Learning rate");
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_learning_rate, "optimal", 0);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_learning_rate, "constant", 1);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_learning_rate, "invscaling", 2);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_learning_rate, "adaptive", 3);

		// LDA
		rKernelContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_LDA_solver, "Loss");
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_LDA_solver, "svd", 0);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_LDA_solver, "lsqr", 1);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_LDA_solver, "eigen", 2);

		// ADA
		rKernelContext.getTypeManager().registerEnumerationType(OVPoly_ClassId_ADA_algorithm, "Algorithm");
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_ADA_algorithm, "SAMME", 0);
		rKernelContext.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_ADA_algorithm, "SAMME.R", 1);

	}

OVP_Declare_End();

#else
#pragma message ("WARNING: Python 2.x headers are required to build the Python plugin, different includes found, skipped")
#endif // #if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 2)

#endif // TARGET_HAS_ThirdPartyPython

