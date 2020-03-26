# -*- coding: utf-8 -*-
'''
 * Software License Agreement (AGPL-3 License)
 *
 * OpenViBE
 * Copyright (C) Inria, 2006-2019
 *
 * Authors
 *
 * 2019, Yannis Bendi-Ouis <yannis.bendiouis@gmail.com>
 * 2019, Jimmy Leblanc <jimmy.leblanc01@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 *
'''

from pandas import Series, DataFrame, read_csv
import numpy as np
import pandas as pd
from natsort import natsorted
from pygame import mixer
from PolyStimulations import Poly_stimulation
import os
import pickle
import random
import inspect

# Dans les settings :
# - "Path directory" : path qui mène au directory de sauvegarde des données. Donc finir par un '/'.
# - "Label_X" : Nom des labels, où X est un nombre. Il en faut autant qu'il y a de labels.
# - "Several CSV" : Boolean ou string qui return "true" ou "false".
# - "Number of folds" : Integer qui indique en combien de différents folds les data doivent être séparées.
# - "Number of actions" : Integer qui indique le nombre d'actions à enregistrer lors d'une session.




ACTION_DURATION = 12 # in seconds
TAMPON_DURATION = 3 # in seconds
BEGIN_RECORD = 2 # in seconds
END_RECORD = ACTION_DURATION
FREQ = 128
NB_POINTS_ONE_RECORD = ACTION_DURATION * FREQ
NB_POINTS_ONE_TAMPON = TAMPON_DURATION * FREQ

CHANNELS_NAME = ['AF3', 'F7', 'F3', 'FC5', 'T7', 'P7', 'O1', 'O2', 'P8', 'T8', 'FC6', 'F4', 'F8', 'AF4'] #14
CHANNELS_STIMULATION = ['Event Id', 'Event Date', 'Event Duration']

PREFIXE_STIM = 'OVPoly_'

def get_path_sounds() :
    path_current_file = inspect.getfile(lambda: None)
    indice = path_current_file.find('pybox-manager')
    return path_current_file[:indice] + 'pybox-manager/Assets/Sounds/'


def ovdf(df, rewrite_stim=True) :
    shape = df.shape
    timer = Series([float(i)/FREQ for i in range(shape[0])])
    epochs = Series([int(i/64) for i in range(shape[0])])

    df.insert(0, 'Time:{}Hz'.format(FREQ), timer)
    df.insert(1, 'Epoch', epochs)
    
    if rewrite_stim : 
        df['Event Id'] = Series([None for i in range(shape[0])])
        df['Event Date'] = Series([None for i in range(shape[0])])
        df['Event Duration'] = Series([None for i in range(shape[0])])

    else :
        # re-compute the time informations for stimulations
        stim = df['Event Id'].values.tolist()
        date = []
        duration = []
        for i, event_id in enumerate(stim) :
            if not pd.isnull(event_id) :
                date += [timer[i]]
                duration += [0]
            else :
                date += [None]
                duration += [None]
        df['Event Date'] = Series(date)
        df['Event Duration'] = Series(duration)

def get_stim_code_from_label(label) :
    key = label.split('_')
    key = "_".join([w[0].upper() + w[1:] for w in key])
    key = PREFIXE_STIM + key
    return Poly_stimulation[key]


class DatasetCreator(OVBox) :

    def __init__(self) :
        OVBox.__init__(self)
        self.dir_name = None
        self.url = None
        self.signalHeader = None
        self.labels_queue = None
        self.current_label = None
        
        self.is_tampon = False
        self.several_csv = False

        self.nb_fold = 0
        self.nb_action_per_session = 0 
        
        self.labels = []
        self.data_recorded = []
        self.labels_order = []
        self.dic_fold = {} 
        self.dic_dicount = {}

    def initialize(self) :

        def verify_labels_correct(self) :
            for label in self.labels :
                try :
                    get_stim_code_from_label(label)         
                except KeyError :
                    raise Exception('Label {} not defined in PolyStimulations. You may want to add it with the manager.'.format(label))

        def get_labels(self) :
            param_names = self.setting.keys()
            param_names = natsorted(param_names)
            for n in param_names :
                if 'Label_' in n :
                    label = self.setting[n]
                    label = label.replace(' ', '_')
                    if len(label) > 0 :
                        self.labels += [label.lower()]

        def verify_sound_label(self) :
            sounds = os.listdir(get_path_sounds())
            for label in self.labels :
                if not label + '.mp3' in sounds :
                    raise Exception('There is no sound for ' + label)

        def init_dict(self) :
            self.dic_fold = {'fold_{}'.format(i) : None for i in range(1, self.nb_fold+1)}
            self.dic_dicount = {'fold_{}'.format(i) : None for i in range(1, self.nb_fold+1)}

        def retrieve_settings(self) :

            # On récupère la booleen indiquant si l'on souhaite plusieurs ou un seul csv par fold
            self.several_csv = self.setting['Several CSV']  
            if self.several_csv == 'true' :
                self.several_csv = True
            elif self.several_csv == 'false' :
                self.several_csv = False
            # On récupère le path du directory où l'on créé les fold
            self.dir_name = self.setting['Path directory']
            if self.dir_name[-1] != '/' :
                self.dir_name += '/'

            # On récupère le nombre de fold
            self.nb_fold = int(self.setting['Number of folds'])
            # On récupère le nombre d'action à record par session
            self.nb_action_per_session = int(self.setting['Number of actions'])
            # On récupère les labels
            get_labels(self)

        def verify_stim_output(self) :
            # Verify that an output stim exist, otherwise prevent the user that the program won't stop
            flag = False
            for out in self.output :
                if out.type() == 'Stimulations' :
                    flag = True
                    break

            if not flag :
                print('WARNING : The DatasetCreator does not have any output Stimulation. The program may never stop.')

        retrieve_settings(self)
        verify_stim_output(self)
        verify_labels_correct(self)
        verify_sound_label(self)
        init_dict(self)

        self.verify_arborescence()
        self.prepare_session()
        self.new_record()

    def process(self) :
        # On parcours tous les inputs
        for inputIndex in range(len(self.input)) :
            for chunkIndex in range(len(self.input[inputIndex])) :

                # Initialisation pour le signal
                if type(self.input[inputIndex][chunkIndex]) == OVStreamedMatrixHeader :
                    self.header_received(inputIndex, chunkIndex)

                # Traitement à effectuer pour chaque chunk reçu
                elif type(self.input[inputIndex][chunkIndex]) == OVStreamedMatrixBuffer :
                    self.chunk_received(inputIndex, chunkIndex)

                # Fin du signal
                elif type(self.input[inputIndex][chunkIndex]) == OVStreamedMatrixEnd :
                    self.end_received(inputIndex, chunkIndex)

    def uninitialize(self) :
        pass



    # -------------- * ------------- * -------------

    def header_received(self, inputIndex, chunkIndex) :
        self.signalHeader = self.input[inputIndex].pop()

    def chunk_received(self, inputIndex, chunkIndex) :
        chunk = self.input[inputIndex].pop()

        # Plusieurs lignes sont envoyées en même temps, il faut les séparer
        indices = [(len(CHANNELS_NAME)*i, len(CHANNELS_NAME)*(i+1)) for i in range(len(chunk)/len(CHANNELS_NAME))]
        for begin, end in indices :
            self.data_recorded += [chunk[begin:end]]
          
        # Fin d'un tampon
        if self.is_tampon and len(self.data_recorded) >= NB_POINTS_ONE_TAMPON :
            self.empty_tampon()
            self.new_record()
        
        # Fin d'un record
        if not self.is_tampon and len(self.data_recorded) >= NB_POINTS_ONE_RECORD :
            if not self.end_record() :
                self.end_of_session()
                self.end_of_box()

    def end_received(self, inputIndex, chunkIndex) :
        self.input[inputIndex].pop()
        print("Error : entry signal stoped.")
        self.end_of_box()

    def end_of_box(self) :
        indice = -1
        for i, out in enumerate(self.output) :
            if out.type() == 'Stimulations' :
                indice = i

        if indice != -1 :
            stimLabel = 'OVTK_StimulationId_ExperimentStop'
            stimCode = OpenViBE_stimulation[stimLabel]
            stimSet = OVStimulationSet(0, self.getCurrentTime())
            stimSet.append(OVStimulation(stimCode, self.getCurrentTime(), 0.))
            self.output[0].append(stimSet)

    # -------------- * ------------- * -------------

    def verify_arborescence(self) :
        # On vérifie que chaque dossier du path existe, sinon on les créé
        # et on vérifie les dicount

        def verify_dir(self) :
            # On vérifie que le dossier existe et ses sous-dossiers, sinon on les créé
            if not os.path.exists(self.dir_name) :
                os.mkdir(self.dir_name, 0775)

        def verify_folds(self) :
            # On vérifie que les dossiers des différents folds existent, sinon on les créés
            for i in range(1, self.nb_fold+1) :
                name = self.dir_name + 'fold_{}/'.format(i)
                if not os.path.exists(name) :
                    os.mkdir(name, 0775)

        def verify_and_load_dicount(self) :
            # On vérifie que les fichiers contenant les compteurs par label existent, sinon on les créé
            for i in range(1, self.nb_fold+1) :
                fold = 'fold_{}'.format(i)
                filename = self.dir_name + fold + '/dicount.pick'

                if not os.path.isfile(filename) :
                    dicount = {l : 0 for l in self.labels}
                    self.dic_dicount[fold] = dicount
                    pickle.dump(dicount, open(filename, 'wb'))
                else :
                    self.dic_dicount[fold] = pickle.load(open(filename, 'rb'))

        verify_dir(self)
        verify_folds(self)
        verify_and_load_dicount(self)



    def prepare_session(self) :

        def create_labels_queue(self) :
            nb_label = len(self.labels)
            nb_total = self.nb_action_per_session / nb_label
            nb_reste = self.nb_action_per_session - nb_total*nb_label
            
            choice_label = []
            tmp = [l for l in self.labels]
            for _ in range(nb_reste) :
                l = random.choice(tmp)
                tmp.remove(l)
                choice_label += [l]

            tmp = [l for l in self.labels]
            self.labels_queue = tmp*nb_total + choice_label
            random.shuffle(self.labels_queue)

        # Prepare la suite aléatoire d'action a exectuer
        create_labels_queue(self)
        
        # Initialise self.dic_fold
        for key in self.dic_fold.keys() :
            self.dic_fold[key] = {l : [] for l in self.labels}

        # Initialize the mixer
        mixer.init()

    
    def empty_tampon(self) :
        # Supprime les données tampons entre deux records
        self.is_tampon = False
        end = TAMPON_DURATION * FREQ
        self.data_recorded = self.data_recorded[end:]
       
    def new_record(self) :
        # Prevent the user a new record begin
        if len(self.labels_queue) > 0 : 
            self.current_label = self.labels_queue.pop(0)
            print('Current label : {}'.format(self.current_label))
            mixer.music.load(get_path_sounds() + '{}.mp3'.format(self.current_label))
            mixer.music.play()
    
    def end_record(self) :
        # Démarre un nouvel enregistrement de 10 sec. Si le dernier est fini, l'enregistre.
        # Return True s'il y a encore d'autres label à étudié pour la session, False sinon.

        def retrieve_record(self) :

            def add_to_fold(self, data, label) :
                # Ajoute les éléments dans la liste data au dic_fold label et mets à jour dic_dicount
                print('hello', self.dic_dicount)
                tmp = [(key, value[label]) for key, value in self.dic_dicount.items()]
                fold = min(tmp, key=lambda x : x[1])[0]
                self.dic_fold[fold][label] += [data]
                self.dic_dicount[fold][label] += len(data)
                self.labels_order += [label]

            # Extract the 10s recorded
            record = self.data_recorded[:NB_POINTS_ONE_RECORD]
            self.data_recorded = self.data_recorded[NB_POINTS_ONE_RECORD:]

            # Extract data between begin and end
            begin = FREQ*BEGIN_RECORD
            end = FREQ*END_RECORD
            data = record[begin:end] 
            add_to_fold(self, data, self.current_label) 

            # End record
            print('Stop.')
            mixer.music.load(get_path_sounds() + 'stop.mp3')
            mixer.music.play()

        retrieve_record(self)
        self.is_tampon = True
        
        return len(self.labels_queue) > 0 


    def end_of_session(self) :

        def maj_dicount(self, fold) :
            filename = self.dir_name + fold + '/dicount.pick'
            dicount = self.dic_dicount[fold]
            pickle.dump(dicount, open(filename, 'wb'))

        def add_label_stimulation(self, data, label) :
            # Add the stimulations indicating the begginning of a label
            length = len(data)
            event_id = [None for _ in range(length)]
            event_date = [None for _ in range(length)]
            event_duration = [None for _ in range(length)]
            event_id[0] = get_stim_code_from_label(label)

            dict_event = {"Event Id" : event_id, "Event Date" : event_date, "Event Duration" : event_duration}
            dict_data = {c : np.array(data)[:,i].tolist() for i, c in enumerate(CHANNELS_NAME)}
            dict_data.update(dict_event)

            return dict_data
        
        def append_data_to_csv(self, data, fold, label, several_csv=False) :
            # Create all csv, either you can us one CSV with stimulations, either one CSV per label
            if several_csv :
                filename = self.dir_name + fold + '/' + label + '.csv'
                columns = CHANNELS_NAME
            else :
                filename = self.dir_name + fold + '/' + fold + '.csv'
                columns = CHANNELS_NAME + CHANNELS_STIMULATION
                data = add_label_stimulation(self, data, label)

            old_df = DataFrame()
            if os.path.isfile(filename) :
                old_df = read_csv(filename).filter(columns)

            add_df = DataFrame(data, columns=columns)
            new_df = old_df.append(add_df, ignore_index=True)

            ovdf(new_df, rewrite_stim=several_csv)
            new_df.to_csv(filename, index=False)


        labels_count = {label : 0 for label in self.labels}

        self.labels_order = {'fold_{}'.format(n) : self.labels_order[self.nb_action_per_session*i : self.nb_action_per_session*(i+1)] 
                            for i in range(self.nb_fold) for n in range(1, self.nb_fold+1)}
        
        for i in range(1, self.nb_fold+1) :
            fold = 'fold_{}'.format(i)

            for label in self.labels_order[fold] :
                count = labels_count[label]
                data = self.dic_fold[fold][label][count]
                labels_count[label] += 1
                append_data_to_csv(self, data, fold, label, several_csv=self.several_csv)

            maj_dicount(self, fold)

box = DatasetCreator()
