# -*- coding: utf-8 -*-
from pyriemann.classification import MDM
from pyriemann.tangentspace import TangentSpace
from pyriemann.estimation import Covariances
import time
from sklearn.pipeline import make_pipeline
from sklearn.tree import DecisionTreeClassifier
from sklearn.linear_model import LogisticRegression, SGDClassifier
from sklearn.neural_network import MLPClassifier
from sklearn.discriminant_analysis import LinearDiscriminantAnalysis
from sklearn.naive_bayes import GaussianNB
from sklearn.svm import LinearSVC
from sklearn.ensemble import RandomForestClassifier, AdaBoostClassifier, ExtraTreesClassifier, BaggingClassifier
from sklearn.neighbors import KNeighborsClassifier, NearestCentroid
from sklearn.metrics import confusion_matrix, classification_report
from natsort import natsorted
import random
import pickle
import numpy as np
from collections import defaultdict
from PolyBox import PolyBox
import warnings
warnings.filterwarnings("ignore")


#----------------------------------------
def train_test_split(data_dict, test_size=0.2):
    def create_xy(data_dict):
        # used to shuffle data in a way that prevent any training data to be in the test set
        y = np.array([])
        for label in data_dict:
            y_tmp = np.array([label for _ in data_dict[label]])
            y = np.concatenate((y, y_tmp))

        array = []
        for label in data_dict:
            array.append(data_dict[label])

        x = np.concatenate(array, axis=0)
        # shuffling
        permutation = np.random.permutation(x.shape[0])

        permut = True
        if permut:
            x = x[permutation]
            y = y[permutation]

        return x, y

    dict_train = {}
    dict_test = {}

    for key, data in data_dict.items():
        icut = int(len(data)*test_size)
        dict_test[key] = data[:icut]
        dict_train[key] = data[icut:]

    x_train, y_train = create_xy(dict_train)
    x_test, y_test = create_xy(dict_test)

    return np.array(x_train), np.array(y_train), np.array(x_test), np.array(y_test)


#----------------------------------------
class TrainerML(PolyBox):

    #----------------------------------------
    # is exec first
    def __init__(self):
        PolyBox.__init__(self)
        self.model_path = None
        self.config_path = None
        self.save_path = None
        self.model = None
        self.clf = None
        self.config = None
        self.std_settings = []
        self.clf_dependant_settings = None

    #----------------------------------------
    def on_initialize(self):

        try:
            self.model_path = self.setting['Filename to save model to']
        except KeyError:
            self.model_path = ''
        if self.model_path == '':
            print('No correct file location has been given for saving the model, thus it won\'t be saved')

        try:
            self.test_set_share = float(self.setting['Test set share'])

            # wrong value
            diff = (1 - self.test_set_share)
            if diff <= 0 or diff > 1:
                self.test_set_share = 0
                print('The value of the test set share must be between 0 and 1 (1 not included), no prediction will be performed')

        except KeyError:
            self.test_set_share = 0
            print('The value of the test set share must be between 0 and 1 (1 not included), no prediction will be performed')

        try:
            self.save_path = self.setting['Filename to load model from']
            self.model = pickle.load(open(self.save_path, 'rb'))
        except KeyError:
            self.save_path = ''
            print('No correct location has been given to load the model from, thus a new model will be created.')

        # if model doesn't exist we will init a new one with params from the box
        # FileNotFoundError doesn't exist in Python 2.7
        except IOError:
            print('No correct location has been given to load the model from, thus a new model will be created.')

            # special case for Riemannian Geometry because it needs a pipeline
            clf = self.setting['Classifier']

            try:
                discriminator, _ = self.map_clf(self.setting['Discriminator'])
            except KeyError:
                discriminator = None

            if clf == 'Riemann Tangent Space':

                if discriminator is not None:
                    self.clf = make_pipeline(Covariances(), TangentSpace(metric='riemann'), discriminator())
                else:
                    self.clf = make_pipeline(Covariances(), TangentSpace(metric='riemann'), LinearDiscriminantAnalysis())

            elif clf == 'Riemann Minimum Distance to Mean':
                if discriminator is not None:
                    self.clf = make_pipeline(Covariances(), MDM(metric=dict(mean='riemann', distance='riemann')), discriminator())
                else:
                    self.clf = make_pipeline(Covariances(), MDM(metric=dict(mean='riemann', distance='riemann')))

            else:
                self.clf, _ = self.map_clf(clf)
                self.init_params()

                try:
                    self.clf = self.clf(**self.clf_dependant_settings)
                except TypeError:
                    self.clf = self.clf()

    #----------------------------------------
    def init_params(self):

        # default settings
        self.std_settings = ['Classifier', 'Discriminator', 'Filename to load model from', 'Test set share',
                             'Filename to save configuration to', 'Filename to save model to', 'Clock frequency (Hz)', 'Labels']
        settings = [key for key in self.setting.keys()]

        # we get only settings that are clf relevant
        clf_dependant_settings = list(set(settings) - set(self.std_settings))
        clf_dependant_settings = dict((k, v) for k, v in self.setting.items() if (k in clf_dependant_settings and len(v) > 0))

        # we convert values that need to be
        for k, v in clf_dependant_settings.items():

            try:
                expr = eval(v)
                clf_dependant_settings[k] = expr
            except:
                if v.lower() == 'true':
                    clf_dependant_settings[k] = True
                elif v.lower() == 'false':
                    clf_dependant_settings[k] = False
                elif v.lower() == 'none':
                    clf_dependant_settings[k] = None
                else:
                    pass

        self.clf_dependant_settings = clf_dependant_settings

    #----------------------------------------
    def map_clf(self, classifier):
        """
        Returns the correct algorithm according to the classifier string
        """

        switcher = {
            '': None,
            'None': None,
            'Nearest Centroid': NearestCentroid,
            'Nearest Neighbors Classifier': KNeighborsClassifier,
            'Gaussian Naive Bayes': GaussianNB,
            'Stochastic Gradient Descent': SGDClassifier,
            'Logistic Regression': LogisticRegression,
            'Decision Tree Classifier': DecisionTreeClassifier,
            'Extra Trees': ExtraTreesClassifier,
            'Bagging': BaggingClassifier,
            'Random Forest': RandomForestClassifier,
            'Support Vector Machine': LinearSVC,
            'Linear Discriminant Analysis': LinearDiscriminantAnalysis,
            'AdaBoost': AdaBoostClassifier,
            'Multi Layer Perceptron': MLPClassifier,
            'Linear SVC': LinearSVC,
        }
        clf = switcher.get(classifier, lambda: 'unknown classifier')
        return clf, switcher

    #----------------------------------------
    def on_chunk_received(self, chunk, label, shape):

        # special case for riemannian geometry
        if self.setting['Classifier'] == 'Riemann Minimum Distance to Mean' or self.setting['Classifier'] == 'Riemann Tangent Space':
            numpyBuffer = np.array(chunk).reshape(shape)
            self.data[label][-1] = numpyBuffer

    #----------------------------------------
    def on_end_box(self):
        try:
            self.train()
            self.save()
        except Exception as e:
            print(e)
            self.send_end_stim()

    #----------------------------------------
    def train(self):
        x_train, y_train, x_test, y_test = train_test_split(self.data, self.test_set_share)

        if self.model != None:
            self.clf = self.model

        self.clf.fit(x_train, y_train)

        # to be used in ProcessML
        self.clf.custom_classifier = self.setting['Classifier']

        if x_test.shape[0] > 0:
            predictions = self.clf.predict(x_test)
            report = classification_report(y_test, predictions, labels=self.data.keys())
            matrix = confusion_matrix(y_test, predictions)

            print("Report :\n{}\n".format(report))
            print("Confusion Matrix : \n{}\n".format(matrix))
        print("Fin de l'entrainement...")

    #----------------------------------------
    def save(self):
        if self.model_path != "":
            pickle.dump(self.clf, open(self.model_path, 'wb'))
            print('Model saved in {}\n'.format(self.model_path))


box = TrainerML()
