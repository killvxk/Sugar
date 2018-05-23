from DataFixed import DataFixed
from DataFixed import ConfidenceLevel

class CodeDataFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self)
        self.__ErrorCount__ = 0
        self.__FixedCount__ = 0
        self.__NumberCount__ = 10
        self.__NumberPatterns__ = (u'0', u'1', u'2', u'3', u'4', u'5', u'6', u'7', u'8', u'9')


    def __BeforeFixed__(self):
        print(u'Start Fixed Code Data=================>\n')

    
    def __FixedData__(self, resultJson):
        result_firstcodelist, result_secondcodelist = self.__ParseData__(resultJson)
        if len(result_firstcodelist) == 0:
            print(u'Data Error')
            return ConfidenceLevel.Bad, ''

        print(result_firstcodelist[0] + u' Fixed To ')
        
        confidencelevel, code = self.__FixedCodeData__(result_firstcodelist, result_secondcodelist)

        print(code)

        return confidencelevel, code


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_firstcodelist, result_secondcodelist = self.__ParseData__(resultJson)
        validated_firstcodelist, validated_secondcodelist = self.__ParseData__(validateJson)

        if len(validated_firstcodelist) == 0 or len(result_firstcodelist) == 0 or not self.__CheckData__(validated_firstcodelist[0]):
            print(u'Validated Data Error')
            return

        if validated_firstcodelist[0] != result_firstcodelist[0]:
            self.__ErrorCount__ += 1
        else:
            print(u'Validated Equal To Result')
            return

        print(u'Validated Not Equal To Result')
        print(result_firstcodelist[0] + u' Fixed To ')
        
        confidencelevel, code = self.__FixedCodeData__(result_firstcodelist, result_secondcodelist)

        print(code)

        if validated_firstcodelist[0] == code:
            self.__FixedCount__ += 1
            print(u'Fixed Success!')
        else:
            print(u'Validated ' + validated_firstcodelist[0])
            print(u'Fixed Falied!')


    def __AfterFixed__(self):
        print(u'Error Count ' + str(self.__ErrorCount__) + u', Fixed Count ' + str(self.__FixedCount__))

        print(u'\n<=================End Fixed Code Data')


    def __ParseData__(self, jsondata):
        firstcodelist = []
        secondcodelist = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return firstcodelist, secondcodelist

        regions = jsondata[u'regions']

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None or region[u'ref_result'] == None:
                continue

            cls = region[u'cls']
            if cls == 1:
                for result in region[u'result']:
                    if len(result):
                        firstcodelist.append(result)

                for result in region[u'ref_result']:
                    if len(result):
                        firstcodelist.append(result)

            elif cls == 6:
                for result in region[u'result']:
                    if len(result):
                        secondcodelist.append(result)

                for result in region[u'ref_result']:
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
                return ConfidenceLevel.Confident, first

        for first in firstdata:
            if self.__CheckData__(first):
                return ConfidenceLevel.Fixed, first;

        for second in seconddata:
            if self.__CheckData__(second):
                return ConfidenceLevel.Fixed, second;

        return ConfidenceLevel.Bad, firstdata[0]
