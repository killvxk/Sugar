from DataFixed import DataFixed
from decimal import Decimal

class AmountDataFixed(DataFixed):
    """description of class"""
    def __init__(self, validatedPath, resultPath):
        DataFixed.__init__(self, validatedPath, resultPath)
        self.__ErrorCount__ = 0


    def __BeforeFixed__(self):
        print('Start Fixed Amount Data=================>' + '\n\n')


    def __FixedData__(self, validateJson, resultJson):
        validated_before_tax, validated_tax, validated_after_tax = self.__ParseData__(validateJson)
        result_before_tax, result_tax, result_after_tax = self.__ParseData__(resultJson)

        if not self.__CheckData__(validated_before_tax) or not self.__CheckData__(validated_tax) or not self.__CheckData__(validated_after_tax):
            print('Validated Data Error\n')
            return

        if validated_before_tax != result_before_tax or validated_tax != result_tax or validated_after_tax != result_after_tax:
            self.__ErrorCount__ += 1;
        else:
            print('Validated Equal To Result\n')

        print('Validated Not Equal To Result\n')
        print(result_before_tax + ', ' + result_tax + ', ' + result_after_tax + '\n');

        result_before_tax, result_tax, result_after_tax = __FixedAmountData__(result_before_tax, result_tax, result_after_tax)




    def __ParseData__(self, jsondata):
        if jsondata == None or not isinstance(jsondata, dict) or jsondata['regions'] == None:
            return

        regions = jsondata['regions']

        before_tax = ''
        tax = ''
        after_tax = ''

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

        return [before_tax, tax, after_tax]


    def __CheckData__(self, data):
        if len(data) == 0:
            return False

        dotindex = data.find('.')
        if (dotindex != -1 and (len(data) - dotindex) == 3):
            return True

        return False


    def __FixedAmountData__(self, before_tax, tax, after_tax):
        d_before_tax = Decimal(before_tax)
        d_tax = Decimal(tax)
        d_after_tax = Decimal(after_tax)

        if (d_after_tax == (d_before_tax + d_tax)):
            if (not self.__CheckData__(before_tax)):
                before_tax = self.__FormatAmountData__(before_tax)
            if (not self.__CheckData__(tax)):
                tax = self.__FormatAmountData__(tax)
            if (not self.__CheckData__(after_tax)):
                after_tax = self.__FormatAmountData__(after_tax)
        return before_tax, tax, after_tax

    def __FormatAmountData__(self, amountdata):
        pass
