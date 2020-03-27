#pragma once


//___________________________________________________________________//
//                                                                   //
// Box                                                               //
//___________________________________________________________________//
//                                                                   //

// <tag> Tag Box Declaration
#define OVP_ClassId_BoxAlgorithm_LDADesc                 OpenViBE::CIdentifier(0x5F536604, 0x31561A20)
#define OVP_ClassId_BoxAlgorithm_LDA                 OpenViBE::CIdentifier(0x47EE7368, 0x4A4547BE)
#define OVP_ClassId_BoxAlgorithm_KNearestNeighborsDesc                 OpenViBE::CIdentifier(0x370856AD, 0x252D258F)
#define OVP_ClassId_BoxAlgorithm_KNearestNeighbors                 OpenViBE::CIdentifier(0x47EB496C, 0x047D69BF)
#define OVP_ClassId_BoxAlgorithm_GaussianNBDesc                 OpenViBE::CIdentifier(0x0EBE4757, 0x190530FF)
#define OVP_ClassId_BoxAlgorithm_GaussianNB                 OpenViBE::CIdentifier(0x47E81F6F, 0x3EB50BC0)
#define OVP_ClassId_BoxAlgorithm_Extra_TreesDesc                 OpenViBE::CIdentifier(0x66733800, 0x0CDC3C6E)
#define OVP_ClassId_BoxAlgorithm_Extra_Trees                 OpenViBE::CIdentifier(0x47E47573, 0x78ED2DC1)
#define OVP_ClassId_BoxAlgorithm_Decision_Tree_ClassifierDesc                 OpenViBE::CIdentifier(0x3E2828A9, 0x00B447DD)
#define OVP_ClassId_BoxAlgorithm_Decision_Tree_Classifier                 OpenViBE::CIdentifier(0x47E14B77, 0x33244FC1)
#define OVP_ClassId_BoxAlgorithm_DataVizDesc                 OpenViBE::CIdentifier(0x15DE1952, 0x748C534D)
#define OVP_ClassId_BoxAlgorithm_DataViz                 OpenViBE::CIdentifier(0x47DE217A, 0x6D5C71C2)
#define OVP_ClassId_BoxAlgorithm_DatasetCreatorDesc                 OpenViBE::CIdentifier(0x45497AA5, 0x5C3B6A2B)
#define OVP_ClassId_BoxAlgorithm_DatasetCreator                 OpenViBE::CIdentifier(0x47D74D81, 0x61CC35C3)
#define OVP_ClassId_BoxAlgorithm_BaggingDesc                 OpenViBE::CIdentifier(0x1CFE6B4E, 0x5012759A)
#define OVP_ClassId_BoxAlgorithm_Bagging                 OpenViBE::CIdentifier(0x47D42385, 0x1C0457C4)
#define OVP_ClassId_BoxAlgorithm_ADADesc                 OpenViBE::CIdentifier(0x74B45BF7, 0x43EA010A)
#define OVP_ClassId_BoxAlgorithm_ADA                 OpenViBE::CIdentifier(0x47D17989, 0x563C79C5)




//___________________________________________________________________//
//                                                                   //
// Custom Settings                                                   //
//___________________________________________________________________//
//                                                                   //

// <tag> Custom Type Settings

#define OVPoly_ClassId_Classifier_Algorithm               OpenViBE::CIdentifier(0x73AE164D, 0xEA21AB0A)
#define OVPoly_ClassId_Knn_Algorithm               OpenViBE::CIdentifier(0xBF763F3B, 0x303EC694)
#define OVPoly_ClassId_Knn_Weights               OpenViBE::CIdentifier(0x178AF88A, 0xEC44DFF3)
#define OVPoly_ClassId_Metric               OpenViBE::CIdentifier(0xA04E99E9, 0x020A6874)
#define OVPoly_ClassId_SVM_Loss             OpenViBE::CIdentifier(0xF05612EC, 0x139AFC45)
#define OVPoly_ClassId_Penalty          OpenViBE::CIdentifier(0xBA4193F2, 0xCD152A47)
#define OVPoly_ClassId_SVM_MultiClass       OpenViBE::CIdentifier(0x743F2BC9, 0xA84BC9DF)
#define OVPoly_ClassId_Criterion                           OpenViBE::CIdentifier(0x7455C643, 0x5D1E74E7)


// Logistic regression 
#define OVPoly_ClassId_Log_reg_solver                                   OpenViBE::CIdentifier(0x8CF680DF, 0x9EA77031)
#define OVPoly_ClassId_Log_reg_multi_class                               OpenViBE::CIdentifier(0xC0B45F28, 0x13AEBFD2)

// Decision Tree
#define OVPoly_ClassId_DecisionTree_splitter                            OpenViBE::CIdentifier(0x4E3C6F6F, 0x9AC56CEA)

// MLP
#define OVPoly_ClassId_MLP_activation                                         OpenViBE::CIdentifier(0x324DA925, 0x58A1BCD9)
#define OVPoly_ClassId_MLP_solver                                              OpenViBE::CIdentifier(0xAD038F27, 0x9B0AB84F)
#define OVPoly_ClassId_MLP_learning_rate                                     OpenViBE::CIdentifier(0x5B10D3BC, 0x57658821)

// SGD
#define OVPoly_ClassId_SGD_loss                                        OpenViBE::CIdentifier(0x57CA4145, 0xB25841A5)
#define OVPoly_ClassId_SGD_learning_rate                                        OpenViBE::CIdentifier(0xB6075C69, 0x93BE696B)

// LDA
#define OVPoly_ClassId_LDA_solver                                        OpenViBE::CIdentifier(0x2B78F491, 0xB86A6DE7)

// ADA 
#define OVPoly_ClassId_ADA_algorithm                                        OpenViBE::CIdentifier(0x27025481, 0x09BEDC31)