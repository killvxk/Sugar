# -*- coding: utf-8 -*-

import logging
import re

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel
from utils import StrUtil

class NameDataFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self, u'Name')
        self.__ErrorCount__ = 0
        self.__FixedCount__ = 0


    def __BeforeFixed__(self):
        logging.info(u'Start Fixed Name Data=================>\n')


    def __FixedData__(self, resultJson):
        names = self.__ParseData__(resultJson)
        if len(names) == 0:
            logging.info(u'Name Data Error')
            return ConfidenceLevel.Bad, ''

        logging.info(names[0] + u' Fixed To ')
        
        confidencelevel, name = self.__FixedNameData__(names)

        logging.info(name)

        return confidencelevel, name


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        name_list = self.__ParseData__(resultJson)
        validated_name_list = self.__ParseData__(validateJson)

        if len(validated_name_list) == 0 or len(name_list) == 0 or not self.__CheckData__(validated_name_list[0]):
            logging.info(u'Validated Data Error')
            return

        if validated_name_list[0] != name_list[0]:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(name_list[0] + u' Fixed To ')
        
        confidencelevel, checkcode, checkcodecandidates = self.__CheckData__(name_list)

        logging.info(checkcode)

        if validated_name_list[0] == checkcode:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_name_list[0])
            logging.info(u'Fixed Falied!')


    def __AfterFixed__(self):
        logging.info(u'Error Count ' + str(self.__ErrorCount__) + u', Fixed Count ' + str(self.__FixedCount__))

        logging.info(u'\n<=================End Fixed Name Data')


    def __ParseData__(self, jsondata):
        names = []
        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions0'] == None:
            return names

        regions = jsondata[u'regions0']
        regions = sorted(regions, key=lambda region: region[u'confidence'], reverse = True)

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None:
                continue

            cls = region[u'cls']
            if cls == 23:
                for result in region[u'result']:
                    names.append(result)

                for result in region[u'ref_result']:
                    names.append(result)

        return names


    def __CheckData__(self, name):
        if len(name) < 2:
            return False

        for ch in name:
            if ch in self.__LetterPatterns__:
                return False

            if ch in self.__NumberPatterns__:
                return False

        return True


    def __FixedNameData__(self, name_list):
        for name in name_list:
            if self.__CheckData__(name):
                return ConfidenceLevel.Bad, name

        return ConfidenceLevel.Bad, name_list[0]
