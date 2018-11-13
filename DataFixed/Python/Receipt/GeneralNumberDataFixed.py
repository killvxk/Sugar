# -*- coding: utf-8 -*-

import logging

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel

class GeneralNumberDataFixed(DataFixed):
    """description of class"""

    def __init__(self, title = 'Number'):
        DataFixed.__init__(self, title)
        self.__DynamicNumberCount__ = False
        self.__NumberCount__ = 10
        self.__Cls__ = 1

    
    def __FixedData__(self, resultJson):
        result_numberlist = self.__ParseData__(resultJson)
        if len(result_numberlist) == 0:
            logging.info(u'Number Data Error')
            return ConfidenceLevel.Bad, ''

        logging.info(result_numberlist[0] + u' Fixed To ')
        
        if self.__DynamicNumberCount__:
            self.__NumberCount__ = self.__GuessCodeNumberCount__(result_numberlist)

        confidencelevel, number = self.__FixedCodeData__(result_numberlist)

        logging.info(number)

        return confidencelevel, number


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_numberlist = self.__ParseData__(resultJson)
        validated_numberlist = self.__ParseData__(validateJson)

        if len(validated_numberlist) == 0 or len(result_numberlist) == 0 or not self.__CheckData__(validated_numberlist[0]):
            logging.info(u'Validated Data Error')
            return

        if validated_numberlist[0] != result_numberlist[0]:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(result_numberlist[0] + u' Fixed To ')

        confidencelevel, number = self.__FixedCodeData__(result_numberlist)

        logging.info(number)

        if validated_numberlist[0] == number:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_numberlist[0])
            logging.info(u'Fixed Falied!')


    def __ParseData__(self, jsondata):
        numberlist = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return numberlist

        regions = jsondata[u'regions']
        regions = sorted(regions, key=lambda region: region[u'confidence'], reverse = True)

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None or region[u'ref_result'] == None:
                continue

            cls = region[u'cls']
            if cls == self.__Cls__:
                for result in region[u'result']:
                    if len(result):
                        numberlist.append(result)

                for result in region[u'ref_result']:
                    if len(result):
                        numberlist.append(result)

        return numberlist


    def __CheckData__(self, data):
        if len(data) == 0 or len(data) != self.__NumberCount__:
            return False

        return self.__CheckNumber__(data)


    def __FixedCodeData__(self, datalist):
        datalist = sorted(datalist, key=lambda x: abs(len(x) - self.__NumberCount__))

        for data in datalist:
            if self.__CheckData__(data):
                return ConfidenceLevel.Bad, data

        if len(datalist):
            return ConfidenceLevel.Bad, datalist[0]
        else:
            return ConfidenceLevel.Bad, ''


    def __GuessCodeNumberCount__(self, datalist):
        numbercountmap = {}
        for code in datalist:
            if numbercountmap.has_key(len(code)):
                numbercountmap[len(code)] += 1
            else:
                numbercountmap[len(code)] = 1

        return max(numbercountmap)


