import logging

from Domestic.Train.NumberDataFixed import NumberDataFixed
from ConfidenceLevel import ConfidenceLevel


class TrainNumberDataFixed(NumberDataFixed):
    """description of class"""

    def __init__(self):
        NumberDataFixed.__init__(self)
        self.__ErrorCount__ = 0
        self.__FixedCount__ = 0


    def __BeforeFixed__(self):
        logging.info(u'Start Fixed Number Data=================>\n')

    
    def __FixedData__(self, resultJson):
        result_trainnumberlist = self.__ParseData__(resultJson)
        if len(result_trainnumberlist) == 0:
            logging.info(u'TrainNumber Data Error')
            return ConfidenceLevel.Bad, ''

        logging.info(result_trainnumberlist[0] + u' Fixed To ')
        
        confidencelevel, trainnumber = ConfidenceLevel.Bad, result_trainnumberlist[0]

        logging.info(trainnumber)

        return confidencelevel, trainnumber


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_trainnumberlist = self.__ParseData__(resultJson)
        validated_trainnumberlist = self.__ParseData__(validateJson)

        if len(validated_trainnumberlist) == 0 or len(result_trainnumberlist) == 0 or not self.__CheckData__(validated_trainnumberlist[0]):
            logging.info(u'Validated Data Error')
            return

        if validated_trainnumberlist[0] != result_trainnumberlist[0]:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(result_trainnumberlist[0] + u' Fixed To ')
        
        confidencelevel, trainnumber = self.__FixedNumberData__(result_trainnumberlist)

        logging.info(trainnumber)

        if validated_trainnumberlist[0] == trainnumber:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_trainnumberlist[0])
            logging.info(u'Fixed Falied!')


    def __AfterFixed__(self):
        logging.info(u'Error Count ' + str(self.__ErrorCount__) + u', Fixed Count ' + str(self.__FixedCount__))

        logging.info(u'\n<=================End Fixed TrainNumber Data')


    def __ParseData__(self, jsondata):
        trainnumberlist = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return trainnumberlist

        regions = jsondata[u'regions']

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None or region[u'ref_result'] == None:
                continue

            cls = region[u'cls']
            if cls == 17:
                for result in region[u'result']:
                    if len(result):
                        trainnumberlist.append(result)

                for result in region[u'ref_result']:
                    if len(result):
                        trainnumberlist.append(result)

        return trainnumberlist


    def __CheckData__(self, data):
        if len(data) == 0:
            return False

        return self.__CheckNumber__(data)