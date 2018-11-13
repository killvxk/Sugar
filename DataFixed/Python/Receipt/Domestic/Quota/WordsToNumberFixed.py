#!/usr/bin/env python2
# -*- coding:utf-8 -*-

import re
import logging

from ConfidenceLevel import ConfidenceLevel
from DataFixed import DataFixed
from Domestic.Quota.AmountInWordsFixed import AmountInWordsFixed as Quota_AmountInWordsFixed


class WordsToNumberFixed(DataFixed):
    def __init__(self, resultJson):
        self.data = resultJson
        DataFixed.__init__(self, u'WordsToNumberFixed')
        self.__WordsPatterns__ = (
        u'壹', u'贰', u'叁', u'肆', u'伍', u'陆', u'柒', u'捌', u'玖', u'零', u'拾', u'佰', u'仟', u'万', u'亿')
        self.__Suffix__ = (u'整', u'元', u'圆')
        self.__Num__ = {u'壹': 1, u'贰': 2, u'叁': 3,
                          u'肆': 4, u'伍': 5, u'陆': 6,
                          u'柒': 7, u'捌': 8, u'玖': 9,
                          u'零': 0}
        self.__Sit__ = {u'拾': 10, u'佰': 100, u'仟': 1000 }
        self.__Large__ = {u'万': 10000, u'亿': 100000000}

    def __FixedData__(self, resultJson):
        amount_confidencelevel, amount = self.__getWords__(resultJson)
        if not amount:
            logging.info(u'Amount Data Error')
            return ConfidenceLevel.Bad, ''

        logging.info(amount + u' Transfor To ')
        confidencelevel, code = self.__trans__(amount_confidencelevel, amount)

        logging.info(str(code))
        self.data[u'fixedamount_number'] = str(code)

        return confidencelevel, str(code)


    def __getWords__(self,receiptData):
        dataFixed = Quota_AmountInWordsFixed()
        amount_confidencelevel, amount = dataFixed.StartFixedFromJson(receiptData)

        return amount_confidencelevel, amount

    def __trans__(self, amount_confidencelevel, amout_words):
        number = 0
        current_number = 1
        if amout_words[0] not in self.__Num__ and amout_words[0] != u'拾':
            return ConfidenceLevel.Bad, 0
        for index in range(len(amout_words)):
            if self.__Num__.has_key(amout_words[index]):
                current_number = self.__Num__.get(amout_words[index],0)
            elif self.__Sit__.has_key(amout_words[index]):
                current_number = current_number*self.__Sit__.get(amout_words[index])
                number += current_number
                current_number = 0
            elif self.__Large__.has_key(amout_words[index]):
                number = number*self.__Large__.get(amout_words[index])
            elif amout_words[index] in self.__Suffix__:
                number += current_number
                return amount_confidencelevel, number
            else:
                return ConfidenceLevel.Bad, 0
        number += current_number

        return amount_confidencelevel, number
