import logging
import re

from GeneralNumberDataFixed import GeneralNumberDataFixed
from ConfidenceLevel import ConfidenceLevel

class AgentCodeFixed(GeneralNumberDataFixed):
    """description of class"""

    def __init__(self):
        GeneralNumberDataFixed.__init__(self)
        self.__CodePatterns__ = r'(\d{8}|^\d{7}$|[0-9A-Z]{3}\d{3}|^[0-9A-Z]{3}$)'

    def __FixedCodeData__(self, datalist):
        datalist = self.__GetStardandData__(datalist)

        for number in datalist:
            if self.__CheckData__(number):
                return ConfidenceLevel.Fixed, number

        return ConfidenceLevel.Bad, datalist[0]

    def __CheckData__(self, data):
        if len(data) == 0:
            return False

        regex = re.search(self.__CodePatterns__, data)
        if regex:
            return True

        return False


    def __GetStardandData__(self, datalist):
        result = []
        for code in datalist:
            temp = re.sub(r'[^0-9A-Z]', u'', code)
            regex = re.search(self.__CodePatterns__, temp)
            if regex:
                result.append(regex.group())
            else:
                result.append(temp)

        return result

    def __ParseData__(self, resultJson):
        codes = []

        if resultJson == None or not isinstance(resultJson, dict):
            return codes
    
        for result in resultJson.get(u'result', []):
            if len(result):
                codes.append(result)

        for result in resultJson.get(u'ref_result', []):
            if len(result):
                codes.append(result)

        tripped_codes = []
        for code in codes:
            tripped_codes.append(re.sub(r'[^a-zA-Z0-9]', u'', code))
    
        return tripped_codes
