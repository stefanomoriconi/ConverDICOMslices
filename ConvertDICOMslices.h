/* Include ITK libraries for Reading DICOM image series */
#include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkImageSeriesWriter.h"

#include "itksys/SystemTools.hxx"
#include "itkVersion.h"

#include "itkImageFileWriter.h"
#include "itkImageFileWriter.h"
#include "itkMetaDataObject.h"

#if ITK_VERSION_MAJOR >= 4
#include "gdcmUIDGenerator.h"
#else
#include "gdcm/src/gdcmFile.h"
#include "gdcm/src/gdcmUtil.h"
#endif

// Include ITK Libraries for image preprocessing
#include "itkRescaleIntensityImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkVersorRigid3DTransform.h"
#include "itkCenteredTransformInitializer.h"
#include "itkResampleImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkShiftScaleImageFilter.h"

// library needed to easily convert int to std::string
#include <sstream>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
