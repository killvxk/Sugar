# -*- coding: utf-8 -*-

import logging
import re

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel

class IdNumberDataFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self,u'ID')
        self.__ErrorCount__ = 0
        self.__FixedCount__ = 0
        self.__Regex__ = u'^\\d{15}|\\d{18}$'


    def __BeforeFixed__(self):
        logging.info(u'Start Fixed ID Number Data=================>\n')

    
    def __FixedData__(self, resultJson):
        id_numbers = self.__ParseData__(resultJson)
        if len(id_numbers) == 0:
            logging.info(u'ID Number Data Error')
            return ConfidenceLevel.Bad, ''

        logging.info(id_numbers[0] + u' Fixed To ')

        confidencelevel, id_number = self.__FixedIDNumberData__(id_numbers)

        logging.info(id_number)

        return confidencelevel, id_number


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_id_numbers = self.__ParseData__(resultJson)
        validated_id_numbers = self.__ParseData__(validateJson)

        if len(validated_id_numbers) == 0 or len(result_id_numbers) == 0 or not self.__CheckData__(validated_id_numbers[0]):
            logging.info(u'Validated Data Error')
            return

        if validated_id_numbers[0] != result_id_numbers[0]:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(result_id_numbers[0] + u' Fixed To ')
        
        confidencelevel, id_number = self.__FixedIDNumberData__(result_id_numbers)

        logging.info(id_number)

        if validated_id_numbers[0] == id_number:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_id_numbers[0])
            logging.info(u'Fixed Falied!')


    def __AfterFixed__(self):
        logging.info(u'Error Count ' + str(self.__ErrorCount__) + u', Fixed Count ' + str(self.__FixedCount__))

        logging.info(u'\n<=================End Fixed ID Number Data')


    def __ParseData__(self, jsondata):
        id_numbers = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions0'] == None:
            return id_numbers

        regions = jsondata[u'regions0']
        regions = sorted(regions, key=lambda region: region[u'confidence'], reverse = True)

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None or region[u'ref_result'] == None:
                continue

            cls = region[u'cls']
            if cls == 22:
                for result in region[u'result']:
                    if len(result):
                        id_numbers.append(result)

                for result in region[u'ref_result']:
                    if len(result):
                        id_numbers.append(result)

        return id_numbers


    def __CheckData__(self, data):
        result = re.search(self.__Regex__, data)
        if result and result.span():
            return True
        else:
            return False


    def __FixedIDNumberData__(self, id_numbers):
        for number in id_numbers:
            if self.__CheckData__(number):
                return ConfidenceLevel.Bad, number

        return ConfidenceLevel.Bad, id_numbers[0]


