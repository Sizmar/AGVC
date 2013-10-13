// Include main Kinect SDK .h file
#include "NuiAPI.h"
 
// Include the face tracking SDK .h file
#include "FaceTrackLib.h"
 
// Create an instance of a face tracker
IFTFaceTracker* pFT = FTCreateFaceTracker();
if(!pFT)
{
    // Handle errors
}
 
// Initialize cameras configuration structures.
// IMPORTANT NOTE: resolutions and focal lengths must be accurate, since it affects tracking precision!
// It is better to use enums defined in NuiAPI.h
 
// Video camera config with width, height, focal length in pixels
// NUI_CAMERA_COLOR_NOMINAL_FOCAL_LENGTH_IN_PIXELS focal length is computed for 640x480 resolution
// If you use different resolutions, multiply this focal length by the scaling factor
FT_CAMERA_CONFIG videoCameraConfig = {640, 480, NUI_CAMERA_COLOR_NOMINAL_FOCAL_LENGTH_IN_PIXELS};
 
// Depth camera config with width, height, focal length in pixels
// NUI_CAMERA_COLOR_NOMINAL_FOCAL_LENGTH_IN_PIXELS focal length is computed for 320x240 resolution
// If you use different resolutions, multiply this focal length by the scaling factor
FT_CAMERA_CONFIG depthCameraConfig = {320, 240, NUI_CAMERA_DEPTH_NOMINAL_FOCAL_LENGTH_IN_PIXELS};
 
// Initialize the face tracker
HRESULT hr = pFT->Initialize(&videoCameraConfig, &depthCameraConfig, NULL, NULL);
if( FAILED(hr) )
{
    // Handle errors
}
 
// Create a face tracking result interface
IFTResult* pFTResult = NULL;
hr = pFT->CreateFTResult(&pFTResult);
if(FAILED(hr))
{
    // Handle errors
}
 
// Prepare image interfaces that hold RGB and depth data
IFTImage* pColorFrame = FTCreateImage();
IFTImage* pDepthFrame = FTCreateImage();
if(!pColorFrame || !pDepthFrame)
{
    // Handle errors
}
 
// Attach created interfaces to the RGB and depth buffers that are filled with
// corresponding RGB and depth frame data from Kinect cameras
pColorFrame->Attach(640, 480, colorCameraFrameBuffer, FORMAT_UINT8_R8G8B8, 640*3);
pDepthFrame->Attach(320, 240, depthCameraFrameBuffer, FTIMAGEFORMAT_UINT16_D13P3, 320*2);
// You can also use Allocate() method in which case IFTImage interfaces own their memory.
// In this case use CopyTo() method to copy buffers
 
FT_SENSOR_DATA sensorData;
sensorData.pVideoFrame = &colorFrame;
sensorData.pDepthFrame = &depthFrame;
sensorData.ZoomFactor = 1.0f;       // Not used must be 1.0
sensorData.ViewOffset = POINT(0,0); // Not used must be (0,0)
 
bool isFaceTracked = false;
 
// Track a face
while ( true )
{
    // Call Kinect API to fill videoCameraFrameBuffer and depthFrameBuffer with RGB and depth data
    ProcessKinectIO();
 
    // Check if we are already tracking a face
    if(!isFaceTracked)
    {
        // Initiate face tracking.
        // This call is more expensive and searches the input frame for a face.
        hr = pFT->StartTracking(&sensorData, NULL, NULL, pFTResult);
        if(SUCCEEDED(hr) && SUCCEEDED(pFTResult->Status))
        {
            isFaceTracked = true;
        }
        else
        {
            // No faces found
            isFaceTracked = false;
        }
    }
    else
    {
        // Continue tracking. It uses a previously known face position.
        // This call is less expensive than StartTracking()
        hr = pFT->ContinueTracking(&sensorData, NULL, pFTResult);
        if(FAILED(hr) || FAILED (pFTResult->Status))
        {
            // Lost the face
            isFaceTracked = false;
        }
    }
 
    // Do something with pFTResult like visualize the mask, drive your 3D avatar,
    // recognize facial expressions
}
 
// Clean up
pFTResult->Release();
pColorFrame->Release();
pDepthFrame->Release();
pFT->Release();