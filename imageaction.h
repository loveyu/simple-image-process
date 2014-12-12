#ifndef IMAGEACTION_H
#define IMAGEACTION_H

#include <QRect>
#include <QImage>
#include <QApplication>
#include <QDebug>
#include <math.h>
#include <QClipboard>

class ImageAction {
public:
    QRect rect;
    int nowZoom;

    ImageAction();

    QImage GetImageZoom(const QImage img, int zoom, int w_w, int w_h, int o_x, int o_y) {
        int left = 0, top = 0, width = 1, height = 1;
        QImage rt_img = img.copy();
        int m_w = img.width();
        int m_h = img.height();
        if (zoom > 0) {
            int o_w = 0;//最终输出大小
            int o_h = 0;

            o_w = m_w * zoom / 100;
            o_h = m_h * zoom / 100;

            if (o_w <= w_w && o_h <= w_h) {
                //可直接输出
                width = o_w;
                height = o_h;
                left = (w_w - o_w) / 2;
                top = (w_h - o_h) / 2;
            } else {
                //不可直接输出
                rt_img = rt_img.scaled(o_w, o_h);//缩放到目标大小
                if (rt_img.width() > w_w && rt_img.height() > w_h) {
                    if (o_x + w_w > rt_img.width()) {
                        o_x = rt_img.width() - w_w;
                    }
                    if (o_y + w_h > rt_img.height()) {
                        o_y = rt_img.height() - w_h;
                    }
                    rt_img = rt_img.copy(o_x, o_y, w_w, w_h);
                } else if (rt_img.width() > w_w) {
                    //较宽
                    if (o_x + w_w > rt_img.width()) {
                        o_x = rt_img.width() - w_w;
                    }
                    rt_img = rt_img.copy(o_x, 0, w_w, rt_img.height());
                } else {
                    //较高
                    if (o_y + w_h > rt_img.height()) {
                        o_y = rt_img.height() - w_h;
                    }
                    rt_img = rt_img.copy(0, o_y, rt_img.width(), w_h);
                }
                width = rt_img.width();
                height = rt_img.height();
                left = (w_w - width) / 2;
                top = (w_h - height) / 2;
            }
            nowZoom = zoom;
        } else {
            if (m_w < w_w && m_h < w_h) {
                left = (w_w - m_w) / 2;
                top = (w_h - m_h) / 2;
                width = m_w;
                height = m_h;
            } else {
                float x1 = (float) m_w / m_h;
                float x2 = (float) w_w / w_h;
                if (x1 > x2) {
                    //max width
                    height = w_w / x1;
                    left = 0;
                    width = w_w;
                    top = (w_h - height) / 2;
                } else {
                    //max height,or eq
                    width = w_h * x1;
                    top = 0;
                    height = w_h;
                    left = (w_w - width) / 2;
                }
            }
            nowZoom = width * 100 / m_w;
        }
        rect = QRect(left, top, width, height);
        return rt_img;
    }

    static void Negative(QImage &image) {
        if (image.isNull()) {
            qDebug() << "Load image failed! ";
            return;
        }
        int width = image.width();
        int height = image.height();
        QRgb color;
        QRgb negative;
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                color = image.pixel(i, j);
                negative = qRgba(255 - qRed(color),
                        255 - qGreen(color),
                        255 - qBlue(color),
                        qAlpha(color));
                image.setPixel(i, j, negative);
            }
        }
    }

    static void Soften(QImage &image) {
        if (image.isNull()) {
            qDebug() << "load failed! ";
            return;
        }
        int width = image.width();
        int height = image.height();
        int r, g, b;
        QRgb color;
        int xLimit = width - 1;
        int yLimit = height - 1;
        for (int i = 1; i < xLimit; i++) {
            for (int j = 1; j < yLimit; j++) {
                r = 0;
                g = 0;
                b = 0;
                for (int m = 0; m < 9; m++) {
                    int s = 0;
                    int p = 0;
                    switch (m) {
                        case 0:
                            s = i - 1;
                            p = j - 1;
                            break;
                        case 1:
                            s = i;
                            p = j - 1;
                            break;
                        case 2:
                            s = i + 1;
                            p = j - 1;
                            break;
                        case 3:
                            s = i + 1;
                            p = j;
                            break;
                        case 4:
                            s = i + 1;
                            p = j + 1;
                            break;
                        case 5:
                            s = i;
                            p = j + 1;
                            break;
                        case 6:
                            s = i - 1;
                            p = j + 1;
                            break;
                        case 7:
                            s = i - 1;
                            p = j;
                            break;
                        case 8:
                            s = i;
                            p = j;
                    }
                    color = image.pixel(s, p);
                    r += qRed(color);
                    g += qGreen(color);
                    b += qBlue(color);
                }

                r = (int) (r / 9.0);
                g = (int) (g / 9.0);
                b = (int) (b / 9.0);

                r = qMin(255, qMax(0, r));
                g = qMin(255, qMax(0, g));
                b = qMin(255, qMax(0, b));

                image.setPixel(i, j, qRgb(r, g, b));
            }
        }
    }

    static void Gray(QImage &image) {
        if (image.isNull()) {
            qDebug() << "load  failed! ";
            return;
        }
        //qDebug() << "depth - " << image.depth();

        int width = image.width();
        int height = image.height();
        QRgb color;
        int gray;
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                color = image.pixel(i, j);
                gray = qGray(color);
                image.setPixel(i, j,
                        qRgba(gray, gray, gray, qAlpha(color)));
            }
        }

    }

    static void Binarize(QImage &image) {
        if (image.isNull()) {
            qDebug() << "load  failed! ";
            return;
        }
        int width = image.width();
        int height = image.height();
        QRgb color;
        QRgb avg;
        QRgb black = qRgb(0, 0, 0);
        QRgb white = qRgb(255, 255, 255);
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                color = image.pixel(i, j);
                avg = (qRed(color) + qGreen(color) + qBlue(color)) / 3;
                image.setPixel(i, j, avg >= 128 ? white : black);
            }
        }
    }

    static void Sharpen(QImage &image) {
        if (image.isNull()) {
            qDebug() << "load  failed! ";
            return;
        }
        int width = image.width();
        int height = image.height();
        int threshold = 80;
        QImage sharpen(width, height, QImage::Format_ARGB32);
        int r, g, b, gradientR, gradientG, gradientB;
        QRgb rgb00, rgb01, rgb10;
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                if (image.valid(i, j) &&
                        image.valid(i + 1, j) &&
                        image.valid(i, j + 1)) {
                    rgb00 = image.pixel(i, j);
                    rgb01 = image.pixel(i, j + 1);
                    rgb10 = image.pixel(i + 1, j);
                    r = qRed(rgb00);
                    g = qGreen(rgb00);
                    b = qBlue(rgb00);
                    gradientR = abs(r - qRed(rgb01)) + abs(r - qRed(rgb10));
                    gradientG = abs(g - qGreen(rgb01)) +
                            abs(g - qGreen(rgb10));
                    gradientB = abs(b - qBlue(rgb01)) +
                            abs(b - qBlue(rgb10));

                    if (gradientR > threshold) {
                        r = qMin(gradientR + 100, 255);
                    }

                    if (gradientG > threshold) {
                        g = qMin(gradientG + 100, 255);
                    }

                    if (gradientB > threshold) {
                        b = qMin(gradientB + 100, 255);
                    }

                    sharpen.setPixel(i, j, qRgb(r, g, b));
                }
            }
        }
        image = sharpen;
    }

    static void Emboss(QImage &image) {
        if (image.isNull()) {
            qDebug() << "load  failed! ";
            return;
        }
        int width = image.width();
        int height = image.height();
        QRgb color;
        QRgb preColor = 0;
        QRgb newColor;
        int gray, r, g, b, a;
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                color = image.pixel(i, j);
                r = qRed(color) - qRed(preColor) + 128;
                g = qGreen(color) - qGreen(preColor) + 128;
                b = qBlue(color) - qBlue(preColor) + 128;
                a = qAlpha(color);
                gray = qGray(r, g, b);
                newColor = qRgba(gray, gray, gray, a);
                image.setPixel(i, j, newColor);
                preColor = newColor;
            }
        }

    }

    static bool Resize(QImage &img, int width, int height) {
        if (width == 0) {
            img = img.scaledToHeight(height);
        } else if (height == 0) {
            img = img.scaledToWidth(width);
        } else {
            img = img.scaled(width, height);
        }
        return true;
    }

    static bool Quality(QImage &img, float percentage) {
        int width = img.width();
        int height = img.height();
        percentage = sqrt(percentage / 100);
        Resize(img, width * percentage, height * percentage);
        Resize(img, width, height);
        return true;
    }

    static bool Transparent(QImage &img, float alpha) {
        if (!img.hasAlphaChannel()) {
            img.setAlphaChannel(img);
        }
        QRgb rgb;
        for (short i = 0; i < img.height(); i++) {
            for (short j = 0; j < img.width(); j++) {
                rgb = img.pixel(j, i);
                img.setPixel(j, i, qRgba(qRed(rgb), qGreen(rgb), qBlue(rgb), alpha));
            }
        }
        return true;
    }

    static bool Rotation(QImage &img, float angle) {
        QMatrix matrix;
        matrix.rotate(angle);
        img = img.transformed(matrix);
        return true;
    }

    static bool Cut(QImage &img, QRect rect, float percentage) {
        rect = QRect(rect.left() * percentage, rect.top() * percentage, rect.width() * percentage, rect.height() * percentage);
        img = img.copy(rect);
        return true;
    }

    static bool Copy(QImage img, QRect rect, float percentage) {
        rect = QRect(rect.left() * percentage, rect.top() * percentage, rect.width() * percentage, rect.height() * percentage);
        QClipboard *cb = QApplication::clipboard();
        cb->clear();
        cb->setImage(img.copy(rect));
        return true;
    }

    static bool Delete(QImage &img, QRect rect, float percentage) {
        rect = QRect(rect.left() * percentage, rect.top() * percentage, rect.width() * percentage, rect.height() * percentage);
        for (short i = rect.top(); i < rect.top() + rect.height(); i++) {
            for (short j = rect.left(); j < rect.left() + rect.width(); j++) {
                img.setPixel(j, i, qRgba(0, 0, 0, 0));
            }
        }
        return true;
    }
};

#endif // IMAGEACTION_H
