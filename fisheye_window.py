import numpy as np
import cv2
import sys

usage_text = '''
usage:
  python fisheye360.py your-fisheye-image-file
'''
howto_text = '''
Hit followings to move your view:
  'r' or 'f' to zoom
  'g' or 't' to rotate alpha
  'h' or 'y' to rotate beta
  'j' or 'u' to rotate theta
  's' to save current view to ./result.png

Hit ESC to exit.
'''

class FishEyeWindow(object):
    """Fisheye Window Class
    Get 
    
    Args:
        nx, ny (int): Number of inner corners of the chessboard pattern, in x and y axes.
    """
    def __init__(
            self,
            srcWidth,
            srcHeight,
            destWidth,
            destHeight
        ):
        # Initial parameters
        self._srcW = srcWidth
        self._srcH = srcHeight
        self._destW = destWidth
        self._destH = destHeight
        self._al = 0
        self._be = 0
        self._th = 0
        self._R  = srcWidth / 2
        self._zoom = 1.0
        # Map storage
        self._mapX = np.zeros((self._destH, self._destW), np.float32)
        self._mapY = np.zeros((self._destH, self._destW), np.float32)
    def buildMap(self, alpha=None, beta=None, theta=None, R=None, zoom=None):
        # Set the angle parameters
        self._al = (alpha, self._al)[alpha == None]
        self._be = (beta, self._be)[beta == None]
        self._th = (theta, self._th)[theta == None]
        self._R = (R, self._R)[R == None]
        self._zoom = (zoom, self._zoom)[zoom == None]
        # Build the fisheye mapping
        al = self._al / 180.0
        be = self._be / 180.0
        th = self._th / 180.0
        A = np.cos(th) * np.cos(al) - np.sin(th) * np.sin(al) * np.cos(be)
        B = np.sin(th) * np.cos(al) + np.cos(th) * np.sin(al) * np.cos(be)
        C = np.cos(th) * np.sin(al) + np.sin(th) * np.cos(al) * np.cos(be)
        D = np.sin(th) * np.sin(al) - np.cos(th) * np.cos(al) * np.cos(be)
        mR = self._zoom * self._R
        mR2 = mR * mR
        mRsinBesinAl = mR * np.sin(be) * np.sin(al)
        mRsinBecosAl = mR * np.sin(be) * np.cos(al)
        centerV = int(self._destH / 2.0)
        centerU = int(self._destW / 2.0)
        centerY = int(self._srcH / 2.0)
        centerX = int(self._srcW / 2.0)
        # Fill in the map
        for absV in range(0, int(self._destH)):
            v = absV - centerV
            vv = v * v
            for absU in range(0, int(self._destW)):
                u = absU - centerU
                uu = u * u
                upperX = self._R * (u * A - v * B + mRsinBesinAl)
                lowerX = np.sqrt(uu + vv + mR2)
                upperY = self._R * (u * C - v * D - mRsinBecosAl)
                lowerY = lowerX
                x = upperX / lowerX + centerX
                y = upperY / lowerY + centerY
                _v = (v + centerV, v)[centerV <= v]
                _u = (u + centerU, u)[centerU <= u]
                self._mapX.itemset((_v, _u), x)
                self._mapY.itemset((_v, _u), y)

    def getImage(self, img):
        # Look through the window
        output = cv2.remap(img, self._mapX, self._mapY, cv2.INTER_LINEAR)
        return output

def main(input_file):
    src_img = cv2.imread(input_file)
    src_size = src_img.shape[:2]
    cv2.imshow('Original', src_img)
    print('source image size = ' + str(src_size))

    fe = FishEyeWindow(src_size[1], src_size[0], 320, 320)
    alpha = -270
    beta = 0
    theta = 270
    zoom = 1.0
    while True:
        fe.buildMap(alpha=alpha, beta=beta, theta=theta, zoom=zoom)
        result_img = fe.getImage(src_img)
        cv2.imshow('Result', result_img)
        key = cv2.waitKey(0)

        if key == 27:
            break
        elif ord('r') == key:
            zoom -= 0.1
        elif ord('f') == key:
            zoom += 0.1
        elif ord('g') == key:
            alpha += 90
        elif ord('t') == key:
            alpha -= 90
        elif ord('h') == key:
            beta += 45
        elif ord('y') == key:
            beta -= 45
        elif ord('j') == key:
            theta += 90
        elif ord('u') == key:
            theta -= 90
        elif ord('s') == key:
            cv2.imwrite('./result.png', result_img)
        print('alpha=%f, beta=%f, theta=%f, zoom=%f' % (alpha, beta, theta, zoom))
        
    cv2.destroyAllWindows()
    

if __name__ == '__main__':
    if len(sys.argv) <= 1:
        print usage_text
    else:
        print howto_text
        main(sys.argv[1])
