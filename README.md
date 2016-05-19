# Fisheye Window

## Thanks to the study
This is snippet program implements dewarped image extraction algorithm exactly as presented in this research paper:

["High Quality Image Correction Algorithm With Cubic Interpolation for Fish-eye Lens"](http://ci.nii.ac.jp/naid/110007684631)
by Mori Takahiro et al.

CAUTION: Main part of this paper - high quality image correction - is not handled here.

## This snippet is...
There are many information found on Web regarding getting panorama image from fisheye image, but these are limited to show off each technique.
My snippet shows exact code for getting image from your fisheye source image freely regarding:
- Angle
- Zooming

You can get images as if you are shitting in the center of fisheye image, and looking around.
I call this as "window" of the fisheye image, so the class name is FishEyeWindow.
Let's see what you can get below.

## Sample

### Source FishEye image
![Source](https://raw.githubusercontent.com/daisukelab/fisheye_window/master/sample.png)

### Sample Results
|         |![Center](https://raw.githubusercontent.com/daisukelab/fisheye_window/master/sample_result/result_up.png)|        |
|:----:|:----:|:----:|
|![Center](https://raw.githubusercontent.com/daisukelab/fisheye_window/master/sample_result/result_left.png)|![Center](https://raw.githubusercontent.com/daisukelab/fisheye_window/master/sample_result/result_center.png)|![Center](https://raw.githubusercontent.com/daisukelab/fisheye_window/master/sample_result/result_right.png)|
|         |![Center](https://raw.githubusercontent.com/daisukelab/fisheye_window/master/sample_result/result_down.png)|        |

#### Parameters
|Angle|Parameter|
|:--:|:----:|
|left|alpha=-270.000000, beta=135.000000, theta=270.000000, zoom=0.300000|
|center|alpha=-270.000000, beta=0.000000, theta=270.000000, zoom=0.300000|
|right|alpha=-270.000000, beta=-135.000000, theta=270.000000, zoom=0.300000|
|up|alpha=0.000000, beta=90.000000, theta=0.000000, zoom=0.300000|
|down|alpha=0.000000, beta=-90.000000, theta=0.000000, zoom=0.300000|

## How to use
Once you start python snippet like this:

    $ python fisheye360.py sample.png

Program will stay running waiting for your control.

	Hit followings to move your view:
	  'r' or 'f' to zoom
	  'g' or 't' to rotate alpha
	  'h' or 'y' to rotate beta
	  'j' or 'u' to rotate theta
	  's' to save current view to ./result.png
	
	Hit ESC to exit.

### Requirements
Opencv 2.x would be ok, or try 3.x if you see anything wrong.

## Design detail
The FishEyeWindow class is designed based on:
- The center of source image is *the center*.
- Top/bottom or left/right part of image can be cut, as long as the center is the center.
- Small workaround done for hiding an issue - orthant exchange; explained below.

### Regarding orthant issue
If I follow simply as the original math, the result image has problem:
- What should be on the orthant I is on the II, and the II on the I.
- Tha same thing for III and IV.

Then I implemented simple workraound to exchange orthant as you can find in the source.

## No copyright, no license
This is just have interpreted math equations into python code, feel free to use.
