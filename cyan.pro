TEMPLATE = subdirs
CONFIG -= ordered
SUBDIRS += app lib
app.depends += lib
