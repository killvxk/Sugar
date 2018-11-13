# -*- coding: utf-8 -*-

import logging
import re

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel
from utils import StrUtil

class AmountDataFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self, u'Amount')
        self.__ErrorCount__ = 0
        self.__FixedCount__ = 0
        self.__AmountsCount__ = 4
        self.__Regex__ = u'(CNY|CN|YQ)'
        self.__Numbers__ = (u'0', u'1', u'2', u'3', u'4', u'5', u'6', u'7', u'8', u'9', u'.', u'0', u',', u'，')


    def __BeforeFixed__(self):
        logging.info(u'Start Fixed Amount Data=================>\n')


    def __FixedData__(self, resultJson):
        amounts = self.__ParseData__(resultJson)

        if len(amounts) == 0:
            logging.info(u'Amount Data Error')
            return ConfidenceLevel.Bad, ''

        logging.info(amounts[0] + u' Fixed To ')

        confidencelevel, amount = self.__FixedAmountData__(amounts)

        return confidencelevel, amount


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        amounts = self.__ParseData__(resultJson)
        validate_amounts = self.__ParseData__(validateJson)

        if len(amounts) == 0 or len(validate_amounts) == 0 or not self.__CheckData__(validate_amounts[0]):
            logging.info(u'Validated Data Error')
            return

        if validate_amounts[0] != amounts[0]:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        confidencelevel, result_total_amount = self.__FixedAmountData__(amounts)

        logging.info(result_total_amount)

        if validate_amounts[0] == result_total_amount:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validate_amounts[0])
            logging.info(u'Fixed Falied!')


    def __AfterFixed__(self):
        logging.info(u'Error Count ' + str(self.__ErrorCount__) + u', Fixed Count ' + str(self.__FixedCount__))

        logging.info(u'\n<=================End Fixed Amount Data')


    def __ParseData__(self, jsondata):
        amounts = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions0'] == None:
            return amounts

        regions = jsondata[u'regions0']

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None:
                continue

            cls = region[u'cls']
            if cls == 21:
                for result in region[u'result']:
                    if len(result):
                        amounts.append(result)

                for result in region[u'ref_result']:
                    if len(result):
                        amounts.append(result)

        return amounts


    def __CheckData__(self, data):
        if not self.__CheckFloat__(data):
            return False

        dotindex = data.find(u'.')
        if (dotindex != -1 and (len(data) - dotindex) == 3):
            return True

        return False


    def __FixedAmountData__(self, amount_list):
        for total_amount_str in amount_list:
            amounts = re.split(self.__Regex__, total_amount_str)
            if not len(amounts):
                continue

            if len(amounts) == self.__AmountsCount__:
                try:
                    basic_amount = float(amounts[0]) * 100
                    cn_amount = float(amounts[1]) * 100
                    yq_amount = float(amounts[2]) * 100
                    total_amount = float(amounts[3]) * 100

                    if basic_amount + cn_amount +yq_amount == total_amount:
                        return ConfidenceLevel.Confident, amounts[3]
                except Exception:
                    pass

        return ConfidenceLevel.Bad, self.__FormatData__(amounts[len(amounts) - 1].strip(u' '))

    def __FormatData__(self, amountdata):
        length = len(amountdata)
        if length == 0:
            return ''

        if len(amountdata) > 3 and not amountdata[len(amountdata) - 3] in self.__Numbers__:
            amount_list = list(amountdata)
            amount_list[len(amount_list) - 3] = u'.'
            return u''.join(amount_list)

        dotindex = amountdata.find(u'.')
        if dotindex != -1:
            if (length - dotindex) > 2:
                return amountdata[0: dotindex + 3]
            elif (length - dotindex) == 2:
                return amountdata + u'0'
            elif (length - dotindex) == 1:
                return amountdata + u'00'
            return amountdata
        elif length > 5:
            # 先取后五位
            temp_amount = amountdata[::-1]
            len_of_0 = 0
            for ch in temp_amount[0: 5]:
                if ch == u'0':
                    len_of_0 += 1
                else:
                    break

            if len_of_0 >= 3:
                formatted_amount = list(amountdata)
                formatted_amount.insert(len(formatted_amount) - 2, u'.')
                return u''.join(formatted_amount)

            return amountdata + u'.00'
        else:
            return amountdata
