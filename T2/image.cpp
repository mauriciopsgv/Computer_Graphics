#include "image.h"

#include <QtMath>

Image::Image()
    : _width(0), _height(0), _depth(0)
{
}


Image::Image(const Image& orig)
    : _width(orig._width)
    , _height(orig._height)
    , _depth(orig._depth)
    , _buffer(orig._buffer)
{
}


Image::Image(int width, int height, int depth)
    : _width(width)
    , _height(height)
    , _depth(depth)
    , _buffer(_width * _height * _depth, 0.0f) //Inicializa o buffer com 0's
{
}


Image::Image(int width, int height, int depth, float* buffer)
    : _width(width)
    , _height(height)
    , _depth(depth)
    , _buffer(buffer, buffer + _width * _height * _depth)
{
}


Image::~Image()
{
}


void Image::getDimensions(int& width, int& height, int& depth)
{
    width = _width;
    height = _height;
    depth = _depth;
}


std::vector<float>& Image::getColorBuffer()
{
    return _buffer;
}


Image Image::toGrayscale()
{
    if (_depth==1) return *this;

    Image outImage(_width,_height, 1);

    std::vector<float>& dst = outImage.getColorBuffer();

    for( int y = 0; y < _height; y++ )
    {
        for( int x = 0; x < _width; x++ )
        {
            int k = y*_width*_depth+x*_depth;
            dst[y*_width+x]=0.2126f*_buffer[k] +0.7152f*_buffer[k+1]+0.0722f*_buffer[k+2];
        }
    }
   return outImage;
}


void Image::fromQImage(const QImage &qImage)
{
    if( !qImage.isNull() )
    {
        _buffer.clear();

        _width = qImage.width();
        _height = qImage.height();
        _depth = qImage.isGrayscale() ? 1 : 3;

        _buffer.resize(_width*_height*_depth, 0.0f);

        int pos = 0;
        for( int y = 0; y < _height; y++ )
        {
            for( int x = 0; x < _width; x++ )
            {
                QColor color = qImage.pixel(x,y);

                _buffer[pos++] = color.red()/255.f;

                if( _depth == 3 )
                {
                    _buffer[pos++] = color.green()/255.f;
                    _buffer[pos++] = color.blue()/255.f;
                }
            }
        }
    }
}


unsigned char fix(float value){
    value=(value<0)?-value:value;
    value=(value>1)?1:value;
    return (unsigned char) (255*value);
}


QImage Image::toQImage()
{
    QImage qImage( _width, _height, QImage::Format_RGB32 );

    if( !_buffer.empty() )
    {
        int pos = 0;
        for( int y = 0; y < _height; y++ )
        {
            for( int x = 0; x < _width; x++ )
            {
                float value = fix(_buffer[pos++]);
                QColor color(value, value, value);

                if( _depth == 3 )
                {
                    color.setGreen(fix(_buffer[pos++]));
                    color.setBlue(fix(_buffer[pos++]));
                }

                qImage.setPixelColor(x,y,color);
            }
        }
    }
    return qImage;
}


/** Algoritmos a serem implementados **/


void smoothing(Image& input, Image& output)
{
    int width, height, nColor;
    input.getDimensions(width, height, nColor);
    std::vector<float> inputBuffer = input.getColorBuffer();
    std::vector<float> outputBuffer(input.getColorBuffer());

    for (int row = 1; row < height - 1; row++)
    {
        for (int column = 1; column < width - 1; column++)
        {
            for (int depth = 0; depth < nColor; depth++)
            {
                int a11 = (row-1)*width*nColor + (column-1)*nColor + depth;     int a12 = (row-1)*width*nColor + (column)*nColor + depth;   int a13 = (row-1)*width*nColor + (column+1)*nColor + depth;
                int a21 = (row)*width*nColor   + (column-1)*nColor + depth;     int a22 = (row)*width*nColor   + (column)*nColor + depth;   int a23 = (row)*width*nColor   + (column+1)*nColor + depth;
                int a31 = (row+1)*width*nColor + (column-1)*nColor + depth;     int a32 = (row+1)*width*nColor + (column)*nColor + depth;   int a33 = (row+1)*width*nColor + (column+1)*nColor + depth;
                outputBuffer[row*width*nColor + column*nColor + depth] = 1/16.0 * (1*inputBuffer[a11] + 2*inputBuffer[a12] + 1*inputBuffer[a13] +
                                                                                   2*inputBuffer[a21] + 4*inputBuffer[a22] + 2*inputBuffer[a23] +
                                                                                   1*inputBuffer[a31] + 2*inputBuffer[a32] + 1*inputBuffer[a33]);
            }
        }
    }
    output = *(new Image(width, height, nColor, &outputBuffer[0]));
}


void sobel(Image& input, Image& output)
{
    int width, height, nColor;
    Image grayInput = input.toGrayscale();
    grayInput.getDimensions(width, height, nColor);
    std::vector<float> grayInputBuffer = grayInput.getColorBuffer();
    std::vector<float> outputBuffer(width * height * nColor, 0.0f);

    for (int row = 1; row < height - 1; row++)
    {
        for (int column = 1; column < width - 1; column++)
        {
            int a11 = (row-1)*width + (column-1) ;  int a12 = (row-1)*width + (column);  int a13 = (row-1)*width + (column+1);
            int a21 = (row)*width   + (column-1) ;                                       int a23 = (row)*width   + (column+1);
            int a31 = (row+1)*width + (column-1) ;  int a32 = (row+1)*width + (column);  int a33 = (row+1)*width + (column+1);
            float deltaX = (1*grayInputBuffer[a11] - 1*grayInputBuffer[a13] +
                            2*grayInputBuffer[a21] - 2*grayInputBuffer[a23] +
                            1*grayInputBuffer[a31] - 1*grayInputBuffer[a33]);
            float deltaY = (1*grayInputBuffer[a11] + 2*grayInputBuffer[a12] + 1*grayInputBuffer[a13] -
                            1*grayInputBuffer[a31] - 2*grayInputBuffer[a32] - 1*grayInputBuffer[a33]);
            outputBuffer[row*width + column] = sqrtf(deltaX*deltaX + deltaY*deltaY);
        }
    }
    output = *(new Image(width, height, nColor, &outputBuffer[0]));
}

void sobelInColors(Image& input, Image& output)
{
    int width, height, nColor;
    input.getDimensions(width, height, nColor);
    std::vector<float> inputBuffer = input.getColorBuffer();
    std::vector<float> outputBuffer(width * height * nColor, 0.0f);

    for (int row = 1; row < height - 1; row++)
    {
        for (int column = 1; column < width - 1; column++)
        {
            for (int depth = 0; depth < nColor; depth++)
            {
                int a11 = (row-1)*width*nColor + (column-1)*nColor + depth;     int a12 = (row-1)*width*nColor + (column)*nColor + depth;   int a13 = (row-1)*width*nColor + (column+1)*nColor + depth;
                int a21 = (row)*width*nColor   + (column-1)*nColor + depth;                                                                 int a23 = (row)*width*nColor   + (column+1)*nColor + depth;
                int a31 = (row+1)*width*nColor + (column-1)*nColor + depth;     int a32 = (row+1)*width*nColor + (column)*nColor + depth;   int a33 = (row+1)*width*nColor + (column+1)*nColor + depth;
                float deltaX = (1*inputBuffer[a11] - 1*inputBuffer[a13] +
                                2*inputBuffer[a21] - 2*inputBuffer[a23] +
                                1*inputBuffer[a31] - 1*inputBuffer[a33]);
                float deltaY = (1*inputBuffer[a11] + 2*inputBuffer[a12] + 1*inputBuffer[a13] -
                                1*inputBuffer[a31] - 2*inputBuffer[a32] - 1*inputBuffer[a33]);
                outputBuffer[row*width*nColor + column*nColor + depth] = sqrtf(deltaX*deltaX + deltaY*deltaY);
            }
        }
    }
    output = *(new Image(width, height, nColor, &outputBuffer[0]));
}


void duplicateHorizontal(Image& input, Image& output)
{
    int width, height, nColor;
    input.getDimensions(width, height, nColor);
    std::vector<float> inputBuffer = input.getColorBuffer();
    std::vector<float> outputBuffer(width * height * nColor, 0.0f);

    for (int row = 0; row < height; row++)
    {
        for (int column = 0; column < width; column+=2)
        {
            for (int depth = 0; depth < nColor; depth++)
            {
                int a1 = (row)*width*nColor   + (column)*nColor + depth;
                int a2 = (row)*width*nColor   + (column+1)*nColor + depth;
                outputBuffer[row*width*nColor + (column/2)*nColor + depth] = (inputBuffer[a1] + inputBuffer[a2])/2.0;
                outputBuffer[row*width*nColor + (column/2)*nColor + (width/2)*nColor + depth] = (inputBuffer[a1] + inputBuffer[a2])/2.0;
            }
        }
    }
    output = *(new Image(width, height, nColor, &outputBuffer[0]));
}


void haar(Image& input, Image& output)
{
    int width, height, nColor;
    input.getDimensions(width, height, nColor);
    std::vector<float> inputBuffer = input.getColorBuffer();
    std::vector<float> haarHorizontalBuffer(width * height * nColor, 0.0f);
    std::vector<float> outputBuffer(width * height * nColor, 0.0f);

    // Aplicando o filtro na horizontal
    for (int row = 0; row < height; row++)
    {
        for (int column = 0; column < width; column+=2)
        {
            for (int depth = 0; depth < nColor; depth++)
            {
                int a1 = (row)*width*nColor   + (column)*nColor + depth;
                int a2 = (row)*width*nColor   + (column+1)*nColor + depth;
                haarHorizontalBuffer[row*width*nColor + (column/2)*nColor + depth] = (inputBuffer[a1] + inputBuffer[a2])/2.0;
                haarHorizontalBuffer[row*width*nColor + (column/2)*nColor + (width/2)*nColor + depth] = (inputBuffer[a1] - inputBuffer[a2])/2.0;
            }
        }
    }

    // Aplicando o filtro na vertical
    for (int column = 0; column < width; column++)
    {
        for (int row = 0; row < height; row+=2)
        {
            for (int depth = 0; depth < nColor; depth++)
            {
                int a1 = (row)*width*nColor   + (column)*nColor + depth;
                int a2 = (row+1)*width*nColor   + (column)*nColor + depth;
                outputBuffer[(row/2)*width*nColor + (column)*nColor + depth] = (haarHorizontalBuffer[a1] + haarHorizontalBuffer[a2])/2.0;
                outputBuffer[(row/2)*width*nColor + (column)*nColor + (height/2)*width*nColor + depth] = (haarHorizontalBuffer[a1] - haarHorizontalBuffer[a2])/2.0;
            }
        }
    }
    output = Image(width, height, nColor, &outputBuffer[0]);
}


void haarInv(Image& input, Image& output)
{
    int width, height, nColor;
    input.getDimensions(width, height, nColor);
    std::vector<float> inputBuffer = input.getColorBuffer();
    std::vector<float> inverseHaarVerticalBuffer(width * height * nColor, 0.0f);
    std::vector<float> outputBuffer(width * height * nColor, 0.0f);

    // Revertendo Haar vertical
    for (int column = 0; column < width; column++)
    {
        for (int row = 0; row < height; row+=2)
        {
            for (int depth = 0; depth < nColor; depth++)
            {
                int indexMedia   = (row/2)*width*nColor + (column)*nColor + depth;
                int indexDetalhe = (row/2)*width*nColor + (column)*nColor + (height/2)*width*nColor + depth;
                inverseHaarVerticalBuffer[(row)*width*nColor   + (column)*nColor + depth] = inputBuffer[indexMedia] + inputBuffer[indexDetalhe];
                inverseHaarVerticalBuffer[(row+1)*width*nColor + (column)*nColor + depth] = inputBuffer[indexMedia] - inputBuffer[indexDetalhe];
            }
        }
    }

    // Revertendo Haar horizontal
    for (int row = 0; row < height; row++)
    {
        for (int column = 0; column < width; column+=2)
        {
            for (int depth = 0; depth < nColor; depth++)
            {
                int indexMedia   = row*width*nColor + (column/2)*nColor + depth;
                int indexDetalhe = row*width*nColor + (column/2)*nColor + (width/2)*nColor + depth;
                outputBuffer[(row)*width*nColor + (column)*nColor   + depth] = inverseHaarVerticalBuffer[indexMedia] + inverseHaarVerticalBuffer[indexDetalhe];
                outputBuffer[(row)*width*nColor + (column+1)*nColor + depth] = inverseHaarVerticalBuffer[indexMedia] - inverseHaarVerticalBuffer[indexDetalhe];
            }
        }
    }

    output = Image(width, height, nColor, &outputBuffer[0]);

}

float gamaCorrection (std::vector<float>& imageBuffer, int gama, int row, int column, int width, int nColor)
{
    float luminance = pow(pow(imageBuffer[row*width*nColor + column*nColor],2) +
                          pow(imageBuffer[row*width*nColor + column*nColor + 1],2) +
                          pow(imageBuffer[row*width*nColor + column*nColor + 2],2), 0.5);
    return pow(luminance, 1.0/gama);
}

void enhanceHaar(Image& input, Image& output)
{
    int gama = 2;
    int width, height, nColor;
    input.getDimensions(width, height, nColor);
    std::vector<float> inputBuffer = input.getColorBuffer();
    std::vector<float> outputBuffer = input.getColorBuffer();

    /* Quadrants
     *   1  2
     *   4  3
     */


    // 2nd Quadrant
    for (int row = 0; row < height/2; row++)
    {
        for (int column = width/2; column < width; column++)
        {
            float gamaCorrected = gamaCorrection(inputBuffer, gama, row, column, width, nColor);
            for (int depth = 0; depth < nColor; depth++)
            {
                outputBuffer[row*width*nColor + column*nColor + depth] = gamaCorrected;
            }
        }
    }

    // 3rd Quadrant
    for (int row = height/2; row < height; row++)
    {
        for (int column = width/2; column < width; column++)
        {
            float luminance = pow(pow(inputBuffer[row*width*nColor + column*nColor],2) +
                                  pow(inputBuffer[row*width*nColor + column*nColor + 1],2) +
                                  pow(inputBuffer[row*width*nColor + column*nColor + 2],2), 0.5);
            float gamaCorrection = pow(luminance, 1.0/gama);

            outputBuffer[row*width*nColor + column*nColor]     = gamaCorrection;
            outputBuffer[row*width*nColor + column*nColor + 1] = gamaCorrection;
            outputBuffer[row*width*nColor + column*nColor + 2] = gamaCorrection;
        }
    }

    // 4th Quadrant
    for (int row = height/2; row < height; row++)
    {
        for (int column = 0; column < width/2; column++)
        {
            float luminance = pow(pow(inputBuffer[row*width*nColor + column*nColor],2) +
                                  pow(inputBuffer[row*width*nColor + column*nColor + 1],2) +
                                  pow(inputBuffer[row*width*nColor + column*nColor + 2],2), 0.5);
            float gamaCorrection = pow(luminance, 1.0/gama);

            outputBuffer[row*width*nColor + column*nColor]     = gamaCorrection;
            outputBuffer[row*width*nColor + column*nColor + 1] = gamaCorrection;
            outputBuffer[row*width*nColor + column*nColor + 2] = gamaCorrection;
        }
    }

    output = Image(width, height, nColor, &outputBuffer[0]);
}
