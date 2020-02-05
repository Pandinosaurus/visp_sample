#include <visp3/core/vpServer.h>
#include <visp3/gui/vpDisplayX.h>
#include <visp3/gui/vpDisplayGDI.h>

//#include "vpRequestImage.h" //See vpRequest class documentation

/* begin vpRequestImage.h */

#include <visp3/core/vpImage.h>
#include <visp3/core/vpImageException.h>
#include <visp3/core/vpImageConvert.h>
#include <visp3/core/vpRequest.h>
#include <string.h>

class vpRequestImage : public vpRequest
{
private:
  vpImage<unsigned char> *I;

public:
  vpRequestImage();
  vpRequestImage(vpImage<unsigned char> *);
  ~vpRequestImage();

  virtual void encode();
  virtual void decode();
};

/* end vpRequestImage.h */

/* begin vpRequestImage.cpp */
//#include "vpRequestImage.h"

vpRequestImage::vpRequestImage(){
  request_id = "image";
}

vpRequestImage::vpRequestImage(vpImage<unsigned char> *Im){
  request_id = "image";
  I = Im;
}

vpRequestImage::~vpRequestImage(){}

void vpRequestImage::encode(){
  clear();

  unsigned int h = I->getHeight();
  unsigned int w = I->getWidth();

  addParameterObject(&h);
  addParameterObject(&w);
  addParameterObject(I->bitmap,h*w*sizeof(unsigned char));
}

void vpRequestImage::decode(){
  if(listOfParams.size() == 3){
    unsigned int w, h;
    memcpy((void*)&h, (void*)listOfParams[0].c_str(), sizeof(unsigned int));
    memcpy((void*)&w, (void*)listOfParams[1].c_str(), sizeof(unsigned int));

    I->resize(h,w);
    memcpy((void*)I->bitmap,(void*)listOfParams[2].c_str(),w*h*sizeof(unsigned char));
  }
}

/* end vpRequestImage.cpp */

int main(int argc,const char** argv)
{
  int port = 35000;

  std::cout << "Port: " << port << std::endl;
  vpServer serv(port);
  serv.start();

#ifdef VISP_HAVE_X11
  vpDisplayX display;
#elif defined VISP_HAVE_GDI
  vpDisplayGDI display;
#endif

  vpImage<unsigned char> I;

  vpRequestImage reqImage(&I);
  serv.addDecodingRequest(&reqImage);

  bool run = true;

  while(run){
    serv.checkForConnections();

    if(serv.getNumberOfClients() > 0)
    {
      int index = serv.receiveAndDecodeRequestOnce();
      std::string id = serv.getRequestIdFromIndex(index);

      if(id == "image")
      {
#if defined(VISP_HAVE_X11) || defined(VISP_HAVE_GDI)
        if (! display.isInitialised() )
          display.init(I, -1, -1, "Remote display");
#endif

        vpDisplay::display(I) ;
        vpDisplay::flush(I);

        // A click in the viewer to exit
        if ( vpDisplay::getClick(I, false) )
          run = false;
      }
    }
  }

  return 0;
}
