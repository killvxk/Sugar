import logging
import re

from GeneralNumberDataFixed import GeneralNumberDataFixed
from ConfidenceLevel import ConfidenceLevel

class SerialNumberFixed(GeneralNumberDataFixed):
    """description of class"""

    def __init__(self):
        GeneralNumberDataFixed.__init__(self)
        self.__NumberCount__ = 11
        self.__Cls__ = 38

    def __FixedCodeData__(self, datalist):
        datalist = self.__FormatByTripWhiteSpace__(datalist)

        for number in datalist:
            if self.__CheckData__(number):
                return ConfidenceLevel.Fixed, number

        return ConfidenceLevel.Bad, datalist[0]

    def __FormatByTripWhiteSpace__(self, datalist):
        result = []
        for number_str in datalist:
            result.append(re.sub(r'[^0-9]', u'', number_str))

        return result
