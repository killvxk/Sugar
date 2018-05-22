import os
import json
from enum import Enum

class ConfidenceLevel(Enum):
    Confident = 'Confident'
    Fixed = 'Fixed'
    Bad = 'Bad'


class DataFixed(object):
    """description of class"""
    def __init__(self):
        pass


    def StartFixedFromJson(self, resultJson):
        return self.__FixedData__(resultJson)


    # for test
    def StartFixedFromPath(self, resultPath, validatedPath):
        self.__ResultPath__ = resultPath
        self.__ValidatedPath__ = validatedPath

        self.__BeforeFixed__()

        self.__Handler__('', '1524103043889_32.48_6.00_38.48.jpg.json')
        self.__VisitFolder__(self.__Handler__)

        self.__AfterFixed__()


    def __BeforeFixed__(self):
        pass


    def __FixedData__(self, resultJson):
        return ''


    def __FixedDataWithValidate__(self, validateJson, resultJson):
        pass


    def __AfterFixed__(self):
        pass


    def __VisitFolder__(self, visitor, subfolder = ''):
        if not os.path.exists(self.__ValidatedPath__ + subfolder):
            return

        names = os.listdir(self.__ValidatedPath__ + subfolder)
        for name in names:
            if os.path.isdir(self.__ValidatedPath__ + subfolder + name):
                subdir = subfolder + name + '/'
                self.__VisitFolder__(subdir, subdir)
            else:
                self.__Handler__(subfolder, name)


    def __Handler__(self, subfolder, filename):
        print('Start Handler ' + filename)

        validateJson = json.load(open(self.__ValidatedPath__ + subfolder + filename, encoding='utf-8'))
        resultJson = json.load(open(self.__ResultPath__ + subfolder + filename, encoding='utf-8'))

        self.__FixedDataWithValidate__(resultJson, validateJson)

        print('End Handler ' + filename + '\n')

