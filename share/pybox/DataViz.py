# -*- coding: utf-8 -*-
from matplotlib import pyplot as plt
from sklearn.discriminant_analysis import LinearDiscriminantAnalysis as LDA
from sklearn.decomposition import PCA
import numpy as np
import pandas as pd
import pickle
from PolyBox import PolyBox
import warnings
warnings.filterwarnings("ignore")


flag_3D = True
try:
    from mpl_toolkits.mplot3d import Axes3D
except:
    print('Unable to import Axes3D from mpl_toolkits.mplot3d, 3D visualization disabled.')
    flag_3D = False


class DataViz(PolyBox):

    def __init__(self):
        PolyBox.__init__(self)

        self.x_data = []
        self.y_data = []
        self.model = None

        self.path_load_model = ''
        self.path_save_model = ''
        self.algo = ''
        self.dimension_reduction = -1

    def on_initialize(self):
        # We retrieve the setting from OpenViBE

        def retrieve_path_save_model(self):
            try:
                self.path_save_model = self.setting["Path to save the model"]
            except KeyError:
                pass
            if self.path_save_model == '':
                print('No path has been given to save the model, thus it won\'t be saved.')

        def retrieve_path_load_model(self):
            try:
                self.path_load_model = self.setting["Path to load the model"]
            except KeyError:
                pass
            if self.path_load_model == '':
                print('No path has been given to load the model, thus a new model will be created.')

        def retrieve_algo(self):
            try:
                self.algo = self.setting['Algorithm (PCA or LDA)'].upper()
            except KeyError:
                pass
            if self.algo == '':
                print('No algo has been given to the model, default algo is PCA.')
                self.algo = 'PCA'

        def retrieve_dimension_reduction(self):
            try:
                self.dimension_reduction = int(self.setting['Dimension reduction'])
            except KeyError:
                pass
            except ValueError:
                print('{} is not a number. Please use 2 or 3 dimensions only.'.format(self.setting['Dimension reduction']))

            if self.dimension_reduction == -1:
                print('No dimension reduction has been given, default value is 2.')
                self.dimension_reduction = 2

            elif self.dimension_reduction == 3 and not flag_3D:
                print('3D disabled, cannot show data in 3 dimensions. Default dimension is 2.')
                self.dimension_reduction = 3

        retrieve_path_save_model(self)
        retrieve_path_load_model(self)
        retrieve_algo(self)
        retrieve_dimension_reduction(self)
    def on_end_box(self):
        self.prepare_data()
        self.make_model_and_transform()
        self.make_plot()

    # ---------

    def prepare_data(self):
        for label in self.data.keys():
            self.x_data += self.data[label]
            self.y_data += [label for _ in range(len(self.data[label]))]

    def make_model_and_transform(self):
        # load the model if it exists, else create a new one
        # then transform the data

        def load_model(self):
            model = pickle.load(open(self.path_load_model, 'rb'))
            print('Model load from {}.'.format(self.path_load_model))
            return model

        def save_model(self):
            pickle.dump(self.model, open(self.path_save_model, 'wb'))
            print('Dataviz model saved in {}'.format(self.path_save_model))

        def map_algo(self):
            switcher = { 'LDA': LDA, 'PCA': PCA }
            clf = switcher.get(self.algo)
            return clf, switcher

        def create_fit_model(self):
            clf, _ = map_algo(self)
            clf = clf(n_components=self.dimension_reduction)
            if self.algo == 'PCA':
                clf.fit(self.x_data)
            elif self.algo == 'LDA':
                clf.fit(self.x_data, self.y_data)
            else:
                raise Exception('{} is not known as an Algorithm. Please use PCA or LDA.'.format(self.algo))
            return clf

        # Load or create the model
        if len(self.path_load_model) > 0:
            self.model = load_model(self)
        else:
            self.model = create_fit_model(self)

        # Save the model
        if self.path_save_model != '':
            save_model(self)

        # Transform data
        self.x_data = self.model.transform(self.x_data)
        self.y_data = np.array(self.y_data)
    def make_plot(self):

        fig = plt.figure(figsize=(12, 12))

        all_labels = list(self.data.keys())
        colors = np.array([all_labels.index(label) for label in self.y_data])

        if self.dimension_reduction == 2:

            ax = plt.axes()

            for label in all_labels:
                ax.text(self.x_data[self.y_data == label, 0].mean(), self.x_data[self.y_data == label, 1].mean(),
                        label, horizontalalignment='center', bbox=dict(alpha=0.5, edgecolor='w', facecolor='w'))

            ax.scatter(self.x_data[:, 0], self.x_data[:, 1], alpha=0.5, c=colors, cmap='Spectral', edgecolor='g')

            plt.show()
        elif self.dimension_reduction == 3:

            ax = Axes3D(fig)

            for label in all_labels:
                ax.text3D(self.x_data[self.y_data == label, 0].mean(), 
                          self.x_data[self.y_data == label, 1].mean(),
                          self.x_data[self.y_data == label, 2].mean(),
                          label, horizontalalignment='center', bbox=dict(alpha=0.5, edgecolor='w', facecolor='w'))

            ax.scatter(self.x_data[:, 0], self.x_data[:, 1], self.x_data[:, 2],
                       alpha=0.5, c=colors, cmap='Spectral', edgecolor='g')

            plt.show()


box = DataViz()

