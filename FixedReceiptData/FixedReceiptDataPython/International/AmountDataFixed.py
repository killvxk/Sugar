from decimal import Decimal
import numpy as np
import re

from DataFixed import DataFixed
from DataFixed import ConfidenceLevel
import Utils

class AmountDataFixed(DataFixed):
    """description of class"""
    def __init__(self):
        DataFixed.__init__(self)
        self.__ErrorCount__ = 0
        self.__FixedCount__ = 0
        self.__NumberPatterns__ = ('0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.' )


    def __BeforeFixed__(self):
        print('Start Fixed Amount Data=================>\n')


    def __FixedData__(self, resultJson):
        result_subtotal, result_tip, result_tip_rate, result_total = self.__ParseData__(resultJson)

        print(result_subtotal + ', ' + result_tip + ', ' + result_total + ' Fixed To ')

        result_tip_rate = self.__FixedTipRate__(result_tip_rate)
        confidencelevel, result_subtotal, result_tip, result_total = self.__FixedAmountData__(result_before_tax, result_tax, result_after_tax)

        print(result_subtotal + ', ' + result_tip + ', ' + result_total)

        return confidencelevel, result_subtotal, result_tip, result_tip_rate, result_total


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_subtotal, result_tip, result_tip_rate, result_total = self.__ParseData__(resultJson)
        validated_subtotal, validated_tip, validated_tip_rate, validated_total = self.__ParseData__(validateJson)

        if not self.__CheckData__(validated_subtotal) or not self.__CheckData__(validated_tip) or not self.__CheckData__(validated_total):
            print('Validated Data Error')
            return

        if validated_subtotal != result_subtotal or validated_tip != result_tip or validated_total != result_total:
            self.__ErrorCount__ += 1
        else:
            print('Validated Equal To Result')
            return

        print('Validated Not Equal To Result')
        print(result_subtotal + ', ' + result_tip + ', ' + result_total + ' Fixed To ')

        result_tip_rate = self.__FixedTipRate__(result_tip_rate)
        confidencelevel, result_subtotal, result_tip, result_total = self.__FixedAmountData__(result_subtotal, result_tip, result_total)

        print(result_subtotal + ', ' + result_tip + ', ' + result_total)

        if validated_subtotal == result_subtotal and validated_tip == result_tip and validated_total == result_total:
            self.__FixedCount__ += 1
            print('Fixed Success!')
        else:
            print('Validated ' + validated_subtotal + ', ' + validated_tip + ', ' + validated_total)
            print('Fixed Falied!')


    def __AfterFixed__(self):
        print('Error Count ' + str(self.__ErrorCount__) + ', Fixed Count ' + str(self.__FixedCount__))

        print('\n<=================End Fixed Amount Data')


    def __ParseData__(self, jsondata):
        subtotal = ''
        tip = ''
        tip_rate = ''
        total = ''

        if jsondata == None or not isinstance(jsondata, dict) or jsondata['regions'] == None:
            return subtotal, tip, tip_rate, total

        regions = jsondata['regions']

        for region in regions:
            if region['cls'] == None or region['result'] == None:
                continue

            cls = region['cls']
            if cls == 3:
                subtotal = region['result'][0]
            elif cls == 5:
                tip = region['result'][0]
            elif cls == 6:
                total = region['result'][0]
            elif cls == 11:
                tip_rate = region['result'][0]

        return subtotal, tip, tip_rate, total


    def __CheckNumber__(self, data):
        if len(data) == 0:
            return False

        for ch in data:
            if ch not in self.__NumberPatterns__:
                return False

        return data.count('.') <= 1


    def __CheckData__(self, data):
        if not self.__CheckNumber__(data):
            return False

        dotindex = data.find('.')
        if dotindex != -1 and (len(data) - dotindex) == 3:
            return True

        return False


    def __FixedAmountData__(self, subtotal, tip, total):
        subtotal = self.__NormalizeData__(subtotal)
        tip = self.__NormalizeData__(tip)
        total = self.__NormalizeData__(total)

        if self.__CheckNumber__(subtotal) and self.__CheckNumber__(total):
            d_subtotal = Decimal(subtotal)
            d_total = Decimal(total)
            if self.__CheckNumber__(tip):
                d_tip = Decimal(tip)
                if (d_total == (d_subtotal + d_tip)):
                    return ConfidenceLevel.Confident, self.__FormatData__(subtotal), self.__FormatData__(tip), self.__FormatData__(total)

                if self.__MissDot__(total):
                    d_temp = d_subtotal + d_tip
                    fixed = str(d_temp)
                    if Utils.Similarity(fixed, total) == 1 and len(fixed) - len(total) == 1:
                        if (d_temp == (d_subtotal + d_tip)):
                            return ConfidenceLevel.Fixed, self.__FormatData__(subtotal), self.__FormatData__(tip), self.__FormatData__(fixed)

                if self.__MissDot__(tip) or self.__MissInteger__(tip):
                    d_temp = d_total - d_subtotal
                    fixed = str(d_temp)
                    if Utils.Similarity(fixed, tip) == 1 and len(fixed) - len(tip) == 1:
                        if (d_total == (d_subtotal + d_temp)):
                            return ConfidenceLevel.Fixed, self.__FormatData__(subtotal), self.__FormatData__(fixed), self.__FormatData__(total)

            elif len(tip) == 0:
                if d_subtotal == d_total:
                    return ConfidenceLevel.Confident, self.__FormatData__(subtotal), tip, self.__FormatData__(total)

            else:
                d_temp = d_total - d_subtotal
                fixed = str(d_temp)
                if Utils.Similarity(fixed, tip) == 1:
                    return ConfidenceLevel.Fixed, self.__FormatData__(subtotal), self.__FormatData__(fixed), self.__FormatData__(total)

        return ConfidenceLevel.Bad, self.__FormatData__(subtotal), self.__FormatData__(tip), self.__FormatData__(total)


    def __FixedTipRate__(self, tip_rate):
        if len(tip_rate) == 0:
            return ''

        tokens = []
        for token in re.split('%|\\$|,|:|-', tip_rate):
            if self.__CheckNumber__(token):
                tokens.append(token)

        if len(tokens) == 3:
            rate = int(tokens[0])
            if rate > 100:
                tokens[0] = str(rate % 100)

            return tokens[0] + '% tip = $' + tokens[1] + ', Total = $' + tokens[2]

        return ''


    def __NormalizeData__(self, amountdata):
        length = len(amountdata)
        if length == 0:
            return amountdata

        amountdata = re.sub('\\$| |%|\\\\', '', amountdata)

        length = len(amountdata)
        dotindex = amountdata.find('.')
        if dotindex != -1:
            if (length - dotindex) > 2:
                return amountdata[0: dotindex + 3]

        return amountdata 


    def __FormatData__(self, amountdata):
        length = len(amountdata)
        if length == 0:
            return ''

        while len(amountdata) > 1 and amountdata[0] == '0' and amountdata[1] == '0':
            amountdata = amountdata[1:]

        dotindex = amountdata.find('.')
        if dotindex != -1:
            if (length - dotindex) > 2:
                return amountdata[0: dotindex + 3]
            elif (length - dotindex) == 2:
                return amountdata + '0'
            elif (length - dotindex) == 1:
                return amountdata + '00'
        else:
            return amountdata + '.00'


    def __MissDot__(self, amountdata):
        dotindex = amountdata.find('.')
        return dotindex == -1
    
    def __MissInteger__(self, amountdata):
        dotindex = amountdata.find('.')
        return dotindex == 0

