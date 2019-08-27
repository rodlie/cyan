TEMPLATE = subdirs
CONFIG -= ordered
SUBDIRS += solutions editor
editor.depends += solutions
