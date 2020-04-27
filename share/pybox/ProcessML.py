# -*- coding: utf-8 -*-
from pyriemann.classification import MDM
from pyriemann.tangentspace import TangentSpace
from pyriemann.estimation import Covariances
from sklearn.pipeline import make_pipeline
from sklearn.tree import DecisionTreeClassifier
from sklearn.linear_model import LogisticRegression, SGDClassifier
from sklearn.neural_network import MLPClassifier
from sklearn.discriminant_analysis import LinearDiscriminantAnalysis
from sklearn.naive_bayes import GaussianNB
from sklearn.svm import SVC
from sklearn.ensemble import RandomForestClassifier, AdaBoostClassifier, ExtraTreesClassifier, BaggingClassifier
from sklearn.neighbors import KNeighborsClassifier, NearestCentroid
from sklearn.metrics import confusion_matrix, classification_report
from collections import defaultdict
import numpy as np
import pickle
import os
from PolyBox import PolyBox
import warnings
warnings.filterwarnings("ignore")


class ProcessML(PolyBox):

    #----------------------------------------
    def __init__(self):
        PolyBox.__init__(self, record=False)
        self.model_path = None
        self.pred_saving_path = None
        self.model = None
        self.predictions = []
        self.shape = None

    #----------------------------------------
    def on_initialize(self):
        # we get the model file
        self.model_path = self.setting['Model filename']

        # we load the model
        try:
            self.model = pickle.load(open(self.model_path, 'rb'))
        except IOError as err:
            print(err)
            print('Please indicate an existing model.')
            self.send_end_stim()

        try:
            self.pred_saving_path = self.setting['Predictions filename']
            if len(self.pred_saving_path.replace(' ', '')) > 0:
                self.pred_saving_path = os.path.abspath(self.pred_saving_path)
        except IOError:
            print('No filename to save predictions has been given, they will not be saved')

    #----------------------------------------
    def on_end_box(self):
        self.save_preds()
        self.make_stats()

    #----------------------------------------
    def list_to_str(self, preds):
        string = ''
        for x in preds:
            string += str(x) + ','
        return string

    #----------------------------------------
    def save_preds(self):

        if self.pred_saving_path is not None and self.pred_saving_path != "":

            preds_str = self.list_to_str(self.predictions)

            with open(self.pred_saving_path, 'wb') as file:
                file.write(preds_str)
            print('Predictions saved in {}\n'.format(self.pred_saving_path))

    #----------------------------------------
    def on_chunk_received(self, chunk, label, shape):

        chunk = np.array(chunk)

        if self.shape is None:
            # Riemanian Geometry
            if self.model.custom_classifier == 'Riemann Minimum Distance to Mean' or self.model.custom_classifier == 'Riemann Tangent Space':
                self.shape = (1, shape[0], shape[1])
            else:
                self.shape = (1, chunk.shape[0])

        chunk = chunk.reshape(self.shape)
        pred = self.model.predict(chunk)

        self.predictions += list(pred)

    #----------------------------------------
    def make_stats(self):

        length = len(self.predictions)
        dictpred = defaultdict(int)

        for elem in self.predictions:
            dictpred[elem] += 1
        print("Metrics : \n")
        print('\n'.join(['{} : {}'.format(l, float(v)/length) for l, v in dictpred.items()]))


box = ProcessML()
