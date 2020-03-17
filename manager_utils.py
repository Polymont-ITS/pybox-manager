# -*- coding: utf-8 -*-

import re
import os
import sys
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
    current_file = inspect.getfile(lambda : None)
    current_file = Path(current_file)
    manager_folder = str(current_file.parent) + '/'
    openvibe_folder = str(current_file.parent.parent.parent.parent.parent.parent) + '/'
    return manager_folder, openvibe_folder

def find_all_stims():
    prefixe_stim = 'OVPoly_'
    stims = [k for k in OpenViBE_stimulation.keys() if prefixe_stim in k]
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
                match = re.search(r"typeManager\.registerEnumerationType\({}, \"[a-zA-Z0-9_ \.]+\"\);".format(ct), text)
                if match is not None :
                    custom_settings_confirmed += [ct]
        return custom_settings_confirmed

    def get_custom_values(path_cpp, custom_settings, prefixe) :
        custom_values = {}

        with open(path_cpp, 'r') as f :
            text = f.read()

            for ct in custom_settings :
                matchs = re.findall(r'typeManager\.registerEnumerationEntry\({}, "[0-9a-zA-Z_ \.]+", [0-9]+\);'.format(ct), text)
                values = []

                for match in matchs :
                    m_text = match.split('"')[1]
                    m_id = int(match.split(',')[-1][1:-2])
                    values += [Value(text=m_text, id=m_id)]

                cs_name = ct[len(prefixe):]
                custom_values[cs_name] = Custom_Setting(ct, values)

        return custom_values

    prefixe = 'OVPoly_ClassId_'
    path_header = "{}/Assets/BoxManager/ovtk_defines.h".format(manager_folder)
    path_cpp = "{}/Assets/BoxManager/ovtk_main.cpp".format(manager_folder)

    custom_settings = get_custom_settings(path_header, prefixe)
    custom_settings = get_custom_settings_confirmed(path_cpp, custom_settings)
    custom_values = get_custom_values(path_cpp, custom_settings, prefixe)
    return custom_values

def find_all_boxes(openvibe_folder, io_dic_type, settings_dic_type):
    """Génère une liste contenant toutes les box existantes pour OpenVibe"""
    path_all_boxes = "{}/extras/contrib/plugins/processing/python3/src/box-algorithms/".format(openvibe_folder)
    list_all_files = os.listdir(path_all_boxes)

    directories = {}
    for elem in list_all_files:
        if os.path.isdir(path_all_boxes + elem):
            # Path Script

            file_path = path_all_boxes + elem + "/ovp{}.cpp".format(elem)
            with open(file_path, 'r') as f:
                file_cpp = f.read()
                match = re.match('(^.*)(?=(_\d+))', elem)

                # case when the box is not the only one with the same script
                if match != None:
                    script_name = match.groups()[0]
                else:
                    script_name = elem

                script = re.findall(r"m_scriptFilename = \"(.*\.py)\";", file_cpp)[0]


            # Description
            with open(path_all_boxes + elem + "/ovp{}.h".format(elem), 'r') as f:
                file_h = f.read()

                desc = re.findall(r'CString getShortDescription\(\) const override\s* { return CString\(\"(.*?)\"\); }', file_h, flags=re.DOTALL)[0]
                desc = desc.replace("\"", "\\\"")

                author = re.search(r'CString getAuthorName\(\) const override\s* { return CString\(\"([a-zA-Z 0-9\-&]*)\"\); }', file_h)
                author = author.groups()[0]

                category = re.search(r'CString getCategory\(\) const override\s* { return CString\(\"([a-zA-Z ]*)\"\); }', file_h)
                category = category.groups()[0]

                # Creation of the object

                # workaround to allow creation of box with spaces
                name = elem.replace('_',' ')

                box = BoxPython(name=name, filename= elem, desc=desc, path_script=script)
                box.author = author
                box.category = category

                # Settings
                setts = re.findall(r"^(?<!//)\s*prototype.addSetting\(\"(.*)\", "
                                   r"\s*((OV_TypeId|OVPoly)_[a-z_A-Z]*), \"(.*)\"\);$", file_h,flags=re.M)
                
                # Same value as the one in counters in the display part
                settings_compt = 2

                for sett in setts:
                    # on devrait pas avoir de script je pense
                    if sett[0] != 'Clock frequency (Hz)':
                        keys = list(settings_dic_type.keys())
                        values = list(settings_dic_type.values())

                        # print('keys :', keys)
                        # print('------------')
                        # print('values :', values)
                        # print('------------')

                        box.settings[settings_compt] = [
                            sett[0], keys[values.index(sett[1])], sett[3]]
                        settings_compt += 1

                # inputs
                inputs = re.findall(
                    r"(?<!//)prototype.addInput\(\"(.*)\", (OV_TypeId_[a-zA-Z]*)\);", file_h)
            
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
                    r"(?<!//)prototype.addOutput\(\"(.*)\", (OV_TypeId_[a-zA-Z]*)\);", file_h)
                # Same value as the one in counters in the display part
                outputs_compt = 2
                for out in outputs:
                    box.outputs[outputs_compt] = [
                        out[0], keys[values.index(out[1])]]
                    outputs_compt += 1

                # modify permission

                permission_input = re.search(r"(\/\/)?prototype\.addFlag\(Kernel::BoxFlag_CanModifyInput\);", file_h)
                permission_output = re.search(r"(\/\/)?prototype\.addFlag\(Kernel::BoxFlag_CanModifyOutput\);", file_h)
                permission_setting = re.search(r"(\/\/)?prototype\.addFlag\(Kernel::BoxFlag_CanModifySetting\);", file_h)


                if permission_input.group()[:2] == '//' :
                    box.modify_inputs = False
                if permission_output.group()[:2] == '//' :
                    box.modify_outputs = False
                if permission_setting.group()[:2] == '//' :
                    box.modify_settings = False

            directories[box.name] = box

    return directories

def warning_msg(msg) :
    box = QMessageBox()
    box.setIcon(QMessageBox.Warning)
    box.setText(msg)
    box.setWindowTitle('Warning')
    box.exec_()

def info_msg(msg) :
    box = QMessageBox()
    box.setIcon(QMessageBox.Information)
    box.setText(msg)
    box.setWindowTitle('Information')
    box.exec_()

def insert_line_in_file(filename, string, tag):

    with open(filename, 'r+') as f:
        text = f.read()


# filename :  box-algorithms/KNearestNeighbors/ovpKNearestNeighbors.h
# string :                  prototype.addInput  ("input_Stimulations", OV_TypeId_Stimulations);
# tag :  //prototype.addInput  ("Input stimulations", OV_TypeId_Stimulations);

        i = text.index(tag) + len(tag)
        f.seek(0)
        f.write(text[:i] + '\n' + string + text[i:])


def replace_in_file(filename, old, new):
    with open(filename, 'r') as f:
        text = f.read()
    text = text.replace(old, new)
    with open(filename, 'w') as f:
        f.write(text)


def remove_line_from_file(filename, tag):
    # Delete the first line in the file with the tag inside
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

def verify_initialization(manager_folder, openvibe_folder):
    """Return true if the initialization has been done, false otherwise."""
    
    # openvibe.py
    filename_to_copy = manager_folder + '/Assets/BoxManager/openvibe.py'
    filename_dist = openvibe_folder + \
        '/extras/contrib/plugins/processing/python3/share/openvibe.py'
    res = compare_file(filename_to_copy, filename_dist)
    if not res :
        return False

    # ovpCBoxAlgorithmPython.h
    filename_to_copy = manager_folder + '/Assets/BoxManager/ovpCBoxAlgorithmPython3.h'
    filename_dist = openvibe_folder + '/extras/contrib/plugins/processing/python3/src/box-algorithms/ovpCBoxAlgorithmPython3.h'
    res = compare_file(filename_to_copy, filename_dist)
    if not res :
        return False

    # StimulationsCodes.py
    filename_to_copy = manager_folder + '/Assets/BoxManager/StimulationsCodes.py'
    filename_dist = openvibe_folder + '/extras/contrib/plugins/processing/python3/share/StimulationsCodes.py'
    res = compare_file(filename_to_copy, filename_dist)
    if not res :
        return False

    # ovtk_defines.h
    filename_to_copy = manager_folder + '/Assets/BoxManager/ovtk_defines.h'
    filename_dist = openvibe_folder + '/sdk/toolkit/include/toolkit/ovtk_defines.h'
    res = compare_file(filename_to_copy, filename_dist)
    if not res :
        return False

    # ovtk_main.cpp
    filename_to_copy = manager_folder + '/Assets/BoxManager/ovtk_main.cpp'
    filename_dist = openvibe_folder + '/sdk/toolkit/src/ovtk_main.cpp'
    res = compare_file(filename_to_copy, filename_dist)
    if not res :
        return False


    return True

def initialize_files(manager_folder, openvibe_folder):

    # openvibe.py
    filename_to_copy = manager_folder + '/Assets/BoxManager/openvibe.py'
    filename_dist = openvibe_folder + \
        '/extras/contrib/plugins/processing/python3/share/openvibe.py'
    copyfile(filename_to_copy, filename_dist)

    # ovpCBoxAlgorithmPython.h
    filename_to_copy = manager_folder + \
        '/Assets/BoxManager/ovpCBoxAlgorithmPython3.h'
    filename_dist = openvibe_folder + \
        '/extras/contrib/plugins/processing/python3/src/box-algorithms/ovpCBoxAlgorithmPython3.h'
    copyfile(filename_to_copy, filename_dist)

    # StimulationsCodes.py
    filename_to_copy = manager_folder + '/Assets/BoxManager/StimulationsCodes.py'
    filename_dist = openvibe_folder + '/extras/contrib/plugins/processing/python3/share/StimulationsCodes.py'
    copyfile(filename_to_copy, filename_dist)

    # ovtk_defines.h
    filename_to_copy = manager_folder + '/Assets/BoxManager/ovtk_defines.h'
    filename_dist = openvibe_folder + '/sdk/toolkit/include/toolkit/ovtk_defines.h'
    copyfile(filename_to_copy, filename_dist)

    # ovtk_main.cpp
    filename_to_copy = manager_folder + '/Assets/BoxManager/ovtk_main.cpp'
    filename_dist = openvibe_folder + '/sdk/toolkit/src/ovtk_main.cpp'
    copyfile(filename_to_copy, filename_dist)

    print('Initialization manager done.')
    info_msg('The manager just realized his initialization. For more informations, you can refer to the documentation.')


def generate_new_id(openvibe_folder, nb_id=4):
    """Génère un ensemble de 4 IDs aléatoires pour la création d'une nouvelle box openvibe"""

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

        # we delete paranthesis
        text = text.replace(')','')

        # we remove all the paranthesis which will be useless
        matchs = re.findall(r"\((.*)$", text, flags=re.MULTILINE)
    
    # we clean the array
    arr = []

    for match in matchs:
        for elem in match.split(','):
            arr.append(elem.replace(' ',''))

    indexes = arr[:nb_id]

    if system == 'Linux' :  
        pass
        os.system('rm {}'.format(filename_tmp))
    elif system == 'Windows' :
        os.system('del {}'.format(filename_tmp))
    return indexes


def create_box(openvibe_folder, manager_folder, setting_type, io_type, box_name, desc, path_script, category, author, settings, inputs, outputs, modify_settings, modify_inputs, modify_outputs, update=False):
    """Créer et modifie les fichiers pour la création d'une nouvelle box."""
    global system

    #preventing the use of spaces which would cause problems for c++ filenames
    box_name = box_name.replace(' ','_')

    # 1/ We place ourselves at the root of the python boxe
    os.chdir(
        "{}/extras/contrib/plugins/processing/python3/src/".format(openvibe_folder))

    # 2/ We create the corresponding directory
    path_dir_box = 'box-algorithms/{}/'.format(box_name)
    try:
        os.mkdir(path_dir_box)
    except OSError:
        print('Path already exists.')

    # We duplicate files from the original box 

    path_file_cpp = path_dir_box + 'ovp{}.cpp'.format(box_name)
    path_file_header = path_dir_box + 'ovp{}.h'.format(box_name)


    path_pattern_cpp = '{}Assets/BoxManager/ovpNewBoxPattern.cpp'.format(
        manager_folder)
    path_pattern_header = '{}Assets/BoxManager/ovpNewBoxPattern.h'.format(
        manager_folder)

    copyfile(path_pattern_cpp, path_file_cpp)
    copyfile(path_pattern_header, path_file_header)


    # 3/ We insert in typeManager.h the declaration of CIdentifiers if it's a new box
    with open('ovp_defines.h', 'r') as f:
        text = f.read()

    regex = '^#define OVP_ClassId_BoxAlgorithm_({})\s+'.format(box_name)

    res = re.search(regex, text,flags=re.M)


    try:
        # Already existing -
        match = res.groups()
        update = True
    except AttributeError:
        # new box
        update = False
    
    if not update:
        filename = 'ovp_defines.h'
        tag = '#define OVP_ClassId_BoxAlgorithm_Python3Desc                OpenViBE::CIdentifier(0x404B6FFD, 0x12BDD423)'
        time.sleep(1)
        new_id1, new_id2, new_id3, new_id4 = generate_new_id(openvibe_folder, nb_id=4)
        new_line = "#define OVP_ClassId_BoxAlgorithm_{}                OpenViBE::CIdentifier({}, {})"\
            .format(box_name, new_id1, new_id2)

        insert_line_in_file(filename, new_line, tag)
        new_line = "#define OVP_ClassId_BoxAlgorithm_{}Desc                OpenViBE::CIdentifier({}, {})"\
            .format(box_name, new_id3, new_id4)
        insert_line_in_file(filename, new_line, tag)

    # 4/ We add our lines in ovp_main.cpp
        filename = 'ovp_main.cpp'
        tag = '#include "box-algorithms/ovpCBoxAlgorithmPython3.h"'
        new_line = '#include "{}"'.format(path_file_header)
        insert_line_in_file(filename, new_line, tag)

        tag = 'OVP_Declare_New(Python::CBoxAlgorithmPython3Desc);'
        new_line = '\t\tOVP_Declare_New(Python::CBoxAlgorithm{}Desc);'.format(
            box_name)
        insert_line_in_file(filename, new_line, tag)

    # 5/ We replace in ovpmyBox.h
    replace_in_file(path_file_header, '#include "../ovp_defines.h"',
                    '#include "../../ovp_defines.h"')
    replace_in_file(path_file_header, 'CBoxAlgorithmNewBoxPattern',
                    'CBoxAlgorithm{}'.format(box_name))
    replace_in_file(path_file_header, '__OpenViBEPlugins_BoxAlgorithm_NewBoxPattern_H__',
                    '__OpenViBEPlugins_BoxAlgorithm_{}_H__'.format(box_name))
    replace_in_file(path_file_header, 'OVP_ClassId_BoxAlgorithm_NewBoxPattern',
                    'OVP_ClassId_BoxAlgorithm_{}'.format(box_name))
    replace_in_file(path_file_header, 'OVP_ClassId_BoxAlgorithm_NewBoxPatternDesc',
                    'OVP_ClassId_BoxAlgorithm_{}Desc'.format(box_name))

    # 6/ We replace in ovpmyBox.cpp
    replace_in_file(path_file_cpp, 'ovpNewBoxPattern.h',
                    'ovp{}.h'.format(box_name))
    replace_in_file(path_file_cpp, 'OVP_ClassId_BoxAlgorithm_NewBoxPatternDesc',
                    'OVP_ClassId_BoxAlgorithm_{}Desc'.format(box_name))
    replace_in_file(path_file_cpp, 'CBoxAlgorithmNewBoxPattern',
                    'CBoxAlgorithm{}'.format(box_name))

    # 7/ We change the name, the description, the author name and the category
    desc = desc.replace("\"", "\\\"")

    # we reset the spaces for the name of the box
    box_name = box_name.replace('_',' ')

    replace_in_file(path_file_header,
                    'CString getName() const override { return CString("Python 3 scripting"); }',
                    'CString getName() const override { return CString("' + box_name + '"); }')
    replace_in_file(path_file_header,
                    'CString getShortDescription() const override { return CString("This box executes a python script."); }',
                    'CString getShortDescription() const override { return CString("' + desc + '"); }')
    replace_in_file(path_file_header,
                    'CString getAuthorName() const override { return CString("Aurelien Van Langhenhove and Laurent George"); }',
                    'CString getAuthorName() const override { return CString("' + author + '"); }')
    replace_in_file(path_file_header,
                    'CString getCategory() const override { return CString("Scripting"); }',
                    'CString getCategory() const override { return CString("' + category + '"); }')

    # 8/ We set the python script to execute
    # On set le script python a executer
    replace_in_file(path_file_cpp,
                    'm_scriptFilename = "/home/vegeta/Documents/openvibe-python/ScriptBox/NewBoxPattern.py";',
                    'm_scriptFilename = "{}";'.format(path_script))

    # 9/ We prevent the path from being modified 
    replace_in_file(path_file_header,
                    'prototype.addSetting("Script", OV_TypeId_Script, "");',
                    '//prototype.addSetting("Script", OV_TypeId_Script, "");')

    # 10/ We can then add our params
    # TODO Ajouter la gestion correcte des tabs au début des lignes.
    tag = 'prototype.addSetting("Script", OV_TypeId_Script, "");'
    for number, (key, kind, value) in reversed(list(settings.items())):
        if key:
            new_line = '                prototype.addSetting("{}", {}, "{}");'.format(
                key, all_settings_type[kind], value)
            insert_line_in_file(path_file_header, new_line, tag)

    # 11/ Essential to be able to read our params
    replace_in_file(path_file_cpp,
                    'for (uint32_t i = 2; i < boxCtx->getSettingCount(); ++i)',
                    'for (uint32_t i = 0; i < boxCtx->getSettingCount(); ++i)')
                    
    # 12/ We can then addd our inputs our outputs
    # TODO Ajouter la gestion correcte des tabs au début des lignes.
    # seems outdated
    # tag_input = '//prototype.addInput("Input stimulations", OV_TypeId_Stimulations);'
    tag_input = 'prototype.addInputSupport(OV_TypeId_StreamedMatrix);'
    for number, (name, kind) in reversed(list(inputs.items())):
        if name:
            new_line = '                prototype.addInput("{}", {});'.format(
                name, io_type[kind])
            insert_line_in_file(path_file_header, new_line, tag_input)

    # 13/ Permissions to modify boxes in OV
    if not modify_settings :
        replace_in_file(path_file_header,
                        "prototype.addFlag(Kernel::BoxFlag_CanModifySetting);",
                        "//prototype.addFlag(Kernel::BoxFlag_CanModifySetting);")
    if not modify_inputs :
        replace_in_file(path_file_header,
                        "prototype.addFlag(Kernel::BoxFlag_CanModifyInput);",
                        "//prototype.addFlag(Kernel::BoxFlag_CanModifyInput);")
    if not modify_outputs :
        replace_in_file(path_file_header,
                        "prototype.addFlag(Kernel::BoxFlag_CanModifyOutput);",
                        "//prototype.addFlag(Kernel::BoxFlag_CanModifyOutput);")

    # TODO Ajouter la gestion correcte des tabs au début des lignes.
    # tag_output = '//prototype.addOutput ("Output stimulations", OV_TypeId_Stimulations);'
    tag_output = 'prototype.addOutputSupport(OV_TypeId_StreamedMatrix);'
    for number, (name, kind) in reversed(list(outputs.items())):
        new_line = '                prototype.addOutput("{}", {});'.format(
            name, io_type[kind])
        insert_line_in_file(path_file_header, new_line, tag_output)

def delete_box(openvibe_folder, box_name, update=False):
    """Delete and modify files when deleting an existing box."""
    global system

    box_name = box_name.replace(' ', '_')

    # 1/ Change directory to src/
    os.chdir(
        "{}/extras/contrib/plugins/processing/python3/src/".format(openvibe_folder))

    # 2/ Remove directory with the box algorithms
    path_box = 'box-algorithms/{}/'.format(box_name)
    if system == 'Linux':
        os.system('rm -rf {}'.format(path_box))
    elif system == 'Windows':
        os.system('del \S \F {}'.format(path_box))

    # 3/ Remove lines from ovp_defines.h
    if not update:

        path = 'ovp_defines.h'
        tag = 'OVP_ClassId_BoxAlgorithm_{}Desc'.format(box_name)
        remove_line_from_file(path, tag)
        tag = 'OVP_ClassId_BoxAlgorithm_{}'.format(box_name)
        remove_line_from_file(path, tag)

        # 4/ Remove lines from ovp_main.cpp
        path = 'ovp_main.cpp'
        tag = 'OVP_Declare_New(Python::CBoxAlgorithm{}Desc);'.format(
            box_name)
        remove_line_from_file(path, tag)
        tag = '#include "box-algorithms/{}/ovp{}.h"'.format(
            box_name, box_name)
        remove_line_from_file(path, tag)

def create_custom_setting(manager_folder, openvibe_folder, cs) :

    prefixe = 'OVPoly_ClassId_'
    path_cpp = '{}/Assets/BoxManager/ovtk_main.cpp'.format(manager_folder)
    path_header = '{}/Assets/BoxManager/ovtk_defines.h'.format(manager_folder)

    tag = '// Custom Type Settings tag'
    cs_define = "{}{}".format(prefixe, cs.name)

    # header
    id_1, id_2 = generate_new_id(openvibe_folder, nb_id=2)
    line_header = '#define {}               OpenViBE::CIdentifier({}, {})'.format(cs_define, id_1, id_2)
    insert_line_in_file(path_header, line_header, tag)

    # cpp
    for value in reversed(cs.values) :
        line_entry = '\ttypeManager.registerEnumerationEntry({}, "{}", {});'.format(cs_define, value.text, value.id)
        insert_line_in_file(path_cpp, line_entry, tag)

    line_type = '\ttypeManager.registerEnumerationType({}, "{}");'.format(cs_define, cs.name)
    insert_line_in_file(path_cpp, line_type, tag)

def delete_custom_setting(manager_folder, cs) :

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
    path_cpp = '{}/Assets/BoxManager/ovtk_main.cpp'.format(manager_folder)
    path_header = '{}/Assets/BoxManager/ovtk_defines.h'.format(manager_folder)

    tag = '{}{}'.format(prefixe, cs.name)
    remove_lines_with_tag_in_file(path_header, tag)
    remove_lines_with_tag_in_file(path_cpp, tag)




def compile(manager_folder, openvibe_folder):
    """Recompile OpenVibe to take into account the changes."""

    if not verify_initialization(manager_folder, openvibe_folder):
        initialize_files(manager_folder, openvibe_folder)

    # we save the box related files in order to rollback if there is any problem

    boxes_path = "{}/extras/contrib/plugins/processing/python3/src/".format(openvibe_folder)
    copy_path = manager_folder + 'boxes_copy/'

    # if system == 'Linux':
        # try:
    #     os.system('rm -rf {}*'.format(copy_path))
    #     os.system('cp -r {}* {}'.format(boxes_path, copy_path))
        # except:
        #     pass
    # elif system == 'Windows':
        # try:
    #     os.system('del \S \F {}'.format(path_box))
    #     os.system('xcopy {} {} /E'.format(boxes_path, copy_path))
        # except:
        #     pass



    os.chdir(openvibe_folder)

    if system == 'Linux':
        os.system('./build.sh | tee compilation.log')
    elif system == 'Windows':
        os.system('build.cmd')

    # catch error when building
    regex = '(recipe .*? failed)|(Error while building) '

    with open('compilation.log', 'r') as log_file:
        f = log_file.read()
        res = re.search(regex, f)

    try:
        matches = res.groups()
    except Exception as e:
        pass

    # There was an error during compilation so we have to rollback the boxes
    # if system == 'Linux':
    #     os.system('rm -rf {}*'.format(boxes_path))
    #     os.system('cp -r {}* {}'.format(copy_path, boxes_path))
    # elif system == 'Windows':
    #     os.system('del \S \F {}'.format(path_box))
    #     os.system('xcopy {} {} /E'.format(copy_path, boxes_path))
        

def add_stimulation(manager_folder, label, file_sound) :

    def find_next_id(path_file_stim) :
        # We open the StimulationCodes.py to find the next id
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

            ids = [int(value) for key, value in OpenViBE_stimulation.items() if 'OVPoly' in key]
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


    path_file_stim = '{}/Assets/BoxManager/StimulationsCodes.py'.format(manager_folder)
    tag = '# <Flag> New Stims'

    line = get_line_to_add(label, path_file_stim)
    insert_line_in_file(path_file_stim, line, tag)

    copyfile(file_sound, '{}/Assets/Sounds/{}.mp3'.format(manager_folder, label))


def get_name_duplicate(dict, name) :

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
    path_file_stim = '{}/Assets/BoxManager/StimulationsCodes.py'.format(manager_folder)
    remove_line_from_file(path_file_stim, label)
    path_sound = '{}/Assets/Sounds/{}.mp3'.format(manager_folder, label.lower())
    os.remove(path_sound)


def retrieve_settings_type(custom_settings, all_settings=False) :
    # If 'mode=developer' in args : load all default and custom settings, 
    # else load only default settings
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
        self.to_be_created = False

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
            'Scripting',
            'Signal processing',
            'Stimulation',
            'Streaming',
            'Tests',
            'Tools',
            'Visualization'
        ]


sys.path.append('{}/Assets/BoxManager/'.format(manager_folder))
from StimulationsCodes import OpenViBE_stimulation

custom_settings = find_all_custom_settings(manager_folder)
all_settings_type = retrieve_settings_type(custom_settings, all_settings=True)
settings_type = retrieve_settings_type(custom_settings)

boxes = find_all_boxes(openvibe_folder, io_type, all_settings_type)
stims = find_all_stims()