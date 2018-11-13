import logging
import re

from GeneralNumberDataFixed import GeneralNumberDataFixed
from ConfidenceLevel import ConfidenceLevel

class CodeDataFixed(GeneralNumberDataFixed):
    """description of class"""

    def __init__(self):
        GeneralNumberDataFixed.__init__(self, 'Code')
        self.__NumberCount__ = 12

    def __CheckData__(self, code):
        if len(code) == 0 or len(code) != self.__NumberCount__:
            return False

        for ch in code:
            if not ch in self.__NumberPatterns__ + self.__LetterPatterns__:
                return False

        return True

    def __FixedCodeData__(self, datalist):
        datalist = self.__TripSpecialChar__(datalist)
        datalist = sorted(datalist, key=lambda x: abs(len(x) - self.__NumberCount__))

        for data in datalist:
            if self.__CheckData__(data):
                return ConfidenceLevel.Bad, data

        if len(datalist):
            return ConfidenceLevel.Bad, datalist[0][:self.__NumberCount__]
        else:
            return ConfidenceLevel.Bad, ''

    def __TripSpecialChar__(self, datalist):
        tripped_list = []
        for code in datalist:
            tripped_list.append(re.sub(r'[^A-Z0-9]', u'', code))

        return tripped_list