#include <iostream>
#include <string>
#include <math.h>

#ifdef _MSC_VER
#pragma warning ( disable : 4786 )
#endif
#include "itkImage.h"
#include "itkVector.h"
#include "vnl/vnl_matrix_fixed.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkSimpleImageRegionIterator.h"
#include "itkGaussianSupervisedClassifier.h"
#include "itkRGBGibbsPriorFilter.h"

#define   IMGWIDTH            20
#define   IMGHEIGHT           20
#define   NFRAMES             1
#define   NUMBANDS            1  
#define   NDIMENSION          3
#define   NUM_CLASSES         3
#define   MAX_NUM_ITER        1

//using namespace itk;

//time_t btime,etime;
const unsigned short TestingImage [400]={
297,277,317,289,300,312,306,283,282,308,308,342,335,325,315,300,304,318,307,308,

319,276,311,282,309,273,308,277,296,313,308,333,322,317,302,330,339,340,325,315,

272,316,296,299,298,310,271,319,315,280,338,342,349,349,330,319,313,314,342,301,

274,274,312,282,277,303,313,300,275,292,341,336,324,310,337,323,322,347,337,305,

296,272,304,304,281,304,302,284,315,270,325,349,337,317,308,332,324,303,334,325,

291,272,289,317,289,310,305,316,292,307,307,343,341,329,309,308,340,323,307,325,

274,286,282,291,270,296,274,288,274,275,341,301,325,333,321,305,347,346,327,317,

282,315,270,314,290,304,297,304,309,290,309,338,341,319,325,344,301,349,328,302,

314,289,296,270,274,277,317,280,278,285,315,347,314,316,307,336,341,335,330,337,

281,291,317,317,302,304,272,277,318,319,305,322,337,334,327,303,321,310,334,314,

321,311,328,326,331,308,325,348,334,346,309,316,308,349,322,349,304,331,304,321,

346,302,344,314,311,338,320,310,331,330,322,323,329,331,342,341,331,336,328,318,

309,336,327,345,312,309,330,334,329,317,324,304,337,330,331,334,340,307,328,343,

345,330,336,302,333,348,315,328,315,308,305,343,342,337,307,316,303,303,332,341,

327,322,320,314,323,325,307,316,336,315,341,347,343,336,315,347,306,303,339,326,

330,347,303,343,332,316,305,325,311,314,345,327,333,305,324,318,324,339,325,319,

334,326,330,319,300,335,305,331,343,324,337,324,319,339,327,317,347,331,308,318,

306,337,347,330,301,316,302,331,306,342,343,329,336,342,300,306,335,330,310,303,

308,331,317,315,318,333,340,340,326,330,339,345,307,331,320,312,306,342,303,321,

328,315,327,311,315,305,340,306,314,339,344,339,337,330,318,342,311,343,311,312
};

int main(){
std::cout<< "Gibbs Prior Test Begins: " << std::endl;
  time(&btime);

// for local testing on image files (256*256*1 RGB)
//  unsigned char TestImage[65536*3];
  unsigned short outImage[400];
//  FILE *input;

//  FILE *output=fopen("../../../../insight/local_copy/Jaw_gibbs_uint8.raw", "wb");
/*
  input = fopen("../../../../insight/local_copy/Jaw_top_uint8.raw", "rb");
  fread(TestImage, 3, 65536, input);
  fclose(input);
*/  
  typedef itk::Image<itk::Vector<unsigned short,NUMBANDS>,NDIMENSION> VecImageType; 

  VecImageType::Pointer vecImage = VecImageType::New();

  VecImageType::SizeType vecImgSize = { IMGWIDTH , IMGHEIGHT, NFRAMES };

  VecImageType::IndexType index = VecImageType::IndexType::ZeroIndex;
  VecImageType::RegionType region;

  region.SetSize( vecImgSize );
  region.SetIndex( index );

  vecImage->SetLargestPossibleRegion( region );
  vecImage->SetBufferedRegion( region );
  vecImage->Allocate();

  // setup the iterators
  typedef VecImageType::PixelType::VectorType VecPixelType;

  enum { VecImageDimension = VecImageType::ImageDimension };
  typedef itk::SimpleImageRegionIterator< VecImageType > VecIterator;

  VecIterator outIt( vecImage, vecImage->GetBufferedRegion() );
  outIt.GoToBegin();

  //Set up the vector to store the image  data
  typedef VecImageType::PixelType     DataVector;
  DataVector   dblVec; 

  //--------------------------------------------------------------------------
  //Manually create and store each vector
  //--------------------------------------------------------------------------
  //Slice 1
  //Vector 1
  int i = 0;
  while ( !outIt.IsAtEnd() ) { 
    dblVec[0] = (unsigned short) TestingImage[i];
//  dblVec[1] = (unsigned short) TestImage[i+65536];
//  dblVec[2] = (unsigned short) TestImage[i+65536*2];
  outIt.Set(dblVec); 
  ++outIt;
  i++;
  }

  //---------------------------------------------------------------
  //Generate the training data
  //---------------------------------------------------------------
  typedef itk::Image<unsigned short, NDIMENSION > ClassImageType; 
  ClassImageType::Pointer classImage  = ClassImageType::New();

  ClassImageType::SizeType classImgSize = { IMGWIDTH , IMGHEIGHT, NFRAMES };

  ClassImageType::IndexType classindex = ClassImageType::IndexType::ZeroIndex;
  ClassImageType::RegionType classregion;

  classregion.SetSize( classImgSize );
  classregion.SetIndex( classindex );

  classImage->SetLargestPossibleRegion( classregion );
  classImage->SetBufferedRegion( classregion );
  classImage->Allocate();

  // setup the iterators
  typedef ClassImageType::PixelType ClassImagePixelType;

  unsigned int ClassImageDimension = NDIMENSION;

  typedef  itk::SimpleImageRegionIterator<ClassImageType>  ClassImageIterator;

  ClassImageIterator classoutIt( classImage, classImage->GetBufferedRegion() );
  classoutIt.GoToBegin();

  //--------------------------------------------------------------------------
  //Manually create and store each vector
  //--------------------------------------------------------------------------
  //Slice 1
  //Pixel no. 1

  i = 0;
  while ( !classoutIt.IsAtEnd() ) {  
  if ( (i%IMGWIDTH<8) && (i%IMGWIDTH>4) && 
    (i/IMGWIDTH<8) && (i/IMGWIDTH>4)) {
    classoutIt.Set( 1 );
  } else {
    if ( (i%IMGWIDTH<18) && (i%IMGWIDTH>14) && 
      (i/IMGWIDTH<18) && (i/IMGWIDTH>14)) {
      classoutIt.Set( 2 );
    }/* else {
    if ( (i%IMGWIDTH<20) && (i%IMGWIDTH>10) && 
      (i/IMGWIDTH<139) && (i/IMGWIDTH>129)) {
      classoutIt.Set( 3 );
    }*/
    else {
      classoutIt.Set( 0 );
    }
    //}
  }
  ++classoutIt;
  i++;
  }

  //----------------------------------------------------------------------
  // Test code for the supervised classifier algorithm
  //----------------------------------------------------------------------

  //---------------------------------------------------------------------
  // Multiband data is now available in the right format
  //---------------------------------------------------------------------
  typedef 
  itk::Classifier<VecImageType,ClassImageType>::Pointer 
    ClassifierType;

  //Instantiate the classifier to be used
  typedef itk::GaussianSupervisedClassifier<VecImageType,ClassImageType> 
    GaussianSupervisedClassifierType;

  GaussianSupervisedClassifierType::Pointer 
    myGaussianClassifier = GaussianSupervisedClassifierType::New();

  //Set the Gibbs Prior labeller
  typedef itk::RGBGibbsPriorFilter<VecImageType,ClassImageType> GibbsPriorFilterType;
  GibbsPriorFilterType::Pointer applyGibbsImageFilter = GibbsPriorFilterType::New();

  // Set the MRF labeller parameters
  applyGibbsImageFilter->SetNumberOfClasses(NUM_CLASSES);
  applyGibbsImageFilter->SetMaximumNumberOfIterations(MAX_NUM_ITER);
//  applyGibbsImageFilter->SetErrorTollerance(0.00);
  applyGibbsImageFilter->SetClusterSize(10);
  applyGibbsImageFilter->SetBoundaryGradient(6);
  applyGibbsImageFilter->SetObjectLabel(1);
 
  applyGibbsImageFilter->SetInput(vecImage);
  applyGibbsImageFilter
    ->SetClassifier((ClassifierType) myGaussianClassifier ); 

  //Since a suvervised classifier is used, it requires a training image
  applyGibbsImageFilter->SetTrainingImage(classImage);  

  time(&etime);

  std::cout<<"Ininialization done in: "<<etime-btime<<" seconds."<<std::endl;  
  //Kick off the Gibbs labeller function
  applyGibbsImageFilter->Update();

  ClassImageType::Pointer  outClassImage = applyGibbsImageFilter->GetOutput();

  //Print the mrf labelled image
  ClassImageIterator labeloutIt( outClassImage, outClassImage->GetBufferedRegion() );

  i = 0;
  int j0 = 0;
  int j1 = 0;
  while ( !labeloutIt.IsAtEnd() ) {
//  outImage[i] = labeloutIt.Get();
  if (labeloutIt.Get() == 0) {
    j0++;
    outImage[i] = 0;
  }
  if (labeloutIt.Get() == 1) {
    j1++;
    outImage[i] = 65535;
  }
  i++;
  ++labeloutIt;
  }

  std::cout<< "j0:" << j0 << std::endl;
  std::cout<< "j1:" << j1 << std::endl;

//  FILE *output=fopen("new.raw", "wb");
//  fwrite(outImage, 2, IMGWIDTH*IMGHEIGHT, output);
//  fclose(output);
  //Verify if the results were as per expectation
  
  bool passTest = true;
/*  int j = 0;
  i = 0;
  labeloutIt.GoToBegin();
  while ( !labeloutIt.IsAtEnd() ) {
  if ((i%IMGWIDTH<10) && (i/IMGWIDTH<10) && (labeloutIt.Get()==1))
    j++;
  i++;
  ++labeloutIt;
  }
*/
  passTest = ((j1>85) && (j1 < 115));
  if( passTest ) 
    std::cout<< "Gibbs Prior Test Passed" << std::endl;
  else 
    std::cout<< "Gibbs Prior Test failed" << std::endl;


  return 0;
}
