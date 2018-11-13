import logging

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel

class NumberDataFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self, 'Number')
        self.__NumberCount__ = 9
        self.__NumberPatterns__ = self.__NumberPatterns__ + self.__LetterPatterns__

    
    def __FixedData__(self, resultJson):
        result_numberlist = self.__ParseData__(resultJson)
        if len(result_numberlist) == 0:
            logging.info(u'Number Data Error')
            return ConfidenceLevel.Bad, ''

        logging.info(result_numberlist[0] + u' Fixed To ')
        
        confidencelevel, number = ConfidenceLevel.Bad, result_numberlist[0]

        logging.info(number)

        return confidencelevel, number


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_numberlist = self.__ParseData__(resultJson)
        validated_numberlist = self.__ParseData__(validateJson)

        if len(validated_numberlist) == 0 or len(result_numberlist) == 0 or not self.__CheckData__(validated_numberlist[0]):
            logging.info(u'Validated Data Error')
            return

        if validated_numberlist[0] != result_numberlist[0]:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(result_numberlist[0] + u' Fixed To ')
        
        confidencelevel, number = self.__FixedNumberData__(result_numberlist)

        logging.info(number)

        if validated_numberlist[0] == number:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_numberlist[0])
            logging.info(u'Fixed Falied!')


    def __ParseData__(self, jsondata):
        numberlist = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return numberlist

        regions = jsondata[u'regions']

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None or region[u'ref_result'] == None:
                continue

            cls = region[u'cls']
            if cls == 2:
                for result in region[u'result']:
                    if len(result):
                        numberlist.append(result)

                for result in region[u'ref_result']:
                    if len(result):
                        numberlist.append(result)

        return numberlist


    def __CheckData__(self, data):
        if len(data) == 0 or len(data) != self.__NumberCount__:
            return False

        return self.__CheckNumber__(data)


    def __FixedNumberData__(self, datalist):
        datalist = sorted(datalist, key=lambda x: abs(len(x) - self.__NumberCount__))

        if len(datalist):
            return ConfidenceLevel.Bad, datalist[0]
        else:
            return ConfidenceLevel.Bad, ''
