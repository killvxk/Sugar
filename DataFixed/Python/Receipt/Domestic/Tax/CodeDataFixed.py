import logging

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel

class CodeDataFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self)
        self.__ErrorCount__ = 0
        self.__FixedCount__ = 0
        self.__NumberCount__ = 12


    def __BeforeFixed__(self):
        logging.info(u'Start Fixed Code Data=================>\n')

    
    def __FixedData__(self, resultJson):
        result_codelist = self.__ParseData__(resultJson)
        if len(result_codelist) == 0:
            logging.info(u'Code Data Error')
            return ConfidenceLevel.Bad, ''

        logging.info(result_codelist[0] + u' Fixed To ')
        
        confidencelevel, code = self.__FixedCodeData__(result_codelist)

        logging.info(code)

        return confidencelevel, code


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_codelist = self.__ParseData__(resultJson)
        validated_codelist = self.__ParseData__(validateJson)

        if len(validated_codelist) == 0 or len(result_codelist) == 0 or not self.__CheckData__(validated_codelist[0]):
            logging.info(u'Validated Data Error')
            return

        if validated_codelist[0] != result_codelist[0]:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(result_codelist[0] + u' Fixed To ')
        
        confidencelevel, code = self.__FixedCodeData__(result_codelist)

        logging.info(code)

        if validated_codelist[0] == code:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_codelist[0])
            logging.info(u'Fixed Falied!')


    def __AfterFixed__(self):
        logging.info(u'Error Count ' + str(self.__ErrorCount__) + u', Fixed Count ' + str(self.__FixedCount__))

        logging.info(u'\n<=================End Fixed Code Data')


    def __ParseData__(self, jsondata):
        codelist = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return codelist

        regions = jsondata[u'regions']

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None or region[u'ref_result'] == None:
                continue

            cls = region[u'cls']
            if cls == 1:
                for result in region[u'result']:
                    if len(result):
                        codelist.append(result)

                for result in region[u'ref_result']:
                    if len(result):
                        codelist.append(result)

        return codelist


    def __CheckData__(self, data):
        if len(data) == 0 or len(data) != self.__NumberCount__:
            return False

        return self.__CheckNumber__(data)


    def __FixedCodeData__(self, datalist):
        datalist = sorted(datalist, key=lambda x: abs(len(x) - self.__NumberCount__))

        if len(datalist):
            return ConfidenceLevel.Bad, datalist[0]
        else:
            return ConfidenceLevel.Bad, ''
