# -*- coding: utf-8 -*-

import logging
from utils import StrUtil
import re
from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel

class SellerCodeFixed(DataFixed):
    def __init__(self):
        DataFixed.__init__(self, 'SellerCode')
        self.__NumberCount__ = 18

    def __FixedData__(self, resultJson):
        logging.info("use fIRST")
        orient_codelist = self.__ParseData__(resultJson)
        if len(orient_codelist) == 0:
            logging.info(u'Code Data Error')
            return ConfidenceLevel.Bad, ''

        logging.info(orient_codelist[0] + u' Fixed To ')
        orient_codelist = self.__ReplaceSpecialCharacter__(orient_codelist)
        self.__NumberCount__ = self.__GuessCodeNumberCount__(orient_codelist)
        confidencelevel, code = self.__FixedCodeData__(orient_codelist)

        logging.info(code)

        return confidencelevel, code

    def __FixedDataWithValidate__(self, resultJson, validateJson):
        orient_codelist = self.__ParseData__(resultJson)
        validated_firstcodelist = self.__ParseData__(validateJson)

        if len(validated_firstcodelist) == 0 or len(orient_codelist) == 0 or not self.__CheckData__(
                validated_firstcodelist[0]):
            logging.info(u'Validated Data Error')
            return

        if validated_firstcodelist[0] != orient_codelist[0]:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(orient_codelist[0] + u' Fixed To ')

        orient_codelist = self.__ReplaceSpecialCharacter__(orient_codelist)
        self.__NumberCount__ = self.__GuessCodeNumberCount__(orient_codelist)
        confidencelevel, code = self.__FixedCodeData__(orient_codelist)


        logging.info(code)

        if validated_firstcodelist[0] == code:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_firstcodelist[0])
            logging.info(u'Fixed Falied!')
    #预处理json
    def __ParseData__(self, jsondata):
        codelist = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return codelist

        regions = jsondata[u'regions']
        regions = sorted(regions, key=lambda region: region[u'confidence'], reverse=True)

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None or region[u'ref_result'] == None:
                continue

            cls = region[u'cls']
            if cls == 28:
                for result in region[u'result']:
                    if len(result):
                        codelist.append(result)

                for result in region[u'ref_result']:
                    if len(result):
                        codelist.append(result)

        return codelist
    #替换所有税号中不存在的字符
    def __ReplaceSpecialCharacter__(self,orient_codelist):
        replaced_code = []
        for index in range(len(orient_codelist)):
            codeReplaced = re.sub(u'O|o', u'0', orient_codelist[index])
            codeReplaced = re.sub(u'S|s', u'5', codeReplaced)
            codeReplaced = re.sub(u'I', u'1', codeReplaced)
            codeReplaced = re.sub(u'Z|z', u'2', codeReplaced)
            codeReplaced = re.sub(u'vv|VV|Vv|vV', u'W', codeReplaced)
            replaced_code.append(codeReplaced)
        return replaced_code
    #税号字数
    def __GuessCodeNumberCount__(self, orient_codelist):
        numbercountmap = {15 : 0, 18 : 0}
        for code in orient_codelist:
            if numbercountmap.has_key(len(code)):
                numbercountmap[len(code)] += 1

        if numbercountmap[18] > numbercountmap[15]:
            return 18
        else:
            return 15

    def __CheckData__(self, data):
        if len(data) == 0 or len(data) != self.__NumberCount__:
            return False

        return self.__CheckNumber__(data)


    def __FixedCodeData__(self, firstdata):
        #firstdata = sorted(firstdata, key=lambda x: abs(len(x) - self.__NumberCount__))

        '''for first in firstdata:
            if self.__CheckData__(first):
                return ConfidenceLevel.Bad, first

        if len(firstdata):
            return ConfidenceLevel.Bad, firstdata[0]
        else:
            return ConfidenceLevel.Bad, '''
        #logging.info(firstdata)
        final_result = []
        if len(firstdata):
            for first in firstdata:
                if self.__CheckData__(first):
                    return ConfidenceLevel.Confident, first
                else:
                    final_result.append(first)
            return ConfidenceLevel.Bad, final_result[0]
        return ConfidenceLevel.Bad, ''


    #判断是否有非数字和字符
    def __CheckNumber__(self, data):
        return self.__CheckInPatterns__(data, self.__NumberPatterns__+self.__LetterPatterns__)