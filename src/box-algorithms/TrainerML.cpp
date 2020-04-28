#include "TrainerML.hpp"
#include <tuple>
#include <vector>

using namespace std;
using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace /*OpenViBE::*/Toolkit;
using namespace /*OpenViBE::Plugins::*/PyBox;

static const vector<tuple<string, CIdentifier, string>> ADA_BOOST_SETTING = {
	make_tuple("Test set share", OV_TypeId_Float, "0.2"),
	make_tuple("Labels", OV_TypeId_String, ""),
	make_tuple("n_estimators", OV_TypeId_Integer, "50"),
	make_tuple("learning_rate", OV_TypeId_Float, "1.0"),
	make_tuple("algorithm", OVPoly_ClassId_ADA_algorithm, "SAMME.R"),
	make_tuple("random_state", OV_TypeId_String, "None")
};

static const vector<tuple<string, CIdentifier, string>> BAGGING_SETTING = {
	make_tuple("Test set share", OV_TypeId_Float, "0.2"),
	make_tuple("Labels", OV_TypeId_String, ""),
	make_tuple("max_features", OV_TypeId_Float, "1.0"),
	make_tuple("random_state", OV_TypeId_String, "None"),
	make_tuple("n_estimators", OV_TypeId_Integer, "10"),
	make_tuple("bootstrap", OV_TypeId_Boolean, "true"),
	make_tuple("bootstrap_features", OV_TypeId_Boolean, "false"),
	make_tuple("oob_score", OV_TypeId_Boolean, "false"),
	make_tuple("warm_start", OV_TypeId_Boolean, "false"),
	make_tuple("n_jobs", OV_TypeId_String, "None"),
	make_tuple("verbose", OV_TypeId_Integer, "0")
};

static const vector<tuple<string, CIdentifier, string>> TREE_SETTING = {
	make_tuple("Test set share", OV_TypeId_Float, "0.2"),
	make_tuple("Labels", OV_TypeId_String, ""),
	make_tuple("criterion", OVPoly_ClassId_Criterion, "gini"),
	make_tuple("splitter", OVPoly_ClassId_DecisionTree_splitter, "best"),
	make_tuple("max_depth", OV_TypeId_String, "None"),
	make_tuple("min_samples_split", OV_TypeId_Float, "2"),
	make_tuple("min_samples_leaf", OV_TypeId_Float, "1"),
	make_tuple("min_weight_fraction_leaf", OV_TypeId_Float, "0"),
	make_tuple("max_features", OV_TypeId_String, "None"),
	make_tuple("random_state", OV_TypeId_String, "None"),
	make_tuple("max_leaf_nodes", OV_TypeId_String, "None"),
	make_tuple("min_impurity_decrease", OV_TypeId_Float, "0"),
	make_tuple("min_impurity_split", OV_TypeId_Float, "1e-7")
};

static const vector<tuple<string, CIdentifier, string>> XTREE_SETTING = {
	make_tuple("Test set share", OV_TypeId_Float, "0.2"),
	make_tuple("Labels", OV_TypeId_String, ""),
	make_tuple("criterion", OVPoly_ClassId_Criterion, "gini"),
	make_tuple("max_depth", OV_TypeId_String, "None"),
	make_tuple("min_samples_split", OV_TypeId_Float, "2"),
	make_tuple("min_samples_leaf", OV_TypeId_Float, "1"),
	make_tuple("min_weight_fraction_leaf", OV_TypeId_Float, "0"),
	make_tuple("max_features", OV_TypeId_String, "auto"),
	make_tuple("random_state", OV_TypeId_String, "None"),
	make_tuple("max_leaf_nodes", OV_TypeId_String, "None"),
	make_tuple("min_impurity_decrease", OV_TypeId_Float, "0"),
	make_tuple("min_impurity_split", OV_TypeId_Float, "1e-7"),
	make_tuple("bootstrap", OV_TypeId_Boolean, "false"),
	make_tuple("oob_score", OV_TypeId_Boolean, "false"),
	make_tuple("verbose", OV_TypeId_Integer, "0"),
	make_tuple("warm_start", OV_TypeId_Boolean, "false"),
	make_tuple("max_samples", OV_TypeId_String, "None"),
};

static const vector<tuple<string, CIdentifier, string>> GAUSSIAN_SETTING = {
	make_tuple("Discriminator", OV_TypeId_String, ""),
	make_tuple("Test set share", OV_TypeId_Float, "0.2"),
	make_tuple("Labels", OV_TypeId_String, ""),
	make_tuple("var_smoothing", OV_TypeId_Float, "0.000000001"),
};

static const vector<tuple<string, CIdentifier, string>> KNN_SETTING = {
	make_tuple("Discriminator", OV_TypeId_String, ""),
	make_tuple("Test set share", OV_TypeId_Float, "0.2"),
	make_tuple("Labels", OV_TypeId_String, ""),
	make_tuple("n_neighbors", OV_TypeId_Integer, "5"),
	make_tuple("weights", OVPoly_ClassId_Knn_Weights, "uniform"),
	make_tuple("algorithm", OVPoly_ClassId_Knn_Algorithm, "auto"),
	make_tuple("leaf_size", OV_TypeId_Integer, "30"),
	make_tuple("p", OV_TypeId_Integer, "2"),
	make_tuple("metric", OVPoly_ClassId_Metric, "minkowski"),
};


static const vector<tuple<string, CIdentifier, string>> LDA_SETTING = {
	make_tuple("Test set share", OV_TypeId_Float, "0.2"),
	make_tuple("Labels", OV_TypeId_String, ""),
	make_tuple("solver", OVPoly_ClassId_LDA_solver, "svd"),
	make_tuple("n_components", OV_TypeId_String, ""),
};

static const vector<tuple<string, CIdentifier, string>> LR_SETTING = {
	make_tuple("Test set share", OV_TypeId_Float, "0.2"),
	make_tuple("Labels", OV_TypeId_String, ""),
	make_tuple("penalty", OVPoly_ClassId_Penalty, "l2"),
	make_tuple("dual", OV_TypeId_Boolean, "false"),
	make_tuple("tol", OV_TypeId_Float, "1e-4"),
	make_tuple("fit_intercept", OV_TypeId_Boolean, "true"),
	make_tuple("intercept_scaling", OV_TypeId_Float, "1"),
	make_tuple("random_state", OV_TypeId_String, "None"),
	make_tuple("solver", OVPoly_ClassId_Log_reg_solver, "lbfgs"),
	make_tuple("max_iter", OV_TypeId_Integer, "100"),
	make_tuple("verbose", OV_TypeId_Integer, "0"),
	make_tuple("warm_start", OV_TypeId_Boolean, "false"),
	make_tuple("n_jobs", OV_TypeId_String, "None"),
	make_tuple("multi_class", OVPoly_ClassId_Log_reg_multi_class, "auto"),
	make_tuple("C", OV_TypeId_Float, "1.0")
};

static const vector<tuple<string, CIdentifier, string>> MLP_SETTING = {
	make_tuple("Test set share", OV_TypeId_Float, "0.2"),
	make_tuple("Labels", OV_TypeId_String, ""),
	make_tuple("hidden_layer_sizes", OV_TypeId_String, "(100,)"),
	make_tuple("activation", OVPoly_ClassId_MLP_activation, "relu"),
	make_tuple("learning_rate", OVPoly_ClassId_MLP_learning_rate, "constant"),
	make_tuple("solver", OVPoly_ClassId_MLP_solver, "adam"),
	make_tuple("alpha", OV_TypeId_Float, "0.0001"),
	make_tuple("batch_size", OV_TypeId_String, "auto"),
	make_tuple("learning_rate_init", OV_TypeId_Float, "0.001"),
	make_tuple("power_t", OV_TypeId_Float, "0.5"),
	make_tuple("max_iter", OV_TypeId_Integer, "200"),
	make_tuple("shuffle", OV_TypeId_Boolean, "true"),
	make_tuple("random_state", OV_TypeId_String, "None"),
	make_tuple("tol", OV_TypeId_Float, "1e-4"),
	make_tuple("verbose", OV_TypeId_Boolean, "true"),
	make_tuple("warm_start", OV_TypeId_Boolean, "false"),
	make_tuple("momentum", OV_TypeId_Float, "0.9"),
	make_tuple("nesterovs_momentum", OV_TypeId_Boolean, "true"),
	make_tuple("early_stopping", OV_TypeId_Boolean, "false"),
	make_tuple("validation_fraction", OV_TypeId_Float, "0.1"),
	make_tuple("beta_1", OV_TypeId_Float, "0.9"),
	make_tuple("beta_2", OV_TypeId_Float, "0.999"),
	make_tuple("epsilon", OV_TypeId_Float, "1e-8"),
	make_tuple("n_iter_no_change", OV_TypeId_Integer, "10")
};

static const vector<tuple<string, CIdentifier, string>> NC_SETTING = {
	make_tuple("Test set share", OV_TypeId_Float, "0.2"),
	make_tuple("Labels", OV_TypeId_String, ""),
	make_tuple("metric", OVPoly_ClassId_Metric, "euclidean"),
	make_tuple("shrink_threshold", OV_TypeId_String, "")
};

static const vector<tuple<string, CIdentifier, string>> RF_SETTING = {
	make_tuple("Test set share", OV_TypeId_Float, "0.2"),
	make_tuple("Labels", OV_TypeId_String, ""),
	make_tuple("criterion", OVPoly_ClassId_Criterion, "gini"),
	make_tuple("max_depth", OV_TypeId_String, "None"),
	make_tuple("min_samples_split", OV_TypeId_Float, "2"),
	make_tuple("min_samples_leaf", OV_TypeId_Float, "1"),
	make_tuple("min_weight_fraction_leaf", OV_TypeId_Float, "0"),
	make_tuple("max_features", OV_TypeId_String, "auto"),
	make_tuple("random_state", OV_TypeId_String, "None"),
	make_tuple("max_leaf_nodes", OV_TypeId_String, "None"),
	make_tuple("min_impurity_decrease", OV_TypeId_Float, "0"),
	make_tuple("min_impurity_split", OV_TypeId_Float, "1e-7"),
	make_tuple("bootstrap", OV_TypeId_Boolean, "false"),
	make_tuple("oob_score", OV_TypeId_Boolean, "false"),
	make_tuple("verbose", OV_TypeId_Integer, "0"),
	make_tuple("warm_start", OV_TypeId_Boolean, "false"),
	make_tuple("max_samples", OV_TypeId_String, "None"),
	make_tuple("n_estimators", OV_TypeId_Integer, "100")
};

static const vector<tuple<string, CIdentifier, string>> RTS_SETTING = {
	make_tuple("Test set share", OV_TypeId_Float, "0.2"),
	make_tuple("Labels", OV_TypeId_String, ""),
	make_tuple("Discriminator", OVPoly_ClassId_Classifier_Algorithm, "Linear Discriminant Analysis")
};

static const vector<tuple<string, CIdentifier, string>> RMDM_SETTING = {
	make_tuple("Test set share", OV_TypeId_Float, "0.2"),
	make_tuple("Labels", OV_TypeId_String, ""),
	make_tuple("Discriminator", OVPoly_ClassId_Classifier_Algorithm, "None")
};

static const vector<tuple<string, CIdentifier, string>> SGD_SETTING = {
	make_tuple("Test set share", OV_TypeId_Float, "0.2"),
	make_tuple("Labels", OV_TypeId_String, ""),
	make_tuple("loss", OVPoly_ClassId_SGD_loss, "hinge"),
	make_tuple("penalty", OVPoly_ClassId_Penalty, "l2"),
	make_tuple("alpha", OV_TypeId_Float, "0.0001"),
	make_tuple("l1_ratio", OV_TypeId_Float, "0.15"),
	make_tuple("fit_intercept", OV_TypeId_Boolean, "true"),
	make_tuple("max_iter", OV_TypeId_Integer, "1000"),
	make_tuple("tol", OV_TypeId_Float, "0.001"),
	make_tuple("shuffle", OV_TypeId_Boolean, "true"),
	make_tuple("verbose", OV_TypeId_Integer, "0"),
	make_tuple("epsilon", OV_TypeId_Float, "0.1"),
	make_tuple("random_state", OV_TypeId_String, ""),
	make_tuple("learning_rate", OVPoly_ClassId_SGD_learning_rate, "optimal"),
	make_tuple("early_stopping", OV_TypeId_Boolean, "false"),
	make_tuple("n_iter_no_change", OV_TypeId_Integer, "5")
};

static const vector<tuple<string, CIdentifier, string>> SVM_SETTING = {
	make_tuple("Test set share", OV_TypeId_Float, "0.2"),
	make_tuple("Labels", OV_TypeId_String, ""),
	make_tuple("penalty", OVPoly_ClassId_Penalty, "l2"),
	make_tuple("loss", OVPoly_ClassId_SVM_Loss, "squared_hinge"),
	make_tuple("dual", OV_TypeId_Boolean, "true"),
	make_tuple("tol", OV_TypeId_Float, "0.0001"),
	make_tuple("C", OV_TypeId_Float, "1.0"),
	make_tuple("multi_class", OVPoly_ClassId_SVM_MultiClass, "ovr"),
	make_tuple("fit_intercept", OV_TypeId_Boolean, "true"),
	make_tuple("intercept_scaling", OV_TypeId_Float, "1"),
	make_tuple("verbose", OV_TypeId_Integer, "0"),
	make_tuple("max_iter", OV_TypeId_Integer, "1000")
};


static void ClearSetting(IBox& box) { while (box.getSettingCount() > 4) { box.removeSetting(4); } }

static bool SetSetting(IBox& box, const vector<tuple<string, CIdentifier, string>>& settings)
{
	for (const auto& t : settings) { box.addSetting(get<0>(t).c_str(), get<1>(t), get<2>(t).c_str()); }
	return true;
}

bool CBoxAlgorithmTrainerMLListener::onSettingValueChanged(IBox& box, const size_t index)
{
	if (index == 3)
	{
		CString value;
		box.getSettingValue(index, value);
		ClearSetting(box);

		if (string(value.toASCIIString()) == toString(EClassifier::NearestCentroid)) { return SetSetting(box, NC_SETTING); }
		if (string(value.toASCIIString()) == toString(EClassifier::NearestNeighbors)) { return SetSetting(box, KNN_SETTING); }
		if (string(value.toASCIIString()) == toString(EClassifier::GaussianNaiveBayes)) { return SetSetting(box, GAUSSIAN_SETTING); }
		if (string(value.toASCIIString()) == toString(EClassifier::StochasticGradientDescent)) { return SetSetting(box, SGD_SETTING); }
		if (string(value.toASCIIString()) == toString(EClassifier::LogisticRegression)) { return SetSetting(box, LR_SETTING); }
		if (string(value.toASCIIString()) == toString(EClassifier::DecisionTree)) { return SetSetting(box, TREE_SETTING); }
		if (string(value.toASCIIString()) == toString(EClassifier::ExtraTrees)) { return SetSetting(box, XTREE_SETTING); }
		if (string(value.toASCIIString()) == toString(EClassifier::Bagging)) { return SetSetting(box, BAGGING_SETTING); }
		if (string(value.toASCIIString()) == toString(EClassifier::RandomForest)) { return SetSetting(box, RF_SETTING); }
		if (string(value.toASCIIString()) == toString(EClassifier::SVM)) { return SetSetting(box, SVM_SETTING); }
		if (string(value.toASCIIString()) == toString(EClassifier::LDA)) { return SetSetting(box, LDA_SETTING); }
		if (string(value.toASCIIString()) == toString(EClassifier::AdaBoost)) { return SetSetting(box, ADA_BOOST_SETTING); }
		if (string(value.toASCIIString()) == toString(EClassifier::MultiLayerPerceptron)) { return SetSetting(box, MLP_SETTING); }
		if (string(value.toASCIIString()) == toString(EClassifier::MDM)) { return SetSetting(box, RMDM_SETTING); }
		if (string(value.toASCIIString()) == toString(EClassifier::TangentSpace)) { return SetSetting(box, RTS_SETTING); }
		return true;
	}
}
