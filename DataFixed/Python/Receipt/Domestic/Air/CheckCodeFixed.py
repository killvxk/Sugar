# -*- encoding: utf-8 -*-

import re
import logging

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel

# TODO 校验码 (cls: 5) 和 印刷序号 (cls: 38) 后4位相同, 可以结合起来 fix
class CheckCodeFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self, u'Checkcode')
        self.__ErrorCount__ = 0
        self.__FixedCount__ = 0
        self.__NumberCount__ = 4
        self.__NumberPatterns__ = (u'0', u'1', u'2', u'3', u'4', u'5', u'6', u'7', u'8', u'9', u' ')

    def __BeforeFixed__(self):
        logging.info(u'Start Fixed CheckCode Data=================>\n')

    
    def __FixedData__(self, resultJson):
        result_checkcodelist = self.__ParseData__(resultJson)
        if len(result_checkcodelist) == 0:
            logging.info(u'CheckCode Data Error')
            return ConfidenceLevel.Bad, '', []

        logging.info(result_checkcodelist[0] + u' Fixed To ')
        
        confidencelevel, checkcode, checkcodecandidates = self.__FixedCheckCodeData__(result_checkcodelist)

        logging.info(checkcode)

        return confidencelevel, checkcode, checkcodecandidates


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
        
        confidencelevel, checkcode, checkcodecandidates = self.__FixedCheckCodeData__(result_checkcodelist)

        logging.info(checkcode)

        if validated_checkcodelist[0] == checkcode:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_checkcodelist[0])
            logging.info(u'Fixed Falied!')


    def __AfterFixed__(self):
        logging.info(u'Error Count ' + str(self.__ErrorCount__) + u', Fixed Count ' + str(self.__FixedCount__))

        logging.info(u'\n<=================End Fixed CheckCode Data')


    def __ParseData__(self, jsondata):
        checkcodelist = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions0'] == None:
            return checkcodelist

        regions = jsondata[u'regions0']

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

        for data in datalist:
            if self.__CheckData__(data):
                if not data in checkcodecandidates and len(checkcodecandidates) < 5:
                    checkcodecandidates.append(data)

        if len(checkcodecandidates):
            return ConfidenceLevel.Fixed, checkcodecandidates[0][:self.__NumberCount__], checkcodecandidates
        elif len(datalist):
            return ConfidenceLevel.Bad, datalist[0][:self.__NumberCount__], checkcodecandidates
        else:
            return ConfidenceLevel.Bad, '', checkcodecandidates
