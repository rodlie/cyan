TEMPLATE = subdirs
CONFIG -= ordered
SUBDIRS += app solutions format
app.depends += format solutions
