import re
import logging

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel

class CheckCodeFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self, 'CheckCode')
        self.__NumberCount__ = 20
        self.__NumberPatterns__ = (u'0', u'1', u'2', u'3', u'4', u'5', u'6', u'7', u'8', u'9', u' ')

    
    def __FixedData__(self, resultJson):
        result_checkcodelist = self.__ParseData__(resultJson)
        if len(result_checkcodelist) == 0:
            logging.info(u'CheckCode Data Error')
            return ConfidenceLevel.Bad, '', [], []

        logging.info(result_checkcodelist[0] + u' Fixed To ')
        
        confidencelevel, checkcode, checkcodecandidates, lastsix = self.__FixedCheckCodeData__(result_checkcodelist)

        logging.info(checkcode)

        return confidencelevel, checkcode, checkcodecandidates, lastsix


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_checkcodelist = self.__ParseData__(resultJson)
        validated_checkcodelist = self.__ParseData__(validateJson)

        if len(validated_checkcodelist) == 0 or len(result_checkcodelist) == 0 or not self.__CheckData__(validated_checkcodelist[0]):
            logging.info(u'Validated Data Error')
            return

        if validated_checkcodelist[0] != result_checkcodelist[0]:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(result_checkcodelist[0] + u' Fixed To ')
        
        confidencelevel, checkcode, checkcodecandidates, lastsix = self.__FixedCheckCodeData__(result_checkcodelist)

        logging.info(checkcode)

        if validated_checkcodelist[0] == checkcode:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_checkcodelist[0])
            logging.info(u'Fixed Falied!')


    def __ParseData__(self, jsondata):
        checkcodelist = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return checkcodelist

        regions = jsondata[u'regions']

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None or region[u'ref_result'] == None:
                continue

            cls = region[u'cls']
            if cls == 5:
                for result in region[u'result']:
                    if len(result):
                        checkcodelist.append(result)

                for result in region[u'ref_result']:
                    if len(result):
                        checkcodelist.append(result)

        checkcodelist = list(map(lambda x: re.sub(u' |-', u'', x), checkcodelist))
        return checkcodelist


    def __CheckData__(self, data):
        if len(data) == 0 or len(data) != self.__NumberCount__:
            return False

        for ch in data:
            if ch not in self.__NumberPatterns__:
                return False

        return True


    def __FixedCheckCodeData__(self, datalist):
        datalist = sorted(datalist, key=lambda x: abs(len(x) - self.__NumberCount__))

        checkcodecandidates = []
        lastsix = []

        for data in datalist:
            if self.__CheckData__(data):
                if not data in checkcodecandidates and len(checkcodecandidates) < 5:
                    checkcodecandidates.append(data)

                if len(data) > 6 and not data[len(data) - 6:] in lastsix and len(lastsix) < 5:
                    lastsix.append(data[len(data) - 6:])

        if len(checkcodecandidates):
            return ConfidenceLevel.Fixed, checkcodecandidates[0], checkcodecandidates, lastsix
        elif len(datalist):
            return ConfidenceLevel.Bad, datalist[0], checkcodecandidates, lastsix
        else:
            return ConfidenceLevel.Bad, '', checkcodecandidates, lastsix
