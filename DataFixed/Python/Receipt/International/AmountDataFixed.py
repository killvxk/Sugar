from decimal import Decimal
import numpy as np
import re
import logging

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel
from utils import StrUtil

class AmountDataFixed(DataFixed):
    """description of class"""
    def __init__(self):
        DataFixed.__init__(self, 'Amount')


    def __FixedData__(self, resultJson):
        result_subtotal, result_tip, result_tip_rate, result_total = self.__ParseData__(resultJson)

        logging.info(result_subtotal + u', ' + result_tip + u', ' + result_total + u' Fixed To ')

        result_tip_rate, tiprate_tip, tiprate_total = self.__FixedTipRate__(result_tip_rate)
        confidencelevel, result_subtotal, result_tip, result_total = self.__MildFixedAmountData__(result_subtotal, result_tip, result_total)
        if confidencelevel != ConfidenceLevel.Confident or confidencelevel != ConfidenceLevel.Fixed:
            confidencelevel, result_subtotal, result_tip, result_total = self.__FixedAmountData__(result_subtotal, result_tip, result_total, tiprate_tip, tiprate_total)

        logging.info(result_subtotal + u', ' + result_tip + u', ' + result_total)

        return confidencelevel, result_subtotal, result_tip, result_tip_rate, result_total


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_subtotal, result_tip, result_tip_rate, result_total = self.__ParseData__(resultJson)
        validated_subtotal, validated_tip, validated_tip_rate, validated_total = self.__ParseData__(validateJson)

        if not self.__CheckData__(validated_subtotal) or not self.__CheckData__(validated_tip) or not self.__CheckData__(validated_total):
            logging.info(u'Validated Data Error')
            return

        if validated_subtotal != result_subtotal or validated_tip != result_tip or validated_total != result_total:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(result_subtotal + u', ' + result_tip + u', ' + result_total + u' Fixed To ')

        result_tip_rate, tiprate_tip, tiprate_total = self.__FixedTipRate__(result_tip_rate)
        confidencelevel, result_subtotal, result_tip, result_total = self.__MildFixedAmountData__(result_subtotal, result_tip, result_total)
        if confidencelevel != ConfidenceLevel.Confident or confidencelevel != ConfidenceLevel.Fixed:
            confidencelevel, result_subtotal, result_tip, result_total = self.__FixedAmountData__(result_subtotal, result_tip, result_total, tiprate_tip, tiprate_total)

        logging.info(result_subtotal + u', ' + result_tip + u', ' + result_total)

        if validated_subtotal == result_subtotal and validated_tip == result_tip and validated_total == result_total:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_subtotal + u', ' + validated_tip + u', ' + validated_total)
            logging.info(u'Fixed Falied!')


    def __ParseData__(self, jsondata):
        subtotal = ''
        tip = ''
        tip_rate = ''
        confidence = 0.0
        total = ''

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return subtotal, tip, tip_rate, total

        regions = jsondata[u'regions']

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None:
                continue

            cls = region[u'cls']
            if cls == 3:
                subtotal = region[u'result'][0]
            elif cls == 5:
                tip = region[u'result'][0]
            elif cls == 6:
                total = region[u'result'][0]
            elif cls == 11 and float(region['confidence']) > confidence:
                tip_rate = region[u'result'][0]
                confidence = float(region['confidence'])

        return subtotal, tip, tip_rate, total


    def __CheckData__(self, data):
        if not self.__CheckFloat__(data):
            return False

        dotindex = data.find(u'.')
        if dotindex != -1 and (len(data) - dotindex) == 3:
            return True

        return False


    def __FixedTipRate__(self, tip_rate):
        if len(tip_rate) == 0:
            return u'', u'', u''

        tokens = []
        for token in re.split(u'%|\\$|,|:|-| ', tip_rate):
            if self.__CheckFloat__(token):
                tokens.append(token)

        if len(tokens) == 3:
            rate = int(tokens[0])
            if rate > 100:
                tokens[0] = str(rate % 100)

            return tokens[0] + u'% tip = $' + tokens[1] + u', Total = $' + tokens[2], tokens[1], tokens[2]

        return tip_rate, u'', u''


    def __MildFixedAmountData__(self, subtotal, tip, total):
        subtotal = self.__NormalizeData__(subtotal)
        tip = self.__NormalizeData__(tip)
        total = self.__NormalizeData__(total)

        if self.__CheckFloat__(subtotal) and self.__CheckFloat__(total):
            d_subtotal = Decimal(subtotal)
            d_total = Decimal(total)
            if self.__CheckFloat__(tip):
                d_tip = Decimal(tip)
                if d_total == (d_subtotal + d_tip):
                    return ConfidenceLevel.Confident, self.__FormatData__(subtotal), self.__FormatData__(tip), self.__FormatData__(total)

                if self.__MissDot__(total):
                    d_temp = d_subtotal + d_tip
                    fixed = str(d_temp)
                    if StrUtil.Similarity(fixed, total) == 1 and len(fixed) - len(total) == 1:
                        if (d_temp == (d_subtotal + d_tip)):
                            return ConfidenceLevel.Confident, self.__FormatData__(subtotal), self.__FormatData__(tip), self.__FormatData__(fixed)

                if self.__MissDot__(tip) or self.__MissInteger__(tip):
                    d_temp = d_total - d_subtotal
                    fixed = str(d_temp)
                    if StrUtil.Similarity(fixed, tip) == 1 and len(fixed) - len(tip) == 1:
                        if (d_total == (d_subtotal + d_temp)):
                            return ConfidenceLevel.Confident, self.__FormatData__(subtotal), self.__FormatData__(fixed), self.__FormatData__(total)

            elif len(tip) == 0:
                if d_subtotal == d_total:
                    return ConfidenceLevel.Confident, self.__FormatData__(subtotal), tip, self.__FormatData__(total)
                elif self.__MissDot__(total) and d_total > d_subtotal:
                    totallen = len(total)
                    if totallen > 1:
                        pos = 1
                        while d_subtotal * 2 < d_total:
                            d_total = Decimal(total[0 : totallen - pos] + '.' + total[totallen - pos:])
                            pos += 1
                        if d_subtotal < d_total:
                            return ConfidenceLevel.Fixed, self.__FormatData__(subtotal), tip, self.__FormatData__(str(d_total))

            else:
                d_temp = d_total - d_subtotal
                fixed = str(d_temp)
                if StrUtil.Similarity(fixed, tip) == 1:
                    return ConfidenceLevel.Confident, self.__FormatData__(subtotal), self.__FormatData__(fixed), self.__FormatData__(total)

        return ConfidenceLevel.Bad, self.__FormatData__(subtotal), self.__FormatData__(tip), self.__FormatData__(total)


    def __FixedAmountData__(self, subtotal, tip, total, tiprate_tip, tiprate_total):
        confidencelevel, result_subtotal, result_tip, result_total = self.__FixedDataByTipRate__(subtotal, tip, total, tiprate_tip, tiprate_total)
        if confidencelevel == ConfidenceLevel.Confident or confidencelevel == ConfidenceLevel.Fixed:
            if len(tip) == 0:
                result_tip = u''
            if len(total) == 0:
                result_total = u''
            return confidencelevel, result_subtotal, result_tip, result_total

        if self.__CheckData__(subtotal):
            confidencelevel, result_subtotal, result_tip, result_total = self.__FixedDataBySubtotal__(subtotal, tip, total)
            if confidencelevel == ConfidenceLevel.Confident or confidencelevel == ConfidenceLevel.Fixed:
                if len(tip) == 0:
                    result_tip = u''
                if len(total) == 0:
                    result_total = u''
                return confidencelevel, result_subtotal, result_tip, result_total

        if self.__CheckData__(total):
            confidencelevel, result_subtotal, result_tip, result_total = self.__FixedDataByTotal__(subtotal, tip, total)
            if confidencelevel == ConfidenceLevel.Confident or confidencelevel == ConfidenceLevel.Fixed:
                if len(tip) == 0:
                    result_tip = u''
                if len(total) == 0:
                    result_total = u''
                return confidencelevel, result_subtotal, result_tip, result_total

        return ConfidenceLevel.Bad, subtotal, tip, total
    
    def __FixedDataByTipRate__(self, subtotal, tip, total, tiprate_tip, tiprate_total):
        if self.__CheckFloat__(tiprate_tip) and self.__CheckFloat__(tiprate_total):
            d_tiprate_total = Decimal(tiprate_total)
            d_tiprate_tip = Decimal(tiprate_tip)
            if self.__CheckFloat__(subtotal):
                d_subtotal = Decimal(subtotal)
                if (d_tiprate_total == (d_subtotal + d_tiprate_tip)):
                    if self.__CheckFloat__(total):
                        d_total = Decimal(total)
                        if d_tiprate_total == d_total:
                            return ConfidenceLevel.Confident, self.__FormatData__(subtotal), self.__FormatData__(tiprate_tip), self.__FormatData__(total)
                        else:
                            if abs(d_tiprate_total - d_total) > min(d_tiprate_total, d_total):
                                d_total = d_tiprate_total
                            else:
                                d_total = min(d_tiprate_total, d_total)
                            if self.__CheckFloat__(tip):
                                d_tip = Decimal(tip)
                                if d_tip == d_tiprate_tip:
                                    return ConfidenceLevel.Confident, self.__FormatData__(subtotal), self.__FormatData__(tiprate_tip), self.__FormatData__(tiprate_total)
                                else:
                                    return ConfidenceLevel.Fixed, self.__FormatData__(subtotal), self.__FormatData__(str(d_tiprate_total - d_subtotal)), self.__FormatData__(tiprate_total)
                            elif len(tip) == 0:
                                return ConfidenceLevel.Fixed, self.__FormatData__(subtotal), tip, self.__FormatData__(str(d_total))
            else:
                d_subtotal = d_tiprate_total - d_tiprate_tip
                fixed = str(d_subtotal)
                if not self.__CheckFloat__(subtotal) or StrUtil.SimilarityRate(fixed, subtotal) > 0.7:
                    return ConfidenceLevel.Fixed, self.__FormatData__(fixed), self.__FormatData__(tiprate_tip), self.__FormatData__(tiprate_total)

        return ConfidenceLevel.Bad, u'', u'', u''


    def __FixedDataBySubtotal__(self, subtotal, tip, total):
        d_subtotal = Decimal(subtotal)
        tipTooLarge = False

        if self.__CheckFloat__(tip):
            d_tip = Decimal(tip)
            tipTooLarge = d_tip > (d_subtotal * Decimal(u'0.4'))
            if not tipTooLarge:
                d_total = d_subtotal + d_tip
                fixed = str(d_total)
                if d_total >= d_subtotal and (len(total) == 0 or not self.__CheckFloat__(total) or StrUtil.SimilarityRate(fixed, total) > 0.5):
                    if self.__CheckFloat__(total):
                        diff = d_total - Decimal(total)
                        if diff < Decimal('1.0') and diff > Decimal('0.0'):
                            fixed = total
                    return ConfidenceLevel.Fixed, self.__FormatData__(subtotal), self.__FormatData__(tip), self.__FormatData__(fixed)

        if self.__CheckFloat__(total):
            d_total = Decimal(total)
            d_tip = d_total - d_subtotal
            fixed = str(d_tip)
            if d_tip >= Decimal(u'0.00') and (len(tip) == 0 or not self.__CheckFloat__(tip) or tipTooLarge or StrUtil.SimilarityRate(fixed, tip) > 0.5):
                return ConfidenceLevel.Fixed, self.__FormatData__(subtotal), self.__FormatData__(fixed), self.__FormatData__(total)
            elif d_tip < Decimal(u'0.00'):
                fixed = subtotal;
                if StrUtil.SimilarityRate(fixed, total) > 0.6:
                    return ConfidenceLevel.Fixed, self.__FormatData__(subtotal), self.__FormatData__(u'0.00'), self.__FormatData__(fixed)

        return ConfidenceLevel.Bad, u'', u'', u''


    def __FixedDataByTotal__(self, subtotal, tip, total):
        d_total = Decimal(total)
        tipTooLarge = False

        if self.__CheckFloat__(tip):
            d_tip = Decimal(tip)
            tipTooLarge = d_tip > (d_total * Decimal(u'0.4'))
            if not tipTooLarge:
                d_subtotal = d_total - d_tip
                fixed = str(d_subtotal)
                if len(subtotal) == 0 or not self.__CheckFloat__(subtotal) or StrUtil.SimilarityRate(fixed, subtotal) > 0.5:
                    return ConfidenceLevel.Fixed, self.__FormatData__(fixed), self.__FormatData__(tip), self.__FormatData__(total)

        return ConfidenceLevel.Bad, u'', u'', u''


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


    def __MissDot__(self, amountdata):
        dotindex = amountdata.find(u'.')
        return dotindex == -1
    

    def __MissInteger__(self, amountdata):
        dotindex = amountdata.find(u'.')
        return dotindex == 0

