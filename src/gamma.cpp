#include "gamma.h"
#include <QFile>
#include <QDebug>

#ifdef Q_OS_UNIX
#ifndef Q_OS_MAC
#if QT_VERSION >= 0x050000
#include <QtX11Extras/QX11Info>
#else
#include <QX11Info>
#endif
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/xf86vmode.h>
#endif
#endif

/*
 * FUNCTION read_vcgt_internal
 *
 * this is a parser for the vcgt tag of ICC profiles which tries to
 * resemble most of the functionality of Graeme Gill's icclib.
 *
 * returns
 * -1: file could not be read
 * 0: file okay but doesn't contain vcgt or MLUT tag
 * 1: success
 */

/*
 * Function taken from xcalib.c (http://xcalib.sourceforge.net/)
 * (c) 2004-2005 Stefan Doehla <stefan AT doehla DOT de>
 *
 * You should have received a copy of the GPL together with this
 * software. Furthermore, it is postcardware. So if you like this
 * program, send me a picture postcard from your country/area to:
 *
 * Stefan Doehla
 * Steinselb 7
 * 95100 Selb
 * GERMANY
 */

#include <math.h>

#define VCGT_TAG     0x76636774L
#define MLUT_TAG     0x6d4c5554L

#ifndef MAX_TABLE_SIZE
# define MAX_TABLE_SIZE   2e10
#endif

#if 1
# define BE_INT(a)    ((a)[3]+((a)[2]<<8)+((a)[1]<<16) +((a)[0]<<24))
# define BE_SHORT(a)  ((a)[1]+((a)[0]<<8))
#else
# warning "big endian is NOT TESTED"
# define BE_INT(a)    (a)
# define BE_SHORT(a)  (a)
#endif

int
read_vcgt_internal(const char * filename, u_int16_t * rRamp, u_int16_t * gRamp,
               u_int16_t * bRamp, unsigned int nEntries)
{
  FILE * fp;
  unsigned int bytesRead;
  unsigned int numTags=0;
  unsigned int tagName=0;
  unsigned int tagOffset=0;
  unsigned int tagSize=0;
  unsigned char cTmp[4];
  unsigned int uTmp;
  unsigned int gammaType;

  float gamma_cor = 1.0;

  signed int retVal=0;

  u_int16_t * redRamp = NULL, * greenRamp = NULL, * blueRamp = NULL;
  unsigned int ratio=0;
  /* formula */
  float rGamma, rMin, rMax;
  float gGamma, gMin, gMax;
  float bGamma, bMin, bMax;
  int i=0;
  /* table */
  unsigned int numChannels=0;
  unsigned int numEntries=0;
  unsigned int entrySize=0;
  int j=0;

  if(filename) {
    fp = fopen(filename, "rb");
    if(!fp)
      return -1; /* file can not be opened */
  } else
    return -1; /* filename char pointer not valid */
  /* skip header */
  if(fseek(fp, 0+128, SEEK_SET))
    return  -1;
  /* check num of tags in current profile */
  bytesRead = fread(cTmp, 1, 4, fp);
  numTags = BE_INT(cTmp);
  for(i=0; i<(int)numTags; i++) {
    bytesRead = fread(cTmp, 1, 4, fp);
    tagName = BE_INT(cTmp);
    bytesRead = fread(cTmp, 1, 4, fp);
    tagOffset = BE_INT(cTmp);
    bytesRead = fread(cTmp, 1, 4, fp);
    tagSize = BE_INT(cTmp);
    if(!bytesRead)
      break;
    if(tagName == MLUT_TAG)
    {
      if(fseek(fp, 0+tagOffset, SEEK_SET))
        break;

      redRamp = (unsigned short *) malloc ((256) * sizeof (unsigned short));
      greenRamp = (unsigned short *) malloc ((256) * sizeof (unsigned short));
      blueRamp = (unsigned short *) malloc ((256) * sizeof (unsigned short));
      {
        for(j=0; j<256; j++) {
          bytesRead = fread(cTmp, 1, 2, fp);
          redRamp[j]= BE_SHORT(cTmp);
        }
        for(j=0; j<256; j++) {
          bytesRead = fread(cTmp, 1, 2, fp);
          greenRamp[j]= BE_SHORT(cTmp);
        }
        for(j=0; j<256; j++) {
          bytesRead = fread(cTmp, 1, 2, fp);
          blueRamp[j]= BE_SHORT(cTmp);
        }
      }
      /* simply copy values to the external table (and leave some values out if table size < 256) */
      ratio = (unsigned int)(256 / (nEntries));
      for(j=0; j<(int)nEntries; j++) {
        rRamp[j] = redRamp[ratio*j];
        gRamp[j] = greenRamp[ratio*j];
        bRamp[j] = blueRamp[ratio*j];
      }
      free(redRamp);
      free(greenRamp);
      free(blueRamp);
      retVal = 1;
      break;
    }
    if(tagName == VCGT_TAG)
    {
      fseek(fp, 0+tagOffset, SEEK_SET);
      bytesRead = fread(cTmp, 1, 4, fp);
      tagName = BE_INT(cTmp);
      if(tagName != VCGT_TAG)
      {
        qWarning() << "invalid content of table vcgt, starting with" << tagName;
        break;
      }
      bytesRead = fread(cTmp, 1, 4, fp);
      bytesRead = fread(cTmp, 1, 4, fp);
      gammaType = BE_INT(cTmp);
      /* VideoCardGammaFormula */
      if(gammaType==1) {
        bytesRead = fread(cTmp, 1, 4, fp);
        uTmp = BE_INT(cTmp);
        rGamma = (float)uTmp/65536.0;
        bytesRead = fread(cTmp, 1, 4, fp);
        uTmp = BE_INT(cTmp);
        rMin = (float)uTmp/65536.0;
        bytesRead = fread(cTmp, 1, 4, fp);
        uTmp = BE_INT(cTmp);
        rMax = (float)uTmp/65536.0;
        bytesRead = fread(cTmp, 1, 4, fp);
        uTmp = BE_INT(cTmp);
        gGamma = (float)uTmp/65536.0;
        bytesRead = fread(cTmp, 1, 4, fp);
        uTmp = BE_INT(cTmp);
        gMin = (float)uTmp/65536.0;
        bytesRead = fread(cTmp, 1, 4, fp);
        uTmp = BE_INT(cTmp);
        gMax = (float)uTmp/65536.0;
        bytesRead = fread(cTmp, 1, 4, fp);
        uTmp = BE_INT(cTmp);
        bGamma = (float)uTmp/65536.0;
        bytesRead = fread(cTmp, 1, 4, fp);
        uTmp = BE_INT(cTmp);
        bMin = (float)uTmp/65536.0;
        bytesRead = fread(cTmp, 1, 4, fp);
        uTmp = BE_INT(cTmp);
        bMax = (float)uTmp/65536.0;

        if(rGamma > 5.0 || gGamma > 5.0 || bGamma > 5.0)
        {
          qWarning() << "Gamma values out of range (> 5.0)" << rGamma << gGamma << bGamma;
          break;
        }
        if(rMin >= 1.0 || gMin >= 1.0 || bMin >= 1.0)
        {
          qWarning() << "Gamma lower limit out of range (>= 1.0)" << rMin << gMin << bMin;
          break;
        }
        if(rMax > 1.0 || gMax > 1.0 || bMax > 1.0)
        {
          qWarning() << "Gamma upper limit out of range (> 1.0)" << rMax << gMax << bMax;
          break;
        }

        for(j=0; j<(int)nEntries; j++) {
          rRamp[j] = 65536.0 *
            ((double) pow ((double) j / (double) (nEntries),
                           rGamma * (double) gamma_cor
                          ) * (rMax - rMin) + rMin);
          gRamp[j] = 65536.0 *
            ((double) pow ((double) j / (double) (nEntries),
                           gGamma * (double) gamma_cor
                          ) * (gMax - gMin) + gMin);
          bRamp[j] = 65536.0 *
            ((double) pow ((double) j / (double) (nEntries),
                           bGamma * (double) gamma_cor
                          ) * (bMax - bMin) + bMin);
        }
        retVal = 1;
      }
      /* VideoCardGammaTable */
      else if(gammaType==0) {
        bytesRead = fread(cTmp, 1, 2, fp);
        numChannels = BE_SHORT(cTmp);
        bytesRead = fread(cTmp, 1, 2, fp);
        numEntries = BE_SHORT(cTmp);
        bytesRead = fread(cTmp, 1, 2, fp);
        entrySize = BE_SHORT(cTmp);

        /* work-around for AdobeGamma-Profiles */
        if(tagSize == 1584) {
          entrySize = 2;
          numEntries = 256;
          numChannels = 3;
        }

        if(numChannels!=3)          /* assume we have always RGB */
          break;

        /* allocate tables for the file plus one entry for extrapolation */
        redRamp = (unsigned short *) malloc ((numEntries+1) * sizeof (unsigned short));
        greenRamp = (unsigned short *) malloc ((numEntries+1) * sizeof (unsigned short));
        blueRamp = (unsigned short *) malloc ((numEntries+1) * sizeof (unsigned short));
        {
          for(j=0; j<(int)numEntries; j++) {
            switch(entrySize) {
              case 1:
                bytesRead = fread(cTmp, 1, 1, fp);
                redRamp[j]= cTmp[0] << 8;
                break;
              case 2:
                bytesRead = fread(cTmp, 1, 2, fp);
                redRamp[j]= BE_SHORT(cTmp);
                break;
            }
          }
          for(j=0; j<(int)numEntries; j++) {
            switch(entrySize) {
              case 1:
                bytesRead = fread(cTmp, 1, 1, fp);
                greenRamp[j]= cTmp[0] << 8;
                break;
              case 2:
                bytesRead = fread(cTmp, 1, 2, fp);
                greenRamp[j]= BE_SHORT(cTmp);
                break;
            }
          }
          for(j=0; j<(int)numEntries; j++) {
            switch(entrySize) {
              case 1:                bytesRead = fread(cTmp, 1, 1, fp);
                blueRamp[j]= cTmp[0] << 8;
                break;
              case 2:
                bytesRead = fread(cTmp, 1, 2, fp);
                blueRamp[j]= BE_SHORT(cTmp);
                break;
            }
          }
        }

        if(numEntries >= nEntries) {
          /* simply subsample if the LUT is smaller than the number of entries in the file */
          ratio = (unsigned int)(numEntries / (nEntries));
          for(j=0; j<(int)nEntries; j++) {
            rRamp[j] = redRamp[ratio*j];
            gRamp[j] = greenRamp[ratio*j];
            bRamp[j] = blueRamp[ratio*j];
          }
        }
        else {
          ratio = (unsigned int)(nEntries / numEntries);
          /* add extrapolated upper limit to the arrays - handle overflow */
          redRamp[numEntries] = (redRamp[numEntries-1] + (redRamp[numEntries-1] - redRamp[numEntries-2])) & 0xffff;
          if(redRamp[numEntries] < 0x4000)
            redRamp[numEntries] = 0xffff;

          greenRamp[numEntries] = (greenRamp[numEntries-1] + (greenRamp[numEntries-1] - greenRamp[numEntries-2])) & 0xffff;
          if(greenRamp[numEntries] < 0x4000)
            greenRamp[numEntries] = 0xffff;

          blueRamp[numEntries] = (blueRamp[numEntries-1] + (blueRamp[numEntries-1] - blueRamp[numEntries-2])) & 0xffff;
          if(blueRamp[numEntries] < 0x4000)
            blueRamp[numEntries] = 0xffff;

          for(j=0; j<(int)numEntries; j++) {
            for(i=0; i<(int)ratio; i++)
            {
              rRamp[j*ratio+i] = (redRamp[j]*(ratio-i) + redRamp[j+1]*(i)) / (ratio);
              gRamp[j*ratio+i] = (greenRamp[j]*(ratio-i) + greenRamp[j+1]*(i)) / (ratio);
              bRamp[j*ratio+i] = (blueRamp[j]*(ratio-i) + blueRamp[j+1]*(i)) / (ratio);
            }
          }
        }
        free(redRamp);
        free(greenRamp);
        free(blueRamp);
        retVal = 1;
      }
      break;
    } /* for all tags */
  }
  fclose(fp);
  return retVal;
}

Gamma::Gamma(QObject *parent) : QObject(parent)
{
}

bool Gamma::clear()
{
#ifdef Q_OS_UNIX
#ifndef Q_OS_MAC
    Display *dpy = NULL;
    if ((dpy = QX11Info::display()) == NULL) {
        return false;
    }

    int screen = QX11Info::appScreen();
    XF86VidModeGamma gamma;
    gamma.red = 1.0;
    gamma.green = 1.0;
    gamma.blue = 1.0;

    if (XF86VidModeSetGamma(dpy, screen, &gamma)) {
        return true;
    }
#endif
#endif
    return false;
}

bool Gamma::apply(QString filename)
{
    QFile file(filename);
    if (!file.exists()) {
        return false;
    }
#ifdef Q_OS_UNIX
#ifndef Q_OS_MAC
    Display *dpy = NULL;
    if ((dpy = QX11Info::display()) == NULL) {
        return false;
    }

    int ramp_size = 0;
    int screen = QX11Info::appScreen();
    if (!XF86VidModeGetGammaRampSize(dpy, screen, &ramp_size)) {
        qWarning() << "Unable to query gamma ramp size - assuming 256";
        ramp_size = 265;
    }

    switch(ramp_size)
    {
      case 16:
      case 32:
      case 64:
      case 128:
      case 256:
      case 512:
      case 1024:
      case 2048:
      case 4096:
      case 8192:
      case 16384:
      case 32768:
      case 65536:
        break;
      default:
        qWarning() << "Unsupported ramp size" << ramp_size;
        return false;
    }

    bool status = true;
    u_int16_t *r_ramp = NULL, *g_ramp = NULL, *b_ramp = NULL;
    r_ramp = (unsigned short *) malloc (ramp_size * sizeof (unsigned short));
    g_ramp = (unsigned short *) malloc (ramp_size * sizeof (unsigned short));
    b_ramp = (unsigned short *) malloc (ramp_size * sizeof (unsigned short));
    int result = read_vcgt_internal(filename.toUtf8().data(), r_ramp, g_ramp, b_ramp, ramp_size);

    if( result <= 0) {
      if(result < 0) {
        qWarning() << "Unable to read file";
      }
      if(result == 0) {
        qWarning() << "No calibration data in ICC profile";
      }
      status = false;
    }

    if (status && !XF86VidModeSetGammaRamp(dpy, screen, ramp_size, r_ramp, g_ramp, b_ramp)) {
        qWarning() << "Unable to calibrate display";
        status = false;
    }

    free(r_ramp);
    free(g_ramp);
    free(b_ramp);
    return status;
#endif // Q_OS_MAC
#endif // Q_OS_UNIX
    return false;
}
