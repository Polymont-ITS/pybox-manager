#pragma once


//___________________________________________________________________//
//                                                                   //
// Box                                                               //
//___________________________________________________________________//
//                                                                   //

// <tag> Tag Box Declaration
#define OVP_ClassId_BoxAlgorithm_LDADesc                 OpenViBE::CIdentifier(0x252850E6, 0x3BAD19C9)
#define OVP_ClassId_BoxAlgorithm_LDA                 OpenViBE::CIdentifier(0x3F142126, 0x1D9E759B)
#define OVP_ClassId_BoxAlgorithm_KNearestNeighborsDesc                 OpenViBE::CIdentifier(0x7CDE418F, 0x2F842538)
#define OVP_ClassId_BoxAlgorithm_KNearestNeighbors                 OpenViBE::CIdentifier(0x3F11772A, 0x57D6179B)
#define OVP_ClassId_BoxAlgorithm_GaussianNBDesc                 OpenViBE::CIdentifier(0x54933238, 0x235C30A8)
#define OVP_ClassId_BoxAlgorithm_GaussianNB                 OpenViBE::CIdentifier(0x3F0D4D2D, 0x120E399C)
#define OVP_ClassId_BoxAlgorithm_Extra_TreesDesc                 OpenViBE::CIdentifier(0x2C4822E1, 0x17333C17)
#define OVP_ClassId_BoxAlgorithm_Extra_Trees                 OpenViBE::CIdentifier(0x3F0A2331, 0x4C465B9D)
#define OVP_ClassId_BoxAlgorithm_Decision_Tree_ClassifierDesc                 OpenViBE::CIdentifier(0x03FE138B, 0x0B0B4786)
#define OVP_ClassId_BoxAlgorithm_Decision_Tree_Classifier                 OpenViBE::CIdentifier(0x3F077935, 0x067D7D9E)
#define OVP_ClassId_BoxAlgorithm_DataVizDesc                 OpenViBE::CIdentifier(0x336974DD, 0x72BA5E65)
#define OVP_ClassId_BoxAlgorithm_DataViz                 OpenViBE::CIdentifier(0x3F00253C, 0x7AED419F)
#define OVP_ClassId_BoxAlgorithm_DatasetCreatorDesc                 OpenViBE::CIdentifier(0x0B1E6586, 0x669269D4)
#define OVP_ClassId_BoxAlgorithm_DatasetCreator                 OpenViBE::CIdentifier(0x3EFD7B3F, 0x352563A0)
#define OVP_ClassId_BoxAlgorithm_BaggingDesc                 OpenViBE::CIdentifier(0x62D45630, 0x5A697543)
#define OVP_ClassId_BoxAlgorithm_Bagging                 OpenViBE::CIdentifier(0x3EFA5143, 0x6F5D05A0)
#define OVP_ClassId_BoxAlgorithm_ADADesc                 OpenViBE::CIdentifier(0x3A8946D9, 0x4E4100B2)
#define OVP_ClassId_BoxAlgorithm_ADA                 OpenViBE::CIdentifier(0x3EF72747, 0x299527A1)




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