from decimal import Decimal
import numpy as np
from DataFixed import DataFixed
import Utils

class AmountDataFixed(DataFixed):
    """description of class"""
    def __init__(self, validatedPath, resultPath):
        DataFixed.__init__(self, validatedPath, resultPath)
        self.__ErrorCount__ = 0
        self.__FixedCount__ = 0
        self.__TaxRate__ = (Decimal('0.03'), Decimal('0.05'), Decimal('0.06'), Decimal('0.11'), Decimal('0.17'))
        self.__Error__ = np.arange(Decimal('-0.09'), Decimal('0.1'), Decimal('0.01'), Decimal)
        self.__numberPatterns__ = ('0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.' )


    def __BeforeFixed__(self):
        print('Start Fixed Amount Data=================>\n')


    def __FixedData__(self, validateJson, resultJson):
        validated_before_tax, validated_tax, validated_after_tax = self.__ParseData__(validateJson)
        result_before_tax, result_tax, result_after_tax = self.__ParseData__(resultJson)

        if not self.__CheckData__(validated_before_tax) or not self.__CheckData__(validated_tax) or not self.__CheckData__(validated_after_tax):
            print(' Data Error')
            return

        if validated_before_tax != result_before_tax or validated_tax != result_tax or validated_after_tax != result_after_tax:
            self.__ErrorCount__ += 1;
        else:
            print('Validated Equal To Result')
            return

        print('Validated Not Equal To Result')
        print(result_before_tax + ', ' + result_tax + ', ' + result_after_tax + ' Fixed To ')

        result_before_tax, result_tax, result_after_tax = self.__FixedAmountData__(result_before_tax, result_tax, result_after_tax)

        print(result_before_tax + ', ' + result_tax + ', ' + result_after_tax)

        if validated_before_tax == result_before_tax and validated_tax == result_tax and validated_after_tax == result_after_tax:
            self.__FixedCount__ += 1
            print('Fixed Success!')
        else:
            print('Validated ' + validated_before_tax + ', ' + validated_tax + ', ' + validated_after_tax)
            print('Fixed Falied!')

    def __AfterFixed__(self):
        print('Error Count ' + str(self.__ErrorCount__) + ', Fixed Count ' + str(self.__FixedCount__))

        print('\n<=================End Fixed Amount Data')

    def __ParseData__(self, jsondata):
        before_tax = ''
        tax = ''
        after_tax = ''

        if jsondata == None or not isinstance(jsondata, dict) or jsondata['regions'] == None:
            return (before_tax, tax, after_tax)

        regions = jsondata['regions']

        for region in regions:
            if region['cls'] == None or region['result'] == None:
                continue

            cls = region['cls']
            if cls == 4:
                before_tax = region['result'][0]
            elif cls == 8:
                tax = region['result'][0]
            elif cls == 9:
                after_tax = region['result'][0]

        return (before_tax, tax, after_tax)


    def __CheckNumber__(self, data):
        if len(data) == 0:
            return False

        for ch in data:
            if ch not in self.__numberPatterns__:
                return False

        return data.count('.') == 1


    def __CheckData__(self, data):
        if not self.__CheckNumber__(data):
            return False

        dotindex = data.find('.')
        if (dotindex != -1 and (len(data) - dotindex) == 3):
            return True

        return False


    def __FixedAmountData__(self, before_tax, tax, after_tax):
        if self.__CheckNumber__(before_tax) and self.__CheckNumber__(tax) and self.__CheckNumber__(after_tax):
            d_before_tax = Decimal(before_tax)
            d_tax = Decimal(tax)
            d_after_tax = Decimal(after_tax)
            if (d_after_tax == (d_before_tax + d_tax)):
                return self.__FormatData__(before_tax), self.__FormatData__(tax), self.__FormatData__(after_tax)

        if self.__CheckData__(after_tax):
            flag, before_tax, tax, after_tax = self.__FixedDataByAfterTax__(before_tax, tax, after_tax)
            if flag:
                return before_tax, tax, after_tax

        if self.__CheckData__(tax):
            flag, before_tax, tax, after_tax = self.__FixedDataByTax__(before_tax, tax, after_tax)
            if flag:
                return before_tax, tax, after_tax

        if self.__CheckData__(before_tax):
            flag, before_tax, tax, after_tax = self.__FixedDataByBeforeTax__(before_tax, tax, after_tax)
            if flag:
                return before_tax, tax, after_tax

        return before_tax, tax, after_tax


    def __FixedDataByAfterTax__(self, before_tax, tax, after_tax):
        d_after_tax = Decimal(after_tax)
        similarity = Decimal(0.0)
        fixed_before_tax = ''
        fixed_tax = ''

        for rate in self.__TaxRate__:
            for error in self.__Error__:
                temp_before_tax = (d_after_tax / (Decimal('1.0') + rate)) + error
                temp_tax = self.__Round__(temp_before_tax * rate)
                if ((int(temp_before_tax * 100) % 100 + int(temp_tax * 100) % 100) % 100) == (int(d_after_tax * 100) % 100):
                    fixed = self.__DoubleToString__(temp_before_tax)
                    fixed_similarity = Utils.SimilarityRate(fixed, before_tax)
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

        if self.__CheckNumber__(before_tax):
            d_before_tax = Decimal(before_tax)
            fixed_after_tax = d_before_tax + d_tax
            fixed = self.__DoubleToString__(fixed_after_tax)
            if Utils.SimilarityRate(fixed, after_tax) > 0.7:
                return True, self.__FormatData__(before_tax), self.__FormatData__(tax), self.__FormatData__(fixed)

        similarity = Decimal(0.0)
        fixed_before_tax = ''
        fixed_after_tax = ''
        for rate in self.__TaxRate__:
            for error in self.__Error__:
                temp_before_tax = (d_tax / rate) + error
                fixed = self.__DoubleToString__(temp_before_tax)
                fixed_similarity = Utils.SimilarityRate(fixed, before_tax)
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
        fixed_after_tax = Decimal(0.0)

        for rate in self.__TaxRate__:
            temp_tax = self.__Round__(d_before_tax * rate)
            fixed = self.__DoubleToString__(d_before_tax + temp_tax)
            fixed_similarity = Utils.SimilarityRate(fixed, after_tax)
            if fixed_similarity > similarity:
                fixed_tax = temp_tax
                similarity = fixed_similarity

        return True, self.__FormatData__(before_tax), self.__FormatData__(self.__DoubleToString__(fixed_tax)), self.__FormatData__(self.__DoubleToString__(d_before_tax + fixed_tax))


    def __CheckBeforeTaxFixedResult__(self, before_tax, fixed_before_tax, similarity):
        length = len(fixed_before_tax)

        if length and similarity > 0.49999:
            if length == len(before_tax):
                patterns = ( ( '8', '6', '0', '5', '9', '4' ) , ( '1', '4', '7' ), ( '3', '8' ), ( '3', '7' ) );
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

            return True

        return False


    def __FormatData__(self, amountdata):
        length = len(amountdata)
        if length == 0:
            return ''

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


    def __Round__(self, value):
        ivalue = int(value * 1000)

        if ivalue % 10 > 4:
            return value + Decimal(0.01)

        return value


    def __DoubleToString__(self, invalue):
        value = str(int(invalue * 100))
        length = len(value)

        if (length >= 3):
            return value[0:length - 2] + '.' + value[length - 2:length]
        else:
            value = str(invalue)
            dotindex = value.find('.')
            if dotindex != -1:
                return value[0: dotindex + 3]

        return value
