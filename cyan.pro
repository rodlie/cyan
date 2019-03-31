TEMPLATE = subdirs
CONFIG -= ordered
SUBDIRS += solutions converter editor
editor.depends += solutions

CONFIG(only_solutions) : SUBDIRS = solutions
CONFIG(only_converter) : SUBDIRS = converter
CONFIG(only_editor) : SUBDIRS = solutions editor 
