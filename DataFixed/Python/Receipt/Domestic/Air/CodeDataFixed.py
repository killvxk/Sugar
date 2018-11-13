# -*- coding: utf-8 -*-

import logging
import re

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel

class CodeDataFixed(DataFixed):
    """description of class"""

    def __init__(self, title = None):
        DataFixed.__init__(self, title or u'Code')
        self.__ErrorCount__ = 0
        self.__FixedCount__ = 0
        self.__NumberCount__ = 13
        self.__NumberPatterns__ = (u'0', u'1', u'2', u'3', u'4', u'5', u'6', u'7', u'8', u'9')


    def __BeforeFixed__(self):
        logging.info(u'Start Fixed Code Data=================>\n')

    
    def __FixedData__(self, resultJson):
        codelist = self.__ParseData__(resultJson)
        if len(codelist) == 0:
            logging.info(u'Code Data Error')
            return ConfidenceLevel.Bad, ''

        logging.info(codelist[0] + u' Fixed To ')

        confidencelevel, code = self.__FixedCodeData__(codelist)

        logging.info(code)

        return confidencelevel, code


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_codelist = self.__ParseData__(resultJson)
        validated_codelist = self.__ParseData__(validateJson)

        if len(validated_codelist) == 0 or len(result_codelist) == 0 or not self.__CheckData__(validated_codelist[0]):
            logging.info(u'Validated Data Error')
            return

        if validated_codelist[0] != result_codelist[0]:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(result_codelist[0] + u' Fixed To ')
        
        confidencelevel, code = self.__FixedCodeData__(result_codelist)

        logging.info(code)

        if validated_codelist[0] == code:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_codelist[0])
            logging.info(u'Fixed Falied!')


    def __AfterFixed__(self):
        logging.info(u'Error Count ' + str(self.__ErrorCount__) + u', Fixed Count ' + str(self.__FixedCount__))

        logging.info(u'\n<=================End Fixed Code Data')


    def __ParseData__(self, jsondata):
        codelist = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions0'] == None:
            return codelist

        regions = jsondata[u'regions0']
        regions = sorted(regions, key=lambda region: region[u'confidence'], reverse = True)

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None or region[u'ref_result'] == None:
                continue

            cls = region[u'cls']
            if cls == 1:
                for result in region[u'result']:
                    if len(result):
                        codelist.append(result)

                for result in region[u'ref_result']:
                    if len(result):
                        codelist.append(result)

        return self.__ReplaceConfusingLetters(codelist)


    def __CheckData__(self, data):
        if len(data) == 0 or len(data) != self.__NumberCount__:
            return False

        return self.__CheckNumber__(data)


    def __FixedCodeData__(self, codes):
        if len(codes):
            code_before_sort = codes[0]
        codes = sorted(codes, key=lambda x: abs(len(x) - self.__NumberCount__))

        for first in codes:
            if self.__CheckData__(first):
                return ConfidenceLevel.Bad, first

        if len(codes):
            return ConfidenceLevel.Bad, code_before_sort
        else:
            return ConfidenceLevel.Bad, ''


    def __ReplaceConfusingLetters(self, codelist):
        result_codelist = []
        for code in codelist:
            result_codelist.append(re.sub(u'L|I|i|l', u'1', code))
        
        return result_codelist

