import logging

from DataFixed import DataFixed
from ConfidenceLevel import ConfidenceLevel


class SeatDataFixed(DataFixed):
    """description of class"""

    def __init__(self):
        DataFixed.__init__(self)
        self.__ErrorCount__ = 0
        self.__FixedCount__ = 0


    def __BeforeFixed__(self):
        logging.info(u'Start Fixed Seat Data=================>\n')

    
    def __FixedData__(self, resultJson):
        result_seatlist = self.__ParseData__(resultJson)
        if len(result_seatlist) == 0:
            logging.info(u'Seat Data Error')
            return ConfidenceLevel.Bad, ''

        logging.info(result_seatlist[0] + u' Fixed To ')
        
        confidencelevel, seat = ConfidenceLevel.Bad, result_seatlist[0]

        logging.info(seat)

        return confidencelevel, seat


    def __FixedDataWithValidate__(self, resultJson, validateJson):
        result_seatlist = self.__ParseData__(resultJson)
        validated_seatlist = self.__ParseData__(validateJson)

        if len(validated_seatlist) == 0 or len(result_seatlist) == 0:
            logging.info(u'Validated Data Error')
            return

        if validated_seatlist[0] != result_seatlist[0]:
            self.__ErrorCount__ += 1
        else:
            logging.info(u'Validated Equal To Result')
            return

        logging.info(u'Validated Not Equal To Result')
        logging.info(result_seatlist[0] + u' Fixed To ')
        
        confidencelevel, seat = ConfidenceLevel.Bad, result_seatlist[0]

        logging.info(seat)

        if validated_seatlist[0] == seat:
            self.__FixedCount__ += 1
            logging.info(u'Fixed Success!')
        else:
            logging.info(u'Validated ' + validated_seatlist[0])
            logging.info(u'Fixed Falied!')


    def __AfterFixed__(self):
        logging.info(u'Error Count ' + str(self.__ErrorCount__) + u', Fixed Count ' + str(self.__FixedCount__))

        logging.info(u'\n<=================End Fixed Seat Data')


    def __ParseData__(self, jsondata):
        seatlist = []

        if jsondata == None or not isinstance(jsondata, dict) or jsondata[u'regions'] == None:
            return seatlist

        regions = jsondata[u'regions']

        for region in regions:
            if region[u'cls'] == None or region[u'result'] == None or region[u'ref_result'] == None:
                continue

            cls = region[u'cls']
            if cls == 18:
                for result in region[u'result']:
                    if len(result):
                        seatlist.append(result)

                if region.get(u'ref_result') != None:
                    for result in region[u'ref_result']:
                        if len(result):
                            seatlist.append(result)

        return seatlist
