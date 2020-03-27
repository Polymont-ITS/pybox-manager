# -*- coding: utf-8 -*-

import re
import os
import sys
import shutil
import platform
import subprocess
import time
import inspect
import ast
import copy
from pathlib import Path
from collections import namedtuple
from shutil import copyfile
from filecmp import cmp as compare_file

from PyQt5.QtWidgets import QMessageBox



system = platform.system()
if system != 'Linux' and system != 'Windows':
    raise Exception("OS {} is not handled with that script.".format(system))

def find_folders() :
    """Find openvibe and the manager folders."""

    current_file = inspect.getfile(lambda : None)
    current_file = Path(current_file)
    manager_folder = str(current_file.parent) + '/'
    openvibe_folder = str(current_file.parent.parent.parent.parent.parent.parent) + '/'
    return manager_folder, openvibe_folder

def find_all_stims():
    """Retrieve all stimulation from Poly_Stimulation"""

    prefixe_stim = 'OVPoly_'
    stims = [k for k in Poly_stimulation.keys() if prefixe_stim in k]
    stims = [k[len(prefixe_stim):] for k in stims]
    return stims

def find_all_custom_settings(manager_folder) :
    """ Generate a list containing all custom settings and their values"""
    
    def get_custom_settings(path_header, prefixe) :
        with open(path_header, 'r') as f :
            text  = f.read()
            matchs = re.findall(r"{}[a-zA-Z0-9_]+".format(prefixe), text)
        return matchs

    def get_custom_settings_confirmed(path_cpp, custom_settings) :
        with open(path_cpp, 'r') as f :
            text = f.read()
            custom_settings_confirmed = []

            for ct in custom_settings :
                match = re.search(r"rPluginModuleContext\.getTypeManager\(\)\.registerEnumerationType\({}, \"[a-zA-Z0-9_ \.]+\"\);".format(ct), text)
                if match is not None :
                    custom_settings_confirmed += [ct]
        return custom_settings_confirmed

    def get_custom_values(path_cpp, custom_settings, prefixe) :
        custom_values = {}

        with open(path_cpp, 'r') as f :
            text = f.read()

            for ct in custom_settings :
                matchs = re.findall(r'rPluginModuleContext\.getTypeManager\(\)\.registerEnumerationEntry\({}, "[0-9a-zA-Z_ \.]+", [0-9]+\);'.format(ct), text)
                values = []

                for match in matchs :
                    m_text = match.split('"')[1]
                    m_id = int(match.split(',')[-1][1:-2])
                    values += [Value(text=m_text, id=m_id)]

                cs_name = ct[len(prefixe):]
                custom_values[cs_name] = Custom_Setting(ct, values)

        return custom_values

    prefixe = 'OVPoly_ClassId_'
    path_header = "{}/src/ovp_defines.h".format(manager_folder)
    path_cpp = "{}/src/ovp_main.cpp".format(manager_folder)

    custom_settings = get_custom_settings(path_header, prefixe)
    custom_settings = get_custom_settings_confirmed(path_cpp, custom_settings)
    custom_values = get_custom_values(path_cpp, custom_settings, prefixe)

    return custom_values

def find_all_boxes(manager_folder, io_dic_type, settings_dic_type):
    """Generate a list containing all the existing box inside"""

    path_all_boxes = "{}/src/box-algorithms/".format(manager_folder)
    list_all_files = os.listdir(path_all_boxes)

    directories = {}
    for elem in list_all_files:
        if elem[:3] == 'ovp' :

            with open(path_all_boxes + elem, 'r') as f:
                file_h = f.read()

                # Path Script
                script = re.findall(
                    r"m_sScriptFilename = \"(.*.py)\";", file_h)[0]

                # Description
                desc = re.findall(r"virtual OpenViBE::CString getShortDescription\(void\) const    "
                                  r"{ return OpenViBE::CString\(\"(.*?)\"\); }", file_h, flags=re.DOTALL)[0]
                desc = desc.replace("\"", "\\\"")

                author = re.search(r'virtual OpenViBE::CString getAuthorName\(void\) const          '
                    '{ return OpenViBE::CString\(\"[a-zA-Z 0-9\-&]*\"\); }', file_h).group()
                author = author[89:-5]

                category = re.search(r'virtual OpenViBE::CString getCategory\(void\) const            '
                    '{ return OpenViBE::CString\(\"[/a-zA-Z ]+\"\); }', file_h).group()
                category = category[89:-5]
                category = category[16:]

                # Creation of the object

                # workaround to allow creation of box with spaces
                boxname = elem[3:-2]
                name = boxname.replace('_',' ')

                box = BoxPython(name=name, filename= boxname, desc=desc, path_script=script)
                box.author = author
                box.category = category

                # Settings
                setts = re.findall(r"(?<!//)prototype.addSetting\(\"(.*)\", "
                                   r"((OV_TypeId|OVPoly)_[a-z_A-Z]*), \"(.*)\"\);", file_h)
                                #    r"(OV_TypeId_[a-z_A-Z]*), \"(.*)\"\);", file_h)
                
                # Same value as the one in counters in the display part
                settings_compt = 2
                for sett in setts:
                    if sett[0] != 'Clock frequency (Hz)':
                        keys = list(settings_dic_type.keys())
                        values = list(settings_dic_type.values())


                        box.settings[settings_compt] = [
                            sett[0], keys[values.index(sett[1])], sett[3]]
                        settings_compt += 1

                # inputs
                inputs = re.findall(
                    r"(?<!//)prototype.addInput  \(\"(.*)\", (OV_TypeId_[a-zA-Z]*)\);", file_h)
            
                # Same value as the one in counters in the display part
                inputs_compt = 2
                keys = list(io_dic_type.keys())
                values = list(io_dic_type.values())
                for inp in inputs:
                    box.inputs[inputs_compt] = [
                        inp[0], keys[values.index(inp[1])]]
                    inputs_compt += 1

                # outputs
                outputs = re.findall(
                    r"(?<!//)prototype.addOutput \(\"(.*)\", (OV_TypeId_[a-zA-Z]*)\);", file_h)
                # Same value as the one in counters in the display part
                outputs_compt = 2
                for out in outputs:
                    box.outputs[outputs_compt] = [
                        out[0], keys[values.index(out[1])]]
                    outputs_compt += 1

                # modify permission
                permission_input = re.search(r"(\/\/)?prototype\.addFlag\(OpenViBE::Kernel::BoxFlag_CanModifyInput\);", file_h)
                permission_output = re.search(r"(\/\/)?prototype\.addFlag\(OpenViBE::Kernel::BoxFlag_CanModifyOutput\);", file_h)
                permission_setting = re.search(r"(\/\/)?prototype\.addFlag\(OpenViBE::Kernel::BoxFlag_CanModifySetting\);", file_h)

                if permission_input.group()[:2] == '//' :
                    box.modify_inputs = False
                if permission_output.group()[:2] == '//' :
                    box.modify_outputs = False
                if permission_setting.group()[:2] == '//' :
                    box.modify_settings = False

            directories[box.name] = box

    return directories

def warning_msg(msg) :
    """Show a warning message : msg in the manager to the user."""
    box = QMessageBox()
    box.setIcon(QMessageBox.Warning)
    box.setText(msg)
    box.setWindowTitle('Warning')
    box.exec_()

def info_msg(msg) :
    """Show an information msg in the manager to the user."""
    box = QMessageBox()
    box.setIcon(QMessageBox.Information)
    box.setText(msg)
    box.setWindowTitle('Information')
    box.exec_()

def insert_line_in_file(filename, string, tag):
    """Insert the line string in the file filename just after the first
    line containing tag."""
    with open(filename, 'r+') as f:
        text = f.read()
        i = text.index(tag) + len(tag)
        f.seek(0)
        f.write(text[:i] + '\n' + string + text[i:])


def replace_in_file(filename, old, new):
    """Replace in filename old by new."""
    with open(filename, 'r') as f:
        text = f.read()
    text = text.replace(old, new)
    with open(filename, 'w') as f:
        f.write(text)


def remove_line_from_file(filename, tag):
    """Delete the first line in the file with the tag inside."""
    flag = False
    with open(filename, 'r') as f:
        text = f.read()
        tab = text.split('\n')
        for i, line in enumerate(tab):
            if tag in line:
                flag = True
                break
        if flag:
            tab = tab[:i] + tab[i + 1:]
            text = '\n'.join(tab)
    if flag:
        with open(filename, 'w') as f:
            f.write(text)


def generate_new_id(openvibe_folder):
    """Generate a set of 4 different random ids for openvibe"""

    def get_folder_sdk_releae(openvibe_folder) :
        path = '{}/build/'.format(openvibe_folder)
        dirs = os.listdir(path)
        path_to_return = path + [f for f in dirs if 'sdk-Release' in f][0]
        return path_to_return 

    def get_path_ov_id_generator(openvibe_folder) :
        path_sdk = get_folder_sdk_releae(openvibe_folder)
        path_to_return = '{}/applications/developer-tools/id-generator/openvibe-id-generator'.format(path_sdk)
        if system == 'Windows' :
            path_to_return += '.exe'
        return path_to_return

    path_generator = get_path_ov_id_generator(openvibe_folder)
    filename_tmp = 'tmp_result'
    os.system('{} > {}'.format(path_generator, filename_tmp))
    with open(filename_tmp, 'r') as f:
        text = f.read()
        text = text.split('\n')
        line = text[0]
        tab = line.split(',')
        val1 = tab[0][-10:]
        val2 = tab[1][1:11]
        line = text[1]
        tab = line.split(',')
        val3 = tab[0][-10:]
        val4 = tab[1][1:11]
    if system == 'Linux' :  
        os.system('rm {}'.format(filename_tmp))
    elif system == 'Windows' :
        os.system('del {}'.format(filename_tmp))
    return val1, val2, val3, val4

def create_box(openvibe_folder, manager_folder, setting_type, io_type, box_name, desc, path_script, category, author, settings, inputs, outputs, modify_settings, modify_inputs, modify_outputs):
    """Create and modify the files to create/modify a new box in openvibe."""

    global system

    # preventing the use of spaces which would cause problems for c++ filenames
    box_name = box_name.replace(' ','_')

    # force category to be in scripting
    category = 'Scripting/PyBox/' + category

    # 1/ We place ourselves at the root of the python boxe
    old_location = os.getcwd()
    os.chdir("{}/src/".format(manager_folder))

    # 2/ We create the corresponding directory
    path_dir_box = 'box-algorithms/'
    full_path_dir_box = os.getcwd() + '/' + path_dir_box

    # We duplicate files from the original box 
    path_file_header = path_dir_box + 'ovp{}.h'.format(box_name)
    path_pattern_header = '{}Assets/BoxManager/NewBoxPattern-skeletton.h'.format(
        manager_folder)
    copyfile(path_pattern_header, path_file_header)


    # 3/ We insert in ovp_defines.h the declaration of CIdentifiers
    filename = 'ovp_defines.h'
    tag = '// <tag> Tag Box Declaration'
    time.sleep(1)
    new_id1, new_id2, new_id3, new_id4 = generate_new_id(openvibe_folder)
    new_line = "#define OVP_ClassId_BoxAlgorithm_{}                 OpenViBE::CIdentifier({}, {})"\
        .format(box_name, new_id1, new_id2)
    insert_line_in_file(filename, new_line, tag)
    new_line = "#define OVP_ClassId_BoxAlgorithm_{}Desc                 OpenViBE::CIdentifier({}, {})"\
        .format(box_name, new_id3, new_id4)
    insert_line_in_file(filename, new_line, tag)

    # 4/ We add our lines in ovp_main.cpp
    filename = 'ovp_main.cpp'
    tag = '#include "box-algorithms/CPolyBox.h"'
    new_line = '#include "{}"'.format(path_file_header)
    insert_line_in_file(filename, new_line, tag)

    tag = '// <tag> OVP_Declare_New'
    new_line = '\t\tOVP_Declare_New(OpenViBEPlugins::Python::CBoxAlgorithm{}Desc);'.format(
        box_name)
    insert_line_in_file(filename, new_line, tag)

    # 5/ We replace in ovpmyBox.h
    replace_in_file(path_file_header, 'CBoxAlgorithmNewBoxPattern',
                    'CBoxAlgorithm{}'.format(box_name))
    replace_in_file(path_file_header, 'OVP_ClassId_BoxAlgorithm_NewBoxPattern',
                    'OVP_ClassId_BoxAlgorithm_{}'.format(box_name))
    replace_in_file(path_file_header, 'OVP_ClassId_BoxAlgorithm_NewBoxPatternDesc',
                    'OVP_ClassId_BoxAlgorithm_{}Desc'.format(box_name))


    # 7/ We change the name, the description, the author name and the category
    desc = desc.replace("\"", "\\\"")

    # we reset the spaces for the name of the box
    box_name = box_name.replace('_',' ')

    replace_in_file(path_file_header,
                    'virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString('
                    '"NewBoxPattern"); }',
                    'virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("'
                    + box_name + '"); }')
    replace_in_file(path_file_header,
                    'virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString('
                    '"Default Python Description"); }',
                    'virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("'
                    + desc + '"); }')
    replace_in_file(path_file_header,
                    'virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("'
                    'NewAuthor"); }',
                    'virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("'
                    + author + '"); }')
    replace_in_file(path_file_header,
                    'virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("'
                    'Scripting/Pybox/"); }',
                    'virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("'
                    + category + '"); }')

    # 8/ We set the python script to execute
    # On set le script python a executer
    replace_in_file(path_file_header,
                    'm_sScriptFilename = "NewScript.py";',
                    'm_sScriptFilename = "{}";'.format(path_script.replace('\\', '/')))


    # 10/ We can then add our params
    tag = '// <tag> settings'
    for number, (key, kind, value) in reversed(list(settings.items())):
        if key:
            new_line = '                prototype.addSetting("{}", {}, "{}");'.format(
                key, all_settings_type[kind], value)
            insert_line_in_file(path_file_header, new_line, tag)

    # 12/ We can then add our inputs and our outputs
    tag_inoutset = '// <tag> input & output'
    for number, (name, kind) in reversed(list(inputs.items())):
        if name:
            new_line = '                prototype.addInput("{}", {});'.format(
                name, io_type[kind])
            insert_line_in_file(path_file_header, new_line, tag_inoutset)

    for number, (name, kind) in reversed(list(outputs.items())):
        new_line = '                prototype.addOutput("{}", {});'.format(
            name, io_type[kind])
        insert_line_in_file(path_file_header, new_line, tag_inoutset)


    # 13/ Permissions to modify boxes in OV
    if not modify_settings :
        replace_in_file(path_file_header,
                        "prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifySetting);",
                        "//prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifySetting);")
    if not modify_inputs :
        replace_in_file(path_file_header,
                        "prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);",
                        "//prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);")
    if not modify_outputs :
        replace_in_file(path_file_header,
                        "prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);",
                        "//prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);")

    os.chdir(old_location)


def delete_box(manager_folder, box_name):
    """Delete and modify files to delete an existing box."""

    global system

    box_name = box_name.replace(' ', '_')

    # 1/ Change directory to src/
    old_location = os.getcwd()
    os.chdir("{}src/".format(manager_folder))

    # 2/ Remove directory with the box algorithms
    path_box = 'box-algorithms/ovp{}.h'.format(box_name)
    os.remove(path_box)

    # 3/ Remove lines from ovp_defines.h
    path = 'ovp_defines.h'
    tag = 'OVP_ClassId_BoxAlgorithm_{}Desc'.format(box_name)
    remove_line_from_file(path, tag)
    tag = 'OVP_ClassId_BoxAlgorithm_{}'.format(box_name)
    remove_line_from_file(path, tag)

    # 4/ Remove lines from ovp_main.cpp
    path = 'ovp_main.cpp'
    tag = 'OVP_Declare_New(OpenViBEPlugins::Python::CBoxAlgorithm{}Desc);'.format(
        box_name)
    remove_line_from_file(path, tag)
    tag = '#include "box-algorithms/ovp{}.h"'.format(
        box_name, box_name)
    remove_line_from_file(path, tag)

    os.chdir(old_location)


# ------------------------------------------------------------


def create_custom_setting(manager_folder, openvibe_folder, cs) :
    """ Make all modification to openvibe to create a custom setting."""

    prefixe = 'OVPoly_ClassId_'
    path_cpp = '{}/src/ovp_main.cpp'.format(manager_folder)
    path_header = '{}/src/ovp_defines.h'.format(manager_folder)

    tag = '// <tag> Custom Type Settings'
    cs_define = "{}{}".format(prefixe, cs.name)

    # header
    id_1, id_2, _, _ = generate_new_id(openvibe_folder)
    line_header = '#define {}               OpenViBE::CIdentifier({}, {})'.format(cs_define, id_1, id_2)
    insert_line_in_file(path_header, line_header, tag)

    # cpp
    for value in reversed(cs.values) :
        line_entry = '\trKernelContext.getTypeManager().registerEnumerationEntry({}, "{}", {});'.format(cs_define, value.text, value.id)
        insert_line_in_file(path_cpp, line_entry, tag)

    line_type = '\trKernelContext.getTypeManager().registerEnumerationType({}, "{}");'.format(cs_define, cs.name)
    insert_line_in_file(path_cpp, line_type, tag)

def delete_custom_setting(manager_folder, cs) :
    """Make all the modification to openvibe to delete a custom setting."""

    def remove_lines_with_tag_in_file(file, tag) :
        with open(file, 'r') as f :
            text = f.read()
            new_text = []
            for line in text.split('\n') :
                if not (tag in line) :
                    new_text += [line]
            new_text = '\n'.join(new_text)
        with open(file, 'w') as f :
            f.write(new_text)

    prefixe = 'OVPoly_ClassId_'
    path_cpp = '{}/src/ovp_main.cpp'.format(manager_folder)
    path_header = '{}/src/ovp_defines.h'.format(manager_folder)

    tag = '{}{}'.format(prefixe, cs.name)
    remove_lines_with_tag_in_file(path_header, tag)
    remove_lines_with_tag_in_file(path_cpp, tag)

def compile(manager_folder, openvibe_folder):
    """Compile OpenVibe."""

    # Go to openvibe folder directory
    old_location = os.getcwd()
    os.chdir(openvibe_folder)

    path_log = manager_folder + 'compilation.log'

    if system == 'Linux':
        os.system('./build.sh | tee {}'.format(path_log))
    elif system == 'Windows':
        os.system('build.cmd > {}'.format(path_log))

    # Set back to normal
    os.chdir(old_location)



def add_stimulation(manager_folder, label, file_sound) :
    """Make all the modifications to add a stimulation to openvibe."""

    def find_next_id(path_file_stim) :
        # We open the PolyStimulations.py to find the next id
        with open(path_file_stim, 'r') as f :
            text = f.read()
            # On charge de dictionnaire de stimulation
            dico_string = ''
            flag = False
            for c in text :
                if c == '{' :
                    flag = True
                elif c == '}' :
                    dico_string += c
                    break 

                if flag :
                    dico_string += c
            dico = ast.literal_eval(dico_string)

            ids = [int(value) for key, value in Poly_stimulation.items() if 'OVPoly' in key]
            ids.sort()
            try :
                current_id = ids[0]
            except IndexError :
                return 0x10001
            while True :
                if not current_id in ids :
                    return hex(current_id)
                current_id += 1

    def get_line_to_add(label, path_file_stim) :
        key = '\'OVPoly_' + label[0].upper() + label[1:].lower() + '\''+ ' '*(29-len(label))
        value = find_next_id(path_file_stim)
        line = '{}: {},'.format(key, value)
        return line


    path_file_stim = '{}/share/PolyStimulations.py'.format(manager_folder)
    tag = '# <Flag> New Stims'

    line = get_line_to_add(label, path_file_stim)
    insert_line_in_file(path_file_stim, line, tag)

    copyfile(file_sound, '{}/Assets/Sounds/{}.mp3'.format(manager_folder, label))


def get_name_duplicate(dict, name) :
    """Find the name for a duplicated box."""

    def regex_handling(regex, name):
        res = re.search(regex, name)

        try:
            prefix_1 = res.group(1)
            idx_1 = res.group(2)
            max_idx = 1 

            # Does we already have a duplicate ?
            # we do so we find the max index and increment it
            for bn in dict.keys() :

                regex = '(' + prefix_1 + ')(\d+)'
                res_2 = re.search(regex,bn)
                try:
                    idx = int(res_2.group(2))

                    if idx >= max_idx:
                        max_idx = idx + 1 
                    
                except:
                    pass

            current_name =  prefix_1 + str(max_idx)

        except AttributeError:
            # current_name = 'rr'
            
            for bn in dict.keys() :
                if bn == name :
                    current_name = '{}_{}'.format(name, 1)
        finally:
            return current_name


    current_name = name

    regex = '(.+?_)(\d+)'
    res = re.search(regex, current_name)


    # We already have doublon
    for bn in dict.keys():

        # if we have the "original" name 
        if current_name in bn:
            current_name = regex_handling(regex, bn)
            break
        else:
            current_name = regex_handling(regex, current_name)
            

    return current_name


def delete_stimulation(manager_folder, label) :
    """Make the modification to remove a stimulation from openvibe."""

    path_file_stim = '{}/share/StimulationsCodes.py'.format(manager_folder)
    remove_line_from_file(path_file_stim, label)
    path_sound = '{}/Assets/Sounds/{}.mp3'.format(manager_folder, label.lower())
    os.remove(path_sound)


def retrieve_settings_type(custom_settings, all_settings=False) :
    """If 'mode=developer' in args : load all default and custom settings, 
    else load only default settings"""

    settings = {
            'Integer': 'OV_TypeId_Integer',
            'Float': 'OV_TypeId_Float',
            'String': 'OV_TypeId_String',
            'Boolean': 'OV_TypeId_Boolean',
            'Filename': 'OV_TypeId_Filename',
            'Stimulation': 'OV_TypeId_Stimulation',
        }

    flag = False
    for arg in sys.argv[1:] :
        if 'mode=developer' in arg :
            flag = True

    if flag or all_settings :
        for k,v in custom_settings.items() :
            settings[k] = v.name

    return settings

Setting = namedtuple('Setting', ['name', 'type', 'value', 'button_delete'])
Input = namedtuple('Input', ['name', 'type', 'button_delete'])
Output = namedtuple('Output', ['name', 'type', 'button_delete'])

Custom_Setting_Line = namedtuple('CSV', ['name', 'value', 'button_delete'])
Custom_Setting = namedtuple("Custom_Setting", ['name', 'values'])
Value = namedtuple("Value", ["text", "id"])


class BoxPython:
    """This class aim to contain all the data needed to create a box in openvibe."""

    def __init__(self, name='Default Box',  filename='Default filename', desc='Default Python Description', path_script='/home/', old_name=None,):
        self.name = name
        self.filename = filename
        self.description = desc
        self.py_script = path_script
        self.category = ''
        self.author = ''
        self.settings = {}
        self.inputs = {}
        self.outputs = {}
        self.modify_settings = True
        self.modify_inputs = True
        self.modify_outputs = True
        self.to_be_updated = False

        if old_name is None :
            self.old_name = name
        else :
            self.old_name = old_name



    def __eq__(self, obj):
        equals = False
        if isinstance(obj, BoxPython):
            equals = self.name == obj.name and \
                self.description == obj.description and \
                self.py_script == obj.py_script and \
                cmp(self.settings, obj.settings) == 0 and \
                cmp(self.inputs, obj.inputs) == 0 and \
                cmp(self.outputs, obj.outputs) == 0
        return equals

    def __ne__(self, obj):
        return not self == obj


# Load all the data necessary to make the manager work.

manager_folder, openvibe_folder = find_folders()

io_type = {
            'Signal': 'OV_TypeId_Signal',
            'Stimulations': 'OV_TypeId_Stimulations',
            'Streamed Matrix': 'OV_TypeId_StreamedMatrix'
        }


modes = ['ov-mode', 'poly-mode']

category = ['Acquisition and network IO',
            'Advanced Visualization',
            'Classification',
            'Data generation',
            'Evaluation',
            'Examples',
            'Feature extraction',
            'File reading and writing',
            'Signal processing',
            'Stimulation',
            'Streaming',
            'Tests',
            'Tools',
            'Visualization'
        ]


sys.path.append('{}/share/'.format(manager_folder))
from PolyStimulations import Poly_stimulation

custom_settings = find_all_custom_settings(manager_folder)
all_settings_type = retrieve_settings_type(custom_settings, all_settings=True)
settings_type = retrieve_settings_type(custom_settings)

boxes = find_all_boxes(manager_folder, io_type, all_settings_type)
stims = find_all_stims()