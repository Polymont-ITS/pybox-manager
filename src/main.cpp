#if defined TARGET_HAS_ThirdPartyPython3 && !(defined(WIN32) && defined(TARGET_BUILDTYPE_Debug))

#include "box-algorithms/CPolyBox.hpp"
#include "box-algorithms/DataViz.hpp"
#include "box-algorithms/DatasetCreator.hpp"
#include "box-algorithms/ProcessML.hpp"
#include "box-algorithms/TrainerML.hpp"

#if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 3)

#ifdef TARGET_OS_Windows
#include "windows.h"
#endif

#include <string>
#include <iostream>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace std;


OVP_Declare_Begin()
	// <tag> OVP_Declare_New
	OVP_Declare_New(PyBox::CBoxAlgorithmDatasetCreatorDesc);
	OVP_Declare_New(PyBox::CBoxAlgorithmDataVizDesc);
	OVP_Declare_New(PyBox::CBoxAlgorithmProcessMLDesc);
	OVP_Declare_New(PyBox::CBoxAlgorithmTrainerMLDesc);

	// <tag> Custom Type Settings
	context.getTypeManager().registerEnumerationType(OVPoly_ClassId_Classifier_Algorithm, "Classifier_Algorithm");
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Nearest Centroid", 0);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Nearest Neighbors", 1);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Gaussian Naive Bayes", 2);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Stochastic Gradient Descent", 3);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Logistic Regression", 4);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Decision Tree", 5);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Extra Trees", 6);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Bagging", 7);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Random Forest", 8);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Support Vector Machine", 9);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Linear Discriminant Analysis", 10);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "AdaBoost", 11);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Multi Layer Perceptron", 12);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Riemann Minimum Distance to Mean", 13);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "Riemann Tangent Space", 14);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Classifier_Algorithm, "None", 15);

	context.getTypeManager().registerEnumerationType(OVPoly_ClassId_Knn_Algorithm, "Knn_Algorithm");
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Knn_Algorithm, "auto", 0);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Knn_Algorithm, "ball_tree", 1);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Knn_Algorithm, "kd_tree", 2);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Knn_Algorithm, "brute", 3);
	context.getTypeManager().registerEnumerationType(OVPoly_ClassId_Knn_Weights, "Knn_Weights");
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Knn_Weights, "uniform", 0);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Knn_Weights, "distance", 1);

	context.getTypeManager().registerEnumerationType(OVPoly_ClassId_Metric, "Metric");
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "cityblock", 0);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "cosine", 1);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "euclidean", 2);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "l1", 3);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "l2", 4);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "manhattan", 5);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "braycurtis", 6);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "canberra", 7);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "chebyshev", 8);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "correlation", 9);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "dice", 10);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "hamming", 11);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "jaccard", 12);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "kulsinski", 13);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "minkowski", 14);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "mahalanobis", 15);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "rogerstanimoto", 16);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "russellrao", 17);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "seuclidean", 18);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "sokalmichener", 19);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "sokalsneath", 20);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "sqeuclidean", 21);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Metric, "yule", 22);

	context.getTypeManager().registerEnumerationType(OVPoly_ClassId_Penalty, "Penalty");
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Penalty, "l1", 0);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Penalty, "l2", 1);

	context.getTypeManager().registerEnumerationType(OVPoly_ClassId_SVM_Loss, "SVM_Loss");
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SVM_Loss, "hinge", 0);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SVM_Loss, "squared_hinge", 1);

	context.getTypeManager().registerEnumerationType(OVPoly_ClassId_SVM_MultiClass, "SVM_MultiClass");
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SVM_MultiClass, "ovr", 0);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SVM_MultiClass, "crammer_singer", 1);

	context.getTypeManager().registerEnumerationType(OVPoly_ClassId_Criterion, "Criterion");
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Criterion, "gini", 0);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Criterion, "entropy", 1);


	// Logistic regression
	context.getTypeManager().registerEnumerationType(OVPoly_ClassId_Log_reg_solver, "Solver");
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Log_reg_solver, "newton-cg", 0);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Log_reg_solver, "lbfgs", 1);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Log_reg_solver, "liblinear", 2);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Log_reg_solver, "sag", 3);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Log_reg_solver, "saga", 4);

	context.getTypeManager().registerEnumerationType(OVPoly_ClassId_Log_reg_multi_class, "Multi_class");
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Log_reg_multi_class, "auto", 0);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Log_reg_multi_class, "ovr", 1);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_Log_reg_multi_class, "multinominal", 2);


	// Decision Tree Classifier
	context.getTypeManager().registerEnumerationType(OVPoly_ClassId_DecisionTree_splitter, "Splitter");
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_DecisionTree_splitter, "best", 0);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_DecisionTree_splitter, "random", 1);

	// MLP 
	context.getTypeManager().registerEnumerationType(OVPoly_ClassId_MLP_activation, "Activation");
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_activation, "identity", 0);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_activation, "logistic", 1);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_activation, "tanh", 2);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_activation, "relu", 3);

	context.getTypeManager().registerEnumerationType(OVPoly_ClassId_MLP_solver, "Solver");
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_solver, "lbfgs", 0);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_solver, "sgd", 1);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_solver, "adam", 2);

	context.getTypeManager().registerEnumerationType(OVPoly_ClassId_MLP_learning_rate, "Learning rate");
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_learning_rate, "constant", 0);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_learning_rate, "invscaling", 1);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_MLP_learning_rate, "adaptive", 2);

	// SGD
	context.getTypeManager().registerEnumerationType(OVPoly_ClassId_SGD_loss, "Loss");
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_loss, "hinge", 0);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_loss, "log", 1);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_loss, "modified_huber", 2);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_loss, "squared_hinge", 3);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_loss, "perceptron", 4);

	context.getTypeManager().registerEnumerationType(OVPoly_ClassId_SGD_learning_rate, "Learning rate");
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_learning_rate, "optimal", 0);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_learning_rate, "constant", 1);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_learning_rate, "invscaling", 2);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_SGD_learning_rate, "adaptive", 3);

	// LDA
	context.getTypeManager().registerEnumerationType(OVPoly_ClassId_LDA_solver, "Loss");
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_LDA_solver, "svd", 0);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_LDA_solver, "lsqr", 1);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_LDA_solver, "eigen", 2);

	// ADA
	context.getTypeManager().registerEnumerationType(OVPoly_ClassId_ADA_algorithm, "Algorithm");
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_ADA_algorithm, "SAMME", 0);
	context.getTypeManager().registerEnumerationEntry(OVPoly_ClassId_ADA_algorithm, "SAMME.R", 1);
OVP_Declare_End()

#else
#pragma message ("WARNING: Python 3.7 headers are required to build the Python plugin, different includes found, skipped")
#endif // #if defined(PY_MAJOR_VERSION) && (PY_MAJOR_VERSION == 3)

#endif // TARGET_HAS_ThirdPartyPython3
