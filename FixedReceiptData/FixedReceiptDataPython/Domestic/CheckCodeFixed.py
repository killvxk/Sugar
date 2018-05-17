import re

from DataFixed import DataFixed

class CheckCodeFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self)
        self.__ErrorCount__ = 0
        self.__FixedCount__ = 0
        self.__NumberCount__ = 20
        self.__NumberPatterns__ = ('0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ' ')

    def __BeforeFixed__(self):
        print('Start Fixed CheckCode Data=================>\n')

    
    def __FixedData__(self, resultJson):
        result_checkcodelist = self.__ParseData__(resultJson)
        if len(result_checkcodelist) == 0:
            print('Data Error')
            return ''

        print(result_checkcodelist[0] + ' Fixed To ')
        
        flag, checkcode = self.__FixedCheckCodeData__(result_checkcodelist)

        print(checkcode)

        return checkcode


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_checkcodelist = self.__ParseData__(resultJson)
        validated_checkcodelist = self.__ParseData__(validateJson)

        if len(validated_checkcodelist) == 0 or len(result_checkcodelist) == 0 or not self.__CheckData__(validated_checkcodelist[0]):
            print('Validated Data Error')
            return

        if validated_checkcodelist[0] != result_checkcodelist[0]:
            self.__ErrorCount__ += 1
        else:
            print('Validated Equal To Result')
            return

        print('Validated Not Equal To Result')
        print(result_checkcodelist[0] + ' Fixed To ')
        
        flag, checkcode = self.__FixedCheckCodeData__(result_checkcodelist)

        print(checkcode)

        if validated_checkcodelist[0] == checkcode:
            self.__FixedCount__ += 1
            print('Fixed Success!')
        else:
            print('Validated ' + validated_checkcodelist[0])
            print('Fixed Falied!')


    def __AfterFixed__(self):
        print('Error Count ' + str(self.__ErrorCount__) + ', Fixed Count ' + str(self.__FixedCount__))

        print('\n<=================End Fixed CheckCode Data')


    def __ParseData__(self, jsondata):
        checkcodelist = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata['regions'] == None:
            return checkcodelist

        regions = jsondata['regions']

        for region in regions:
            if region['cls'] == None or region['result'] == None or region['ref_result'] == None:
                continue

            cls = region['cls']
            if cls == 5:
                for result in region['result']:
                    if len(result):
                        checkcodelist.append(result)

                for result in region['ref_result']:
                    if len(result):
                        checkcodelist.append(result)

        checkcodelist = list(map(lambda x: re.sub(' |-', '', x), checkcodelist))
        return checkcodelist


    def __CheckData__(self, data):
        if len(data) == 0 or len(data) != self.__NumberCount__:
            return False

        for ch in data:
            if ch not in self.__NumberPatterns__:
                return False

        return True


    def __FixedCheckCodeData__(self, datalist):
        datalist = sorted(datalist, key=lambda x: abs(len(x) - self.__NumberCount__))

        for data in datalist:
            if self.__CheckData__(data):
                return True, data;

        return False, datalist[0]