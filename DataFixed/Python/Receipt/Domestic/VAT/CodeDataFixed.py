import logging

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel

class CodeDataFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self)
        self.__ErrorCount__ = 0
        self.__FixedCount__ = 0
        self.__NumberCount__ = 10


    def __BeforeFixed__(self):
        logging.info(u'Start Fixed Code Data=================>\n')

    
    def __FixedData__(self, resultJson):
        result_firstcodelist, result_secondcodelist = self.__ParseData__(resultJson)
        if len(result_firstcodelist) == 0:
            logging.info(u'Code Data Error')
            return ConfidenceLevel.Bad, ''

        logging.info(result_firstcodelist[0] + u' Fixed To ')
        
        self.__NumberCount__ = self.__GuessCodeNumberCount__(result_firstcodelist, result_secondcodelist)
        confidencelevel, code = self.__FixedCodeData__(result_firstcodelist, result_secondcodelist)

        logging.info(code)

        return confidencelevel, code


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_firstcodelist, result_secondcodelist = self.__ParseData__(resultJson)
        validated_firstcodelist, validated_secondcodelist = self.__ParseData__(validateJson)

        if len(validated_firstcodelist) == 0 or len(result_firstcodelist) == 0 or not self.__CheckData__(validated_firstcodelist[0]):
            logging.info(u'Validated Data Error')
            return

        if validated_firstcodelist[0] != result_firstcodelist[0]:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(result_firstcodelist[0] + u' Fixed To ')
        
        self.__NumberCount__ = self.__GuessCodeNumberCount__(result_firstcodelist, result_secondcodelist)
        confidencelevel, code = self.__FixedCodeData__(result_firstcodelist, result_secondcodelist)

        logging.info(code)

        if validated_firstcodelist[0] == code:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_firstcodelist[0])
            logging.info(u'Fixed Falied!')


    def __AfterFixed__(self):
        logging.info(u'Error Count ' + str(self.__ErrorCount__) + u', Fixed Count ' + str(self.__FixedCount__))

        logging.info(u'\n<=================End Fixed Code Data')


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

        return self.__CheckNumber__(data)


    def __FixedCodeData__(self, firstdata, seconddata):
        firstdata = sorted(firstdata, key=lambda x: abs(len(x) - self.__NumberCount__))
        seconddata = sorted(seconddata, key=lambda x: abs(len(x) - self.__NumberCount__))

        for first in firstdata:
            if first in seconddata and self.__CheckData__(first):
                return ConfidenceLevel.Confident, first

        for first in firstdata:
            if self.__CheckData__(first):
                return ConfidenceLevel.Fixed, first

        for second in seconddata:
            if self.__CheckData__(second):
                return ConfidenceLevel.Fixed, second

        if len(firstdata):
            return ConfidenceLevel.Bad, firstdata[0]
        else:
            return ConfidenceLevel.Bad, ''


    def __GuessCodeNumberCount__(self, firstcodelist, secondcodelist):
        numbercountmap = {10 : 0, 12 : 0}
        for code in firstcodelist:
            if numbercountmap.has_key(len(code)):
                numbercountmap[len(code)] += 1;

        for code in secondcodelist:
            if numbercountmap.has_key(len(code)):
                numbercountmap[len(code)] += 1;

        if numbercountmap[12] > numbercountmap[10]:
            return 12;
        else:
            return 10;

