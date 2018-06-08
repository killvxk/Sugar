import logging

from Domestic.VAT.CodeDataFixed import CodeDataFixed
from ConfidenceLevel import ConfidenceLevel

class NumberDataFixed(CodeDataFixed):
    """description of class"""

    def __init__(self):
        CodeDataFixed.__init__(self)
        self.__ErrorCount__ = 0
        self.__FixedCount__ = 0
        self.__NumberCount__ = 8

    def __BeforeFixed__(self):
        logging.info(u'Start Fixed Number Data=================>\n')

    
    def __FixedData__(self, resultJson):
        result_firstnumberlist, result_firstconfidence, result_secondnumberlist, result_secondconfidence = self.__ParseData__(resultJson)
        if len(result_firstnumberlist) == 0:
            logging.info(u'Number Data Error')
            return ConfidenceLevel.Bad, ''

        logging.info(result_firstnumberlist[0] + u' Fixed To ')
        
        confidencelevel = ConfidenceLevel.Bad
        if (result_firstconfidence >= result_secondconfidence):
            confidencelevel, number = self.__FixedCodeData__(result_firstnumberlist, result_secondnumberlist)
        else:
            confidencelevel, number = self.__FixedCodeData__(result_secondnumberlist, result_firstnumberlist)

        logging.info(number)

        return confidencelevel, number


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_firstnumberlist, result_firstconfidence, result_secondnumberlist, result_secondconfidence = self.__ParseData__(resultJson)
        validated_firstnumberlist, validated_firstconfidence, validated_secondnumberlist, validated_secondconfidence = self.__ParseData__(validateJson)

        if len(validated_firstnumberlist) == 0 or len(result_firstnumberlist) == 0 or not self.__CheckData__(validated_firstnumberlist[0]):
            logging.info(u'Validated Data Error')
            return

        if validated_firstnumberlist[0] != result_firstnumberlist[0]:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(result_firstnumberlist[0] + u' Fixed To ')
        
        if (result_firstconfidence > result_secondconfidence):
            confidencelevel, number = self.__FixedCodeData__(result_firstnumberlist, result_secondnumberlist)
        else:
            confidencelevel, number = self.__FixedCodeData__(result_secondnumberlist, result_firstnumberlist)

        logging.info(number)

        if validated_firstnumberlist[0] == number:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_firstnumberlist[0])
            logging.info(u'Fixed Falied!')


    def __AfterFixed__(self):
        logging.info(u'Error Count ' + str(self.__ErrorCount__) + u', Fixed Count ' + str(self.__FixedCount__))

        logging.info(u'\n<=================End Fixed Number Data')


    def __ParseData__(self, jsondata):
        firstnumberlist = []
        firstconfidence = 0.0
        secondnumberlist = []
        secondconfidence = 0.0

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return firstnumberlist, firstconfidence, secondnumberlist, secondconfidence

        regions = jsondata[u'regions']

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None or region[u'ref_result'] == None or region[u'confidence'] == None:
                continue

            cls = region[u'cls']
            if cls == 2:
                for result in region[u'result']:
                    if len(result):
                        firstnumberlist.append(result)

                for result in region[u'ref_result']:
                    if len(result):
                        firstnumberlist.append(result)

                firstconfidence = region[u'confidence']

            elif cls == 7:
                for result in region[u'result']:
                    if len(result):
                        secondnumberlist.append(result)

                for result in region[u'ref_result']:
                    if len(result):
                        secondnumberlist.append(result)

                secondconfidence = region[u'confidence']

        return firstnumberlist, firstconfidence, secondnumberlist, secondconfidence