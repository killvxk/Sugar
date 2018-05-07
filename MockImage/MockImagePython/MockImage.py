#!/usr/bin/env python3
# -*- coding:utf-8 -*-
 
import os, shutil
import sys
import random
from enum import Enum

import PIL
from PIL import Image
from PIL import ImageFilter
from PIL import ImageDraw

MockIndex = 1

def Vistor(rootdir):
    fileList = []
    if not os.path.exists(rootdir):
        return

    names = os.listdir(rootdir)
    for name in names:
        if os.path.isdir(rootdir + name):
            subdir = rootdir + name + '/'
            subnames = Vistor(subdir)
            for subname in subnames:
                fileList.append(subname)
        elif os.path.splitext(name)[1] == '.jpg':
            fileList.append(rootdir + name)

    return fileList

class EMock(Enum):
    BLEND = 1
    BLUR = 2
    DRAWLINE = 3
    DRAWPOLY = 4
    MIX = 5
    MAX = 6

chaptersFolder = os.path.split(os.path.realpath(__file__))[0] + '/chapters/'
chapters = [chaptersFolder + '1.png', chaptersFolder + '2.png', chaptersFolder + '3.png', chaptersFolder + '4.png', chaptersFolder + '5.png', chaptersFolder + '6.png']
def Stamp(image):
    imageResult = image.copy()
    imageChapter = Image.open(chapters[int(random.uniform(0, 6))]).convert('RGBA')
    offset = (int(random.uniform(-imageChapter.size[0], imageResult.size[0] * 0.8)), int(random.uniform(-imageChapter.size[1], imageResult.size[1] * 0.8)))
    for height in range(0, imageChapter.size[1]):
        if ((height + offset[1]) < imageResult.size[1] and (height + offset[1]) > 0):
            for width in range(0, imageChapter.size[0]):
                if ((width + offset[0]) < imageResult.size[0] and (width + offset[0]) > 0):
                    pixel = imageChapter.getpixel((width, height))
                    if (pixel[3] != 0):
                        alpha = pixel[3] / 255.0 * 0.5
                        origin = imageResult.getpixel((width + offset[0], height + offset[1]))
                        red = int(origin[0] * (1 - alpha) + pixel[0] * alpha)
                        green = int(origin[1] * (1 - alpha) + pixel[1] * alpha)
                        blue = int(origin[2] * (1 - alpha) + pixel[2] * alpha)
                        imageResult.putpixel((width + offset[0], height + offset[1]), (red, green, blue, 255))
    return imageResult

def Blur(image):
    imageResult = image.filter(ImageFilter.GaussianBlur(radius = int(random.uniform(1, 5))))
    return imageResult

def DrawLine(image):
    imageResult = image.copy()
    draw = ImageDraw.Draw(imageResult)
    start = [int(random.uniform(0, imageResult.size[0] * 0.5)), int(random.uniform(0, imageResult.size[1] * 0.5))]
    if (int(random.uniform(0, 2))):
        start[0] = 0
    else:
        start[1] = 0
    end = [int(random.uniform(0, imageResult.size[0] * 0.5) + imageResult.size[0] * 0.5), int(random.uniform(0, imageResult.size[1] * 0.5) + imageResult.size[1] * 0.5)]
    if (int(random.uniform(0, 2))):
        end[0] = imageResult.size[0]
    else:
        end[1] = imageResult.size[1]
    draw.line([tuple(start), tuple(end)], fill=(156, 133, 117), width = 4)
    return imageResult

def DrawPoly(image):
    imageResult = image.copy()
    draw = ImageDraw.Draw(imageResult)

    poly = []
    if (int(random.uniform(0, 2))):
        poly = [(0, 0), (imageResult.size[0], 0), (imageResult.size[0], int(random.uniform(0, imageResult.size[1] * 0.25) + imageResult.size[1] * 0.25)), (0, int(random.uniform(0, imageResult.size[1] * 0.5)))]
    else:
        poly = [(0, imageResult.size[1]), (imageResult.size[0], imageResult.size[1]), (imageResult.size[0], int(random.uniform(0, imageResult.size[1] * 0.25) + imageResult.size[1] * 0.5)), (0, int(random.uniform(0, imageResult.size[1] * 0.25) + imageResult.size[1] * 0.75))]

    draw.polygon(poly, fill=(0, 0, 0, 200))
    return imageResult

def MockData(mockType, InFilePath, InOutputPath):
    folder = os.path.split(InFilePath)[0]
    fileName = os.path.split(InFilePath)[1]
    name, ext = os.path.splitext(fileName)

    print(name)

    with Image.open(InFilePath) as imageResult:
        if mockType == EMock.BLEND:
            imageResult = Stamp(imageResult)
        elif mockType == EMock.BLUR:
            imageResult = Blur(imageResult)
        elif mockType == EMock.DRAWLINE:
            imageResult = DrawLine(imageResult)
        elif mockType == EMock.DRAWPOLY:
            imageResult = DrawPoly(imageResult)
        elif mockType == EMock.MIX:
            if (int(random.uniform(0, 2))):
                imageResult = Stamp(imageResult)

            if (int(random.uniform(0, 2))):
                imageResult = DrawLine(imageResult)

            if (int(random.uniform(0, 2))):
                imageResult = DrawPoly(imageResult)

            if (int(random.uniform(0, 2))):
                imageResult = Blur(imageResult)

        global MockIndex
        imageResult.save(InOutputPath + str(MockIndex) + '-' + name + ext)
        shutil.copy(folder + '/' +name + '.txt', InOutputPath + str(MockIndex) + '-' + name + '.txt')

        MockIndex = MockIndex + 1

def Generate(mockType, fileList, rate, InOutputPath):
    print('Start Generate')

    count = int(len(fileList) * rate)
    for index in range(0, count):
        MockData(mockType, fileList[index], InOutputPath)

    print('End Generate')

if __name__ == '__main__':
    fileList = Vistor('C:/Users/User/Desktop/Test/')
    Generate(EMock.MIX, fileList, 1.0, 'C:/Users/User/Desktop/output/')
