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
        self.__TaxRate__ = (Decimal(u'0.03'), Decimal(u'0.05'), Decimal(u'0.06'), Decimal(u'0.11'), Decimal(u'0.16'), Decimal(u'0.17'))
        self.__Error__ = np.arange(Decimal(u'-0.09'), Decimal(u'0.1'), Decimal(u'0.01'), Decimal)


    def __FixedData__(self, resultJson):
        result_before_tax, result_tax, result_after_tax = self.__ParseData__(resultJson)

        logging.info(result_before_tax + u', ' + result_tax + u', ' + result_after_tax + u' Fixed To ')

        confidencelevel, result_before_tax, result_tax, result_after_tax = self.__FixedAmountData__(result_before_tax, result_tax, result_after_tax)

        logging.info(result_before_tax + u', ' + result_tax + u', ' + result_after_tax)

        return confidencelevel, result_before_tax, result_tax, result_after_tax


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_before_tax, result_tax, result_after_tax = self.__ParseData__(resultJson)
        validated_before_tax, validated_tax, validated_after_tax = self.__ParseData__(validateJson)

        if not self.__CheckData__(validated_before_tax) or not self.__CheckData__(validated_tax) or not self.__CheckData__(validated_after_tax):
            logging.info(u'Validated Data Error')
            return

        if validated_before_tax != result_before_tax or validated_tax != result_tax or validated_after_tax != result_after_tax:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(result_before_tax + u', ' + result_tax + u', ' + result_after_tax + u' Fixed To ')

        confidencelevel, result_before_tax, result_tax, result_after_tax = self.__FixedAmountData__(result_before_tax, result_tax, result_after_tax)

        logging.info(result_before_tax + u', ' + result_tax + u', ' + result_after_tax)

        if validated_before_tax == result_before_tax and validated_tax == result_tax and validated_after_tax == result_after_tax:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_before_tax + u', ' + validated_tax + u', ' + validated_after_tax)
            logging.info(u'Fixed Falied!')


    def __ParseData__(self, jsondata):
        before_tax = ''
        tax = ''
        after_tax = ''

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return before_tax, tax, after_tax

        regions = jsondata[u'regions']

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None:
                continue

            cls = region[u'cls']
            if cls == 4:
                before_tax = region[u'result'][0]
            elif cls == 8:
                tax = region[u'result'][0]
            elif cls == 9:
                after_tax = region[u'result'][0]

        return before_tax, tax, after_tax


    def __CheckData__(self, data):
        if not self.__CheckFloat__(data):
            return False

        dotindex = data.find(u'.')
        if (dotindex != -1 and (len(data) - dotindex) == 3):
            return True

        return False


    def __FixedAmountData__(self, before_tax, tax, after_tax):
        if self.__CheckFloat__(before_tax) and self.__CheckFloat__(after_tax):
            d_before_tax = Decimal(before_tax)
            d_after_tax = Decimal(after_tax)
            if len(tax) == 0 and d_after_tax == d_before_tax:
                return ConfidenceLevel.Confident, self.__FormatData__(before_tax), '', self.__FormatData__(after_tax)
            elif self.__CheckFloat__(tax):
                d_tax = Decimal(tax)
                if d_after_tax == (d_before_tax + d_tax):
                    return ConfidenceLevel.Confident, self.__FormatData__(before_tax), self.__FormatData__(tax), self.__FormatData__(after_tax)

        if self.__CheckData__(after_tax):
            flag, before_tax, tax, after_tax = self.__FixedDataByAfterTax__(before_tax, tax, after_tax)
            if flag:
                return ConfidenceLevel.Fixed, before_tax, tax, after_tax

        if self.__CheckData__(tax):
            flag, before_tax, tax, after_tax = self.__FixedDataByTax__(before_tax, tax, after_tax)
            if flag:
                return ConfidenceLevel.Fixed, before_tax, tax, after_tax

        if self.__CheckData__(before_tax):
            flag, before_tax, tax, after_tax = self.__FixedDataByBeforeTax__(before_tax, tax, after_tax)
            if flag:
                return ConfidenceLevel.Fixed, before_tax, tax, after_tax

        return ConfidenceLevel.Bad, before_tax, tax, after_tax


    def __FixedDataByAfterTax__(self, before_tax, tax, after_tax):
        d_after_tax = Decimal(after_tax)
        similarity = Decimal(0.0)
        fixed_before_tax = ''
        fixed_tax = ''

        for rate in self.__TaxRate__:
            for error in self.__Error__:
                temp_before_tax = (d_after_tax / (Decimal(u'1.0') + rate)) + error
                temp_tax = self.__Round__(temp_before_tax * rate)
                if ((int(temp_before_tax * 100) % 100 + int(temp_tax * 100) % 100) % 100) == (int(d_after_tax * 100) % 100):
                    fixed = self.__DoubleToString__(temp_before_tax)
                    fixed_similarity = StrUtil.SimilarityRate(fixed, before_tax)
                    if not (fixed_similarity < similarity):
                        fixed_before_tax = fixed
                        fixed_tax = self.__DoubleToString__(temp_tax)
                        if tax == fixed_tax:
                            before_tax = fixed_before_tax
                            return True, before_tax, tax, after_tax
                        similarity = fixed_similarity
        
        if self.__CheckBeforeTaxFixedResult__(before_tax, fixed_before_tax, similarity):
            return True, self.__FormatData__(fixed_before_tax), self.__FormatData__(fixed_tax), self.__FormatData__(after_tax)

        return False, before_tax, tax, after_tax


    def __FixedDataByTax__(self, before_tax, tax, after_tax):
        d_tax = Decimal(tax)

        if self.__CheckFloat__(before_tax):
            d_before_tax = Decimal(before_tax)
            valiedTax = False
            for rate in self.__TaxRate__:
                temp_tax = self.__DoubleToString__(self.__Round__(d_before_tax * rate))
                if temp_tax == tax:
                    valiedTax = True

            if valiedTax:
                fixed_after_tax = d_before_tax + d_tax
                fixed = self.__DoubleToString__(fixed_after_tax)
                if StrUtil.SimilarityRate(fixed, after_tax) > 0.7:
                    return True, self.__FormatData__(before_tax), self.__FormatData__(tax), self.__FormatData__(fixed)

        similarity = Decimal(0.0)
        fixed_before_tax = u''
        fixed_after_tax = u''
        for rate in self.__TaxRate__:
            for error in self.__Error__:
                temp_before_tax = (d_tax / rate) + error
                fixed = self.__DoubleToString__(temp_before_tax)
                fixed_similarity = StrUtil.SimilarityRate(fixed, before_tax)
                if fixed_similarity > similarity:
                    fixed_before_tax = fixed
                    fixed_after_tax = self.__DoubleToString__(temp_before_tax + d_tax)
                    similarity = fixed_similarity

        if self.__CheckBeforeTaxFixedResult__(before_tax, fixed_before_tax, similarity):
            return True, self.__FormatData__(fixed_before_tax), self.__FormatData__(tax), self.__FormatData__(fixed_after_tax)

        return False, before_tax, tax, after_tax


    def __FixedDataByBeforeTax__(self, before_tax, tax, after_tax):
        d_before_tax = Decimal(before_tax)
        similarity = Decimal(0.0)
        fixed_tax = Decimal(0.0)

        for rate in self.__TaxRate__:
            temp_tax = self.__Round__(d_before_tax * rate)
            fixed = self.__DoubleToString__(d_before_tax + temp_tax)
            fixed_similarity = StrUtil.SimilarityRate(fixed, after_tax)
            if fixed_similarity > similarity:
                fixed_tax = temp_tax
                similarity = fixed_similarity

        return True, self.__FormatData__(before_tax), self.__FormatData__(self.__DoubleToString__(fixed_tax)), self.__FormatData__(self.__DoubleToString__(d_before_tax + fixed_tax))


    def __CheckBeforeTaxFixedResult__(self, before_tax, fixed_before_tax, similarity):
        length = len(fixed_before_tax)

        if length and similarity > 0.49999:
            if length == len(before_tax):
                patterns = ( ( u'8', u'6', u'0' ) , ( u'1', u'4', u'7' ), ( u'3', u'8' ), ( u'3', u'7' ), ( u'0', u'4' ) )
                for index in range(length):
                    if fixed_before_tax[index] != before_tax[index]:
                        match = False
                        for pattern in patterns:
                            if fixed_before_tax[index] in pattern and before_tax[index] in pattern:
                                match = True
                                break
                        if not match:
                            return False

            else:
                if similarity < 0.6 and max(length, len(before_tax) * similarity) > 3.0:
                    return False

            return len(before_tax) == len(fixed_before_tax) or StrUtil.Similarity(before_tax, fixed_before_tax) == 1

        return False


    def __FormatData__(self, amountdata):
        length = len(amountdata)
        if length == 0:
            return ''

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


    def __Round__(self, value):
        ivalue = int(value * 1000)

        if ivalue % 10 > 4:
            return value + Decimal(0.01)

        return value


    def __DoubleToString__(self, invalue):
        value = str(int(invalue * 100))
        length = len(value)

        if (length >= 3):
            return value[0:length - 2] + u'.' + value[length - 2:length]
        else:
            value = str(invalue)
            dotindex = value.find(u'.')
            if dotindex != -1:
                return value[0: dotindex + 3]

        return value