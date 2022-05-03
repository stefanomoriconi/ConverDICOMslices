/* This Software is developed using ITK v.4.5
 * Please refer to official documentation at http://www.itk.org/Doxygen/html/
 * Author: Stefano Moriconi - CNR IBFM - Milano, Italy - October 31st 2014
 * Contacts: stefano.nicola.moriconi@gmail.com
 * 
 * Please Note: the following software manages the DICOM images with an intrinsic orientation to be corrected according to the orthogonal planes.
 * This means that, the output Converted Image will have orientation equal to: [1 0 0;0 1 0;0 0 1], after a proper interpolation.
 * Moreover in that case, a new set of re-oriented DICOM images will be created (within a new subfolder)!
 * NB: Since an interpolation is required, the more the intrinsic angle, the less the final accuracy!
 * 
 * WARNING: There is a known bug with the ITK DICOM SERIES MANAGER when exporting new DICOM images:
 * The dictionary for DICOM tags might have been changed with the standard updates, and so might have been changed also some default values.
 * (i.e. DICOM Rescale Intercept / Rescale Slope)
 * 
 * To overcome this issue, a Matlab® script is attached to correct the output headers of the re-created DICOM images.
 * Please note that this is a necessary workaround, in order to let the corrected re-created DICOM images be compatible with multi-platforms systems.
 */


#include "ConvertDICOMslices.h"

//static void CopyDictionary (itk::MetaDataDictionary &fromDict, itk::MetaDataDictionary &toDict);

int main( int argc, char* argv[] )
{
  // Controllo sul numero degli input
if( argc < 2 )
{
     std::cout << std::endl << "-------------------------------------------------" << std::endl << std::endl;
     std::cout << "ConvertDICOM is a tool for DICOM images conversion." << std::endl;
     std::cout << "Parse the desired DICOMpath with the images to be converted as input, and specify the final image output." << std::endl << std::endl;
     std::cout << "Usage: " << argv[0] << " DICOMpath " << "OutputImage.(nii,mhd,...) " << std::endl << std::endl;
     std::cout << "Example: To convert a set of DICOM images to 'image.nii' file..." << std::endl;
     std::cout << "type in the terminal : " << argv[0] << " /home/DICOMimages/ " << "/home/DICOMimages/image.nii" << std::endl;
     std::cout << "Et voilà!" << std::endl << std::endl;
     std::cout << "WARNING:" << std::endl;
     std::cout << "This software manages the DICOM images with an intrinsic orientation to be re-oriented according to the orthogonal planes." << std::endl;
     std::cout << "The output Converted Image will have orientation equal to: [1 0 0;0 1 0;0 0 1], after a proper interpolation." << std::endl;
     std::cout << "Moreover a new set of re-oriented DICOM images will be created (within a new subfolder)." << std::endl;
     std::cout << "Please Note: the more the intrinsic angle, the less the final accuracy." << std::endl;
     std::cout << "Optional outputs: OutputImage_OP.(nii,mhd,...) -- Re-Oriented Image" << std::endl;
     std::cout << "                  RestoredDICOMs               -- folder with Re-Oriented DICOM images" << std::endl;
     std::cout << std::endl << "-------------------------------------------------" << std::endl << std::endl;

return EXIT_FAILURE;
}

// Definizione del tipo di Pixel
typedef float itkDCMPixelType;
// Definizione della dimensione dell'immagine (3D - x,y,z)
const unsigned int imageDimension = 3;

// Definizione del tipo di immagine singola
typedef itk::Image<itkDCMPixelType,imageDimension> ImageType;
// Definizione del tipo di immagine 3D come DicomSeries
typedef itk::ImageSeriesReader< ImageType > ReaderType;

ReaderType::Pointer reader = ReaderType::New();
  
  typedef itk::GDCMImageIO ImageIOType;
  ImageIOType::Pointer dicomIO = ImageIOType::New();
  ImageIOType::Pointer dicomIOext = ImageIOType::New();
  reader->SetImageIO( dicomIO );
  
  typedef itk::GDCMSeriesFileNames NamesGeneratorType;
  NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
  nameGenerator->SetDirectory( argv[1] );

// Listare tutti i nomi dei file DICOM presenti nella directory
try
{
      //std::cout << std::endl << "The directory: " << std::endl;
      //std::cout << std::endl << argv[1] << std::endl << std::endl;
      //std::cout << "Contains the following DICOM Series: ";
      //std::cout << std::endl << std::endl;
      
      typedef std::vector< std::string > SeriesIdContainer;
      const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();
      SeriesIdContainer::const_iterator seriesItr = seriesUID.begin();
      SeriesIdContainer::const_iterator seriesEnd = seriesUID.end();

      /*
      while( seriesItr != seriesEnd )
      {
      std::cout << seriesItr->c_str() << std::endl;
      ++seriesItr;
      }
      */

      std::string seriesIdentifier;
      seriesIdentifier = seriesUID.begin()->c_str();
      
      //std::cout << std::endl << std::endl;
      //std::cout << "Now reading series: " << std::endl << std::endl;
      //std::cout << seriesIdentifier << std::endl;
      //std::cout << std::endl << std::endl;
      
      typedef std::vector< std::string > FileNamesContainer;
      FileNamesContainer fileNames;
      fileNames = nameGenerator->GetFileNames(seriesIdentifier);
      
      reader->SetFileNames(fileNames);

try
{
reader->Update();
}
catch (itk::ExceptionObject &ex)
{
std::cout << ex << std::endl;
return EXIT_FAILURE;
}

// Forse qui ci vuole qualcosa per far capire che ha caricato correttamente
std::cout << std::endl << "DICOM Series Loading: Complete" << std::endl<<std::endl;

//std::cout << "-- DICOM Series Image Data --" << std::endl;
//std::cout << "Image Size: " << reader->GetOutput()->GetLargestPossibleRegion().GetSize()<< std::endl;
//std::cout << "Image Origin: " << reader->GetOutput()->GetOrigin() << std::endl;
//std::cout << "Image Orientation: " << std::endl << reader->GetOutput()->GetDirection() << std::endl << std::endl;

// Export dell'immagine in formato NIFTI o MHD in base agli input
if (argc > 2)
{
    
	std::string IMGname = argv[2];
	std::string IMGisNii = ".nii";
	if ( strcmp (IMGname.substr(IMGname.rfind("."),IMGname.length()).c_str() , IMGisNii.c_str()) == 0 )
	{
		IMGname.append(".gz");
	}

    typedef itk::ImageFileWriter< ImageType > WriterType;
    WriterType::Pointer writerOriginal = WriterType::New();
    writerOriginal->SetFileName( IMGname );
    writerOriginal->SetInput( reader->GetOutput() );
   
    std::cout << "Writing the Image as: " << std::endl;
    std::cout << argv[2] << std::endl << std::endl;
   
    ImageType::DirectionType FinalDirection;
    FinalDirection.SetIdentity();
    
    if (reader->GetOutput()->GetDirection() != FinalDirection)
      {
	std::cout << "** WARNING: DICOM Series Image Data have an intrinsic Rotation! **" << std::endl << std::endl;
	
	typedef itk::ResampleImageFilter<ImageType,ImageType> FilterType;
	FilterType::Pointer AntiRotationFilter = FilterType::New();

	typedef itk::VersorRigid3DTransform< double > TransformType;
	TransformType::Pointer AntiRotationTransform = TransformType::New();
	//AntiRotationTransform->SetMatrix(reader->GetOutput()->GetDirection().GetInverse().as_matrix());
	AntiRotationTransform->SetMatrix(reader->GetOutput()->GetDirection());
    
	typedef itk::LinearInterpolateImageFunction<ImageType, double > InterpolatorType;
	InterpolatorType::Pointer LinearImageInterpolator = InterpolatorType::New();
	
	AntiRotationFilter->SetTransform(AntiRotationTransform);
	AntiRotationFilter->SetInterpolator( LinearImageInterpolator );
	AntiRotationFilter->SetDefaultPixelValue( 0 );
	AntiRotationFilter->SetOutputSpacing(reader->GetOutput()->GetSpacing());
	// Attenzione: va ruotato anche l'origine!
	
	vnl_vector< itk::SpacePrecisionType > AntiRotOrigVals = reader->GetOutput()->GetOrigin().GetVnlVector()*(reader->GetOutput()->GetDirection().GetVnlMatrix().as_matrix());
	
	const double RotatedOrigin[ imageDimension ] = { AntiRotOrigVals[0], AntiRotOrigVals[1], AntiRotOrigVals[2] };
	
	//AntiRotationFilter->SetOutputOrigin(reader->GetOutput()->GetOrigin());
	AntiRotationFilter->SetOutputOrigin(RotatedOrigin);
	
	AntiRotationFilter->SetOutputDirection(FinalDirection);

	AntiRotationFilter->SetSize(reader->GetOutput()->GetLargestPossibleRegion().GetSize());
	
	AntiRotationFilter->SetInput(reader->GetOutput());
	
	//Esportare l'Immagine Antiruotata
	typedef itk::ImageFileWriter< ImageType > WriterType;
	WriterType::Pointer writerAntiRotImage = WriterType::New();
	
	std::string ARname = argv[2];
	std::string ARsuffix = "_OP";
	std::string ARextension = ".nii.gz";

	std::string ARFileName;
	ARFileName.append(ARname.substr(0,ARname.rfind(".")));
	ARFileName.append(ARsuffix);
	ARFileName.append(ARextension);
	//ARFileName.append(ARname.substr(ARname.rfind("."),ARname.length()));
	
	writerAntiRotImage->SetFileName( ARFileName );
	writerAntiRotImage->SetInput( AntiRotationFilter->GetOutput() );
	
	//Stampo a video i Valori
	std::cout << "-- Restoring Image Orientation: Complete. " << std::endl << std::endl;
	std::cout << "Restored Image Size: " << reader->GetOutput()->GetLargestPossibleRegion().GetSize()<< std::endl;
	std::cout << "Restored Image Origin: " << AntiRotationFilter->GetOutputOrigin() << std::endl;
	std::cout << "Restored Image Orientation: " << std::endl << FinalDirection << std::endl;
	
	std::cout << "Writing the Restored Image as: " << std::endl;
	std::cout << ARFileName << std::endl << std::endl;
	
	try
	{
	  writerAntiRotImage->Update();
	}
	catch (itk::ExceptionObject &ex)
	{
	  std::cout << ex << std::endl;
	  return EXIT_FAILURE;
	}
	
	// Export also a txt file with all the changes in Header File, sorted by SOPinstanceUID of DICOM file
	std::string DICOMoutputDIR = argv[2];
	DICOMoutputDIR = DICOMoutputDIR.substr(0,DICOMoutputDIR.find_last_of("/"));
	DICOMoutputDIR.append("/RestoredDICOMs");
	itksys::SystemTools::MakeDirectory( DICOMoutputDIR.c_str() );
	
	std::string DICOMheaderchange = DICOMoutputDIR;
	DICOMheaderchange.append("/HeaderChangeReport.txt");
	
	std::fstream myHeaderChange;
	myHeaderChange.open(DICOMheaderchange.c_str(), std::ios::out | std::ios::app);
	
	// - BY STEFANO -
	// Modify DicomSeries Header - By modifying every single header of ech DICOM file. - The only way possible!
	unsigned int nSlices = fileNames.size();
	ReaderType::DictionaryRawPointer InputDictionary[nSlices];
	ReaderType::DictionaryArrayType OutputDictionary;
	for (unsigned int f = 0; f < nSlices; f++)
	{
	  // Read the dictionary (metadata) of the f-th slice
	  InputDictionary[f] = (*(reader->GetMetaDataDictionaryArray()))[f];
	  
	  std::string InstanceNumber = "";
	  itk::ExposeMetaData<std::string>(*InputDictionary[f],"0020|0013",InstanceNumber);
	  
	  myHeaderChange << "0020|0013 " << InstanceNumber.c_str() << "\n";
	  
	  // Modify selectively the fields of the f-th slice
	  
	  // Series Description - Append new description to current series description
	  std::string oldDesc = "";
	  
	  //bugged line below:
	  //itksys_ios::ostringstream value;
	  
	  //fix:
	  std::ostringstream value;

	  itk::ExposeMetaData<std::string>(*InputDictionary[f], "0008|103e", oldDesc);
	  value.str("");
	  typedef itk::Version itkVersion;
	  itkVersion::Pointer Version;
	  value << oldDesc << " Processed with: ITK " << Version->GetITKVersion();
	  itk::EncapsulateMetaData<std::string>(*InputDictionary[f],"0008|103e", value.str());
	  
	  myHeaderChange << "0008|103e "<< value.str() << "\n";
	  
	  // Image Position Patient: This is calculated by computing the
	  // physical coordinate of the first pixel in each slice.
	  ImageType::PointType POS2WriteintoDICOMheader;
	  ImageType::IndexType indexvoxel;
	  indexvoxel[0] = 0;
	  indexvoxel[1] = 0;
	  indexvoxel[2] = f;
	  // Calcola l'origine della f-esima slice
	  AntiRotationFilter->GetOutput()->TransformIndexToPhysicalPoint(indexvoxel,POS2WriteintoDICOMheader);
	  value.str("");
	  value << POS2WriteintoDICOMheader[0] << "\\" << POS2WriteintoDICOMheader[1] << "\\" << POS2WriteintoDICOMheader[2];
	  itk::EncapsulateMetaData<std::string>(*InputDictionary[f],"0020|0032", value.str());
	  
	  myHeaderChange << "0020|0032 "<< value.str() << "\n";
	  
	  // Slice Location: For now, we store the z component of the Image
	  // Position Patient.
	  value.str("");
	  value << POS2WriteintoDICOMheader[2];
	  itk::EncapsulateMetaData<std::string>(*InputDictionary[f],"0020|1041", value.str());
	  
	  myHeaderChange << "0020|1041 "<< value.str() << "\n";
	  
	  // Image Orientation
	  value.str("");
	  value << "1" << "\\" <<
		   "0" << "\\" <<
	  	   "0" << "\\" <<
		   "0" << "\\" <<
	  	   "1" << "\\" <<
	  	   "0";
	  itk::EncapsulateMetaData<std::string>(*InputDictionary[f],"0020|0037",value.str());
	  
	  myHeaderChange << "0020|0037 "<< value.str() << "\n";
	  
	  OutputDictionary.push_back(InputDictionary[f]);
	  
	}
	
	myHeaderChange.close();
	
	
	// Esportare la nuova DICOM Series
	std::cout << "++ Restoring DICOM Series Orientation:... ++" << std::endl << std::endl;
	std::cout << "-- Exporting Restored DICOM Series:" << std::endl;
	std::cout << DICOMoutputDIR.c_str() << std::endl << std::endl;
	std::cout << "-- Exporting DICOM Series Header Changes: " << std::endl;
	std::cout << DICOMheaderchange.c_str() << std::endl;

	typedef float DICOMoutputPixelType;
	const unsigned int DICOMoutputImageDim = 2;
	
	typedef itk::Image < DICOMoutputPixelType, DICOMoutputImageDim > DICOM2DImageType;
	typedef itk::ImageSeriesWriter < ImageType, DICOM2DImageType > DICOMSeriesWriterType;
	
	DICOMSeriesWriterType::Pointer WriterDICOMseries = DICOMSeriesWriterType::New();
	WriterDICOMseries->SetInput(AntiRotationFilter->GetOutput());
	
	
	//WriterDICOMseries->SetImageIO(dicomIO);
	WriterDICOMseries->SetImageIO(dicomIOext);
	nameGenerator->SetOutputDirectory(DICOMoutputDIR);
	WriterDICOMseries->SetFileNames( nameGenerator->GetOutputFileNames() );
	// ATTENZIONE: Modificare L'Header delle DICOM!!!
	//WriterDICOMseries->SetMetaDataDictionaryArray(reader->GetMetaDataDictionaryArray());
	WriterDICOMseries->SetMetaDataDictionaryArray(&OutputDictionary);
	try
	{
	  WriterDICOMseries->Update();
	}
	catch( itk::ExceptionObject & excp )
	{
	  std::cerr << "Exception thrown while writing the series " << std::endl;
	  std::cerr << excp << std::endl;
	  return EXIT_FAILURE;
	}
	
	
      }
    
    try
    {
      writerOriginal->Update();
    }
    catch (itk::ExceptionObject &ex)
    {
      std::cout << ex << std::endl;
      return EXIT_FAILURE;
    }
   
}

}
catch (itk::ExceptionObject &ex)
{
std::cout << ex;
return EXIT_FAILURE;
}

std::cout << std::endl << "-- DICOM Conversion: Complete! " << std::endl << std::endl;

return EXIT_SUCCESS;
}

