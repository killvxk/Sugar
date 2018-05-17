from DataFixed import DataFixed

class CodeDataFixed(DataFixed):
    """description of class"""

    def __init__(self, validatedPath, resultPath):
        DataFixed.__init__(self, validatedPath, resultPath)
        self.__ErrorCount__ = 0
        self.__FixedCount__ = 0
        self.__NumberCount__ = 10
        self.__NumberPatterns__ = ('0', '1', '2', '3', '4', '5', '6', '7', '8', '9')


    def __BeforeFixed__(self):
        print('Start Fixed Code Data=================>\n')

    
    def __FixedData__(self, validateJson, resultJson):
        validated_firstcodelist, validated_secondcodelist = self.__ParseData__(validateJson)
        result_firstcodelist, result_secondcodelist = self.__ParseData__(resultJson)

        if len(validated_firstcodelist) == 0 or len(result_firstcodelist) == 0 or not self.__CheckData__(validated_firstcodelist[0]):
            print('Validated Data Error')
            return

        if validated_firstcodelist[0] != result_firstcodelist[0]:
            self.__ErrorCount__ += 1
        else:
            print('Validated Equal To Result')
            return

        print('Validated Not Equal To Result')
        print(result_firstcodelist[0] + ' Fixed To ')
        
        flag, code = self.__FixedCodeData__(result_firstcodelist, result_secondcodelist)

        print(code)

        if validated_firstcodelist[0] == code:
            self.__FixedCount__ += 1
            print('Fixed Success!')
        else:
            print('Validated ' + validated_firstcodelist[0])
            print('Fixed Falied!')


    def __AfterFixed__(self):
        print('Error Count ' + str(self.__ErrorCount__) + ', Fixed Count ' + str(self.__FixedCount__))

        print('\n<=================End Fixed Code Data')


    def __ParseData__(self, jsondata):
        firstcodelist = []
        secondcodelist = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata['regions'] == None:
            return firstcodelist, secondcodelist

        regions = jsondata['regions']

        for region in regions:
            if region['cls'] == None or region['result'] == None or region['ref_result'] == None:
                continue

            cls = region['cls']
            if cls == 1:
                for result in region['result']:
                    if len(result):
                        firstcodelist.append(result)

                for result in region['ref_result']:
                    if len(result):
                        firstcodelist.append(result)

            elif cls == 6:
                for result in region['result']:
                    if len(result):
                        secondcodelist.append(result)

                for result in region['ref_result']:
                    if len(result):
                        secondcodelist.append(result)

        return firstcodelist, secondcodelist


    def __CheckData__(self, data):
        if len(data) == 0 or len(data) != self.__NumberCount__:
            return False

        for ch in data:
            if ch not in self.__NumberPatterns__:
                return False

        return True


    def __FixedCodeData__(self, firstdata, seconddata):
        firstdata = sorted(firstdata, key=lambda x: abs(len(x) - self.__NumberCount__))
        seconddata = sorted(seconddata, key=lambda x: abs(len(x) - self.__NumberCount__))

        for first in firstdata:
            if first in seconddata and self.__CheckData__(first):
                return True, first

        for first in firstdata:
            if self.__CheckData__(first):
                return True, first;

        for second in seconddata:
            if self.__CheckData__(second):
                return True, second;

        return False, firstdata[0]
