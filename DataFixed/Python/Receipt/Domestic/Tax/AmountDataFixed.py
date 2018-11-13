import logging
import re

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel


class AmountDataFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self, 'Amount')


    def __FixedData__(self, resultJson):
        result_amountlist = self.__ParseData__(resultJson)

        if len(result_amountlist) == 0:
            logging.info(u'Date Data Error')
            return ConfidenceLevel.Bad, ''

        logging.info(result_amountlist[0] + u' Fixed To ')

        confidenceLevel, result_amount = self.__FixedAmountData__(result_amountlist)

        logging.info(result_amount)

        return confidenceLevel, result_amount


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_amountlist = self.__ParseData__(resultJson)
        validate_amountlist = self.__ParseData__(validateJson)

        if len(validate_amountlist) == 0 or len(result_amountlist) == 0 or not self.__CheckData__(validate_amountlist[0]):
            logging.info(u'Validated Data Error')
            return

        if validate_amountlist[0] != result_amountlist[0]:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(result_amountlist[0] + u' Fixed To ')

        confidenceLevel, result_amount = self.__FixedAmountData__(result_amountlist)

        logging.info(result_amount)

        if validate_amountlist[0] == result_amount:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validate_amountlist[0])
            logging.info(u'Fixed Falied!')


    def __ParseData__(self, jsondata):
        amountlist = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return amountlist

        regions = jsondata[u'regions']

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None:
                continue

            cls = region[u'cls']
            if cls == 9:
                for result in region[u'result']:
                    if len(result):
                        amountlist.append(result)

                if region.get(u'ref_result') != None:
                    for result in region[u'ref_result']:
                        if len(result):
                            amountlist.append(result)

        return amountlist


    def __CheckData__(self, data):
        if not self.__CheckFloat__(data):
            return False

        dotindex = data.find(u'.')
        if (dotindex != -1 and (len(data) - dotindex) == 3):
            return True

        return False


    def __FixedAmountData__(self, amountlist):
        return ConfidenceLevel.Bad, self.__FormatData__(self.__NormalizeData__(amountlist[0]))


    def __NormalizeData__(self, amountdata):
        length = len(amountdata)
        if length == 0:
            return amountdata

        amountdata = re.sub(u'\\$| |%|\\\\', u'', amountdata)
        amountdata = ''.join([ch for ch in amountdata if (ch in self.__FloatPatterns__)])

        length = len(amountdata)
        dotindex = amountdata.find(u'.')
        if dotindex != -1:
            if (length - dotindex) > 2:
                return amountdata[0: dotindex + 3]

        return amountdata 


    def __FormatData__(self, amountdata):
        length = len(amountdata)
        if length == 0:
            return ''

        while len(amountdata) > 1 and amountdata[0] == u'0' and amountdata[1] == u'0':
            amountdata = amountdata[1:]

        dotindex = amountdata.find(u'.')
        if dotindex != -1:
            if (length - dotindex) > 2:
                return amountdata[0: dotindex + 3]
            elif (length - dotindex) == 2:
                return amountdata + u'0'
            elif (length - dotindex) == 1:
                return amountdata + u'00'
            return amountdata
        else:
            return amountdata + u'.00'