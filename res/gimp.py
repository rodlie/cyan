#!/usr/bin/env python

'''
# Copyright or © or Copr. Ole-André Rodlie, INRIA.
#
# ole.andre.rodlie@gmail.com
#
# This software is governed by the CeCILL license under French law and
# abiding by the rules of distribution of free software. You can use,
# modify and / or redistribute the software under the terms of the CeCILL
# license as circulated by CEA, CNRS and INRIA at the following URL
# "https://www.cecill.info".
#
# As a counterpart to the access to the source code and rights to
# modify and redistribute granted by the license, users are provided only
# with a limited warranty and the software's author, the holder of the
# economic rights and the subsequent licensors have only limited
# liability.
#
# In this respect, the user's attention is drawn to the associated risks
# with loading, using, modifying and / or developing or reproducing the
# software by the user in light of its specific status of free software,
# that can mean that it is complicated to manipulate, and that also
# so that it is for developers and experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to test and test the software's suitability
# Requirements in the conditions of their systems
# data to be ensured and, more generally, to use and operate
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL license and that you accept its terms.
'''

from gimpfu import *
import subprocess
import os
import sys
import time
import shutil
import os.path
import tempfile

cyanversion = "1.2.0"
cyanbin = "cyan"

def plugin_maketempfile( image, src ):

    tempimage = pdb.gimp_image_duplicate( image )

    if not tempimage:
        print "Could not create temporary image file."
        return None, None, None

    tempfilename = pdb.gimp_temp_name( "tif" )

    if sys.platform == "darwin":
        tempfilename = os.path.join(tempfile.gettempdir(), "cyan-tmp.tif")

    if sys.platform.startswith( "win" ):
        tempfilename = tempfilename.replace( "\\", "/" )
    
    tempdrawable = pdb.gimp_image_get_active_drawable( tempimage )

    pdb.gimp_progress_set_text( "Saving a copy" )
   
    tempimage.flatten() 
    pdb.gimp_file_save( tempimage, tempimage.layers[0], tempfilename, "" )
    
    return tempfilename, tempdrawable, tempimage

#----------------------------------------------------------------------------------

def plugin_export( image, src):

    tempfilename, tempdrawable, tempimage = plugin_maketempfile( image, 0 )
    
    if tempfilename == None:
        return
    
    pdb.gimp_image_undo_group_start(image)
    pdb.gimp_progress_pulse()
    child = subprocess.Popen( cyanbin + " " + tempfilename, stderr=subprocess.PIPE, stdout=subprocess.PIPE, shell=True )

    pdb.gimp_progress_set_text( "Waiting on Cyan ..." )
    while child.poll() is None:
        pdb.gimp_progress_pulse()
        time.sleep(0.2)
        
    plugin_tidyup( tempfilename )
    pdb.gimp_image_undo_group_end(image)

def plugin_import(image,src):

    tempfilename, tempdrawable, tempimage = plugin_maketempfile( image, src )

    if tempfilename == None:
        return

    pdb.gimp_image_undo_group_start(image)
    pdb.gimp_progress_pulse()
    child = subprocess.Popen( cyanbin + " -o " + tempfilename, stderr=subprocess.PIPE, stdout=subprocess.PIPE, shell=True )

    pdb.gimp_progress_set_text( "Waiting on Cyan ..." )
    while child.poll() is None:
        pdb.gimp_progress_pulse()
        time.sleep(0.2)

    dest = 0;
    plugin_saveresult( image, dest, tempfilename, tempimage )

    plugin_tidyup( tempfilename )
    pdb.gimp_image_undo_group_end(image)

def plugin_saveresult( image, dest, tempfilename, tempimage ):

    # Get image file name
    name = image.filename

    if dest == 0 :
        # new image
        try:
            newimage = pdb.file_tiff_load( tempfilename, "" )

            # Write name
            if name != None:
                newimage.filename = name
	    gimp.Display( newimage )

	except:
            print "Could not load tmep file as new image."

    elif dest == 1:
        # Replace current layer

        pos = pdb.gimp_image_get_item_position( image, image.active_layer )

        try:
            newlayer = pdb.gimp_file_load_layer( image, tempfilename )

            image.remove_layer( image.active_layer )

            image.add_layer( newlayer, pos )
        except:
            print "Could not load temp file into existing layer."

    elif dest == 2:
        # Add as a new layer in the opened image
        try:
            newlayer = pdb.gimp_file_load_layer( image, tempfilename )

            image.add_layer( newlayer,0 )
        except:
            print "Could not load temp file into new layer."

    # cleanup
    plugin_tidyup( tempfilename )

    # Note the new image is dirty in Gimp and the user will be asked to save before closing.

    gimp.displays_flush()

    gimp.delete( tempimage )   # delete the temporary image

#----------------------------------------------------------------------------------

def plugin_tidyup( fname ):

    if os.access( fname, os.F_OK ):
        os.remove( fname )

#----------------------------------------------------------------------------------

register(
                "cyan-export",
                "Export Image.",
                "Export Image.",
                "Ole-Andre Rodlie (olear@dracolinux.org)",
                "Copyright 2017 Ole-Andre Rodlie",
                "2017",
                "<Image>/Cyan/Export Image",
                "*", # image types
		[],
		[],
                plugin_export,
                )
register(
                "cyan-import",
                "Import Image.",
                "Import Image.",
                "Ole-Andre Rodlie (olear@dracolinux.org)",
                "Copyright 2017 Ole Andre Rodlie",
                "2017",
                "<Image>/Cyan/Import Image",
                "*", # image types
		[],
                [],
                plugin_import,
                )

main()
  
#----------------------------------------------------------------------------------
