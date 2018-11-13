import os
import json
from enum import Enum
import codecs
import logging


class DataFixed(object):
    """description of class"""
    def __init__(self, title):
        self.__NumberPatterns__ = (u'0', u'1', u'2', u'3', u'4', u'5', u'6', u'7', u'8', u'9' )
        self.__FloatPatterns__ = self.__NumberPatterns__ +  (u'.', )
        self.__LetterPatterns__ = (u'A', u'B', u'C', u'D', u'E', u'F', u'G', u'H', u'I', u'J', u'K', 
                                   u'L', u'M', u'N', u'O', u'P', u'Q', u'R', u'S', u'T', u'U', u'V', u'W', u'X', u'Y', u'Z' )
        self.__Title__ = title
        self.__ErrorCount__ = 0
        self.__FixedCount__ = 0


    def StartFixedFromJson(self, resultJson):
        return self.__FixedData__(resultJson)
        #return self.__FixedDataWithValidate__(validateJson,resultJson)


    # for test
    def StartFixedFromPath(self, resultPath, validatedPath):
        self.__ResultPath__ = resultPath
        self.__ValidatedPath__ = validatedPath

        self.__BeforeFixed__()

        self.__VisitFolder__(self.__Handler__)

        self.__AfterFixed__()


    def __BeforeFixed__(self):
        logging.info(u'Start Fixed ' + self.__Title__ + ' Data=================>\n')


    def __FixedData__(self, resultJson):
        pass


    def __FixedDataWithValidate__(self, validateJson, resultJson):
        pass


    def __AfterFixed__(self):
        logging.info(u'Error Count ' + str(self.__ErrorCount__) + u', Fixed Count ' + str(self.__FixedCount__))

        logging.info(u'\n<=================End Fixed ' + self.__Title__ + ' Data')


    def __VisitFolder__(self, visitor, subfolder = ''):
        if not os.path.exists(self.__ValidatedPath__ + subfolder):
            return

        names = os.listdir(self.__ValidatedPath__ + subfolder)
        for name in names:
            if os.path.isdir(self.__ValidatedPath__ + subfolder + name):
                subdir = subfolder + name + '/'
                self.__VisitFolder__(visitor, subdir)
            else:
                visitor(subfolder, name)


    def __Handler__(self, subfolder, filename):
        logging.info(u'Start Handler ' + filename)

        if os.path.exists(self.__ValidatedPath__ + subfolder + filename) and os.path.exists(self.__ResultPath__ + subfolder + filename):
            validateJson = json.load(codecs.open(self.__ValidatedPath__ + subfolder + filename, encoding='utf-8'))
            resultJson = json.load(codecs.open(self.__ResultPath__ + subfolder + filename, encoding='utf-8'))

            self.__FixedDataWithValidate__(resultJson, validateJson)
        else:
            logging.info(u'Miss ' + filename + '\n')

        logging.info(u'End Handler ' + filename + '\n')


    def __CheckInPatterns__(self, data, patterns):
        if len(data) == 0 or not isinstance(patterns, tuple):
            return False

        for ch in data:
            if ch not in patterns:
                return False

        return True


    def __CheckNumber__(self, data):
        return self.__CheckInPatterns__(data, self.__NumberPatterns__)


    def __CheckFloat__(self, data):
        if not self.__CheckInPatterns__(data, self.__FloatPatterns__):
            return False

        return data.count(u'.') <= 1


    def __CheckLetter__(self, data):
        return self.__CheckInPatterns__(data, self.__LetterPatterns__)

