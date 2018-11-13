import logging

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel

class TimeDataFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self, 'Time')

    
    def __FixedData__(self, resultJson):
        starttime, endtime = self.__ParseData__(resultJson)

        logging.info(starttime + ', ' + endtime + u' Fixed To ')
        
        confidencelevel, starttime, endtime = self.__FixedTimeData__(starttime, endtime)

        logging.info(starttime + ', ' + endtime)

        return confidencelevel, starttime, endtime


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_starttime, result_endtime = self.__ParseData__(resultJson)
        validated_starttime, validated_endtime = self.__ParseData__(validateJson)

        if not self.__CheckData__(validated_mileage):
            logging.info(u'Validated Data Error')
            return

        if validated_mileage != result_mileage:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(result_mileage + u' Fixed To ')
        
        confidencelevel, result_mileage = self.__FixedTimeData__(result_mileage)

        logging.info(result_mileage)

        if validated_mileage == result_mileage:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_mileage)
            logging.info(u'Fixed Falied!')


    def __ParseData__(self, jsondata):
        starttime = ''
        endtime = ''

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return starttime, endtime

        regions = jsondata[u'regions']

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None:
                continue

            cls = region[u'cls']
            if cls == 31:
                starttime = region[u'result'][0]

            elif cls == 32:
                endtime = region[u'result'][0]

        return starttime, endtime


    def __CheckData__(self, data):
        return len(data) == 5


    def __FixedTimeData__(self, starttime, endtime):
        return ConfidenceLevel.Bad, starttime, endtime