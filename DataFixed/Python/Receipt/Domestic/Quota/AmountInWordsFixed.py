#!/usr/bin/env python2
# -*- coding:utf-8 -*-

import re
import logging

from ConfidenceLevel import ConfidenceLevel
from DataFixed import DataFixed

class AmountInWordsFixed(DataFixed):
    def __init__(self):
        DataFixed.__init__(self, u'AmountInWords')
        self.__WordsPatterns__ = (u'壹', u'贰', u'叁', u'肆', u'伍', u'陆', u'柒', u'捌', u'玖', u'零', u'拾', u'佰', u'仟', u'万', u'亿')
        self.__Suffix__ = (u'整', u'元', u'圆')


    def __FixedData__(self, resultJson):
        amount_list = self.__ParseData__(resultJson)
        if len(amount_list) == 0:
            logging.info(u'Amount Data Error')
            return ConfidenceLevel.Bad, ''

        logging.info(amount_list[0] + u' Fixed To ')

        confidencelevel, code = self.__FixAmount__(amount_list)

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
        
        confidencelevel, code = self.__FixAmount__(result_codelist)

        logging.info(code)

        if validated_codelist[0] == code:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_codelist[0])
            logging.info(u'Fixed Falied!')


    def __CheckData__(self, amount):
        for ch in amount:
            if not ch in self.__WordsPatterns__ + self.__Suffix__:
                return False

            if ch in self.__Suffix__ and amount.find(ch) != len(amount) - 1:
                if ch == self.__Suffix__[0]:
                    return False

                if ch != self.__Suffix__[0] and amount[-1] != self.__Suffix__[0]:
                    return False

        return True


    def __ParseData__(self, jsondata):
        amounts = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return amounts

        regions = jsondata[u'regions']
        regions = sorted(regions, key=lambda region: region[u'confidence'], reverse = True)

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None or region[u'ref_result'] == None:
                continue

            cls = region[u'cls']
            if cls == 29:
                for result in region[u'result']:
                    if len(result):
                        amounts.append(self.__TripWhiteSpace__(result))

                for result in region[u'ref_result']:
                    if len(result):
                        amounts.append(self.__TripWhiteSpace__(result))

        return amounts


    def __FixAmount__(self, amounts):
        for amount in amounts:
            if self.__CheckData__(amount):
                return ConfidenceLevel.Confident, amount

        return ConfidenceLevel.Bad, self.__RemoveSpecialChar__(amounts[0])


    def __TripWhiteSpace__(self, amount):
        return amount.replace(u' ', u'')


    def __RemoveSpecialChar__(self, amount):
        removed_amount = u''
        for ch in amount:
            if ch in self.__WordsPatterns__ + self.__Suffix__:
                removed_amount += ch

        return removed_amount