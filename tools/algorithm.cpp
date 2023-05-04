﻿#include "algorithm.h"
#include "medianfilter.h"
/*
 * 参考文献:
 * https://blog.csdn.net/Aidam_Bo/article/details/84391558
 * https://www.cnblogs.com/kekec/p/3643578.html
 * https://blog.csdn.net/bagboy_taobao_com/article/details/5574159
 * https://blog.csdn.net/chen134225/article/details/80874367
 */

/*****************************************************************************
 *                            灰度处理
 * 计算公式:
 * Gray = 0.299*R+0.587*G+0.114*B
 * 也可以直接使用qGray(origin.pixel(i,j))->使用公式:(r*11+g*16+b*5)/32
 * **************************************************************************/
QImage Algorithm::GreyScale(QImage origin)
{
    //以原图的大小生成一张图片
    QImage *newImage = new QImage(origin.width(), origin.height(),QImage::Format_ARGB32);

    QColor oldColor;

    for (int i=0; i<newImage->width(); ++i) {
        for (int j=0; j<newImage->height(); ++j) {
            //读取原图片的Rgb
            oldColor = QColor(origin.pixel(i,j));
            //计算公式
            int x = (oldColor.red()*299+oldColor.green()*587+oldColor.blue()*114)/1000;
            //设置新图片的Rgb
            newImage->setPixel(i,j,qRgb(x,x,x));
        }
    }
    return *newImage;
}

/*****************************************************************************
 *                              暖色调
 * 公式:
 * 增加黄色=红色R+delta && 绿色G+delta
 * 并保证在[0,255]范围
 * **************************************************************************/
QImage Algorithm::Warm(int delta, QImage origin)
{
    QImage *newImage = new QImage(origin.width(), origin.height(),QImage::Format_ARGB32);

    QColor oldColor;
    int r, g, b;

    for (int i=0; i<newImage->width(); ++i)
    {
        for (int j=0; j<newImage->height(); ++j)
        {
            oldColor = QColor(origin.pixel(i,j));

            //增加黄色
            r = oldColor.red() + delta;
            g = oldColor.green() + delta;
            b = oldColor.blue();

            //保证在0到255之间
            r = qBound(0, r, 255);
            g = qBound(0, g, 255);

            newImage->setPixel(i,j, qRgb(r,g,b));
        }
    }
    return *newImage;
}

/*****************************************************************************
 *                           冷色调
 * 同理暖色调，但是更改成相反
 * **************************************************************************/
QImage Algorithm::Cool(int delta, QImage origin)
{
    QImage *newImage = new QImage(origin.width(), origin.height(),
                                  QImage::Format_ARGB32);

    QColor oldColor;
    int r, g, b;

    for (int  i=0; i<newImage->width(); ++i)
    {
        for (int j=0; j<newImage->height(); ++j)
        {
            oldColor = QColor(origin.pixel(i,j));

            r = oldColor.red();
            g = oldColor.green();
            b = oldColor.blue() + delta;

            b = qBound(0, r, 255);

            newImage->setPixel(i,j, qRgb(r,g,b));
        }
    }
    return *newImage;
}



/*****************************************************************************
 *                                亮度
 * 冷热一起调
 * **************************************************************************/
QImage Algorithm::Brightness(int delta, QImage origin)
{
    QImage *newImage = new QImage(origin.width(), origin.height(),QImage::Format_ARGB32);

    QColor oldColor;
    int r, g, b;

    for (int i=0; i<newImage->width(); ++i)
    {
        for (int j=0; j<newImage->height(); ++j)
        {
            oldColor = QColor(origin.pixel(i,j));

            r = oldColor.red() + delta;
            g = oldColor.green() + delta;
            b = oldColor.blue() + delta;

            r = qBound(0, r, 255);
            g = qBound(0, g, 255);
            b = qBound(0, b, 255);

            newImage->setPixel(i,j, qRgb(r,g,b));
        }
    }
    return *newImage;
}

/*****************************************************************************
 *                                   水平翻转
 * 算法:
 * 左右任意一点替换为width-i  竖直方向不变
 * **************************************************************************/
QImage Algorithm::Horizontal(const QImage &origin)
{
    QImage *newImage = new QImage(QSize(origin.width(), origin.height()),QImage::Format_ARGB32);

    QColor tmpColor;
    int r, g, b;
    for (int i=0; i<newImage->width(); ++i)
    {
        for (int j=0; j<newImage->height(); ++j)
        {
            tmpColor = QColor(origin.pixel(i, j));

            r = tmpColor.red();
            g = tmpColor.green();
            b = tmpColor.blue();

            //左右置换
            newImage->setPixel(newImage->width()-i-1,j, qRgb(r,g,b));
        }
    }
    return *newImage;
}

/*****************************************************************************
 *                                   垂直翻转
 * 同理水平翻转
 * **************************************************************************/
QImage Algorithm::Vertical(const QImage &origin)
{
    QImage *newImage = new QImage(QSize(origin.width(), origin.height()),
                                  QImage::Format_ARGB32);
    QColor tmpColor;
    int r, g, b;
    for (int i=0; i<newImage->width(); ++i)
    {
        for (int j=0; j<newImage->height(); ++j)
        {
            tmpColor = QColor(origin.pixel(i, j));
            r = tmpColor.red();
            g = tmpColor.green();
            b = tmpColor.blue();

            newImage->setPixel(i, newImage->height()-j-1, qRgb(r,g,b));
        }
    }
    return *newImage;
}


/*****************************************************************************
 *                               简单平滑处理
 * kernel矩阵决定计算中相邻像素的影响程度
 * 然后使用滤波器进行计算
 * eg:
 *R = 20 102 99
 *    150 200 77
 *    170 210 105
 *Kenel = 0 2 0
 *        2 5 2
 *        0 2 0
 * r = ( (102*2) + (150*2) + (200*5) + (77*2) + (210*2) ) / (2+2+5+2+2) = 159
 * **************************************************************************/
QImage Algorithm::SimpleSmooth(const QImage &origin)
{
    QImage *newImage = new QImage(origin);

    int kernel[5][5] = {
        {0,0,1,0,0},
        {0,1,3,1,0},
        {1,3,7,3,1},
        {0,1,3,1,0},
        {0,0,1,0,0}
    };
    int startKernel=2;
    int sumKernel=27;
    int r,g,b;
    QColor color;

    //起始点第3个像素点 结束点是倒数第3个像素点
    for(int x=startKernel; x<newImage->width()-startKernel; ++x)
    {
        for (int y=startKernel; y<newImage->height()-startKernel; ++y)
        {
            r = g = b = 0;
            for (int i=-startKernel; i<=startKernel; ++i)
            {
                for (int j=-startKernel; j<=startKernel; ++j)
                {
                    color = QColor(origin.pixel(x+i,y+j));
                    r += color.red()*kernel[startKernel+i][startKernel+j];
                    g += color.green()*kernel[startKernel+i][startKernel+j];
                    b += color.blue()*kernel[startKernel+i][startKernel+j];
                }
            }
            r = qBound(0, r/sumKernel, 255);
            g = qBound(0, g/sumKernel, 255);
            b = qBound(0, b/sumKernel, 255);

            newImage->setPixel(x,y,qRgb(r,g,b));

        }
    }
    return *newImage;
}
