# -*- coding: utf-8 -*-

import platform
import shutil
import re
import sys
import copy
import os
import manager_utils as mu
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets import *

system = platform.system()
if system != 'Linux' and system != 'Windows':
    raise Exception("OS {} is not handled with that script.".format(system))

class Ui_MainWindow(object):
    """ This class aim to define the manager's ui, 
    It calls function from the file manager_utils.py to 
    create box, stimulations or settings in openvibe."""

    def __init__(self) :
        self.current_box = None
        self.current_custom_setting = None
        self.is_loaded = False
        self.current_settings = []
        self.current_inputs = []
        self.current_outputs = []
        self.current_custom_settings_lines = []

        self._translate = QtCore.QCoreApplication.translate

    def setupUi(self, MainWindow):
        MainWindow.setObjectName("Pybox-Manager")
        MainWindow.resize(1000, 800)

        # Main widget
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")


        # Main tabs
        self.main_tabs = QtWidgets.QTabWidget(self.centralwidget)
        self.main_tabs.setGeometry(QtCore.QRect(-1, -1, 1000, 800))
        self.main_tabs.setAutoFillBackground(False)
        self.main_tabs.setObjectName("main_tabs")


        # Tab Box Manager
        self.tab_boxmanager = QtWidgets.QWidget()
        self.tab_boxmanager.setObjectName("tab_boxmanager")


        ## Box Select
        self.label_boxname = QtWidgets.QLabel(self.tab_boxmanager)
        self.label_boxname.setGeometry(QtCore.QRect(40, 10, 91, 30))
        self.label_boxname.setObjectName("label_boxname")

        self.dropdown_boxname = QtWidgets.QComboBox(self.tab_boxmanager)
        self.dropdown_boxname.setGeometry(QtCore.QRect(140, 10, 201, 30))
        self.dropdown_boxname.setEditable(True)
        self.dropdown_boxname.setObjectName("dropdown_boxname")

        regex=QtCore.QRegExp("[a-z-A-Z0-9_- ]+")
        validator = QtGui.QRegExpValidator(regex)
        self.dropdown_boxname.setValidator(validator)

        self.button_new = QtWidgets.QPushButton(self.tab_boxmanager)
        self.button_new.setGeometry(QtCore.QRect(390, 10, 70, 30))
        self.button_new.setObjectName("button_new")

        self.button_duplicate = QtWidgets.QPushButton(self.tab_boxmanager)
        self.button_duplicate.setGeometry(QtCore.QRect(470, 10, 120, 30))
        self.button_duplicate.setObjectName("button_duplicate")

        self.label_script = QtWidgets.QLabel(self.tab_boxmanager)
        self.label_script.setGeometry(QtCore.QRect(70, 50, 67, 30))
        self.label_script.setObjectName("label_script")

        self.text_script = QtWidgets.QLineEdit(self.tab_boxmanager)
        self.text_script.setGeometry(QtCore.QRect(140, 50, 321, 30))
        self.text_script.setObjectName("text_script")

        self.button_script = QtWidgets.QPushButton(self.tab_boxmanager)
        self.button_script.setGeometry(QtCore.QRect(470, 50, 120, 30))
        self.button_script.setObjectName("button_script")

        self.label_description = QtWidgets.QLabel(self.tab_boxmanager)
        self.label_description.setGeometry(QtCore.QRect(30, 90, 91, 30))
        self.label_description.setObjectName("label_description")

        self.text_description = QtWidgets.QTextEdit(self.tab_boxmanager)
        self.text_description.setGeometry(QtCore.QRect(140, 100, 321, 75))
        self.text_description.setObjectName("text_description")

        self.button_reset = QtWidgets.QPushButton(self.tab_boxmanager)
        self.button_reset.setGeometry(QtCore.QRect(470, 100, 120, 30))
        self.button_reset.setObjectName("button_reset")

        self.label_category = QtWidgets.QLabel(self.tab_boxmanager)
        self.label_category.setGeometry(QtCore.QRect(30, 200, 120, 30))
        self.label_category.setObjectName("label_category")

        self.dropdown_category = QtWidgets.QComboBox(self.tab_boxmanager)
        self.dropdown_category.setGeometry(QtCore.QRect(140, 200, 320, 30))
        self.dropdown_category.setObjectName("dropdown_category")

        self.label_author = QtWidgets.QLabel(self.tab_boxmanager)
        self.label_author.setGeometry(QtCore.QRect(30, 250, 120, 30))
        self.label_author.setObjectName("label_author")

        self.text_author = QtWidgets.QLineEdit(self.tab_boxmanager)
        self.text_author.setGeometry(QtCore.QRect(140, 250, 320, 30))
        self.text_author.setObjectName("text_author")



        ## CheckBox
        self.label_settings_modifs = QtWidgets.QLabel(self.tab_boxmanager)
        self.label_settings_modifs.setGeometry(675, 10, 210, 30)
        self.label_settings_modifs.setObjectName("label_settings_modifs")

        self.checkbox_settings_modifs = QtWidgets.QCheckBox(self.tab_boxmanager)
        self.checkbox_settings_modifs.setGeometry(QtCore.QRect(910, 17, 20, 20))
        self.checkbox_settings_modifs.setObjectName("checkbox_settings_modifs")

        self.label_inputs_modifs = QtWidgets.QLabel(self.tab_boxmanager)
        self.label_inputs_modifs.setGeometry(675, 40, 210, 30)
        self.label_inputs_modifs.setObjectName("label_inputs_modifs")

        self.checkbox_inputs_modifs = QtWidgets.QCheckBox(self.tab_boxmanager)
        self.checkbox_inputs_modifs.setGeometry(QtCore.QRect(910, 47, 20, 20))
        self.checkbox_inputs_modifs.setObjectName("checkbox_inputs_modifs")

        self.label_outputs_modifs = QtWidgets.QLabel(self.tab_boxmanager)
        self.label_outputs_modifs.setGeometry(675, 70, 210, 30)
        self.label_outputs_modifs.setObjectName("label_outputs_modifs")

        self.checkbox_outputs_modifs = QtWidgets.QCheckBox(self.tab_boxmanager)
        self.checkbox_outputs_modifs.setGeometry(QtCore.QRect(910, 77, 20, 20))
        self.checkbox_outputs_modifs.setObjectName("checkbox_outputs_modifs")

        self.img_info_checkbox_modifications = QtWidgets.QLabel(self.tab_boxmanager)
        self.img_info_checkbox_modifications.setPixmap(QtGui.QPixmap('Assets/BoxManager/infobulle.png'))
        self.img_info_checkbox_modifications.setScaledContents(True)
        self.img_info_checkbox_modifications.setGeometry(QtCore.QRect(950, 42, 30, 30))
        self.img_info_checkbox_modifications.setObjectName("img_info_checkbox_modifications")


        ## Set Mode
        self.label_mode = QtWidgets.QLabel(self.tab_boxmanager)
        self.label_mode.setGeometry(QtCore.QRect(660, 135, 67, 30))
        self.label_mode.setObjectName("label_mode")

        self.img_info_custom_setting_id = QtWidgets.QLabel(self.tab_boxmanager)
        self.img_info_custom_setting_id.setPixmap(QtGui.QPixmap('Assets/BoxManager/infobulle.png'))
        self.img_info_custom_setting_id.setScaledContents(True)
        self.img_info_custom_setting_id.setGeometry(QtCore.QRect(950, 135, 30, 30))
        self.img_info_custom_setting_id.setObjectName("image_info_custom_setting_id")

        self.dropdown_mode = QtWidgets.QComboBox(self.tab_boxmanager)
        self.dropdown_mode.setGeometry(QtCore.QRect(730, 135, 200, 30))
        self.dropdown_mode.setEditable(True)
        self.dropdown_mode.setObjectName("dropdown_mode")

        self.button_mode = QtWidgets.QPushButton(self.tab_boxmanager)
        self.button_mode.setGeometry(QtCore.QRect(730, 175, 200, 30))
        self.button_mode.setObjectName("button_mode")


        ## Config Tabs
        self.config_tabs = QtWidgets.QTabWidget(self.tab_boxmanager)
        self.config_tabs.setGeometry(QtCore.QRect(30, 320, 941, 361))
        self.config_tabs.setObjectName("config_tabs")

        ### Tab Setting
        self.tab_settings = QtWidgets.QWidget()
        self.tab_settings.setObjectName("tab_settings")

        self.label_setting_name = QtWidgets.QLabel(self.tab_settings)
        self.label_setting_name.setGeometry(QtCore.QRect(45, 10, 67, 31))
        self.label_setting_name.setObjectName("label_setting_name")

        self.label_setting_type = QtWidgets.QLabel(self.tab_settings)
        self.label_setting_type.setGeometry(QtCore.QRect(400, 10, 67, 31))
        self.label_setting_type.setObjectName("label_setting_type")

        self.img_info_setting_type = QtWidgets.QLabel(self.tab_settings)
        self.img_info_setting_type.setPixmap(QtGui.QPixmap('Assets/BoxManager/infobulle.png'))
        self.img_info_setting_type.setScaledContents(True)
        self.img_info_setting_type.setGeometry(QtCore.QRect(440, 10, 30, 30))
        self.img_info_setting_type.setObjectName("image_info_setting_type") 

        self.label_setting_value = QtWidgets.QLabel(self.tab_settings)
        self.label_setting_value.setGeometry(QtCore.QRect(650, 10, 67, 31))
        self.label_setting_value.setObjectName("label_setting_value")

        self.button_add_setting = QtWidgets.QPushButton(self.tab_settings)
        self.button_add_setting.setGeometry(QtCore.QRect(820, 10, 99, 31))
        self.button_add_setting.setObjectName("button_add_setting")


        self.scroll_settings = QtWidgets.QScrollArea(self.tab_settings)
        self.scroll_settings.setGeometry(QtCore.QRect(20, 50, 901, 251))
        self.scroll_settings.setWidgetResizable(True)
        self.scroll_settings.setObjectName("scroll_settings")

        self.scroll_settings_widget = QtWidgets.QWidget()
        self.scroll_settings_widget.setGeometry(QtCore.QRect(0, 0, 899, 249))
        self.scroll_settings_widget.setObjectName("scroll_settings_widget")

        self.layout_settings = QtWidgets.QGridLayout(self.scroll_settings_widget)
        self.layout_settings.setObjectName('layout_settings')

        self.scroll_settings.setWidget(self.scroll_settings_widget)
        self.config_tabs.addTab(self.tab_settings, "")


        ### Tab Inputs
        self.tab_inputs = QtWidgets.QWidget()
        self.tab_inputs.setObjectName("tab_inputs")

        self.label_inputs_name = QtWidgets.QLabel(self.tab_inputs)
        self.label_inputs_name.setGeometry(QtCore.QRect(45, 10, 67, 31))
        self.label_inputs_name.setObjectName("label_inputs_name")

        self.label_inputs_type = QtWidgets.QLabel(self.tab_inputs)
        self.label_inputs_type.setGeometry(QtCore.QRect(685, 10, 67, 31))
        self.label_inputs_type.setObjectName("label_inputs_type")

        self.button_add_input = QtWidgets.QPushButton(self.tab_inputs)
        self.button_add_input.setGeometry(QtCore.QRect(820, 10, 99, 31))
        self.button_add_input.setObjectName("button_add_input")

        self.scroll_inputs = QtWidgets.QScrollArea(self.tab_inputs)
        self.scroll_inputs.setGeometry(QtCore.QRect(20, 50, 901, 251))
        self.scroll_inputs.setWidgetResizable(True)
        self.scroll_inputs.setObjectName("scroll_inputs")

        self.scroll_inputs_widget = QtWidgets.QWidget()
        self.scroll_inputs_widget.setGeometry(QtCore.QRect(0, 0, 899, 249))
        self.scroll_inputs_widget.setObjectName("scroll_inputs_widget")

        self.layout_inputs = QtWidgets.QGridLayout(self.scroll_inputs_widget)
        self.layout_inputs.setObjectName('layout_inputs')

        self.scroll_inputs.setWidget(self.scroll_inputs_widget)
        self.config_tabs.addTab(self.tab_inputs, "")


        ### Tab Outputs
        self.tab_outputs = QtWidgets.QWidget()
        self.tab_outputs.setObjectName("tab_outputs")

        self.label_outputs_name = QtWidgets.QLabel(self.tab_outputs)
        self.label_outputs_name.setGeometry(QtCore.QRect(45, 10, 67, 31))
        self.label_outputs_name.setObjectName("label_outputs_name")

        self.label_outputs_type = QtWidgets.QLabel(self.tab_outputs)
        self.label_outputs_type.setGeometry(QtCore.QRect(685, 10, 67, 31))
        self.label_outputs_type.setObjectName("label_outputs_type")

        self.button_add_output = QtWidgets.QPushButton(self.tab_outputs)
        self.button_add_output.setGeometry(QtCore.QRect(820, 10, 99, 31))
        self.button_add_output.setObjectName("button_add_output")

        self.scroll_outputs = QtWidgets.QScrollArea(self.tab_outputs)
        self.scroll_outputs.setGeometry(QtCore.QRect(20, 50, 901, 251))
        self.scroll_outputs.setWidgetResizable(True)
        self.scroll_outputs.setObjectName("scroll_outputs")

        self.scroll_outputs_widget = QtWidgets.QWidget()
        self.scroll_outputs_widget.setGeometry(QtCore.QRect(0, 0, 899, 249))
        self.scroll_outputs_widget.setObjectName("scroll_outputs_widget")

        self.layout_outputs = QtWidgets.QGridLayout(self.scroll_outputs_widget)
        self.layout_outputs.setObjectName('layout_outputs')

        self.scroll_outputs.setWidget(self.scroll_outputs_widget)
        self.config_tabs.addTab(self.tab_outputs, "")


        ## Button Create/Modify and Delete
        self.button_box_create = QtWidgets.QPushButton(self.tab_boxmanager)
        self.button_box_create.setGeometry(QtCore.QRect(70, 700, 351, 41))
        self.button_box_create.setObjectName("button_box_create")

        self.button_box_delete = QtWidgets.QPushButton(self.tab_boxmanager)
        self.button_box_delete.setGeometry(QtCore.QRect(550, 700, 351, 41))
        self.button_box_delete.setObjectName("button_box_delete")

        ## Line sep
        self.line_sep_boxmanager = QtWidgets.QFrame(self.tab_boxmanager)
        self.line_sep_boxmanager.setGeometry(QtCore.QRect(610, 0, 20, 350))
        self.line_sep_boxmanager.setFrameShape(QtWidgets.QFrame.VLine)
        self.line_sep_boxmanager.setFrameShadow(QtWidgets.QFrame.Sunken)
        self.line_sep_boxmanager.setObjectName("line_sep_boxmanager")

        self.line_sep2_boxmanager = QtWidgets.QFrame(self.tab_boxmanager)
        self.line_sep2_boxmanager.setGeometry(QtCore.QRect(620, 100, 511, 20))
        self.line_sep2_boxmanager.setFrameShape(QtWidgets.QFrame.HLine)
        self.line_sep2_boxmanager.setFrameShadow(QtWidgets.QFrame.Sunken)
        self.line_sep2_boxmanager.setObjectName("line_sep2_boxmanager")


        self.main_tabs.addTab(self.tab_boxmanager, "")


        # Tab Stimulations/label manager
        self.tab_labels_manager = QtWidgets.QWidget()
        self.tab_labels_manager.setObjectName("tab_labels_manager")

        ## Add stim side
        self.label_add_stim = QtWidgets.QLabel(self.tab_labels_manager)
        self.label_add_stim.setGeometry(QtCore.QRect(150, 30, 221, 31))
        font = QtGui.QFont()
        font.setPointSize(15)
        self.label_add_stim.setFont(font)
        self.label_add_stim.setObjectName("label_add_stim")

        self.label_name_stim_add = QtWidgets.QLabel(self.tab_labels_manager)
        self.label_name_stim_add.setGeometry(QtCore.QRect(30, 110, 67, 31))
        self.label_name_stim_add.setObjectName("label_name_stim_add")

        self.text_name_stim_add = QtWidgets.QLineEdit(self.tab_labels_manager)
        self.text_name_stim_add.setGeometry(QtCore.QRect(100, 110, 361, 31))
        self.text_name_stim_add.setObjectName("text_name_stim_add")

        self.label_sound_stim_add = QtWidgets.QLabel(self.tab_labels_manager)
        self.label_sound_stim_add.setGeometry(QtCore.QRect(30, 160, 67, 31))
        self.label_sound_stim_add.setObjectName("label_sound_stim_add")

        self.text_sound_stim_add = QtWidgets.QLineEdit(self.tab_labels_manager)
        self.text_sound_stim_add.setGeometry(QtCore.QRect(100, 160, 361, 31))
        self.text_sound_stim_add.setObjectName("text_sound_stim_add")

        self.button_sound_stim_add = QtWidgets.QPushButton(self.tab_labels_manager)
        self.button_sound_stim_add.setGeometry(QtCore.QRect(100, 200, 99, 31))
        self.button_sound_stim_add.setObjectName("button_sound_stim_add")

        self.label_delete_stim = QtWidgets.QLabel(self.tab_labels_manager)
        self.label_delete_stim.setGeometry(QtCore.QRect(630, 30, 261, 31))
        font = QtGui.QFont()
        font.setPointSize(15)
        self.label_delete_stim.setFont(font)
        self.label_delete_stim.setObjectName("label_delete_stim")

        # Delete stim side
        self.button_stim_create = QtWidgets.QPushButton(self.tab_labels_manager)
        self.button_stim_create.setGeometry(QtCore.QRect(120, 650, 241, 51))
        self.button_stim_create.setObjectName("button_stim_create")

        self.button_stim_delete = QtWidgets.QPushButton(self.tab_labels_manager)
        self.button_stim_delete.setGeometry(QtCore.QRect(640, 650, 241, 51))
        self.button_stim_delete.setObjectName("button_stim_delete")

        self.dropdown_stim_del = QtWidgets.QComboBox(self.tab_labels_manager)
        self.dropdown_stim_del.setGeometry(QtCore.QRect(620, 110, 281, 31))
        self.dropdown_stim_del.setEditable(True)
        self.dropdown_stim_del.setObjectName("dropdown_stim_del")

        self.label_name_stim_del = QtWidgets.QLabel(self.tab_labels_manager)
        self.label_name_stim_del.setGeometry(QtCore.QRect(550, 110, 91, 30))
        self.label_name_stim_del.setObjectName("label_name_stim_del")

        # sep
        self.line_sep_stim = QtWidgets.QFrame(self.tab_labels_manager)
        self.line_sep_stim.setGeometry(QtCore.QRect(490, 0, 20, 800))
        self.line_sep_stim.setFrameShape(QtWidgets.QFrame.VLine)
        self.line_sep_stim.setFrameShadow(QtWidgets.QFrame.Sunken)
        self.line_sep_stim.setObjectName("line_sep_stim")

        self.main_tabs.addTab(self.tab_labels_manager, "")



        # Tab Custom Setting Manager
        self.tab_custom_settings = QtWidgets.QWidget()
        self.tab_custom_settings.setObjectName("tab_custom_settings")

        self.label_custom_setting_create_delete = QtWidgets.QLabel(self.tab_custom_settings)
        self.label_custom_setting_create_delete.setGeometry(QtCore.QRect(75, 0, 400, 100))
        font = QtGui.QFont()
        font.setPointSize(15)
        self.label_custom_setting_create_delete.setFont(font)
        self.label_custom_setting_create_delete.setObjectName("label_custom_setting_create_delete")

        self.label_custom_setting_manage_value = QtWidgets.QLabel(self.tab_custom_settings)
        self.label_custom_setting_manage_value.setGeometry(QtCore.QRect(680, 0, 400, 100))
        font = QtGui.QFont()
        font.setPointSize(15)
        self.label_custom_setting_manage_value.setFont(font)
        self.label_custom_setting_manage_value.setObjectName("label_custom_setting_manage_value")

        self.label_custom_setting_name = QtWidgets.QLabel(self.tab_custom_settings)
        self.label_custom_setting_name.setGeometry(QtCore.QRect(50, 120, 100, 30))
        self.label_custom_setting_name.setObjectName("label_custom_setting_name")

        self.dropdown_custom_setting_name = QtWidgets.QComboBox(self.tab_custom_settings)
        self.dropdown_custom_setting_name.setGeometry(QtCore.QRect(165, 120, 300, 30))
        self.dropdown_custom_setting_name.setEditable(True)
        self.dropdown_custom_setting_name.setObjectName("dropdown_custom_setting_name")

        self.button_custom_setting_new = QtWidgets.QPushButton(self.tab_custom_settings)
        self.button_custom_setting_new.setGeometry(QtCore.QRect(165, 170, 130, 30))
        self.button_custom_setting_new.setObjectName("button_custom_setting_new")

        self.button_custom_setting_reset = QtWidgets.QPushButton(self.tab_custom_settings)
        self.button_custom_setting_reset.setGeometry(QtCore.QRect(165, 220, 130, 30))
        self.button_custom_setting_reset.setObjectName("button_custom_setting_reset")

        self.button_custom_setting_duplicate = QtWidgets.QPushButton(self.tab_custom_settings)
        self.button_custom_setting_duplicate.setGeometry(QtCore.QRect(335, 170, 130, 30))
        self.button_custom_setting_duplicate.setObjectName("button_custom_setting_duplicate")

        self.button_custom_setting_create = QtWidgets.QPushButton(self.tab_custom_settings)
        self.button_custom_setting_create.setGeometry(QtCore.QRect(45, 620, 200, 80))
        self.button_custom_setting_create.setObjectName("button_custom_setting_create")

        self.button_custom_setting_delete = QtWidgets.QPushButton(self.tab_custom_settings)
        self.button_custom_setting_delete.setGeometry(QtCore.QRect(280, 620, 200, 80))
        self.button_custom_setting_delete.setObjectName("button_custom_setting_delete")

        self.label_custom_setting_scroll_name = QtWidgets.QLabel(self.tab_custom_settings)
        self.label_custom_setting_scroll_name.setGeometry(QtCore.QRect(610, 90, 100, 30))
        self.label_custom_setting_scroll_name.setObjectName("label_custom_setting_scroll_name")


        self.label_custom_setting_scroll_id = QtWidgets.QLabel(self.tab_custom_settings)
        self.label_custom_setting_scroll_id.setGeometry(QtCore.QRect(750, 90, 100, 30))
        self.label_custom_setting_scroll_id.setObjectName("label_custom_setting_scroll_id")

        self.img_info_custom_setting_id = QtWidgets.QLabel(self.tab_custom_settings)
        self.img_info_custom_setting_id.setPixmap(QtGui.QPixmap('Assets/BoxManager/infobulle.png'))
        self.img_info_custom_setting_id.setScaledContents(True)
        self.img_info_custom_setting_id.setGeometry(QtCore.QRect(770, 90, 30, 30))
        self.img_info_custom_setting_id.setObjectName("img_info_custom_setting_id")

        self.button_custom_setting_scroll_add = QtWidgets.QPushButton(self.tab_custom_settings)
        self.button_custom_setting_scroll_add.setGeometry(QtCore.QRect(850, 90, 100, 30))
        self.button_custom_setting_scroll_add.setObjectName("button_custom_setting_scroll_add")

        self.scroll_custom_setting = QtWidgets.QScrollArea(self.tab_custom_settings)
        self.scroll_custom_setting.setGeometry(QtCore.QRect(560, 130, 400, 570))
        self.scroll_custom_setting.setWidgetResizable(True)
        self.scroll_custom_setting.setObjectName("scroll_custom_setting")

        self.scroll_custom_setting_widget = QtWidgets.QWidget()
        self.scroll_custom_setting_widget.setGeometry(QtCore.QRect(0, 0, 400, 450))
        self.scroll_custom_setting_widget.setObjectName("scroll_custom_setting_widget")

        self.layout_custom_setting = QtWidgets.QGridLayout(self.scroll_custom_setting_widget)
        self.layout_custom_setting.setObjectName('layout_custom_setting')

        ## Line sep
        self.line_sep_custom_setting = QtWidgets.QFrame(self.tab_custom_settings)
        self.line_sep_custom_setting.setGeometry(QtCore.QRect(510, 0, 20, 800))
        self.line_sep_custom_setting.setFrameShape(QtWidgets.QFrame.VLine)
        self.line_sep_custom_setting.setFrameShadow(QtWidgets.QFrame.Sunken)
        self.line_sep_custom_setting.setObjectName("line_sep_custom_setting")

        self.scroll_custom_setting.setWidget(self.scroll_custom_setting_widget)

        self.main_tabs.addTab(self.tab_custom_settings, "")



        MainWindow.setCentralWidget(self.centralwidget)
        self.retranslateUi(MainWindow)

        self.main_tabs.setCurrentIndex(0)
        self.config_tabs.setCurrentIndex(0)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

        boxname = self.dropdown_boxname.currentText()
        if len(boxname) == 0 :
            self.new_box()
        else :
            self.load_box(reset=False)

        cs_name = self.dropdown_custom_setting_name.currentText()
        if len(cs_name) == 0 :
            self.new_custom_setting()
        else :
            self.load_custom_setting()

    def retranslateUi(self, MainWindow):
        """Defines values and bind ui's button to function"""

        MainWindow.setWindowTitle(self._translate("Pybox-Manager", "Pybox-Manager"))

        # labels
        self.label_boxname.setText(self._translate("Pybox-Manager", "Box Name :"))
        self.label_script.setText(self._translate("Pybox-Manager", "Script :"))
        self.label_description.setText(self._translate("Pybox-Manager", "Description :"))
        self.label_mode.setText(self._translate("Pybox-Manager", "Mode :"))
        self.label_setting_name.setText(self._translate("Pybox-Manager", "Name"))
        self.label_setting_type.setText(self._translate("Pybox-Manager", "Type"))
        self.label_setting_value.setText(self._translate("Pybox-Manager", "Value"))
        self.label_inputs_name.setText(self._translate("Pybox-Manager", "Name"))
        self.label_inputs_type.setText(self._translate("Pybox-Manager", "Type"))
        self.label_outputs_name.setText(self._translate("Pybox-Manager", "Name"))
        self.label_outputs_type.setText(self._translate("Pybox-Manager", "Type"))
        self.label_settings_modifs.setText(self._translate("Pybox-Manager", "Enable settings modifications : "))
        self.label_inputs_modifs.setText(self._translate("Pybox-Manager", "Enable inputs modifications : "))
        self.label_outputs_modifs.setText(self._translate("Pybox-Manager", "Enable outputs modifications : "))
        self.label_category.setText(self._translate("PyBox-Manager", "Category : "))
        self.label_author.setText(self._translate("PyBox-Manager", "Author : "))

        self.label_add_stim.setText(self._translate("Pybox-Manager", "Add Stimulation/Label"))
        self.label_delete_stim.setText(self._translate("Pybox-Manager", "Delete Stimulation/Label"))
        self.label_name_stim_add.setText(self._translate("Pybox-Manager", "Name :"))
        self.label_sound_stim_add.setText(self._translate("Pybox-Manager", "Sound :"))
        self.label_name_stim_del.setText(self._translate("Pybox-Manager", "Name :"))

        self.label_custom_setting_create_delete.setText(self._translate("PyBox-Manager", "Create / Modify / Delete custom settings"))
        self.label_custom_setting_manage_value.setText(self._translate("PyBox-Manager", "Manage values"))
        self.label_custom_setting_name.setText(self._translate("PyBox-Manager", "Setting type :"))
        self.label_custom_setting_scroll_name.setText(self._translate("PyBox-Manager", "Name"))
        self.label_custom_setting_scroll_id.setText(self._translate("PyBox-Manager", "id"))

        # tabs
        self.main_tabs.setTabText(self.main_tabs.indexOf(self.tab_boxmanager), self._translate("Pybox-Manager", "Box Manager"))
        self.main_tabs.setTabText(self.main_tabs.indexOf(self.tab_labels_manager), self._translate("Pybox-Manager", "Stimulations/Labels Manager"))
        self.main_tabs.setTabText(self.main_tabs.indexOf(self.tab_custom_settings), self._translate("PyBox-Manager", "Custom Settings Manager"))
        
        self.config_tabs.setTabText(self.config_tabs.indexOf(self.tab_settings), self._translate("Pybox-Manager", "Settings"))
        self.config_tabs.setTabText(self.config_tabs.indexOf(self.tab_inputs), self._translate("Pybox-Manager", "Inputs"))
        self.config_tabs.setTabText(self.config_tabs.indexOf(self.tab_outputs), self._translate("Pybox-Manager", "Outputs"))

        # dropdown
        self.config_dropdown_boxname(mu.boxes)
        self.config_dropdown_category(mu.category)
        self.config_dropdown_mode(mu.modes)
        self.config_dropdown_stim_del(mu.stims)
        self.config_dropdown_custom_setting(mu.custom_settings)

        # button
        self.button_new.setText(self._translate("Pybox-Manager", "New"))
        self.button_new.clicked.connect(self.new_box)

        self.button_duplicate.setText(self._translate("Pybox-Manager", "Duplicate"))
        self.button_duplicate.clicked.connect(self.duplicate_box)

        self.button_script.setText(self._translate("Pybox-Manager", "Load File"))
        self.button_script.clicked.connect(self.load_python_script)

        self.button_reset.setText(self._translate("Pybox-Manager", "Reset Box"))
        self.button_reset.clicked.connect(self.reset_box)

        self.button_mode.setText(self._translate("Pybox-Manager", "Apply"))
        self.button_mode.clicked.connect(self.apply_mode)

        self.button_add_setting.setText(self._translate("Pybox-Manager", "Add Setting"))
        self.button_add_setting.clicked.connect(self.create_line_setting)

        self.button_add_input.setText(self._translate("Pybox-Manager", "Add Input"))
        self.button_add_input.clicked.connect(self.create_line_input)

        self.button_add_output.setText(self._translate("Pybox-Manager", "Add Output"))
        self.button_add_output.clicked.connect(self.create_line_output)

        self.button_box_create.setText(self._translate("Pybox-Manager", "Build"))
        self.button_box_create.clicked.connect(self.compile_create_box)

        self.button_box_delete.setText(self._translate("Pybox-Manager", "Delete Box"))
        self.button_box_delete.clicked.connect(self.compile_delete_box)



        self.button_sound_stim_add.setText(self._translate("Pybox-Manager", "Load File"))
        self.button_sound_stim_add.clicked.connect(self.load_sound_file)

        self.button_stim_create.setText(self._translate("Pybox-Manager", "Create"))
        self.button_stim_create.clicked.connect(self.create_label)

        self.button_stim_delete.setText(self._translate("Pybox-Manager", "Delete"))
        self.button_stim_delete.clicked.connect(self.delete_label)



        self.button_custom_setting_new.setText(self._translate("PyBox-Manager", "New"))
        self.button_custom_setting_new.clicked.connect(self.new_custom_setting)

        self.button_custom_setting_reset.setText(self._translate("PyBox-Manager", "Reset"))
        self.button_custom_setting_reset.clicked.connect(self.reset_custom_setting)

        self.button_custom_setting_duplicate.setText(self._translate("PyBox-Manager", "Duplicate"))
        self.button_custom_setting_duplicate.clicked.connect(self.duplicate_custom_setting)

        self.button_custom_setting_create.setText(self._translate("PyBox-Manager", "Create / Modify"))
        self.button_custom_setting_create.clicked.connect(self.compile_create_custom_setting)

        self.button_custom_setting_delete.setText(self._translate("PyBox-Manager", "Delete"))
        self.button_custom_setting_delete.clicked.connect(self.compile_delete_custom_setting)

        self.button_custom_setting_scroll_add.setText(self._translate("PyBox-Manager", "Add Value"))
        self.button_custom_setting_scroll_add.clicked.connect(self.create_line_add_value)

        # Update when value changed
        self.dropdown_boxname.currentTextChanged.connect(self.update_box)
        self.text_script.textChanged.connect(self.update_box)
        self.text_description.textChanged.connect(self.update_box)
        self.text_author.textChanged.connect(self.update_box)
        self.dropdown_category.activated.connect(lambda: self.update_box())

        self.checkbox_settings_modifs.stateChanged.connect(self.update_box)
        self.checkbox_inputs_modifs.stateChanged.connect(self.update_box)
        self.checkbox_outputs_modifs.stateChanged.connect(self.update_box)



        self.dropdown_custom_setting_name.currentTextChanged.connect(self.update_custom_settings)

        # Tooltips
        self.img_info_custom_setting_id.setToolTip("""You can choose to apply one of the two following modes :\n
    OV-mode : 
    This is the default mode in OpenViBE to use boxes. The box will have only one 
    Streamed Matrix and one Stimulation as inputs. The box is able to identify the data's 
    label with the stimulation it receives.\n
    Poly-mode : 
    This mode is usefull when you have several files containing your data : one 
    file for each label. The box will have several Streamed Matrix inputs, but no Stimulations or
    Signals inputs. The box will then consider that each input corresponds to a different label.""")

        self.img_info_setting_type.setToolTip("""If you want to use custom type, you have to launch
the manager with the argument : \"mode=developer\".""")

        self.img_info_checkbox_modifications.setToolTip("""Uncheck to disable modifications in OpenViBE.""")

        self.img_info_custom_setting_id.setToolTip("""Each id must be Integer and different.""" )




    # config dropdown functions

    def config_dropdown_boxname(self, boxes) :
        boxes = list(boxes.keys())
        boxes.sort()

        for i, boxname in enumerate(boxes) :
            self.dropdown_boxname.addItem("")
            self.dropdown_boxname.setItemText(i, boxname)
        self.dropdown_boxname.setCurrentIndex(0)
        self.dropdown_boxname.activated.connect(lambda: self.load_box(reset=False))

    def config_dropdown_category(self, categories) :
        categories.sort()

        for i, cat in enumerate(categories) :
            self.dropdown_category.addItem(cat)
        self.dropdown_category.setCurrentIndex(0)

    def config_dropdown_stim_del(self, stims) :
        stims.sort()
        self.dropdown_stim_del.setCurrentText(self._translate("Pybox-Manager", stims[0]))
        for i, s in enumerate(stims) :
            self.dropdown_stim_del.addItem("")
            self.dropdown_stim_del.setItemText(i, self._translate("Pybox-Manager", s))

    def config_dropdown_mode(self, modes) :
        self.dropdown_mode.setCurrentText(self._translate("Pybox-Manager", modes[0]))
        for i, m in enumerate(modes) :
            self.dropdown_mode.addItem("")
            self.dropdown_mode.setItemText(i, self._translate("Pybox-Manager", m))

    def config_dropdown_custom_setting(self, custom_settings) :
        custom_settings = list(custom_settings.keys())
        custom_settings.sort()

        for i, cs_name in enumerate(custom_settings) :
            self.dropdown_custom_setting_name.addItem(self._translate("PyBox-Manager", cs_name))
        self.dropdown_custom_setting_name.setCurrentIndex(0)
        self.dropdown_custom_setting_name.activated.connect(self.load_custom_setting)

    # Behaviour all tabs

    def delete_line(self, button) :
        # We search which settings / inputs / outputs lines do have buttons
        # We delete them and update the boxes

        def update_layout(layout, lines) :
            # Replace all the lines 
            for line in lines :
                for elem in line :
                    layout.removeWidget(elem)

            for i, line in enumerate(lines) :
                for j, elem in enumerate(line) :
                    layout.addWidget(elem, i, j, 1, 1)


        for line in self.current_settings :
            if line.button_delete == button :
                line.name.deleteLater()
                line.type.deleteLater()
                line.value.deleteLater()
                line.button_delete.deleteLater()
                self.current_settings.remove(line)
                
                update_layout(self.layout_settings, self.current_settings)
                self.update_box()
                return

        for line in self.current_inputs :
            if line.button_delete == button :          
                line.name.deleteLater()
                line.type.deleteLater()
                line.button_delete.deleteLater()
                self.current_inputs.remove(line)
                lines_layout = self.current_inputs

                update_layout(self.layout_inputs, self.current_inputs)
                self.update_box()
                return

        for line in self.current_outputs :
            if line.button_delete == button :          
                line.name.deleteLater()
                line.type.deleteLater()
                line.button_delete.deleteLater()
                self.current_outputs.remove(line)
                lines_layout = self.current_outputs

                update_layout(self.layout_outputs, self.current_outputs)
                self.update_box()
                return

        for line in self.current_custom_settings_lines :
            if line.button_delete == button :
                line.name.deleteLater()
                line.value.deleteLater()
                line.button_delete.deleteLater()
                self.current_custom_settings_lines.remove(line)
                lines_layout = self.current_custom_settings_lines

                update_layout(self.layout_custom_setting, self.current_custom_settings_lines)
                self.update_custom_settings()
                return

    # Behaviour BoxManager

    def clear_boxmanager(self, clear_name=True) :
        """ Clear all fields in the manager """

        self.current_box = None
        if clear_name :
            self.dropdown_boxname.setCurrentText(self._translate("Pybox-Manager", ""))
        self.text_script.setText(self._translate("Pybox-Manager", ""))
        self.text_description.setText(self._translate("Pybox-Manager", ""))

        for line in self.current_settings :
            line.name.deleteLater()
            line.type.deleteLater()
            line.value.deleteLater()
            line.button_delete.deleteLater()

        for line in self.current_inputs + self.current_outputs :
            line.name.deleteLater()
            line.type.deleteLater()
            line.button_delete.deleteLater()

        self.checkbox_settings_modifs.setChecked(True)
        self.checkbox_inputs_modifs.setChecked(True)
        self.checkbox_outputs_modifs.setChecked(True)

        self.current_settings = []
        self.current_inputs = []
        self.current_outputs = []

    def load_python_script(self) :
        """Help the user to select the python script that will be used by the current box"""

        def get_boxname(dico, uri) :
            symbol = '/'
            if system == 'Windows' :
                symbol = '\\'

            name = uri.split(symbol)[-1].split(".")[0]
            current_name = name
            i = 0
            while True :
                flag = False
                for bn in dico.keys() :
                    if bn == current_name :
                        i += 1
                        current_name = '{}_{}'.format(name, i)
                        flag = True
                        break
                if not flag :
                    return current_name


        uri = QFileDialog().getOpenFileName(caption="Select the python script to insert.",
                                            directory=mu.manager_folder,
                                            filter="*.py")[0]
        if uri == "" :
            # The user close the dialog without pick any file.
            return 

        uri = os.path.relpath(uri, mu.openvibe_folder + 'dist/extras-Release/')

        if len(self.current_box.name) == 0 :
            boxname = get_boxname(mu.boxes, uri)
            del(mu.boxes[''])
            mu.boxes[boxname] = mu.BoxPython(name=boxname, path_script=uri)

            index = len(mu.boxes.keys()) - 1
            self.dropdown_boxname.setItemText(index, self._translate("Pybox-Manager", boxname))
            self.dropdown_boxname.setCurrentIndex(index)

        self.text_script.setText(self._translate("Pybox-Manager", uri))

    def load_box(self, reset=False) :
        """Load a box and all of its settings."""

        self.clear_boxmanager(clear_name=False)

        boxname = self.dropdown_boxname.currentText()
        
        if len(boxname) == 0 :
            return

        self.is_loaded = True


        if reset :
            # We reset the box
            self.current_box = mu.find_all_boxes(mu.manager_folder, mu.io_type, mu.all_settings_type)[boxname]
        else :
            # we load the box
            self.current_box = mu.boxes[boxname]

        self.current_settings = []
        self.current_inputs = []
        self.current_outputs = []

        # description, script, author, category
        self.text_description.setText(self._translate("Pybox-Manager", self.current_box.description.replace('\\n', '\n')))
        self.text_script.setText(self._translate("Pybox-Manager", self.current_box.py_script))
        self.text_author.setText(self._translate("PyBox-Manager", self.current_box.author))

        index = self.dropdown_category.findText(self.current_box.category, QtCore.Qt.MatchFixedString)
        self.dropdown_category.setCurrentIndex(index)

        # Settings loading
        for i, setting in enumerate(self.current_box.settings.values()) :
            # Retrieve the values
            self.create_line_setting()
            line = self.current_settings[i]
            name, kind, value = setting
            # Set the values
            line.name.setText(self._translate("Pybox-Manager", name))
            index = line.type.findText(kind, QtCore.Qt.MatchFixedString)
            if index >= 0 :
                line.type.setCurrentIndex(index)
            line.value.setText(self._translate("Pybox-Manager", str(value)))

        # Inputs loading
        for i, my_input in enumerate(self.current_box.inputs.values()) :
            # Retrieve the values
            self.create_line_input()
            line = self.current_inputs[i]
            name, kind = my_input
            # Set the values
            line.name.setText(self._translate("Pybox-Manager", name))
            index = line.type.findText(kind, QtCore.Qt.MatchFixedString)
            if index >= 0 :
                line.type.setCurrentIndex(index)

        # Outputs loading
        for i, my_output in enumerate(self.current_box.outputs.values()) :
            # Retrieve the values
            self.create_line_output()
            line = self.current_outputs[i]
            name, kind = my_output
            # Set the values
            line.name.setText(self._translate("Pybox-Manager", name))
            index = line.type.findText(kind, QtCore.Qt.MatchFixedString)
            if index >= 0 :
                line.type.setCurrentIndex(index)

        # Modify Permissions
        self.checkbox_settings_modifs.setChecked(self.current_box.modify_settings)
        self.checkbox_inputs_modifs.setChecked(self.current_box.modify_inputs)
        self.checkbox_outputs_modifs.setChecked(self.current_box.modify_outputs)

        self.is_loaded = False
        self.update_box()

    def reset_box(self) :
        """Remove all changes since the last compilation.""" 
        if self.current_box is None  :
            return

        boxes = mu.find_all_boxes(mu.manager_folder, mu.io_type, mu.all_settings_type)
        current_name = self.current_box.name

        if current_name in boxes :
            # the box already exists
            self.load_box(reset=True)

        else :
            self.clear_boxmanager(clear_name=False)
            self.current_box = mu.BoxPython(name=current_name, desc='', path_script='')

    def new_box(self) :
        """Clear every field and prepare new box"""

        self.clear_boxmanager()

        self.current_box = mu.BoxPython(name='', desc='', path_script='')
        mu.boxes[''] = self.current_box

        index = len(mu.boxes.keys())-1
        self.dropdown_boxname.addItem('')
        self.dropdown_boxname.setCurrentIndex(index)

    def duplicate_box(self) :
        """ Duplicate the current box"""

        if self.current_box is None :
            mu.warning_msg('Cannot duplicate the box, no box has been selected.')
            return

        box = self.extract_infos_box()
        box.name = mu.get_name_duplicate(mu.boxes, box.name)
        # print(mu.boxes)
        mu.boxes[box.name] = box
        self.current_box = box
        # print(mu.boxes)

        index = len(mu.boxes.keys())
        self.dropdown_boxname.addItem('')
        self.dropdown_boxname.setItemText(index-1, self._translate("Pybox-Manager", box.name))
        self.dropdown_boxname.setCurrentIndex(index-1)

    def create_line_setting(self) :
        """Create a new line of setting and update the box"""

        def config_dropdown_settings_type(self, dropdown) :
            types = sorted(list(mu.settings_type.keys()))
            dropdown.setCurrentText(self._translate("Pybox-Manager", types[0]))
            for i, t in enumerate(types) :
                dropdown.addItem("")
                dropdown.setItemText(i, self._translate("Pybox-Manager", t))

            # We add to the dropdown all custom settings used by the box
            for k, (name, cid, value) in self.current_box.settings.items() :
                if cid not in mu.settings_type :
                    dropdown.addItem(self._translate("PyBox-Manager", cid))

        y = len(self.current_settings)

        text = QtWidgets.QLineEdit(self.scroll_settings_widget)
        text.setObjectName("text_setting_name")
        text.textChanged.connect(self.update_box)
        
        dropdown = QtWidgets.QComboBox(self.scroll_settings_widget)
        dropdown.setObjectName("dropdown_setting_type")
        config_dropdown_settings_type(self, dropdown)
        dropdown.currentIndexChanged.connect(self.update_box)

        value = QtWidgets.QLineEdit(self.scroll_settings_widget)
        value.setObjectName("value_setting_value")
        value.textChanged.connect(self.update_box)


        button_delete = QtWidgets.QPushButton(self.scroll_settings_widget)
        button_delete.setObjectName('button_setting_delete')
        button_delete.setText(self._translate("Pybox-Manager", "Delete"))
        button_delete.clicked.connect(lambda : self.delete_line(button_delete))

        self.layout_settings.addWidget(text, y, 0, 1, 1)
        self.layout_settings.addWidget(dropdown, y, 1, 1, 1)
        self.layout_settings.addWidget(value, y, 2, 1, 1)
        self.layout_settings.addWidget(button_delete, y, 3, 1, 1)

        self.current_settings += [mu.Setting(text, dropdown, value, button_delete)]
        self.centralwidget.show()
        self.update_box()

    def create_line_input(self) :
        """Create a new line of input and update the box"""

        def config_dropdown_input_type(self, dropdown) :
            types = list(mu.io_type.keys())
            dropdown.setCurrentText(self._translate("Pybox-Manager", types[0]))
            for i, t in enumerate(types) :
                dropdown.addItem("")
                dropdown.setItemText(i, self._translate("Pybox-Manager", t))

        y = len(self.current_inputs)

        text = QtWidgets.QLineEdit(self.scroll_inputs_widget)
        text.setObjectName("text_input_name")
        text.textChanged.connect(self.update_box)

        dropdown = QtWidgets.QComboBox(self.scroll_inputs_widget)
        dropdown.setObjectName("dropdown_input_type")
        config_dropdown_input_type(self, dropdown)
        dropdown.currentIndexChanged.connect(self.update_box)

        button_delete = QtWidgets.QPushButton(self.scroll_inputs_widget)
        button_delete.setObjectName('button_input_delete')
        button_delete.setText(self._translate("Pybox-Manager", "Delete"))
        button_delete.clicked.connect(lambda : self.delete_line(button_delete))

        self.layout_inputs.addWidget(text, y, 0, 1, 1)
        self.layout_inputs.addWidget(dropdown, y, 1, 1, 1)
        self.layout_inputs.addWidget(button_delete, y, 3, 1, 1)

        self.current_inputs += [mu.Input(text, dropdown, button_delete)]
        self.centralwidget.show()
        self.update_box()

    def create_line_output(self) :
        """Create a new line of output and update the box"""

        def config_dropdown_output_type(self, dropdown) :
            types = list(mu.io_type.keys())
            dropdown.setCurrentText(self._translate("Pybox-Manager", types[0]))
            for i, t in enumerate(types) :
                dropdown.addItem("")
                dropdown.setItemText(i, self._translate("Pybox-Manager", t))

        y = len(self.current_outputs)

        text = QtWidgets.QLineEdit(self.scroll_outputs_widget)
        text.setObjectName("text_output_name")
        text.textChanged.connect(self.update_box)

        dropdown = QtWidgets.QComboBox(self.scroll_outputs_widget)
        dropdown.setObjectName("dropdown_output_type")
        config_dropdown_output_type(self, dropdown)
        dropdown.currentIndexChanged.connect(self.update_box)

        button_delete = QtWidgets.QPushButton(self.scroll_outputs_widget)
        button_delete.setObjectName('button_output_delete')
        button_delete.setText(self._translate("Pybox-Manager", "Delete"))
        button_delete.clicked.connect(lambda : self.delete_line(button_delete))

        self.layout_outputs.addWidget(text, y, 0, 1, 1)
        self.layout_outputs.addWidget(dropdown, y, 1, 1, 1)
        self.layout_outputs.addWidget(button_delete, y, 3, 1, 1)

        self.current_outputs += [mu.Output(text, dropdown, button_delete)]
        self.centralwidget.show()
        self.update_box()

    def apply_mode(self) :
        """ Reset the inputs and add one steamed matrix and one stimulation input"""

        mode = self.dropdown_mode.currentText()
        # delete old inputs
        for line in self.current_inputs :
            line.name.deleteLater()
            line.type.deleteLater()
            line.button_delete.deleteLater()
        self.current_inputs = []

        if mode == 'poly-mode' :
            for i in range(6) :
                self.create_line_input()
                line = self.current_inputs[i]
                line.name.setText(self._translate("Pybox-Manager", "input_{}".format(i)))
                index = line.type.findText('Streamed Matrix', QtCore.Qt.MatchFixedString)
                if index >= 0 :
                    line.type.setCurrentIndex(index)

        elif mode == 'ov-mode' :
            self.create_line_input()
            line = self.current_inputs[0]
            line.name.setText(self._translate("Pybox-Manager", "input_StreamMatrix"))
            index = line.type.findText('Streamed Matrix', QtCore.Qt.MatchFixedString)
            if index >= 0 :
                line.type.setCurrentIndex(index)

            self.create_line_input()
            line = self.current_inputs[1]
            line.name.setText(self._translate("Pybox-Manager", "input_Stimulations"))
            index = line.type.findText('Stimulations', QtCore.Qt.MatchFixedString)
            if index >= 0 :
                line.type.setCurrentIndex(index)

        else :
            raise Exception("Mode {} not known.".format(mode))

    def extract_infos_box(self) :
        """Read all the fields and retrieve all information from the current box."""

        boxname = self.dropdown_boxname.currentText()
        boxdesc = self.text_description.toPlainText().replace('\n', '\\n')
        boxscript = self.text_script.text()
        box_author = self.text_author.text()
        box_category = self.dropdown_category.currentText()

        settings = {}
        for i, setting in enumerate(self.current_settings) :
            settings[i+2] = [setting.name.text(), setting.type.currentText(), setting.value.text()]

        inputs = {}
        
        # This operation is useful for streamed matrix label
        for i, inp in enumerate(self.current_inputs) :
            inputs[i+2] = [inp.name.text(), ' '.join([w for w in inp.type.currentText().split(' ') if len(w) > 0])]

        outputs = {}
        for i, out in enumerate(self.current_outputs) :
            outputs[i+2] = [out.name.text(), ' '.join([w for w in out.type.currentText().split(' ') if len(w) > 0])]

        box = mu.BoxPython(name=boxname, desc=boxdesc, path_script=boxscript, old_name=self.current_box.old_name)
        box.author = box_author
        box.category = box_category
        box.settings = settings
        box.inputs = inputs
        box.outputs = outputs
        box.modify_settings = self.checkbox_settings_modifs.isChecked()
        box.modify_inputs = self.checkbox_inputs_modifs.isChecked()
        box.modify_outputs = self.checkbox_outputs_modifs.isChecked()
        return box

    def compile_create_box(self) :
        """Modify the files for all the box that have been opened by the manager and compile openvibe.
        If the compilation failed, set all the files back to before all modifications."""

        if self.current_box is None :
            return

        if len(self.current_box.name.replace(' ', '')) == 0 :
            mu.warning_msg('The box needs a name.')
            return

        if len(self.current_box.py_script.replace(' ', '')) == 0 :
            mu.warning_msg('The box needs a script.')
            return

        old_boxes = mu.find_all_boxes(mu.manager_folder, mu.io_type, mu.all_settings_type)

        # rollback prep
        boxes_path = "{}/src/".format(mu.manager_folder)
        copy_path = mu.manager_folder + 'boxes_copy/'
        log_path = mu.manager_folder + 'compilation.log'

        try:
            shutil.copytree(boxes_path, copy_path)

        except Exception as e:
            print(e)

        
        for box in filter(lambda x: mu.boxes[x].to_be_updated, mu.boxes):

            cb = mu.boxes[box]

            if cb.name in old_boxes.keys() and cb.to_be_updated :
                # Modification of an existing box
                mu.delete_box(mu.manager_folder, cb.name)

            cb.to_be_updated = False
            
            mu.create_box(mu.openvibe_folder,
                        mu.manager_folder,
                        mu.settings_type,
                        mu.io_type,
                        cb.name,
                        cb.description,
                        cb.py_script,
                        cb.category,
                        cb.author,
                        cb.settings,
                        cb.inputs,
                        cb.outputs,
                        cb.modify_settings,
                        cb.modify_inputs,
                        cb.modify_outputs)

            
        print('Compilation in progress...')
        mu.compile(mu.manager_folder, mu.openvibe_folder)

        # catch error when building
        regex = '(recipe .*? failed)|(Error while building) '

        with open(log_path, 'r') as log_file:
            f = log_file.read()
            res = re.search(regex, f)

        if res is not None :
            # There was an error during compilation so we have to rollback the boxes
            print('An error occured during the compilation. A rollback has been applied. For more information, please refer to the logs in {}.'.format(log_path))

            shutil.rmtree(boxes_path)
            shutil.copytree(copy_path, boxes_path)

        else :
            print('Compilation successfull !\n')

            bns = list(mu.boxes.keys())
            path_bns = [boxes_path + 'box-algorithms/' + bns[i] + '.h' for i in range(len(bns))]
            path_bns = [pbns.replace('\\', '/').replace('//', '/') for pbns in path_bns]
            print('List of Python Box existing :\n{}'.format('\n'.join(['{} - {}'.format(bns[i], path_bns[i]) for i in range(len(bns))])))

        shutil.rmtree(copy_path)


    def compile_delete_box(self) :
        """Delete a box from the manager, to apply the deletion, openvibe needs to compile."""

        if self.current_box is None :
            return

        mu.delete_box(mu.manager_folder, self.current_box.name)
        # mu.compile(mu.manager_folder, mu.openvibe_folder)

        self.dropdown_boxname.removeItem(self.dropdown_boxname.currentIndex())
        boxname = self.dropdown_boxname.currentText()
        if len(boxname) > 0 :
            self.load_box(reset=False)
        else :
            self.new_box()
    
    def update_box(self) :
        """Retrieve all the settings the user as set to a local session memory."""

        if self.current_box is not None and not self.is_loaded : 

            old_name = self.current_box.name
            self.current_box = self.extract_infos_box()

            if self.current_box.name != old_name and self.current_box.name not in mu.boxes.keys() :
            # The name of the box has been modified
                del(mu.boxes[old_name])
                mu.boxes[self.current_box.name] = self.current_box

                index = self.dropdown_boxname.currentIndex()
                self.dropdown_boxname.setItemText(index, self._translate("Pybox-Manager", self.current_box.name))

            elif self.current_box.name == old_name :
                mu.boxes[self.current_box.name] = self.current_box
            
            mu.boxes[self.current_box.name].to_be_updated = True


    # Behaviour Stim manager

    def load_sound_file(self) :
        """Help the user to select a .mp3 file in his computer system.
        This will be the sound played by the box DatasetCreator when it comes to record
        the associated action."""

        uri = QFileDialog().getOpenFileName(caption="Select the .mp3 file.",
                                            directory=mu.manager_folder,
                                            filter="*.mp3")[0]
        self.text_sound_stim_add.setText(self._translate("Pybox-Manager", uri))

    def create_label(self) :
        """Add a stimlulation to openvibe, and compile openvibe."""

        label = self.text_name_stim_add.text()
        label = label.lower().replace(' ', '_')
        path_sound = self.text_sound_stim_add.text()
        if len(label) >= 2 and len(path_sound) != 0 :
            mu.add_stimulation(mu.manager_folder, label, path_sound)
            mu.compile(mu.manager_folder, mu.openvibe_folder)
            label = label[0].upper() + label[1:].lower()
            self.dropdown_stim_del.addItem(label)
        else :
            print('The stimulation name must have a size above 2, and the path to the sound has to be defined.')

    def delete_label(self) :
        """Remove a stimulation from openvibe and compile openvibe."""

        label = self.dropdown_stim_del.currentText()
        mu.delete_stimulation(mu.manager_folder, label)
        self.dropdown_stim_del.removeItem(self.dropdown_stim_del.currentIndex())
        mu.compile(mu.manager_folder, mu.openvibe_folder)

    # Behaviour custom types

    def create_line_add_value(self) :
        """Create a line in the ui to set a value to a custom setting"""
    
        y = len(self.current_custom_settings_lines)

        text = QtWidgets.QLineEdit(self.scroll_custom_setting_widget)
        text.setObjectName("setting_v_name")
        text.textChanged.connect(self.update_custom_settings)

        regex=QtCore.QRegExp('^[a-zA-Z0-9_]+( [a-zA-Z0-9_]+)*$')
        validator = QtGui.QRegExpValidator(regex)
        text.setValidator(validator)

        text_2 = QtWidgets.QLineEdit(self.scroll_custom_setting_widget)
        text_2.setObjectName("setting_v_value")
        text_2.textChanged.connect(self.update_custom_settings)

        button = QtWidgets.QPushButton(self.scroll_custom_setting_widget)
        button.setText(self._translate("PyBox-Manager", "Delete"))
        button.clicked.connect(lambda : self.delete_line(button))

        self.layout_custom_setting.addWidget(text, y, 0, 1, 1)
        self.layout_custom_setting.addWidget(text_2, y, 1, 1, 1)
        self.layout_custom_setting.addWidget(button, y, 2, 1, 1)

        self.current_custom_settings_lines += [mu.Custom_Setting_Line(text, text_2, button)]
        self.centralwidget.show()
        self.update_custom_settings()

    def clear_custom_settings_manager(self, clear_name=True) :
        """Clear every field in the custom setting window."""

        self.current_custom_setting = None
        if clear_name :
            self.dropdown_custom_setting_name.setCurrentText(self._translate("PyBox-Manager", ""))

        for line in self.current_custom_settings_lines :
            line.name.deleteLater()
            line.value.deleteLater()
            line.button_delete.deleteLater()

        self.current_custom_settings_lines = []

    def extract_infos_custom_setting(self) :
        """Retireve all information of the current custom setting, and 
        add it to a local session memory."""

        cs_name = self.dropdown_custom_setting_name.currentText()

        values = []
        for line in self.current_custom_settings_lines :
            values += [mu.Value(line.name.text(), line.value.text())]

        return mu.Custom_Setting(cs_name, values)

    def load_custom_setting(self, reset=False) :
        """Load all information from a custom setting and
        fill the fields with it."""

        self.clear_custom_settings_manager(clear_name=False)

        cs_name = self.dropdown_custom_setting_name.currentText()
        if len(cs_name) == 0 :
            return

        self.is_loaded = True

        if reset :
            # We reset the box
            self.current_custom_setting = mu.find_all_custom_settings(mu.manager_folder)[cs_name]
        else :
            # We load the box
            self.current_custom_setting = mu.custom_settings[cs_name]

        for i, value in enumerate(self.current_custom_setting.values) :
            # Retrieve the values
            self.create_line_add_value()
            line = self.current_custom_settings_lines[i]
            v_text, v_id = value
            # Set the values
            line.name.setText(self._translate("PyBox-Manager", v_text))
            line.value.setText(self._translate("PyBox-Manager", str(v_id)))

        self.is_loaded = False
        self.update_custom_settings()

    def new_custom_setting(self) :
        """Clear every fields and prepare a new custom setting."""

        self.clear_custom_settings_manager()

        self.current_custom_setting = mu.Custom_Setting(name='', values=[])
        mu.custom_settings[''] = self.current_custom_setting

        index = len(mu.custom_settings.keys()) - 1
        self.dropdown_custom_setting_name.addItem('')
        self.dropdown_custom_setting_name.setCurrentIndex(index)

    def reset_custom_setting(self) :
        """Reset all fields from the current custom setting."""

        if self.current_custom_setting is None :
            return

        custom_settings = mu.find_all_custom_settings(mu.manager_folder)
        current_name = self.current_custom_setting.name

        if current_name in custom_settings.keys() :
            # the setting already exists
            self.load_custom_setting(reset=True)
        else :
            self.clear_custom_settings_manager(clear_name=False)
            self.current_custom_setting = mu.Custom_Setting(name=current_name, values=[])

    def duplicate_custom_setting(self) :
        """Duplicate the current custom setting."""

        if self.current_custom_setting is None :
            mu.warning_msg('Cannot duplicate the custom setting, no custom setting have been selected.')
            return

        cs = self.extract_infos_custom_setting()
        cs_name = mu.get_name_duplicate(mu.custom_settings, cs.name)

        cs = mu.Custom_Setting(cs_name, cs.values)

        mu.custom_settings[cs.name] = cs
        self.current_custom_setting = cs

        index = len(mu.custom_settings.keys())
        self.dropdown_custom_setting_name.addItem(self._translate("PyBox-Manager", cs_name))
        self.dropdown_custom_setting_name.setCurrentIndex(index-1)

    def update_custom_settings(self) :
        """Verify if a modification has been made. If so,
        retireve all information from the current custom settings, 
        and save it to the local session memory """
        if self.current_custom_setting is not None and not self.is_loaded :

            old_name = self.current_custom_setting.name
            self.current_custom_setting = self.extract_infos_custom_setting()

            if self.current_custom_setting.name != old_name and \
                self.current_custom_setting.name not in mu.custom_settings.keys() :
                # The name of the custom setting has been modified
                del(mu.custom_settings[old_name])
                mu.custom_settings[self.current_custom_setting.name] = self.current_custom_setting

                index = self.dropdown_custom_setting_name.currentIndex()
                self.dropdown_custom_setting_name.setItemText(index, self._translate("PyBox-Manager", self.current_custom_setting.name))

            elif self.current_custom_setting.name == old_name :
                mu.custom_settings[self.current_custom_setting.name] = self.current_custom_setting

    def compile_create_custom_setting(self) :
        """Make all the modification to create/modify the current custom setting
        Then compile OpenViBE"""
        if self.current_custom_setting is None :
            return

        old_custom_settings = mu.find_all_custom_settings(mu.manager_folder)
        if self.current_custom_setting.name in old_custom_settings.keys() :
            # Modification of an existing custom setting
            mu.delete_custom_setting(mu.manager_folder, self.current_custom_setting)

        mu.create_custom_setting(mu.manager_folder, mu.openvibe_folder, self.current_custom_setting)
        mu.compile(mu.manager_folder, mu.openvibe_folder)

    def compile_delete_custom_setting(self) :
        """Make all the modification to delete the current custom setting
        Then compile OpenViBE"""
        if self.current_custom_setting is None :
            return 

        mu.delete_custom_setting(mu.manager_folder, self.current_custom_setting)
        mu.compile(mu.manager_folder, mu.openvibe_folder)

        self.dropdown_custom_setting_name.removeItem(self.dropdown_custom_setting_name.currentIndex())
        cs_name = self.dropdown_custom_setting_name.currentText()
        if len(cs_name) > 0 :
            self.load_custom_setting()
        else :
            self.new_custom_setting()




if __name__ == "__main__":
    
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = Ui_MainWindow()
    ui.setupUi(MainWindow)
    MainWindow.show()
    sys.exit(app.exec_())

