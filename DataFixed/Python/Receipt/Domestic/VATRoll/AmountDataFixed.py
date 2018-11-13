# -*- coding: utf-8 -*-

from decimal import Decimal
import numpy as np
import logging

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel
from utils import StrUtil

class AmountDataFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self, 'Amount')
        self.__Numbers__ = (u'0', u'1', u'2', u'3', u'4', u'5', u'6', u'7', u'8', u'9', u'.', u'0', u',', u'，')


    def __FixedData__(self, resultJson):
        result_total_amount = self.__ParseData__(resultJson)

        logging.info(result_total_amount + u' Fixed To ')

        confidencelevel, result_total_amount = self.__FixedAmountData__(result_total_amount)

        logging.info(result_total_amount)

        return confidencelevel, result_total_amount


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_total_amount = self.__ParseData__(resultJson)
        validated_total_amount = self.__ParseData__(validateJson)

        if not self.__CheckData__(validated_total_amount):
            logging.info(u'Validated Data Error')
            return

        if validated_total_amount != result_total_amount:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(result_total_amount + u' Fixed To ')

        confidencelevel, result_total_amount = self.__FixedAmountData__(result_total_amount)

        logging.info(result_total_amount)

        if validated_total_amount == result_total_amount:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_total_amount)
            logging.info(u'Fixed Falied!')


    def __ParseData__(self, jsondata):
        total_amount = u''

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return total_amount

        regions = jsondata[u'regions']

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None:
                continue

            cls = region[u'cls']
            if cls == 9:
                total_amount = region[u'result'][0]

        number_list = list(total_amount)
        total_amount = u''
        for number in number_list:
            if number in self.__Numbers__:
                total_amount += number

        return total_amount


    def __CheckData__(self, data):
        if not self.__CheckFloat__(data):
            return False

        dotindex = data.find(u'.')
        if (dotindex != -1 and (len(data) - dotindex) == 3):
            return True

        return False


    def __FixedAmountData__(self, total_amount):
        return ConfidenceLevel.Bad, self.__FormatData__(total_amount)


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
                logging.info(u'formatted_amount: {}'.format(formatted_amount))
                return u''.join(formatted_amount)

            return amountdata + u'.00'
        else:
            return amountdata
