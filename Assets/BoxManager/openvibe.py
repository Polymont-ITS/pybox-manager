# -*- coding: utf-8 -*-
#File Name : openvibe.py
#Created By : Aurelien Van Langhenhove

from StimulationsCodes import *
import sys, traceback, collections
from io import StringIO
from pathlib import Path
import re


class NewStd(StringIO):
    def __init__(self):
        StringIO.__init__(self)
    def flush(self):
        self.truncate(0)
        self.seek(0)

    def isempty(self):
        if len(self.len) == 0:
            return True
        else:
            return False

sys.stdout = NewStd()
sys.stderr = NewStd()


def execfileHandlingException(filename, maindictionary):
    print("executing script file")

    # absolute path is ok 
    try:
        maindictionary['box'] = OVBox(default=True) 

        exec(compile(open(filename, "rb").read(), filename, 'exec'), maindictionary) 
        return 0;

    # need relative_path 
    except IOError:
        
        path = str(Path().absolute())
        path +=  '/../../extras/contrib/applications/developer-tools/openvibe-python/ScriptBox/'
        
        try: 
            # we got to get only the filename and not the whole path
            regex = '\/(?!\/)([^\/]+\.py)$'
            res = re.search(regex, filename)

            script_name = res.group(1)
            path += script_name

            maindictionary['box'] = OVBox(default=True) 

            execfile(path, maindictionary) 
            return 0;

        # no regex match with filename
        except AttributeError as err:

            # if we just gave the name of the python script
            try:
                path += filename
                maindictionary['box'] = OVBox(default=True) 
                execfile(path, maindictionary) 
                return 0;
                
            except IOError as err:
                print(err)
                print(traceback.format_exc())
                return -1;
            
        # neither asolute nor relative works 
        except IOError as err:
            print(err)
            print(traceback.format_exc())
            return -1;
        else:
            print("error")
            print(traceback.format_exc())
            return -1;


def decoratorFunction(target):
    """ add a try except block to protect openvibe box in case of exception """
    def wrapper(self):
        try :
            print('using decorator')
            print('Calling function "%s"' % target.__name__)
            return target(self)
        except:
            print(traceback.format_exc())

    return wrapper

class OVChunk(object):
    def __init__(self, startTime, endTime):
        self.startTime = startTime
        self.endTime = endTime


class OVStreamedMatrixHeader(OVChunk):
    def __init__(self, startTime, endTime, dimensionSizes, dimensionLabels):
        OVChunk.__init__(self, startTime, endTime)
        self.dimensionSizes = list(dimensionSizes)
        self.dimensionLabels = list(dimensionLabels)
        
    def getDimensionCount(self):
        return len(self.dimensionSizes)
        
    def getBufferElementCount(self):
        elementCount = 0
        for dimension, size in enumerate(self.dimensionSizes):
            if dimension == 0:
                elementCount = int(size)
            else:
                elementCount *= int(size)
        return elementCount

class OVStreamedMatrixBuffer(OVChunk, list):
    def __init__(self, startTime, endTime, bufferElements):
        OVChunk.__init__(self, startTime, endTime)
        list.__init__(self, bufferElements)

class OVStreamedMatrixEnd(OVChunk):
    pass 


    
class OVSignalHeader(OVStreamedMatrixHeader):
    def __init__(self, startTime, endTime, dimensionSizes, dimensionLabels, samplingRate):
        OVStreamedMatrixHeader.__init__(self, startTime, endTime, dimensionSizes, dimensionLabels)
        self.samplingRate = int(samplingRate)
        
class OVSignalBuffer(OVStreamedMatrixBuffer):
    pass 
    
class OVSignalEnd(OVChunk):
    pass 
    


class OVStimulation(object):
    def __init__(self, identifier, date, duration):
        self.identifier = identifier
        self.date = date
        self.duration = duration

class OVStimulationHeader(OVChunk):
    pass

class OVStimulationSet(OVChunk, list):
    def __init__(self, startTime, endTime):
        list.__init__(self)
        OVChunk.__init__(self, startTime, endTime)
        
    def append(self, item):
        if isinstance(item, OVStimulation):
            list.append(self, item)
        else:
            raise TypeError("The item must be an OVStimulation")

class OVStimulationEnd(OVChunk):
    pass 
    


class OVBuffer(object):
    def __init__(self, inputType):
        self.__deque = collections.deque()
        self.__type = inputType
    def __len__(self):
        return len(self.__deque)
    def __getitem__(self, key):
        return self.__deque[key]
    def __setitem__(self, key, item):
        self.__deque[key] = item
    def __delitem__(self, key):
        del self.__deque[key]
    def append(self, toAppend):
        self.__deque.appendleft(toAppend)
    def pop(self):
        return self.__deque.pop()
    def type(self):
        return self.__type


class OVBox(object):
    def __init__(self, default=False):
        self.input = list()
        self.output = list()
        self.setting = dict()
        self.var = dict()
        self._clock = 0
        self._currentTime = 0.
        self.default= default
    def addInput(self, inputType):
        self.input.append(OVBuffer(inputType))
    def addOutput(self, outputType):
        self.output.append(OVBuffer(outputType))
    def getClock(self):
        return self._clock
    def getCurrentTime(self):
        return self._currentTime
    def initialize(self):
        if self.default == True:
            print("The box instance has not been created by user script, using default one from openvibe.py (dummy box).")
        pass
    def process(self):
        pass
    def uninitialize(self):
        pass
    def realInitialize(self):
        try :
            self.initialize()
        except:
            print(traceback.format_exc())
    def realProcess(self):
        try :
            self.process()
        except:
            print(traceback.format_exc())
    def realUninitialize(self):
        try :
            self.uninitialize()
        except:
            print(traceback.format_exc())





def get_label_from_stim(stim) :
    inv_dictstim = {v : k for k,v in OpenViBE_stimulation.items()}
    label = inv_dictstim[stim.identifier][7:].lower()
    label = ' '.join(label.split('_'))
    return label

class PolyBox(OVBox) :

    def __init__(self, record=True) :
        OVBox.__init__(self)
        self.acquiring_channel = []
        self.signalHeader = []
        self.data = {}
        self.record = record

        self.mode = ''
        self.current_stimulation = None
        self.labels = []

    def initialize(self) :

        def verify_entry(self) :
            # Verify that the inputs corresponds to ov-mode or poly-mode
            # ov-mode : 1 stimulations and 1 streamed matrix
            # poly-mode : several streamed-matrix
            # others : others -> warning
            list_entry_type = [entry.type() for entry in self.input]
            nb_matrix = list_entry_type.count('StreamedMatrix')
            nb_stim = list_entry_type.count('Stimulations')
            nb_signal = list_entry_type.count('Signal')

            if nb_stim == 1 and nb_matrix == 1 and nb_signal == 0 :
                # ov-mode
                self.mode = 'ov-mode'
            elif nb_stim == 0 and nb_matrix >= 1 and nb_signal == 0 :
                # poly-mode
                self.mode = 'poly-mode'
            else :
                raise Exception("ERROR : Entry of the box does not corresponds to any mode. \
                You can use 1 stimulations and 1 streamed matrix, or several streamed matrix. \
                But you can not use {} StreamedMatrix, {} Stimulations and {} Signal entry.".format(nb_matrix, nb_stim, nb_signal))

        def verify_stim_output(self) :
            # Verify that an output stim exist, otherwise prevent the user that the program won't stop
            flag = False
            for out in self.output :
                if out.type() == 'Stimulations' :
                    flag = True
                    break

            if not flag :
                print('WARNING : The DatasetCreator does not have any output Stimulation. The program may never stop.')

        def get_labels(self) :
            # retrieve labels in form : label1, label2, label3, mon label4
            # Useless if you are in OV-MODE
            if 'Labels' in self.setting.keys() : 
                string = self.setting['Labels']
                if len(string) > 0 :
                    labels_cut = string.lower().split(',')
                    for label in labels_cut :
                        self.labels += ['_'.join([w for w in label.split(' ') if w != ''])]

        def init_acquiring_channel(self) :
            # We get data for every input channel
            for _ in range(len(self.input)) :
                self.acquiring_channel += [False]
                self.signalHeader += [None]

        verify_entry(self)
        verify_stim_output(self)
        get_labels(self)
        init_acquiring_channel(self)
        self.on_initialize()

    def process(self) :
        # we go through every input
        for inputIndex in range(len(self.input)) :
            for chunkIndex in range(len(self.input[inputIndex])) :

                # Signal init
                if type(self.input[inputIndex][chunkIndex]) == OVStreamedMatrixHeader :
                    self.header_received(inputIndex, chunkIndex)

                # Process every chunk received 
                elif type(self.input[inputIndex][chunkIndex]) == OVStreamedMatrixBuffer :
                    self.chunk_received(inputIndex, chunkIndex)

                # End of signal
                elif type(self.input[inputIndex][chunkIndex]) == OVStreamedMatrixEnd :
                    self.end_received(inputIndex, chunkIndex)

                # Stimulations init
                elif type(self.input[inputIndex][chunkIndex]) == OVStimulationHeader :
                    self.header_received(inputIndex, chunkIndex)

                # Process every stimulation
                elif type(self.input[inputIndex][chunkIndex]) == OVStimulationSet :
                    self.stimulation_received(inputIndex, chunkIndex)

                # End of stim
                elif type(self.input[inputIndex][chunkIndex]) == OVStimulationEnd :
                    self.end_received(inputIndex, chunkIndex)

    def uninitialize(self) :
        pass


    # ------- * -------- * ---------


    def header_received(self, inputIndex, chunkIndex) :
        header = self.input[inputIndex].pop()
        self.signalHeader[inputIndex] = header
        self.acquiring_channel[inputIndex] = True
        self.on_header_received(header)

    def chunk_received(self, inputIndex, chunkIndex) :
        chunk = list(self.input[inputIndex].pop())
        if self.acquiring_channel[inputIndex] :

            # We look for the best key to use in function of mode and settings labels.
            key = None
            if self.mode == 'poly-mode' :
                if len(self.labels) > 0 : 
                    key = self.labels[inputIndex]
                else :
                    key = inputIndex

            elif self.mode == 'ov-mode' :
                key = get_label_from_stim(self.current_stimulation)                            

            if self.record :
                try :
                    self.data[key].append(chunk)
                except KeyError :
                    self.data[key] = [chunk]

            shape = tuple(self.signalHeader[inputIndex].dimensionSizes)
            self.on_chunk_received(chunk, key, shape)
    
    def stimulation_received(self, inputIndex, chunkIndex) :
        stim_list = self.input[inputIndex].pop()
        if len(stim_list) > 0 :
            self.current_stimulation = stim_list[0]

    def end_received(self, inputIndex, chunkIndex) :
        self.acquiring_channel[inputIndex] = False
        self.input[inputIndex].pop()
        if not self.is_acquiring() :
            print("Fin de l'acquisition des données...")
            self.on_end_box()
            self.send_end_stim()

    def is_acquiring(self) :
        # Return false when all inputs of type StreamedMatrix received End flag.
        nb_inputs = len(self.input)
        for i in range(nb_inputs) :
            acquiring = self.acquiring_channel[i]
            kind = self.input[i].type()
            if kind == 'StreamedMatrix' and acquiring :
                return True
        return False

    def send_end_stim(self) :
        indice = -1
        for i, out in enumerate(self.output) :
            if out.type() == 'Stimulations' :
                indice = i

        if indice != -1 :
            stimLabel = 'OVTK_StimulationId_ExperimentStop'
            stimCode = OpenViBE_stimulation[stimLabel]
            stimSet = OVStimulationSet(0, self.getCurrentTime())
            stimSet.append(OVStimulation(stimCode, self.getCurrentTime(), 0.))
            self.output[indice].append(stimSet)

    # ---------- * -------------- * ---------------

    def on_initialize(self) :
        pass

    def on_header_received(self, header) :
        pass

    def on_chunk_received(self, chunk, label, shape) :
        pass

    def on_end_box(self) :
        pass
